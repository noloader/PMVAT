// SquatView.cpp : implementation file
//

#include "stdafx.h"
#include "PmvatApp.h"
#include "PmvatDoc.h"
#include "MainFrame.h"
#include "LogView.h"
#include "SquatView.h"
#include "ProcTreeView.h"
#include "ProcMutexView.h"
#include "FreeformDlg.h"

#ifdef _DEBUG
# define new DEBUG_NEW
#endif

#ifdef NDEBUG
# ifdef OutputDebugString
# undef OutputDebugString
# define OutputDebugString(x) (void)0;
# endif
#endif

BEGIN_MESSAGE_MAP(CSquatView, CListView)
    ON_WM_DESTROY()
    ON_WM_CLOSE()
    ON_WM_CONTEXTMENU()
    ON_UPDATE_COMMAND_UI(ID_ATTACK_CANCEL, &CSquatView::OnUpdateAttackCancel)
    ON_UPDATE_COMMAND_UI(ID_ATTACK_RELEASE, &CSquatView::OnUpdateAttackRelease)
    ON_UPDATE_COMMAND_UI(ID_ATTACK_ABANDON, &CSquatView::OnUpdateAttackAbandon)
    ON_UPDATE_COMMAND_UI(ID_ATTACK_CANCELALL, &CSquatView::OnUpdateAttackCancelAll)
    ON_UPDATE_COMMAND_UI(ID_ATTACK_RELEASEALL, &CSquatView::OnUpdateAttackReleaseAll)
    ON_UPDATE_COMMAND_UI(ID_ATTACK_ABANDONALL, &CSquatView::OnUpdateAttackAbandonAll)
    ON_UPDATE_COMMAND_UI(ID_ATTACK_TIMEDRELEASE, &CSquatView::OnUpdateAttackTimedRelease)
    ON_UPDATE_COMMAND_UI(ID_ATTACK_TIMEDABANDON, &CSquatView::OnUpdateAttackTimedAbandon)
    ON_UPDATE_COMMAND_UI(ID_ATTACK_CLEARCOMPLETE, &CSquatView::OnUpdateAttackClearComplete)
    ON_COMMAND(ID_ATTACK_CANCEL, &CSquatView::OnAttackCancel)
    ON_COMMAND(ID_ATTACK_RELEASE, &CSquatView::OnAttackRelease)
    ON_COMMAND(ID_ATTACK_ABANDON, &CSquatView::OnAttackAbandon)
    ON_COMMAND(ID_ATTACK_CANCELALL, &CSquatView::OnAttackCancelAll)
    ON_COMMAND(ID_ATTACK_RELEASEALL, &CSquatView::OnAttackReleaseAll)
    ON_COMMAND(ID_ATTACK_ABANDONALL, &CSquatView::OnAttackAbandonAll)
    ON_COMMAND(ID_ATTACK_TIMEDRELEASE, &CSquatView::OnAttackTimedRelease)
    ON_COMMAND(ID_ATTACK_TIMEDABANDON, &CSquatView::OnAttackTimedAbandon)
    ON_COMMAND(ID_ATTACK_CLEARCOMPLETE, &CSquatView::OnAttackClearComplete)
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CSquatView, CListView)

CSquatView::CSquatView(): m_vsb_width(0)
{
    m_vsb_width = GetSystemMetrics(SM_CXVSCROLL);
}

CSquatView::~CSquatView(){ }

CSquatView* CSquatView::GetView()
{
    CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
    ASSERT( pFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)) );
    if( !pFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)) ) { return NULL; }

    return pFrame->GetSquatView();
}

void CSquatView::DoDataExchange(CDataExchange* pDX)
{
    CListView::DoDataExchange(pDX);
}

// CSquatView diagnostics

#ifdef _DEBUG
void CSquatView::AssertValid() const
{
    CListView::AssertValid();
}

void CSquatView::Dump(CDumpContext& dc) const
{
    CListView::Dump(dc);
}

CPmvatDoc* CSquatView::GetDocument()
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPmvatDoc)));
    return (CPmvatDoc*)m_pDocument;
}
#endif //_DEBUG

BOOL CSquatView::PreCreateWindow(CREATESTRUCT& cs)
{
    if( !CListView::PreCreateWindow(cs) )
        return FALSE;

    // default is report view and full row selection
    cs.style &= ~LVS_TYPEMASK;
    cs.style |= LVS_SHOWSELALWAYS;
    cs.style |= LVS_REPORT;

    return TRUE;
}

