// MainFrame.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"

#include "PmvatApp.h"
#include "EulaDlg.h"
#include "PmvatDoc.h"
#include "StrSafe.h"
#include "LogView.h"
#include "InfoView.h"
#include "MainFrame.h"
#include "SquatView.h"
#include "VersionInfo.h"
#include "ProcTreeView.h"
#include "ProcMutexView.h"

#ifdef _DEBUG
# define new DEBUG_NEW
#endif

#ifdef NDEBUG
# ifdef OutputDebugString
# undef OutputDebugString
# define OutputDebugString(x) (void)0;
# endif
#endif

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
    ON_WM_CREATE()

    ON_UPDATE_COMMAND_UI(ID_INDICATOR_MUTANT_TOTAL, &CMainFrame::OnUpdateTotalMutants)
    ON_UPDATE_COMMAND_UI(ID_INDICATOR_MUTANT_NAMED, &CMainFrame::OnUpdateNamedMutants)
    ON_UPDATE_COMMAND_UI(ID_INDICATOR_MUTANT_UNNAMED, &CMainFrame::OnUpdateUnnamedMutants)
    ON_UPDATE_COMMAND_UI(ID_VIEW_REFRESH, &CMainFrame::OnUpdateViewRefresh)
    ON_UPDATE_COMMAND_UI(ID_PRIVILEGES_DEBUG, &CMainFrame::OnUpdatePrivilegesDebug)

    ON_COMMAND(ID_VIEW_REFRESH, &CMainFrame::OnViewRefresh)

    ON_MESSAGE(MSG_UPDATE_AVAILABLE, &CMainFrame::OnMsgAppUpdateAvailable)
    ON_MESSAGE(MSG_UPDATE_HANDLE_TABLE, &CMainFrame::OnMsgUpdateHandleTable)
    ON_MESSAGE(MSG_LOG_MESSAGE, &CMainFrame::OnMsgLogMessage)

    ON_MESSAGE(MSG_WAITING_MUTEX, &CMainFrame::OnMsgWaitingMutex)
    ON_MESSAGE(MSG_ACQUIRED_MUTEX, &CMainFrame::OnMsgAcquiredMutex)
    ON_MESSAGE(MSG_RELEASED_MUTEX, &CMainFrame::OnMsgReleasedMutex)
    ON_MESSAGE(MSG_ABANDONED_MUTEX, &CMainFrame::OnMsgAbandonedMutex)
    ON_MESSAGE(MSG_WAITFAILED, &CMainFrame::OnMsgWaitFailed)
    ON_MESSAGE(MSG_CANCEL_WAITING, &CMainFrame::OnMsgCancelWaiting)
    ON_MESSAGE(MSG_PROCESS_LIST_CHANGED, &CMainFrame::OnMsgProcessListChanged)

    ON_UPDATE_COMMAND_UI(ID_PRIVILEGES_BACKUP, &CMainFrame::OnUpdatePrivilegesBackup)
    ON_UPDATE_COMMAND_UI(ID_PRIVILEGES_ASSIGNPRIMARYTOKEN, &CMainFrame::OnUpdatePrivilegesAssignToken)
    ON_UPDATE_COMMAND_UI(ID_PRIVILEGES_TCB, &CMainFrame::OnUpdatePrivilegesActAsOS)
    ON_UPDATE_COMMAND_UI(ID_PRIVILEGES_SECURITY, &CMainFrame::OnUpdatePrivilegesSecurity)

    ON_COMMAND(ID_PRIVILEGES_DEBUG, &CMainFrame::OnPrivilegesDebug)
    ON_COMMAND(ID_PRIVILEGES_SECURITY, &CMainFrame::OnPrivilegesSecurity)
    ON_COMMAND(ID_PRIVILEGES_BACKUP, &CMainFrame::OnPrivilegesBackup)
    ON_COMMAND(ID_PRIVILEGES_ASSIGNPRIMARYTOKEN, &CMainFrame::OnUpdatePrivilegesAssignToken)
    ON_COMMAND(ID_PRIVILEGES_TCB, &CMainFrame::OnPrivilegesTcb)

    ON_UPDATE_COMMAND_UI(ID_ATTACK_FREEFORM, &CMainFrame::OnUpdateAttackFreeform)
    ON_COMMAND(ID_ATTACK_FREEFORM, &CMainFrame::OnAttackFreeform)

    ON_WM_DESTROY()
END_MESSAGE_MAP()

