// MainFrame.Menucpp : implementation of the CMainFrame menu items
//

#include "stdafx.h"

#include "PmvatApp.h"
#include "PmvatDoc.h"
#include "StrSafe.h"
#include "LogView.h"
#include "InfoView.h"
#include "MainFrame.h"
#include "SquatView.h"
#include "FreeformDlg.h"
#include "ProcTreeView.h"
#include "ProcMutexView.h"
#include "UserPrivilege.h"

#ifdef _DEBUG
# define new DEBUG_NEW
#endif

#ifdef NDEBUG
# ifdef OutputDebugString
# undef OutputDebugString
# define OutputDebugString(x) (void)0;
# endif
#endif

void CMainFrame::OnUpdateViewRefresh(CCmdUI *pCmdUI)
{
    if( !pCmdUI ) { return; }
    pCmdUI->Enable();
}

void CMainFrame::OnUpdateTotalMutants(CCmdUI *pCmdUI)
{
    if( !pCmdUI ) { return; }
    pCmdUI->Enable();

    CPmvatDoc* pDoc = GetDocument();
    ASSERT( NULL != pDoc );
    if( NULL == pDoc ) { return; }

    const DWORD CCH = 64;
    WCHAR wsz[CCH];

    StringCchPrintf( wsz, CCH, L"Total: %d", pDoc->m_total );
    pCmdUI->SetText( wsz );
}

void CMainFrame::OnUpdateNamedMutants(CCmdUI *pCmdUI)
{
    if( !pCmdUI ) { return; }
    pCmdUI->Enable();

    CPmvatDoc* pDoc = GetDocument();
    ASSERT( NULL != pDoc );
    if( NULL == pDoc ) { return; }

    const DWORD CCH = 64;
    WCHAR wsz[CCH];

    StringCchPrintf( wsz, CCH, L"Named: %d", pDoc->m_named );
    pCmdUI->SetText( wsz );
}

void CMainFrame::OnUpdateUnnamedMutants(CCmdUI *pCmdUI)
{
    if( !pCmdUI ) { return; }
    pCmdUI->Enable();

    CPmvatDoc* pDoc = GetDocument();
    ASSERT( NULL != pDoc );
    if( NULL == pDoc ) { return; }

    const DWORD CCH = 64;
    WCHAR wsz[CCH];

    StringCchPrintf( wsz, CCH, L"Unnamed: %d", pDoc->m_unnamed );
    pCmdUI->SetText( wsz );
}

void CMainFrame::OnViewRefresh()
{
    CPmvatDoc* pDoc = GetDocument();
    ASSERT( NULL != pDoc );
    if( NULL == pDoc ) { return; }

    pDoc->UpdateAllViews( NULL, HINT_REFRESH_PROCESSES, 0 );
}

void CMainFrame::OnUpdatePrivilegesDebug(CCmdUI *pCmdUI)
{
    if( !pCmdUI ) { return; }
    if( !EnablePrivilegesMenuItem(pCmdUI) ) { return; }

    if( CUserPrivilege::Enabled == CUserPrivilege::HasDebugPrivilege() )
    {
        pCmdUI->SetCheck(TRUE);
    }
    else
    {
        pCmdUI->SetCheck(FALSE);
    }
}

void CMainFrame::OnUpdatePrivilegesBackup(CCmdUI *pCmdUI)
{
    if( !pCmdUI ) { return; }
    if( !EnablePrivilegesMenuItem(pCmdUI) ) { return; }

    if( CUserPrivilege::Enabled == CUserPrivilege::HasBackupPrivilege() )
    {
        pCmdUI->SetCheck(TRUE);
    }
    else
    {
        pCmdUI->SetCheck(FALSE);
    }
}

void CMainFrame::OnUpdatePrivilegesAssignToken(CCmdUI *pCmdUI)
{
    if( !pCmdUI ) { return; }
    if( !EnablePrivilegesMenuItem(pCmdUI) ) { return; }

    if( CUserPrivilege::Enabled == CUserPrivilege::HasAssignPrimaryTokenPrivilege() )
    {
        pCmdUI->SetCheck(TRUE);
    }
    else
    {
        pCmdUI->SetCheck(FALSE);
    }
}

void CMainFrame::OnUpdatePrivilegesActAsOS(CCmdUI *pCmdUI)
{
    if( !pCmdUI ) { return; }
    if( !EnablePrivilegesMenuItem(pCmdUI) ) { return; }

    if( CUserPrivilege::Enabled == CUserPrivilege::HasTcbPrivilege() )
    {
        pCmdUI->SetCheck(TRUE);
    }
    else
    {
        pCmdUI->SetCheck(FALSE);
    }
}