void CSquatView::OnInitialUpdate()
{
    CListView::OnInitialUpdate();

    DWORD dwStyle = GetListCtrl().GetExtendedStyle();
    dwStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_INFOTIP;
    GetListCtrl().SetExtendedStyle(dwStyle);

    CRect r;
    GetClientRect( &r );

    const DWORD COLUMN_MIN_WIDTH = 60;
    const DWORD COLUMN_0_WIDTH = 60;

    INT w = r.Width() - COLUMN_0_WIDTH - (m_vsb_width+1);
    w < COLUMN_MIN_WIDTH ? (w = COLUMN_MIN_WIDTH) : w;

    GetListCtrl().InsertColumn( 0, L"Status", LVCFMT_LEFT, COLUMN_0_WIDTH );
    GetListCtrl().InsertColumn( 1, L"Mutex", LVCFMT_LEFT, w );
}

void CSquatView::OnAttackCancel()
{
    AttackCancel();
}

void CSquatView::OnAttackRelease()
{
    AttackRelease();
}

void CSquatView::OnAttackAbandon()
{
    AttackAbandon();
}

void CSquatView::OnAttackCancelAll()
{
    AttackCancelAll();
}

void CSquatView::OnAttackReleaseAll()
{
    AttackReleaseAll();
}

void CSquatView::OnAttackAbandonAll()
{
    AttackAbandonAll();
}

void CSquatView::OnAttackTimedRelease()
{
    AttackTimedRelease();
}

void CSquatView::OnAttackTimedAbandon()
{
    AttackTimedAbandon();
}

void CSquatView::OnAttackClearComplete()
{
    AttackClearComplete();
}

void CSquatView::OnUpdateAttackCancel(CCmdUI *pCmdUI)
{
    ASSERT( NULL != pCmdUI );
    if( NULL == pCmdUI ) { return; }

    BOOL bEnable = FALSE;

    INT selected = -1;
    INT count = GetListCtrl().GetSelectedCount();
    for( INT i = 0; i <count; i++ )
    {
        selected = GetListCtrl().GetNextItem( selected, LVNI_SELECTED );
        ASSERT( -1 != selected );
        if( -1 == selected ) { continue; }

        const SquatParams* params = (SquatParams*)GetListCtrl().GetItemData(selected);
        ASSERT( NULL != params );
        if( NULL == params ) { continue; }

        if( params->m_state == SquatParams::Waiting )
        {
            bEnable = TRUE;
            break;
        }
    }

    pCmdUI->Enable(bEnable);
}

void CSquatView::OnUpdateAttackRelease(CCmdUI *pCmdUI)
{
    ASSERT( NULL != pCmdUI );
    if( NULL == pCmdUI ) { return; }

    BOOL bEnable = FALSE;

    INT selected = -1;
    INT count = GetListCtrl().GetSelectedCount();
    for( INT i = 0; i <count; i++ )
    {
        selected = GetListCtrl().GetNextItem( selected, LVNI_SELECTED );
        ASSERT( -1 != selected );
        if( -1 == selected ) { continue; }

        const SquatParams* params = (SquatParams*)GetListCtrl().GetItemData(selected);
        ASSERT( NULL != params );
        if( NULL == params ) { continue; }

        if( params->m_state == SquatParams::Acquired )
        {
            bEnable = TRUE;
            break;
        }
    }

    pCmdUI->Enable(bEnable);
}

void CSquatView::OnUpdateAttackAbandon(CCmdUI *pCmdUI)
{
    ASSERT( NULL != pCmdUI );
    if( NULL == pCmdUI ) { return; }

    BOOL bEnable = FALSE;

    INT selected = -1;
    INT count = GetListCtrl().GetSelectedCount();
    for( INT i = 0; i <count; i++ )
    {
        selected = GetListCtrl().GetNextItem( selected, LVNI_SELECTED );
        ASSERT( -1 != selected );
        if( -1 == selected ) { continue; }

        const SquatParams* params = (SquatParams*)GetListCtrl().GetItemData(selected);
        ASSERT( NULL != params );
        if( NULL == params ) { continue; }

        if( params->m_state == SquatParams::Acquired )
        {
            bEnable = TRUE;
            break;
        }
    }

    pCmdUI->Enable(bEnable);
}

