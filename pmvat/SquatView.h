#pragma once

#include "PmvatApp.h"
#include "PmvatDoc.h"
#include "Common.h"

// CSquatView form view

class CSquatView: public CListView
{
    DECLARE_DYNCREATE(CSquatView)

    // CMainFrame needs access to OnUpdate
    friend CMainFrame;
    // CProcMutexView needs access to AddAttackThread
    friend CProcMutexView;

public:
    static CSquatView* GetView();
    BOOL FreeformAttack();

protected:
    CSquatView(); // protected constructor used by dynamic creation
    virtual ~CSquatView();

    enum{ MenuNotFound = -1 };

protected:
    CPmvatDoc* GetDocument();

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
    virtual void OnInitialUpdate();

    afx_msg void OnDestroy();
    afx_msg void OnClose();

    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);

    afx_msg void OnAttackCancel();
    afx_msg void OnAttackRelease();
    afx_msg void OnAttackAbandon();
    afx_msg void OnAttackCancelAll();
    afx_msg void OnAttackReleaseAll();
    afx_msg void OnAttackAbandonAll();
    afx_msg void OnAttackTimedRelease();
    afx_msg void OnAttackTimedAbandon();
    afx_msg void OnAttackClearComplete();

    afx_msg void OnUpdateAttackCancel(CCmdUI *pCmdUI);
    afx_msg void OnUpdateAttackRelease(CCmdUI *pCmdUI);
    afx_msg void OnUpdateAttackAbandon(CCmdUI *pCmdUI);
    afx_msg void OnUpdateAttackCancelAll(CCmdUI *pCmdUI);
    afx_msg void OnUpdateAttackReleaseAll(CCmdUI *pCmdUI);
    afx_msg void OnUpdateAttackAbandonAll(CCmdUI *pCmdUI);
    afx_msg void OnUpdateAttackTimedRelease(CCmdUI *pCmdUI);
    afx_msg void OnUpdateAttackTimedAbandon(CCmdUI *pCmdUI);
    afx_msg void OnUpdateAttackClearComplete(CCmdUI *pCmdUI);

    DECLARE_MESSAGE_MAP()

protected:
    INT FindMenuPosition( CMenu* pMenu, LPCWSTR pszText );

    BOOL AttackCancel();
    BOOL AttackCancelAll();
    BOOL AttackCancel(INT pos);

    BOOL AttackRelease();
    BOOL AttackTimedRelease();
    BOOL AttackReleaseAll();
    BOOL AttackRelease(INT pos);

    BOOL AttackAbandon();
    BOOL AttackTimedAbandon();
    BOOL AttackAbandonAll();
    BOOL AttackAbandon(INT pos);

    BOOL AttackClearComplete();

    BOOL ResetView();
    BOOL UpdateSquatState();
    BOOL FreeItemData(INT pos);
    BOOL DeleteAllItems();    

    void AddAttackThread( SquatParams* params );

private:
    INT m_vsb_width;
    CCriticalSection m_lock;
};

// The attack thread
UINT CDECL AttackThreadProc( LPVOID lpParams );

BOOL SendLogMessage( LPCWSTR );
VOID FormatLogMessage( LPCWSTR pwszAction, WCHAR* psz, DWORD cch, const SquatParams& params );
VOID FormatAcquiredMessage( WCHAR* psz, DWORD cch, const SquatParams& params );
VOID FormatCanceledMessage( WCHAR* psz, DWORD cch, const SquatParams& params );
VOID FormatReleasedMessage( WCHAR* psz, DWORD cch, const SquatParams& params );
VOID FormatAbandonedMessage( WCHAR* psz, DWORD cch, const SquatParams& params );
VOID FormatFailedWaitMessage( WCHAR* psz, DWORD cch, const SquatParams& params, DWORD dwError );

// Returns WAIT_OBJ_0, WAIT_TIMEOUT, or GetLastError()
inline DWORD TestMutex(HANDLE h);

#ifndef _DEBUG// debug version in ProcTreeView.cpp
inline CPmvatDoc* CSquatView::GetDocument()
{ return reinterpret_cast<CPmvatDoc*>(m_pDocument); }
#endif
