// PmvatDoc.cpp : implementation of the CPmvatDoc class
//

#include "stdafx.h"

#include "PmvatApp.h"
#include "Common.h"
#include "LogView.h"
#include "PmvatDoc.h"
#include "VersionInfo.h"
#include "ProcTreeView.h"

#ifdef _DEBUG
# define new DEBUG_NEW
#endif

#ifdef NDEBUG
# ifdef OutputDebugString
# undef OutputDebugString
# define OutputDebugString(x) (void)0;
# endif
#endif

IMPLEMENT_DYNCREATE(CPmvatDoc, CDocument)

BEGIN_MESSAGE_MAP(CPmvatDoc, CDocument)
END_MESSAGE_MAP()

// CPmvatDoc construction/destruction

CPmvatDoc::CPmvatDoc()
: m_procs(0), m_pid( PROCESS_ID_INVALID ), m_named(0), m_unnamed(0), m_total(0)
{
    m_hTable.reserve(64);
    ASSERT( 0 == m_hTable.size() );

    assert( NULL != (HMODULE)m_hfuncs );
}

CPmvatDoc::~CPmvatDoc(){ }

// CPmvatDoc diagnostics

#ifdef _DEBUG
void CPmvatDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void CPmvatDoc::Dump(CDumpContext& dc) const
{
    CDocument::Dump(dc);
}
#endif //_DEBUG

BOOL CPmvatDoc::ProcessChanged( LPCWSTR procName, INT newPid )
{
    // Returned to caller
    BOOL bResult = TRUE;

    CSingleLock lock( &m_csection );
    bResult = lock.Lock();
    ASSERT( TRUE == bResult );
    if( !bResult ) { return FALSE; }

    ResetDocument();

    m_name = procName;
    m_pid = newPid;

    try
    {
        bResult = DuplicateMutexHandles( m_pid );
        ASSERT( bResult );
    }
    catch( const ProgramException& e )
    {
        CLogView::WriteLine( e.what() );
        return FALSE;
    }

    WCHAR wsz[255];
    StringCchPrintf( wsz, _countof(wsz), L"Opened process %s (%d).", m_name, m_pid );
    CLogView::WriteLine( wsz );
    StringCchPrintf( wsz, _countof(wsz), L" Mutex handles: total: %d, named: %d," \
        L" unnamed: %d.", m_total, m_named, m_unnamed );
    CLogView::WriteLine( wsz );

    return bResult;
}

BOOL CPmvatDoc::PrivilegeChanged( )
{
    // Returned to caller
    BOOL bResult = TRUE;

    CSingleLock lock( &m_csection );
    bResult = lock.Lock();
    ASSERT( TRUE == bResult );
    if( !bResult ) { return FALSE; }

    CString savedName = m_name;
    INT savedPid = m_pid;

    ResetDocument();

    m_name = savedName;
    m_pid = savedPid;

    return bResult;
}

BOOL CPmvatDoc::ResetDocument()
{
    // Lock the document for reading
    CSingleLock lock( &m_csection );
    lock.Lock();

    m_pid = PROCESS_ID_INVALID;
    m_name.Empty();

    m_named = 0;
    m_unnamed = 0;
    m_total = 0;

    m_hTable.clear();

    m_procs = CProcTreeView::GetView()->GetTreeCtrl().GetCount();

    return TRUE;
}