void CSquatView::OnUpdateAttackCancelAll(CCmdUI *pCmdUI)
{
    ASSERT( NULL != pCmdUI );
    if( NULL == pCmdUI ) { return; }

    BOOL bEnable = FALSE;

    INT count = GetListCtrl().GetItemCount();
    for( INT i = 0; i <count; i++ )
    {
        const SquatParams* params = (SquatParams*)GetListCtrl().GetItemData(i);
        ASSERT( NULL != params );
        if( NULL == params ) { continue; }

        if( params->m_state == SquatParams::Waiting )
        {
            bEnable = TRUE;
            break;
        }
    }

    pCmdUI->Enable(bEnable);
}

void CSquatView::OnUpdateAttackReleaseAll(CCmdUI *pCmdUI)
{
    ASSERT( NULL != pCmdUI );
    if( NULL == pCmdUI ) { return; }

    BOOL bEnable = FALSE;

    INT count = GetListCtrl().GetItemCount();
    for( INT i = 0; i <count; i++ )
    {
        const SquatParams* params = (SquatParams*)GetListCtrl().GetItemData(i);
        ASSERT( NULL != params );
        if( NULL == params ) { continue; }

        if( params->m_state == SquatParams::Acquired )
        {
            bEnable = TRUE;
            break;
        }
    }

    pCmdUI->Enable(bEnable);
}

void CSquatView::OnUpdateAttackTimedRelease(CCmdUI *pCmdUI)
{
    OnUpdateAttackReleaseAll(pCmdUI);
}

void CSquatView::OnUpdateAttackAbandonAll(CCmdUI *pCmdUI)
{
    ASSERT( NULL != pCmdUI );
    if( NULL == pCmdUI ) { return; }

    BOOL bEnable = FALSE;

    INT count = GetListCtrl().GetItemCount();
    for( INT i = 0; i <count; i++ )
    {
        const SquatParams* params = (SquatParams*)GetListCtrl().GetItemData(i);
        ASSERT( NULL != params );
        if( NULL == params ) { continue; }

        if( params->m_state == SquatParams::Acquired )
        {
            bEnable = TRUE;
            break;
        }
    }

    pCmdUI->Enable(bEnable);
}

void CSquatView::OnUpdateAttackTimedAbandon(CCmdUI *pCmdUI)
{
    OnUpdateAttackAbandonAll(pCmdUI);
}

void CSquatView::OnUpdateAttackClearComplete(CCmdUI *pCmdUI)
{
    ASSERT( NULL != pCmdUI );
    if( NULL == pCmdUI ) { return; }

    BOOL bEnable = FALSE;

    INT count = GetListCtrl().GetItemCount();
    for( INT i = 0; i <count; i++ )
    {
        const SquatParams* params = (SquatParams*)GetListCtrl().GetItemData(i);
        ASSERT( NULL != params );
        if( NULL == params ) { continue; }

        if( params->m_state == SquatParams::Released ||
            params->m_state == SquatParams::Abandoned )
        {
            bEnable = TRUE;
            break;
        }
    }

    pCmdUI->Enable(bEnable);
}

void CSquatView::OnDestroy()
{
    g_shutdown = TRUE;

    DeleteAllItems();

    CListView::OnDestroy();
}

void CSquatView::OnClose()
{
    DeleteAllItems();

    CListView::OnClose();
}

BOOL CSquatView::DeleteAllItems()
{
    // Start high, pull low
    BOOL bResult = TRUE;
    INT idx = GetListCtrl().GetItemCount()-1;
    for( ; idx >= 0; idx-- )
    {
        bResult &= FreeItemData(idx);
    }

    ASSERT(bResult);
    return bResult;
}

BOOL CSquatView::FreeItemData(INT pos)
{
    ASSERT( -1 != pos );
    if( -1 == pos ) { return FALSE; }

    SquatParams* params = (SquatParams*)GetListCtrl().GetItemData(pos);
    ASSERT( NULL != params );
    if( NULL == params ) { return FALSE; }

    // Before we delete the params, signal the thread
    if( params->m_state == SquatParams::Waiting )
    {
        if( NULL != params->m_cancel )
            SetEvent( params->m_cancel.Ref() );
    }

    // Next, Release any acquired resources
    if( params->m_state == SquatParams::Acquired )
    {
        if( NULL != params->m_release )
            SetEvent( params->m_release.Ref() );
    }

    // Wait for the thread before deleting params
    HANDLE h = params->m_thread.Ref();
    DWORD dwWait = WaitForSingleObject( h, INFINITE );
    DWORD dwLastError = GetLastError();
    ASSERT( dwWait == WAIT_OBJECT_0 ); UNUSED( dwWait );
    ASSERT( ERROR_SUCCESS == dwLastError ); UNUSED( dwLastError );

    // Finally, cleanup
    if( NULL != params )
    {
        delete params; params = NULL;
        GetListCtrl().SetItemData(pos, NULL);
    }

    return TRUE;
}

