// ProcMutexView.cpp : implementation file
//

#include "stdafx.h"

#include "PmvatApp.h"
#include "Common.h"
#include "MainFrame.h"
#include "SquatView.h"
#include "FreeformDlg.h"
#include "ProcTreeView.h"
#include "ProcMutexView.h"

using std::wostringstream;

#ifdef _DEBUG
# define new DEBUG_NEW
#endif

#ifdef NDEBUG
# ifdef OutputDebugString
# undef OutputDebugString
# define OutputDebugString(x) (void)0;
# endif
#endif

IMPLEMENT_DYNCREATE(CProcMutexView, CListView)

CProcMutexView::CProcMutexView()
: m_vsb_width( GetSystemMetrics(SM_CXVSCROLL) )
{ }

CProcMutexView::~CProcMutexView(){ }

BEGIN_MESSAGE_MAP(CProcMutexView, CListView)
    ON_COMMAND(ID_EDIT_COPY, &CProcMutexView::OnEditCopy)
    ON_WM_CONTEXTMENU()
    ON_UPDATE_COMMAND_UI(ID_ATTACK_SQUAT, &CProcMutexView::OnUpdateAttackSquat)
    ON_COMMAND(ID_ATTACK_SQUAT, &CProcMutexView::OnAttackSquat)
    ON_UPDATE_COMMAND_UI(ID_ATTACK_SQUATALL, &CProcMutexView::OnUpdateAttackSquatAll)
    ON_COMMAND(ID_ATTACK_SQUATALL, &CProcMutexView::OnAttackSquatAll)
END_MESSAGE_MAP()

// CProcMutexView diagnostics

#ifdef _DEBUG
void CProcMutexView::AssertValid() const
{
    CListView::AssertValid();
}

CPmvatDoc* CProcMutexView::GetDocument()
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPmvatDoc)));
    return (CPmvatDoc*)m_pDocument;
}

void CProcMutexView::Dump(CDumpContext& dc) const
{
    CListView::Dump(dc);
}
#endif //_DEBUG

CProcMutexView* CProcMutexView::GetView()
{
    CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
    ASSERT( pFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)) );
    if( !pFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)) ) { return NULL; }

    return pFrame->GetProcMutexView();
}

BOOL CProcMutexView::PreCreateWindow(CREATESTRUCT& cs)
{
    if( !CListView::PreCreateWindow(cs) )
        return FALSE;

    // default is report view and full row selection
    cs.style &= ~LVS_TYPEMASK;
    cs.style |= LVS_SHOWSELALWAYS;
    cs.style |= LVS_REPORT;

    return TRUE;
}

void CProcMutexView::OnInitialUpdate()
{
    CListView::OnInitialUpdate();

    DWORD dwStyle = GetListCtrl().GetExtendedStyle();
    dwStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_INFOTIP;
    GetListCtrl().SetExtendedStyle(dwStyle);

    CRect r;
    GetClientRect( &r );

    const DWORD COLUMN_MIN_WIDTH = 60;
    const DWORD COLUMN_0_WIDTH = 60;
    const DWORD COLUMN_1_WIDTH = 60;
    const DWORD COLUMN_2_WIDTH = 85;

    INT w = r.Width() - COLUMN_0_WIDTH - COLUMN_1_WIDTH - COLUMN_2_WIDTH - (m_vsb_width+1);
    w < COLUMN_MIN_WIDTH ? (w = COLUMN_MIN_WIDTH) : w;

    GetListCtrl().InsertColumn( Number, L"Number", LVCFMT_LEFT, COLUMN_0_WIDTH );
    GetListCtrl().InsertColumn( Dacl, L"DACL", LVCFMT_LEFT, COLUMN_1_WIDTH );
    GetListCtrl().InsertColumn( State, L"State", LVCFMT_LEFT, COLUMN_2_WIDTH );
    GetListCtrl().InsertColumn( Name, L"Name", LVCFMT_LEFT, w );
}

void CProcMutexView::OnUpdateAttackSquat(CCmdUI *pCmdUI)
{
    ASSERT( NULL != pCmdUI );
    if( NULL == pCmdUI ) { return; }

    if( GetListCtrl().GetSelectedCount() )
    {
        pCmdUI->Enable(TRUE);
    }
    else
    {
        pCmdUI->Enable(FALSE);
    }
}

void CProcMutexView::OnAttackSquat()
{
    SquatMutants();
}

