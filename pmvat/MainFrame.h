// MainFrame.h : interface of the CMainFrame class
//

#pragma once

#include "Common.h"

class CLogView;
class CPmvatDoc;
class CProcTreeView;
class CProcMutexView;
class CSquatView;

class CMainFrame: public CFrameWnd
{

protected:
    CMainFrame();
    virtual ~CMainFrame();

    DECLARE_DYNCREATE(CMainFrame)

    // Generated message map functions
protected:
    virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);    
    virtual void OnUpdateFrameTitle(BOOL bAddToTitle);

    afx_msg void OnDestroy();
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

    afx_msg LRESULT OnMsgProcessListChanged( WPARAM wparam, LPARAM lparam );
    afx_msg LRESULT OnMsgLogMessage( WPARAM wparam, LPARAM lparam );
    afx_msg LRESULT OnMsgUpdateHandleTable(WPARAM wparam, LPARAM lparam);
    afx_msg LRESULT OnMsgAppUpdateAvailable(WPARAM wparam, LPARAM lparam);
    afx_msg LRESULT OnMsgWaitingMutex(WPARAM wparam, LPARAM lparam);
    afx_msg LRESULT OnMsgReleasedMutex(WPARAM wparam, LPARAM lparam);
    afx_msg LRESULT OnMsgAbandonedMutex(WPARAM wparam, LPARAM lparam);
    afx_msg LRESULT OnMsgAcquiredMutex(WPARAM wparam, LPARAM lparam);
    afx_msg LRESULT OnMsgWaitFailed(WPARAM wparam, LPARAM lparam);
    afx_msg LRESULT OnMsgCancelWaiting(WPARAM wparam, LPARAM lparam);

    afx_msg void OnUpdateTotalMutants(CCmdUI *pCmdUI);
    afx_msg void OnUpdateNamedMutants(CCmdUI *pCmdUI);
    afx_msg void OnUpdateUnnamedMutants(CCmdUI *pCmdUI);

    afx_msg void OnPrivilegesDebug();
    afx_msg void OnUpdatePrivilegesBackup(CCmdUI *pCmdUI);

    afx_msg void OnPrivilegesSecurity();
    afx_msg void OnUpdatePrivilegesDebug(CCmdUI *pCmdUI);

    afx_msg void OnPrivilegesBackup();
    afx_msg void OnUpdatePrivilegesAssignToken(CCmdUI *pCmdUI);

    afx_msg void OnUpdatePrivilegesAssignToken();
    afx_msg void OnUpdatePrivilegesActAsOS(CCmdUI *pCmdUI);

    afx_msg void OnPrivilegesTcb();
    afx_msg void OnUpdatePrivilegesSecurity(CCmdUI *pCmdUI);

    BOOL EnablePrivilegesMenuItem(CCmdUI *pCmdUI);
    void UpdatePrivilegMenuItem();

    afx_msg void OnViewRefresh();
    afx_msg void OnUpdateViewRefresh(CCmdUI *pCmdUI);

    afx_msg void OnAttackFreeform();
    afx_msg void OnUpdateAttackFreeform(CCmdUI *pCmdUI);

    DECLARE_MESSAGE_MAP()

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    CStatusBar m_wndStatusBar;
    CSplitterWnd m_VertSplitter;
    CSplitterWnd m_HorzSplitter;

public:
    CPmvatDoc* GetDocument();

    CLogView* GetLogView();
    CProcTreeView* GetProcTreeView();
    CProcMutexView* GetProcMutexView();
    CSquatView* GetSquatView();
};

#ifndef _DEBUG// debug version in ProcTreeView.cpp
inline CPmvatDoc* CMainFrame::GetDocument()
{ return reinterpret_cast<CPmvatDoc*>(GetActiveDocument()); }
#endif