BOOL CSquatView::ResetView()
{
    DeleteAllItems();

    return GetListCtrl().DeleteAllItems();
}

void CSquatView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
    if( g_shutdown )
    {
        CListView::OnUpdate(pSender, lHint, pHint);
        return;
    }

    switch( lHint )
    {
    case HINT_WAITING_MUTEX:
    case HINT_WAITFAILED:
    case HINT_CANCEL_WAITING:
    case HINT_ACQUIRED_MUTEX:
    case HINT_RELEASED_MUTEX:
    case HINT_ABANDONED_MUTEX:
        UpdateSquatState();
        break;
    case HINT_ATTACK_SQUAT:
        AddAttackThread( (SquatParams*)pHint );
        break;
    default:
        CListView::OnUpdate(pSender, lHint, pHint);
        break;
    }
}

BOOL CSquatView::UpdateSquatState()
{
    // Returned to caller
    BOOL bResult = TRUE;

    CSingleLock( &m_lock, TRUE );

    INT count = GetListCtrl().GetItemCount();
    for( INT i = 0; i <count; i++ )
    {
        const SquatParams* params = (SquatParams*)GetListCtrl().GetItemData(i);
        ASSERT( NULL != params );
        if( NULL == params ) { continue; }

        switch( params->m_state )
        {
        case SquatParams::Waiting:
            GetListCtrl().SetItemText( i, 0, L"Waiting" );
            break;
        case SquatParams::Failed:
            GetListCtrl().SetItemText( i, 0, L"Failed" );
            break;
        case SquatParams::Canceled:
            GetListCtrl().SetItemText( i, 0, L"Canceled" );
            break;
        case SquatParams::Acquired:
            GetListCtrl().SetItemText( i, 0, L"Acquired" );
            break;
        case SquatParams::Released:
            GetListCtrl().SetItemText( i, 0, L"Released" );
            break;
        case SquatParams::Abandoned:
            GetListCtrl().SetItemText( i, 0, L"Abandoned" );
            break;
        default: ;;;

        }
    }

    return bResult;
}

void CSquatView::AddAttackThread( SquatParams* params )
{
    ASSERT( NULL != params );
    if( NULL == params ) { return; }

    if( g_shutdown ) { delete params; params = NULL; return; }

    WCHAR wsz[255];
    if( params->m_pid == PROCESS_ID_INVALID && params->m_number )
    {
        if( params->m_name.empty() )
        {
            StringCchCopy( wsz, _countof(wsz), L"Freeform (unnamed)" );
        }
        else
        {
            StringCchPrintf( wsz, _countof(wsz), L"Freeform (%s)", params->m_name.c_str() );
        }
    }
    else
    {
        if( params->m_name.empty() )
        {
            StringCchPrintf( wsz, _countof(wsz), L"Process %d, Number %d",
                params->m_pid, params->m_number );
        }
        else
        {
            StringCchPrintf( wsz, _countof(wsz), L"Process %d, Number %d (%s)",
                params->m_pid, params->m_number, params->m_name.c_str() );
        }
    }

    CSingleLock( &m_lock, TRUE );

    INT pos = GetListCtrl().GetItemCount();
    pos = params->m_pos = GetListCtrl().InsertItem( pos, L"" );
    ASSERT( -1 != pos );
    if( -1 == pos ) { delete params; params = NULL; return; }

    GetListCtrl().SetItemText(pos, 1, wsz);
    GetListCtrl().SetItemData(pos, (DWORD_PTR)params);

    CWinThread* pThread = AfxBeginThread( AttackThreadProc, (LPVOID)params,
        THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED, NULL );

    // AutoHandle::operator= will duplicate
    params->m_thread = pThread->m_hThread;

    pThread->m_bAutoDelete = TRUE;
    pThread->ResumeThread();
}