void CProcMutexView::OnUpdateAttackSquatAll(CCmdUI *pCmdUI)
{
    ASSERT( NULL != pCmdUI );
    if( NULL == pCmdUI ) { return; }

    if( GetListCtrl().GetSelectedCount() )
    {
        pCmdUI->Enable(TRUE);
    }
    else
    {
        pCmdUI->Enable(FALSE);
    }
}

void CProcMutexView::OnAttackSquatAll()
{
    SquatAllMutants();
}

void CProcMutexView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
    CPmvatDoc* pDoc = GetDocument();
    ASSERT( NULL != pDoc );
    if( NULL == pDoc ) {
        return CListView::OnUpdate( pSender, lHint, pHint );
    }

    switch( lHint )
    {
    case HINT_PROCESS_CHANGED:
        {
            GetListCtrl().SetRedraw(FALSE);
            {
                ResetView();
                DisplayMutexHandles();
            }
            GetListCtrl().SetRedraw(TRUE);

            if( NULL != pDoc ) {
                pDoc->UpdateAllViews( this, HINT_HANDLETABLE_CHANGED, NULL );
            }
            break;
        }
    case HINT_UPDATE_HANDLE_TABLE:
        {
            INT item = (INT)pHint;
            ASSERT( item >= 0 );
            ASSERT( item < GetListCtrl().GetItemCount() );

            // We're reading, but not locking (I know)
            // Need to cut in SWMR locking

            MutexHandle& mhandle = pDoc->m_hTable[item];

            // Test object after attack
            mhandle.m_state = MutexStateAsString( mhandle.m_handle );

            if( NULL != pDoc ) {
                GetListCtrl().SetItemText( item, State, mhandle.m_state.c_str() );
            }
            break;
        }
    case HINT_PRIVILEGE_CHANGED:
        {
            ResetView();
            DisplayMutexHandles();

            if( NULL != pDoc ) {
                pDoc->UpdateAllViews( this, HINT_HANDLETABLE_CHANGED, NULL );
            }
            break;
        }
    default:
        CListView::OnUpdate( pSender, lHint, pHint );
        break;
    }
}

BOOL CProcMutexView::ResetView()
{
    return GetListCtrl().DeleteAllItems();
}

BOOL CProcMutexView::DisplayMutexHandles()
{
    CPmvatDoc* pDoc = GetDocument();
    ASSERT_VALID( pDoc );
    if( NULL == pDoc ) { return FALSE; }

    // Lock the document for reading
    CSingleLock lock( &(pDoc->m_csection) );
    lock.Lock();

    const HandleTable& htable = pDoc->m_hTable;
    HandleTable::const_iterator it = htable.begin();
    for( INT i=0; it != htable.end(); it++, i++ )
    {
        const DWORD CCH = 64;
        WCHAR wsz[CCH];

        StringCchPrintf( wsz, CCH, L"%d", i );

        // Number column
        INT pos = GetListCtrl().InsertItem( i, wsz );
        if( -1 == pos ) { continue; }

        // DACL column
        GetListCtrl().SetItemText(pos, 1, it->m_dacl.c_str());

        // State column
        GetListCtrl().SetItemText(pos, 2, it->m_state.c_str());

        // Name column
        GetListCtrl().SetItemText(pos, 3, it->m_name.c_str());
    }

    // Release as soon as possible
    lock.Unlock();

    return GetListCtrl().GetItemCount() == (INT)htable.size();
}

void CProcMutexView::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
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
        case ID_ATTACK_SQUAT:
            SquatMutants();
            break;
        case ID_ATTACK_SQUATALL:
            SquatAllMutants();
            break;
        case ID_ATTACK_FREEFORM:
            SquatFreeform();
            break;
        default:
            ASSERT(FALSE);
            break;
        }

    } while(false);
}