static UINT indicators[] =
{
    ID_SEPARATOR, // status line indicator
    ID_INDICATOR_MUTANT_TOTAL,
    ID_INDICATOR_MUTANT_NAMED,
    ID_INDICATOR_MUTANT_UNNAMED
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame(){ }

CMainFrame::~CMainFrame(){ }

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    if (!m_wndStatusBar.Create(this) ||
        !m_wndStatusBar.SetIndicators(indicators, _countof(indicators)))
    {
        TRACE0("Failed to create status bar\n");
        return -1; // fail to create
    }

    return 0;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/, CCreateContext* pContext)
{
    BOOL bReturn = FALSE;

    // What we are trying to accomplish:
    //
    // |.....|...........|.........|
    // |.....|.....B.....|.........|
    // |.....|...........|.........|
    // |.....|===========|.........|
    // |.....|...........|.........|
    // |..A..|.....C.....|....E....|
    // |.....|...........|.........|
    // |.....|===========|.........|
    // |.....|...........|.........|
    // |.....|.....D.....|.........|
    // |.....|...........|.........|
    //
    // A: ProcTreeView
    // B: InfoView
    // C: ProcMutexView
    // D: LogView
    // E: SquatView

    CRect r;
    GetClientRect( &r );

    INT a_w = r.Width()/5;
    INT a_h = r.Height();

    INT b_w = r.Width()*1/2;
    INT b_h = r.Height()/6;

    INT c_w = r.Width()*1/2;
    INT c_h = r.Height()*1/2;

    INT d_w = r.Width()*1/2;
    INT d_h = r.Height()*2/5;

    INT e_w = r.Width()*2/5 - 20;
    INT e_h = r.Height();

    do
    {
        // Create the vertical splitter window
        if (!m_VertSplitter.CreateStatic(this, 1, 3,
            WS_VISIBLE | WS_CHILD, AFX_IDW_PANE_FIRST)) {
                break;
        }

        if (!m_VertSplitter.CreateView(0, 0,
            RUNTIME_CLASS(CProcTreeView), CSize(a_w, a_h), pContext)) {
                break;
        }

        if (!m_VertSplitter.CreateView(0, 2,
            RUNTIME_CLASS(CSquatView), CSize(e_w, e_h), pContext)) {
                break;
        }

        // Add the second splitter pane - which is a nested splitter with 3 rows
        if(!m_HorzSplitter.CreateStatic(&m_VertSplitter, 3, 1,
            WS_CHILD | WS_VISIBLE, m_VertSplitter.IdFromRowCol(0, 1))) {
                break;
        }

        if (!m_HorzSplitter.CreateView(0, 0,
            RUNTIME_CLASS(CInfoView), CSize(b_w, b_h), pContext)) {
                break;
        }

        if (!m_HorzSplitter.CreateView(1 , 0,
            RUNTIME_CLASS(CProcMutexView), CSize(c_w, c_h), pContext)) {
                break;
        }

        if (!m_HorzSplitter.CreateView(2, 0,
            RUNTIME_CLASS(CLogView), CSize(d_w, d_h), pContext)) {
                break;
        }

        // We need to set the preferred size; otherwise
        // views A and E will dominate
        m_VertSplitter.SetColumnInfo( 0, a_w, 0 );
        m_VertSplitter.SetColumnInfo( 1, c_w, 0 ); // b, c, or d will do
        m_VertSplitter.SetColumnInfo( 2, e_w, 0 );

        bReturn = TRUE;

    } while(false);

    if( !bReturn )
    {
        if( IsWindow( m_HorzSplitter.GetSafeHwnd() ) ) {
            m_HorzSplitter.DestroyWindow();
        }
        if( IsWindow( m_VertSplitter.GetSafeHwnd() ) ) {
            m_VertSplitter.DestroyWindow();
        }
    }

    return bReturn;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    cs.style &= ~FWS_ADDTOTITLE;

    if( !CFrameWnd::PreCreateWindow(cs) )
        return FALSE;

    return TRUE;
}

void CMainFrame::OnDestroy()
{
    g_shutdown = true;

    CFrameWnd::OnDestroy();
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
    CFrameWnd::AssertValid();
}

CPmvatDoc* CMainFrame::GetDocument()
{
    CDocument* pDoc = GetActiveDocument();
    ASSERT(pDoc);
    if( !pDoc ) { return NULL; }

    ASSERT(pDoc->IsKindOf(RUNTIME_CLASS(CPmvatDoc)));
    return (CPmvatDoc*)pDoc;
}

void CMainFrame::Dump(CDumpContext& dc) const
{
    CFrameWnd::Dump(dc);
}

#endif //_DEBUG

CProcTreeView* CMainFrame::GetProcTreeView()
{
    CWnd* pWnd = m_VertSplitter.GetPane(0, 0);
    CProcTreeView* pView = DYNAMIC_DOWNCAST(CProcTreeView, pWnd);

    ASSERT( NULL != pView );
    return pView;
}

CProcMutexView* CMainFrame::GetProcMutexView()
{
    CWnd* pWnd = m_HorzSplitter.GetPane(1, 0);
    CProcMutexView* pView = DYNAMIC_DOWNCAST(CProcMutexView, pWnd);

    ASSERT( NULL != pView );
    return pView;
}

CLogView* CMainFrame::GetLogView()
{
    CWnd* pWnd = m_HorzSplitter.GetPane(2, 0);
    CLogView* pView = DYNAMIC_DOWNCAST(CLogView, pWnd);

    ASSERT( NULL != pView );
    return pView;
}

CSquatView* CMainFrame::GetSquatView()
{
    CWnd* pWnd = m_VertSplitter.GetPane(0, 2);
    CSquatView* pView = DYNAMIC_DOWNCAST(CSquatView, pWnd);

    ASSERT( NULL != pView );
    return pView;
}