UINT CDECL AttackThreadProc( LPVOID lpParams )
{
    ASSERT( NULL != lpParams );
    if( NULL == lpParams ) { return ERROR_INVALID_PARAMETER; }

    // The list view holds the parameters. It will delete them. The
    // list view does not delete from under us. It waits on our
    // thread to exit.
    SquatParams* params = (SquatParams*)lpParams;

    // Stash away the HWND
    ASSERT( AfxGetApp()->m_pMainWnd );
    HWND hWnd = AfxGetApp()->m_pMainWnd->GetSafeHwnd();

    // Scratch for messages
    WCHAR wsz[255];
    DWORD CCH = _countof(wsz);

    // Wait Results
    DWORD dwWait = WAIT_FAILED, dwLastError = ERROR_SUCCESS;

    // Notify that we are getting ready to wait
    params->m_state = SquatParams::Waiting;
    if( hWnd && !g_shutdown ) {
        ::SendMessage( hWnd, MSG_WAITING_MUTEX, params->m_pos, NULL );
    }

    HANDLE h1[2] = { params->m_target.Ref(), params->m_cancel.Ref() };
    ASSERT( NULL != h1[0] );
    ASSERT( NULL != h1[1] );
    dwWait = WaitForMultipleObjects( _countof(h1), h1, FALSE, INFINITE );
    dwLastError = GetLastError();

    switch( dwWait )
    {
        // Acquired Mutex
    case WAIT_OBJECT_0:
        params->m_state = SquatParams::Acquired;
        if( hWnd && !g_shutdown ) {
            ::SendMessage( hWnd, MSG_ACQUIRED_MUTEX, params->m_pos, NULL );
            FormatAcquiredMessage( wsz, CCH, *params );
            SendLogMessage( wsz );
        }
        break;
        // Wait canceled
    case WAIT_OBJECT_0+1:
        params->m_state = SquatParams::Canceled;
        if( hWnd && !g_shutdown ) {
            ::SendMessage( hWnd, MSG_CANCEL_WAITING, params->m_pos, NULL );
            FormatCanceledMessage( wsz, CCH, *params );
            SendLogMessage( wsz );
        }
        break;
    case WAIT_FAILED:
    default:
        ASSERT( FALSE );
        params->m_state = SquatParams::Failed;

        if( hWnd && !g_shutdown ) {
            ::SendMessage( hWnd, MSG_WAITFAILED, params->m_pos, NULL );
            FormatFailedWaitMessage( wsz, CCH, *params, dwLastError );
            SendLogMessage( wsz );
        }
        break;
    }

    // We only Wait if we gained ownership
    if( WAIT_OBJECT_0 != dwWait )
    {
        // Was it the cancel message (or failure)?
        if( WAIT_OBJECT_0+1 == dwWait ) { return ERROR_SUCCESS; }
        // Failure
        return dwLastError;
    }

    // We got the mutex... Just hang on to it until signaled
    HANDLE h2[] = { params->m_release.Ref(), params->m_abandon.Ref() };
    ASSERT( NULL != h2[0] );
    ASSERT( NULL != h2[1] );

    dwWait = WaitForMultipleObjects( _countof(h2), h2, FALSE, INFINITE );
    dwLastError = GetLastError();

    if( WAIT_OBJECT_0 == dwWait )
    {
        BOOL b = ReleaseMutex( params->m_target.Ref() );
        ASSERT( b ); UNUSED( b );

        params->m_state = SquatParams::Released;

        ASSERT( b );

        if( hWnd && !g_shutdown ) {
            // Notify that we are getting ready to wait
            ::SendMessage( hWnd, MSG_RELEASED_MUTEX, params->m_pos, NULL );
            FormatReleasedMessage( wsz, CCH, *params );
            SendLogMessage( wsz );
        }

        dwLastError = ERROR_SUCCESS;
    }
    else if( WAIT_OBJECT_0+1 == dwWait )
    {
        params->m_state = SquatParams::Abandoned;

        if( hWnd && !g_shutdown ) {
            // Notify that we are getting ready to wait
            ::SendMessage( hWnd, MSG_ABANDONED_MUTEX, params->m_pos, NULL );            
            FormatAbandonedMessage( wsz, CCH, *params );
            SendLogMessage( wsz );
        }

        dwLastError = ERROR_SUCCESS;
    }
    else
    {
        params->m_state = SquatParams::Failed;

        if( hWnd && !g_shutdown ) {
            ASSERT( FALSE );
            ::SendMessage( hWnd, MSG_WAITFAILED, params->m_pos, NULL );            
            FormatFailedWaitMessage( wsz, CCH, *params, dwLastError );
            SendLogMessage( wsz );
        }
    }

    // The list deletes the params on closing. However, for housekeeping,
    // we clean the handles now... We have to keep the thread handle open
    // so that the main thread can wait on it during shutdown
    params->m_target.Close();
    params->m_cancel.Close();
    params->m_abandon.Close();
    params->m_release.Close();

    // We completed successfully.
    return dwLastError;
}