BOOL CProcMutexView::SquatFreeform()
{
    CSquatView* pView = CSquatView::GetView();
    ASSERT( NULL != pView );
    if( NULL == pView ) { return FALSE; }

    return pView->FreeformAttack();


    //CFreeformDlg dlg;
    //if( IDOK != dlg.DoModal() ) { return TRUE; }
    //LPCWSTR pszName = dlg.GetMutexName();

    //// We need to make sure the HANDLE *is not* duplicated.
    //// Also, create the Mutex without initial owner since
    //// the first thing AttackThread tries to do is squat it.
    //HANDLE h = CreateMutex( NULL, FALSE, pszName );
    //DWORD dwError = GetLastError();

    //if( h == NULL )
    //{
    //    CString szErr;
    //    szErr.Format( L"Failed to open mutex %s. dwLastError=%d.", pszName, dwError );
    //    AfxMessageBox( szErr, MB_ICONERROR );
    //    return FALSE;
    //}

    //SquatParams* params = new SquatParams();
    //ASSERT( NULL != params );
    //if( NULL == params ) { return FALSE; }

    //params->m_target.Attach( h );
    //params->m_name = pszName;

    //pView->AddAttackThread( params );

    //return TRUE;
}

BOOL CProcMutexView::SquatMutants()
{
    CPmvatDoc* pDoc = GetDocument();
    ASSERT_VALID( pDoc );
    if( NULL == pDoc ) { return FALSE; }

    // CString str;

    INT selected = -1;
    INT count = GetListCtrl().GetSelectedCount();
    for( INT i = 0; i <count; i++ )
    {
        selected = GetListCtrl().GetNextItem( selected, LVNI_SELECTED );
        ASSERT( -1 != selected );
        if( -1 == selected ) { continue; }

        SquatParams* params = new SquatParams;
        ASSERT( NULL != params );
        if( NULL == params ) { continue; }

        // AutoHandle::operator= will duplicate
        params->m_pid = pDoc->m_pid;
        params->m_number = selected;
        params->m_name = pDoc->m_hTable[selected].m_name;
        params->m_target = pDoc->m_hTable[selected].m_handle;

        pDoc->UpdateAllViews( this, HINT_ATTACK_SQUAT, (CObject*)params );
    }

    return TRUE;
}

BOOL CProcMutexView::SquatAllMutants()
{
    CPmvatDoc* pDoc = GetDocument();
    ASSERT_VALID( pDoc );
    if( NULL == pDoc ) { return FALSE; }

    INT count = GetListCtrl().GetItemCount();
    for( INT i = 0; i <count; i++ )
    {
        SquatParams* params = new SquatParams;
        ASSERT( NULL != params );
        if( NULL == params ) { continue; }

        // AutoHandle::operator= will duplicate
        params->m_pid = pDoc->m_pid;
        params->m_number = i;
        params->m_name = pDoc->m_hTable[i].m_name;
        params->m_target = pDoc->m_hTable[i].m_handle;

        pDoc->UpdateAllViews( this, HINT_ATTACK_SQUAT, (CObject*)params );
    }

    return TRUE;
}

INT CProcMutexView::FindMenuPosition( CMenu* pMenu, LPCWSTR pszText )
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

void CProcMutexView::OnEditCopy()
{
    // Clipboard string
    CString str;

    INT selected = -1;
    INT count = GetListCtrl().GetSelectedCount();
    for( INT i = 0; i <count; i++ )
    {
        selected = GetListCtrl().GetNextItem( selected, LVNI_SELECTED );

        CString line = GetListCtrl().GetItemText( selected, 3 );
        if( !line.IsEmpty() ) {
            str += line;
            if( i < count-1 ) { str += L"\n"; }
        }
    }

    if( str.IsEmpty() ) { return; }

    if (!OpenClipboard())
    {
        AfxMessageBox(L"Cannot open the Clipboard.", MB_ICONERROR);
        return;
    }

    do
    {
        // Remove the current Clipboard contents
        if(!EmptyClipboard())
        {
            AfxMessageBox(L"Cannot empty the Clipboard.", MB_ICONERROR);
            break;
        }

        // Get the currently selected data, hData handle to
        // global memory of data
        size_t cbStr = (str.GetLength() + 1) * sizeof(WCHAR);
        HGLOBAL hData = GlobalAlloc(GMEM_MOVEABLE, cbStr);
        ASSERT( NULL != hData );
        if (NULL == hData)
        {
            AfxMessageBox(L"Cannot allocate Clipboard memory.", MB_ICONERROR);
            break;
        }

        memcpy_s(GlobalLock(hData), cbStr, str.LockBuffer(), cbStr);
        GlobalUnlock(hData);
        str.UnlockBuffer();

        // For the appropriate data formats...
        if (::SetClipboardData(CF_UNICODETEXT, hData) == NULL)
        {
            AfxMessageBox(L"Unable copy data to Clipboard.", MB_ICONERROR);
            break;
        }

    } while(false);

    CloseClipboard();
}