BOOL CPmvatDoc::DuplicateMutexHandles( INT pid )
{
    // General purpose results
    NTSTATUS status = STATUS_SUCCESS;

    // Function pointers of interest
    assert( NULL != (HMODULE)m_hfuncs );
    if( NULL == m_hfuncs ) { return FALSE; }

    if( PROCESS_ID_INVALID == pid || PROCESS_ID_SYSTEM == pid ) { return FALSE; }

    // Open the target process
    AutoHandle hProcess;
    hProcess.Attach( OpenProcess(PROCESS_DUP_HANDLE, FALSE, pid) );
    DWORD dwLastError = GetLastError();

    // Fires too often for a standard user
    // ASSERT( NULL != hProcess );
    if( NULL == hProcess )
    {
        wostringstream err;
        err << L"Failed to open process " << pid;
        err << L". OpenProcess returned ";
        err << std::hex << dwLastError << L".";
        throw ProgramException( err.str().c_str() );
    }

    // We can save on reallocations by estimating the required size
    size_t size = 0x40000; // minimum size, tuned for XP (Vista gets at least one realloc)
    if( m_procs > 0 ) { size = 128 * 1024 * m_procs; }
    HandleTableAutoPtr htable( size );

    while ((status = m_hfuncs.pfnNtQuerySystemInformation( SystemHandleInformation,
        (LPVOID)htable, (ULONG)htable.Size(), NULL )) == STATUS_INFO_LENGTH_MISMATCH)
    {
        // Default for Reallocate is to double current size
        htable.Reallocate();
        OutputDebugString( L"SystemHandleInformation reallocation\n" );
    }

    assert( NT_SUCCESS(status) );
    if( !NT_SUCCESS(status) )
    {
        wostringstream err;
        err << L"Failed to query handle table. NtQuerySystemInformation ";
        err << L"returned " << std::hex << status << L".";
        throw ProgramException( err.str().c_str() );
    }

    // Back to C interface
    PSYSTEM_HANDLE_INFORMATION pcbHandleInfo = (PSYSTEM_HANDLE_INFORMATION)(LPVOID)htable;
# ifndef WIN64
    if( 0 == pcbHandleInfo->HandleCount )
    {
        wostringstream err;
        err << L"Handle count is 0." << endl;
        err << L" - Are you using an x86 build on an x64 system?" << endl;
        err << L" - Is an Antivirus program blocking access?" << endl;
        throw ProgramException( err.str().c_str() );
    }
# endif

    // Lock the document for modification
    CSingleLock lock( &m_csection );
    lock.Lock();

    for(ULONG i = 0; i < pcbHandleInfo->HandleCount; i++)
    {
        const SYSTEM_HANDLE& handle = pcbHandleInfo->Handles[i];
        if( (ULONG)pid != handle.ProcessId ) { continue; }

        DuplicateMutexHandle(hProcess, handle);
    }

    HandleTable::iterator it = m_hTable.begin();
    for( ; it != m_hTable.end(); it++ )
    {
        QueryMutexHandleName( *it );
        QueryMutexHandleDacl( *it );
        QueryMutexHandleState( *it );

        m_total++;

        if( !it->m_name.empty() )
        {
            m_named++;
        }
        else
        {
            m_unnamed++;
        }
    }

    ASSERT( m_total == (INT)m_hTable.size() );
    return TRUE;
}

BOOL CPmvatDoc::DuplicateMutexHandle(const HANDLE& hProcess, const SYSTEM_HANDLE& syshandle)
{
    // Function pointers of interest
    assert( NULL != (HMODULE)m_hfuncs );
    if( NULL == m_hfuncs ) { return FALSE; }

    // General purpose results
    NTSTATUS status = STATUS_SUCCESS;

    // Returned to caller
    BOOL bResult = FALSE;

    do
    {
        DWORD dwAccess = 0;
        AutoHandle dup;

        for( INT i=0; i < _countof(MUTEX_ACCESS); i++ )
        {
            dwAccess = MUTEX_ACCESS[i];
            status = m_hfuncs.pfnNtDuplicateObject( hProcess, (HANDLE)syshandle.Handle,
                GetCurrentProcess(), &dup.Ref(), dwAccess, 0, 0 );
            if(NT_SUCCESS(status) ) { break; }
        }
        if(!NT_SUCCESS(status) ) { dwAccess=0; }

        // Query the object type
        AutoBuffer info( 0x100 );

        while( (status = m_hfuncs.pfnNtQueryObject( dup, ObjectTypeInformation,
            (PVOID)info, (ULONG)info.Size(), NULL )) == STATUS_INFO_LENGTH_MISMATCH )
        {
            // Default for Reallocate is to double current size
            info.Reallocate();
            OutputDebugString( L"ObjectTypeInformation reallocation\n" );
        }

        // Fires too often for objects which we do not care about
        // ASSERT( NT_SUCCESS(status) );
        if( !NT_SUCCESS(status) ) { break; }

        POBJECT_TYPE_INFORMATION pObjectTypeInfo =
            (POBJECT_TYPE_INFORMATION)(LPVOID)info;

        wstring type( pObjectTypeInfo->Name.Buffer, pObjectTypeInfo->Name.Length );
        if( 0 != wcscmp( L"Mutant", type.c_str() ) ) { break; }

        // Assignment duplicates the handle
        MutexHandle mhandle;
        mhandle.m_handle = dup;
        //mhandle.m_access = dwAccess;

        m_hTable.push_back( mhandle );

        bResult = TRUE;

    } while( false );

    return bResult;
}