VOID FormatCanceledMessage( WCHAR* pwsz, DWORD cch, const SquatParams& params )
{
    // Lots of interesing stuff happens when we are shutting down
    if( g_shutdown ) { return; }

    FormatLogMessage( L"Canceled", pwsz, cch, params );
}

VOID FormatReleasedMessage( WCHAR* pwsz, DWORD cch, const SquatParams& params )
{
    // Lots of interesing stuff happens when we are shutting down
    if( g_shutdown ) { return; }

    FormatLogMessage( L"Released", pwsz, cch, params );
}

VOID FormatAbandonedMessage( WCHAR* pwsz, DWORD cch, const SquatParams& params )
{
    // Lots of interesing stuff happens when we are shutting down
    if( g_shutdown ) { return; }

    FormatLogMessage( L"Abandoned", pwsz, cch, params );
}

VOID FormatFailedWaitMessage( WCHAR* pwsz, DWORD cch, const SquatParams& params, DWORD dwError )
{
    // Lots of interesing stuff happens when we are shutting down
    if( g_shutdown ) { return; }

    FormatLogMessage( L"Failed", pwsz, cch, params );

    WCHAR err[64];
    StringCchPrintf( err, _countof(err), L". LastError=%d", dwError );
    StringCchCat( pwsz, cch, err );
}

VOID FormatAcquiredMessage( WCHAR* pwsz, DWORD cch, const SquatParams& params )
{
    // Lots of interesing stuff happens when we are shutting down
    if( g_shutdown ) { return; }

    FormatLogMessage( L"Acquired", pwsz, cch, params );
}

VOID FormatLogMessage( LPCWSTR pwszAction, WCHAR* pwsz, DWORD cch, const SquatParams& params )
{
    // Manual squat (not taken from a Process handle table)
    if( -1 == params.m_number && -1 == params.m_pid )
    {
        if( !params.m_name.empty() )
        {
            StringCchPrintf( pwsz, cch, L"%s mutex %s (freeform)",
                pwszAction, params.m_name.c_str() );
        }
        else
        {
            StringCchPrintf( pwsz, cch, L"%s unnamed mutex (freeform)", pwszAction );
        }
    }
    else
    {
        if( !params.m_name.empty() )
        {
            StringCchPrintf( pwsz, cch, L"%s mutex %d from pid %d (%s)",
                pwszAction, params.m_number, params.m_pid, params.m_name.c_str() );
        }
        else
        {
            StringCchPrintf( pwsz, cch, L"%s unnamed mutex %d from pid %d",
                pwszAction, params.m_number, params.m_pid );
        }
    }
}

BOOL SendLogMessage( LPCWSTR pwsz )
{
    // No sense in logging if shutting down
    if( g_shutdown ) { return FALSE; }

    ASSERT( AfxGetApp()->m_pMainWnd );
    HWND hWnd = AfxGetApp()->m_pMainWnd->GetSafeHwnd();
    if( hWnd && !g_shutdown ) {
        ::SendMessage( hWnd, MSG_LOG_MESSAGE, (WPARAM) new CString(pwsz), NULL );
    }

    return hWnd != NULL;
}

DWORD TestMutex(HANDLE handle)
{
    // No Asserts in this routine
    DWORD dwWait = WAIT_FAILED;

    __try
    {
        dwWait = WaitForSingleObject(handle, 0);

        if( dwWait == WAIT_FAILED )
        {
            dwWait = GetLastError();
        }

    }
    __finally
    {
        if( dwWait == WAIT_OBJECT_0 || dwWait == WAIT_ABANDONED )
        {
            ReleaseMutex(handle);
        }
    }

    return dwWait;
}