LRESULT CMainFrame::OnMsgLogMessage( WPARAM wparam, LPARAM /*lparam*/ )
{
    const LRESULT PROCESSED = 1;

    CLogView* pView = CLogView::GetView();
    ASSERT( NULL != pView );
    if( NULL == pView ) { return !PROCESSED; }

    pView->OnUpdate( NULL, HINT_LOG_MESSAGE, (CObject*)wparam );

    return PROCESSED;
}

LRESULT CMainFrame::OnMsgProcessListChanged(WPARAM wparam, LPARAM /*lparam*/)
{
    const LRESULT PROCESSED = 1;

    CPmvatDoc* pDoc = GetDocument();
    ASSERT( NULL != pDoc );
    if( NULL == pDoc ) { return !PROCESSED; }

    pDoc->UpdateAllViews( NULL, HINT_PROCESS_LIST_CHANGED, (CObject*)wparam );

    return PROCESSED;
}

LRESULT CMainFrame::OnMsgWaitingMutex(WPARAM wparam, LPARAM /*lparam*/)
{
    const LRESULT PROCESSED = 1;

    CSquatView* pView = CSquatView::GetView();
    ASSERT( NULL != pView );
    if( NULL == pView ) { return !PROCESSED; }

    pView->OnUpdate( NULL, HINT_WAITING_MUTEX, (CObject*)wparam );

    return PROCESSED;
}
LRESULT CMainFrame::OnMsgReleasedMutex(WPARAM wparam, LPARAM /*lparam*/)
{
    const LRESULT PROCESSED = 1;

    CSquatView* pView = CSquatView::GetView();
    ASSERT( NULL != pView );
    if( NULL == pView ) { return !PROCESSED; }

    pView->OnUpdate( NULL, HINT_RELEASED_MUTEX, (CObject*)wparam );

    return PROCESSED;
}

LRESULT CMainFrame::OnMsgCancelWaiting(WPARAM wparam, LPARAM /*lparam*/)
{
    const LRESULT PROCESSED = 1;

    CSquatView* pView = CSquatView::GetView();
    ASSERT( NULL != pView );
    if( NULL == pView ) { return !PROCESSED; }

    pView->OnUpdate( NULL, HINT_CANCEL_WAITING, (CObject*)wparam );

    return PROCESSED;
}

LRESULT CMainFrame::OnMsgWaitFailed(WPARAM wparam, LPARAM /*lparam*/)
{
    const LRESULT PROCESSED = 1;

    CSquatView* pView = CSquatView::GetView();
    ASSERT( NULL != pView );
    if( NULL == pView ) { return !PROCESSED; }

    pView->OnUpdate( NULL, HINT_WAITFAILED, (CObject*)wparam );

    return PROCESSED;
}

LRESULT CMainFrame::OnMsgAbandonedMutex(WPARAM wparam, LPARAM /*lparam*/)
{
    const LRESULT PROCESSED = 1;

    CSquatView* pView = CSquatView::GetView();
    ASSERT( NULL != pView );
    if( NULL == pView ) { return !PROCESSED; }

    pView->OnUpdate( NULL, HINT_ABANDONED_MUTEX, (CObject*)wparam );

    return PROCESSED;
}

LRESULT CMainFrame::OnMsgAcquiredMutex(WPARAM wparam, LPARAM /*lparam*/)
{
    const LRESULT PROCESSED = 1;

    CSquatView* pView = CSquatView::GetView();
    ASSERT( NULL != pView );
    if( NULL == pView ) { return !PROCESSED; }

    pView->OnUpdate( NULL, HINT_ACQUIRED_MUTEX, (CObject*)wparam );

    return PROCESSED;
}

LRESULT CMainFrame::OnMsgUpdateHandleTable(WPARAM wparam, LPARAM /*lparam*/)
{
    const LRESULT PROCESSED = 1;

    CPmvatDoc* pDoc = GetDocument();
    ASSERT( NULL != pDoc );
    if( NULL == pDoc ) { return !PROCESSED; }

    pDoc->UpdateAllViews( NULL, HINT_UPDATE_HANDLE_TABLE, (CObject*)wparam );

    return PROCESSED;
}

LRESULT CMainFrame::OnMsgAppUpdateAvailable(WPARAM /*wparam*/, LPARAM /*lparam*/)
{
    const LRESULT PROCESSED = 1;

    AfxMessageBox(L"An update is available. Please visit www.softwareintegrity.com.");

    return PROCESSED;
}

void CMainFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
    CString product;

    do
    {
        BOOL bResult = FALSE;
        WCHAR module[MAX_PATH];

        bResult = GetModuleFileName( NULL, module, _countof(module) );
        ASSERT( bResult );
        if( !bResult ) { break; }

        CVersionInfo info;
        bResult = info.Load( module );
        ASSERT( bResult );
        if( !bResult ) { break; }

        product = info.GetProductName();

#if defined WIN64
        product += L" (x64)";
#elif defined WIN32
        product += L" (x86)";
#else
        product += L" (unknown)";
#endif

    } while(false);

    SetWindowText(product);

    CFrameWnd::OnUpdateFrameTitle(bAddToTitle);
}