BOOL CPmvatDoc::QueryMutexHandleName( MutexHandle& handle )
{
    // Function pointers of interest
    assert( NULL != (HMODULE)m_hfuncs );
    if( NULL == m_hfuncs ) { return FALSE; }

    // General purpose results
    NTSTATUS status = STATUS_SUCCESS;

    // Query the object type
    AutoBuffer info( 0x100 );

    // Query object name (reuse info buffer)
    while( (status = m_hfuncs.pfnNtQueryObject( handle.m_handle, ObjectNameInformation,
        (PVOID)info, (ULONG)info.Size(), NULL )) == STATUS_INFO_LENGTH_MISMATCH )
    {
        // Default for Reallocate is to double current size
        info.Reallocate();
        OutputDebugString( L"ObjectNameInformation reallocation\n" );
    }

    ASSERT( NT_SUCCESS(status) );
    if( NT_SUCCESS(status) )
    {
        const UNICODE_STRING& name = *((PUNICODE_STRING)(LPVOID)info);
        if( name.Length ) {
            handle.m_name = wstring( name.Buffer, name.Length );
        }
    }

    return NT_SUCCESS(status);
}

BOOL CPmvatDoc::QueryMutexHandleState( MutexHandle& handle )
{
    handle.m_state = MutexStateAsString( handle.m_handle );

    return TRUE;
}

BOOL CPmvatDoc::QueryMutexHandleDacl( MutexHandle& handle )
{
    PSECURITY_DESCRIPTOR pSecDesc = NULL;
    PACL pDacl = NULL;

    DWORD dwReturn = ERROR_SUCCESS;

    __try
    {
        // Retrieve the SD and ACL for the object
        dwReturn = GetSecurityInfo( handle.m_handle, SE_KERNEL_OBJECT, DACL_SECURITY_INFORMATION,
            NULL /*&pOwner*/, NULL /*&pGroup*/, &pDacl, NULL/*&pSacl*/, &pSecDesc );
        if( dwReturn != ERROR_SUCCESS ) { __leave; }

        if( NULL == pDacl ) { handle.m_dacl = L"NULL"; }
    }
    __finally
    {
        if( NULL != pSecDesc ) {
            LocalFree( pSecDesc ); pSecDesc = NULL;
        }
    }

    return dwReturn == ERROR_SUCCESS;
}

BOOL CPmvatDoc::SaveModified()
{
    SetModifiedFlag(FALSE);

    return CDocument::SaveModified();
}

void CPmvatDoc::Serialize(CArchive& ar)
{
    if (!ar.IsStoring()) { return; }

    CFile* pFile = ar.GetFile();
    ASSERT( NULL != pFile );
    if( !pFile ) { return; }

    CLogView* pView = CLogView::GetView();
    ASSERT( NULL != pView );
    if( !pView ) { return; }

    CString sz;
    pView->GetEditCtrl().GetWindowText(sz);

    try
    {
        // Unicode BOM
        const BYTE b[] = { 0xFF, 0xFE };
        pFile->Write( b, _countof(b) );
        pFile->Write( sz.GetBuffer(), sz.GetLength()*sizeof(WCHAR) );
    }
    catch( CFileException& e )
    {
        WCHAR msg[ 128 ];
        StringCchPrintf( msg, _countof(msg), L"Failed to savelog file. LastError=%d", e.m_lOsError );
        AfxMessageBox( msg, MB_ICONERROR );
    }
}