void CSquatView::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
    if( 0 == GetListCtrl().GetSelectedCount() ) { return; }

    do
    {
        // [0 to n-1]
        CMenu* pMenu = dynamic_cast<CMenu*>(AfxGetMainWnd()->GetMenu());
        ASSERT( NULL != pMenu );
        if( NULL == pMenu ) { break; }

        INT pos = FindMenuPosition( pMenu, L"Attack" );
        ASSERT( MenuNotFound != pos );
        if( MenuNotFound == pos ) { break; }

        CMenu* pPopup = pMenu->GetSubMenu( pos );
        ASSERT( NULL != pPopup );
        if( NULL == pPopup ) { break; }

        const UINT nFlags = TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD;
        INT nID = pPopup->TrackPopupMenu(nFlags, point.x, point.y, AfxGetMainWnd());
        if( 0 == nID ) { break; }

        switch( nID )
        {
        case ID_ATTACK_CANCEL:
            OnAttackCancel();
            break;
        case ID_ATTACK_RELEASE:
            OnAttackRelease();
            break;
        case ID_ATTACK_ABANDON:
            OnAttackAbandon();
            break;
        case ID_ATTACK_CANCELALL:
            OnAttackCancelAll();
            break;
        case ID_ATTACK_RELEASEALL:
            OnAttackReleaseAll();
            break;
        case ID_ATTACK_ABANDONALL:
            OnAttackAbandonAll();
            break;
        case ID_ATTACK_TIMEDRELEASE:
            OnAttackTimedRelease();
            break;
        case ID_ATTACK_TIMEDABANDON:
            OnAttackTimedAbandon();
            break;
        case ID_ATTACK_CLEARCOMPLETE:
            OnAttackClearComplete();
            break;
        case ID_ATTACK_FREEFORM:
            FreeformAttack();
            break;
        default:
            ASSERT(FALSE);
            break;
        }

    } while(false);
}

BOOL CSquatView::FreeformAttack()
{
    CFreeformDlg dlg;
    if( IDOK != dlg.DoModal() ) { return TRUE; }
    LPCWSTR pszName = dlg.GetMutexName();

    // We need to make sure the HANDLE *is not* duplicated.
    // Also, create the Mutex without initial owner since
    // the first thing AttackThread tries to do is squat it.
    HANDLE h = CreateMutex( NULL, FALSE, pszName );
    DWORD dwError = GetLastError();

    if( h == NULL )
    {
        CString szErr;
        szErr.Format( L"Failed to open mutex %s. dwLastError=%d.", pszName, dwError );
        AfxMessageBox( szErr, MB_ICONERROR );
        return FALSE;
    }

    SquatParams* params = new SquatParams();
    ASSERT( NULL != params );
    if( NULL == params ) { return FALSE; }

    params->m_target.Attach( h );
    params->m_name = pszName;

    AddAttackThread( params );

    return TRUE;
}

INT CSquatView::FindMenuPosition( CMenu* pMenu, LPCWSTR pszText )
{
    ASSERT( NULL != pMenu );
    if( NULL == pMenu ) { return MenuNotFound; }

    ASSERT( NULL != pszText );
    if( NULL == pszText ) { return MenuNotFound; }

    INT count = pMenu->GetMenuItemCount();
    ASSERT( count > 0 );
    if( !(count > 0) ) { return MenuNotFound; }

    INT pos = MenuNotFound;
    for(INT i = 0; i < count; i++ )
    {
        // Fix up the stupid ampersands
        CString text = pszText; text.Replace(L"&", L"");
        CString str;

        if( pMenu->GetMenuString(i, str, MF_BYPOSITION) ) {
            str.Replace( L"&", L"" );
            if ( 0 == text.CompareNoCase( str ) ) {
                pos = i; break;
            }
        }
    }

    ASSERT( pos != MenuNotFound );
    return pos;
}

BOOL CSquatView::AttackCancel()
{
    return AttackCancel(GetListCtrl().GetNextItem(-1, LVNI_SELECTED));
}

BOOL CSquatView::AttackCancelAll()
{
    // Returned to caller
    BOOL bResult = TRUE;

    CSingleLock( &m_lock, TRUE );

    INT count = GetListCtrl().GetItemCount();
    for( INT i = 0; i <count; i++ )
    {
        bResult &= AttackCancel( i );
    }

    ASSERT( bResult );
    return bResult;
}

BOOL CSquatView::AttackCancel(INT pos)
{
    ASSERT( -1 != pos );
    if( -1 == pos ) { return FALSE; }

    CSingleLock( &m_lock, TRUE );

    SquatParams* params = (SquatParams*)GetListCtrl().GetItemData(pos);
    ASSERT( NULL != params );
    if( NULL == params ) { return FALSE; }

    if( params->m_state != SquatParams::Waiting ) { return FALSE; }

    BOOL bReturn = SetEvent( params->m_cancel );
    ASSERT( bReturn );

    UpdateSquatState();

    return bReturn;
}