void CMainFrame::OnUpdatePrivilegesSecurity(CCmdUI *pCmdUI)
{
    if( !pCmdUI ) { return; }
    if( !EnablePrivilegesMenuItem(pCmdUI) ) { return; }

    if( CUserPrivilege::Enabled == CUserPrivilege::HasSecurityPrivilege() )
    {
        pCmdUI->SetCheck(TRUE);
    }
    else
    {
        pCmdUI->SetCheck(FALSE);
    }
}

void CMainFrame::OnPrivilegesDebug()
{
    if( CUserPrivilege::Enabled == CUserPrivilege::HasDebugPrivilege() )
    {
        if( CUserPrivilege::DisableDebugPrivilege() ) {
            CLogView::WriteLine( L"Disabled debug privilege" );
        }
    }
    else
    {
        if( CUserPrivilege::EnableDebugPrivilege() ) {
            CLogView::WriteLine( L"Enabled debug privilege" );
        }
    }

    UpdatePrivilegMenuItem();
}

void CMainFrame::OnPrivilegesSecurity()
{
    if( CUserPrivilege::Enabled == CUserPrivilege::HasSecurityPrivilege() )
    {
        if( CUserPrivilege::DisableSecurityPrivilege() ) {
            CLogView::WriteLine( L"Disabled security privilege" );
        }
    }
    else
    {
        if( CUserPrivilege::EnableSecurityPrivilege() ) {
            CLogView::WriteLine( L"Enabled security privilege" );
        }
    }

    UpdatePrivilegMenuItem();
}

void CMainFrame::OnPrivilegesBackup()
{
    if( CUserPrivilege::Enabled == CUserPrivilege::HasBackupPrivilege() )
    {
        if( CUserPrivilege::DisableBackupPrivilege() ) {
            CLogView::WriteLine( L"Disabled backup privilege" );
        }
    }
    else
    {
        if( CUserPrivilege::EnableBackupPrivilege() ) {
            CLogView::WriteLine( L"Enabled backup privilege" );
        }
    }

    UpdatePrivilegMenuItem();
}

void CMainFrame::OnUpdatePrivilegesAssignToken()
{
    if( CUserPrivilege::Enabled == CUserPrivilege::HasAssignPrimaryTokenPrivilege() )
    {
        if( CUserPrivilege::DisableAssignPrimaryTokenPrivilege() ) {
            CLogView::WriteLine( L"Disabled assign token privilege" );
        }
    }
    else
    {
        if( CUserPrivilege::EnableAssignPrimaryTokenPrivilege() ) {
            CLogView::WriteLine( L"Enabled assign token privilege" );
        }
    }

    UpdatePrivilegMenuItem();
}

void CMainFrame::OnPrivilegesTcb()
{
    if( CUserPrivilege::Enabled == CUserPrivilege::HasTcbPrivilege() )
    {
        if( CUserPrivilege::DisableTcbPrivilege() ) {
            CLogView::WriteLine( L"Disabled TCB privilege" );
        }
    }
    else
    {
        if( CUserPrivilege::EnableTcbPrivilege() ) {
            CLogView::WriteLine( L"Enabled TCB privilege" );
        }
    }

    UpdatePrivilegMenuItem();
}

BOOL CMainFrame::EnablePrivilegesMenuItem(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(TRUE);

    return TRUE;
}

void CMainFrame::UpdatePrivilegMenuItem()
{
    CPmvatDoc* pDoc = GetDocument();
    ASSERT( NULL != pDoc );
    if( NULL == pDoc ) { return; }

    pDoc->PrivilegeChanged( );
    pDoc->UpdateAllViews( NULL, HINT_PRIVILEGE_CHANGED, NULL );
}

void CMainFrame::OnUpdateAttackFreeform(CCmdUI *pCmdUI)
{
    ASSERT( NULL != pCmdUI );
    if( NULL == pCmdUI ) { return; }

    pCmdUI->Enable(TRUE);
}

void CMainFrame::OnAttackFreeform()
{
    CSquatView* pView = CSquatView::GetView();
    ASSERT( NULL != pView );
    if( NULL == pView ) { return; }

    CFreeformDlg dlg;
    if( IDOK != dlg.DoModal() ) { return; }
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
        return;
    }

    SquatParams* params = new SquatParams();
    ASSERT( NULL != params );
    if( NULL == params ) { return; }

    params->m_target.Attach( h );
    params->m_name = pszName;

    pView->AddAttackThread( params );
}