BOOL CSquatView::AttackRelease()
{
    return AttackRelease(GetListCtrl().GetNextItem(-1, LVNI_SELECTED));
}

BOOL CSquatView::AttackReleaseAll()
{
    // Returned to caller
    BOOL bResult = TRUE;

    CSingleLock( &m_lock, TRUE );

    INT count = GetListCtrl().GetItemCount();
    for( INT i = 0; i <count; i++ )
    {
        bResult &= AttackRelease( i );
    }

    ASSERT( bResult );
    return bResult;
}

BOOL CSquatView::AttackTimedRelease()
{
    // Returned to caller
    BOOL bResult = TRUE;

    CSingleLock( &m_lock, TRUE );

    INT count = GetListCtrl().GetItemCount();
    for( INT i = 0; i <count; i++ )
    {
        bResult &= AttackRelease( i );

        Sleep( 1000 );
    }

    ASSERT( bResult );
    return bResult;
}

BOOL CSquatView::AttackRelease(INT pos)
{
    ASSERT( -1 != pos );
    if( -1 == pos ) { return FALSE; }

    CSingleLock( &m_lock, TRUE );

    SquatParams* params = (SquatParams*)GetListCtrl().GetItemData(pos);
    ASSERT( NULL != params );
    if( NULL == params ) { return FALSE; }

    if( params->m_state != SquatParams::Acquired ) { return FALSE; }

    BOOL bReturn = SetEvent( params->m_release );
    ASSERT( bReturn );

    UpdateSquatState();

    return bReturn;
}

BOOL CSquatView::AttackAbandon()
{
    return AttackAbandon(GetListCtrl().GetNextItem(-1, LVNI_SELECTED));
}

BOOL CSquatView::AttackAbandonAll()
{
    // Returned to caller
    BOOL bResult = TRUE;

    CSingleLock( &m_lock, TRUE );

    INT count = GetListCtrl().GetItemCount();
    for( INT i = 0; i <count; i++ )
    {
        bResult &= AttackAbandon( i );
    }

    ASSERT( bResult );
    return bResult;
}

BOOL CSquatView::AttackTimedAbandon()
{
    // Returned to caller
    BOOL bResult = TRUE;

    INT count = GetListCtrl().GetItemCount();
    for( INT i = 0; i <count; i++ )
    {
        bResult &= AttackAbandon( i );

        Sleep( 1000 );
    }

    ASSERT( bResult );
    return bResult;
}

BOOL CSquatView::AttackAbandon(INT pos)
{
    ASSERT( -1 != pos );
    if( -1 == pos ) { return FALSE; }

    CSingleLock( &m_lock, TRUE );

    SquatParams* params = (SquatParams*)GetListCtrl().GetItemData(pos);
    ASSERT( NULL != params );
    if( NULL == params ) { return FALSE; }

    if( params->m_state != SquatParams::Acquired ) { return FALSE; }

    BOOL bReturn = SetEvent( params->m_abandon );
    ASSERT( bReturn );

    UpdateSquatState();

    return bReturn;
}

BOOL CSquatView::AttackClearComplete()
{
    // Returned to caller
    BOOL bResult = TRUE;

    CSingleLock( &m_lock, TRUE );

    INT count = GetListCtrl().GetItemCount();
    for( INT i = count-1; i >=0; i-- )
    {
        SquatParams* params = (SquatParams*)GetListCtrl().GetItemData(i);
        ASSERT( NULL != params );
        if( NULL == params ) { continue; }

        if( params->m_state == SquatParams::Canceled ||
            params->m_state == SquatParams::Abandoned ||
            params->m_state == SquatParams::Released )
        {
            GetListCtrl().SetItemData(i,NULL);
            GetListCtrl().DeleteItem(i);

            delete params; params = NULL;
        }
    }

    // Next we have to update indexes
    count = GetListCtrl().GetItemCount();
    for( INT i = count-1; i >=0; i-- )
    {
        SquatParams* params = (SquatParams*)GetListCtrl().GetItemData(i);
        ASSERT( NULL != params );
        if( NULL == params ) { continue; }

        params->m_pos = i;
    }

    UpdateSquatState();

    ASSERT( bResult );
    return bResult;
}