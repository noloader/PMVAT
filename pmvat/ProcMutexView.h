#pragma once

#include "PmvatApp.h"
#include "PmvatDoc.h"
#include "Common.h"

class CProcMutexView: public CListView
{
    DECLARE_DYNCREATE(CProcMutexView)

    enum Column { Number=0, Dacl, State, Name };

private:
    enum{ MenuNotFound = -1 };

public:
    static CProcMutexView* GetView();

protected:
    CProcMutexView(); // protected constructor used by dynamic creation
    virtual ~CProcMutexView();

protected:
    CPmvatDoc* GetDocument();

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
    virtual void OnInitialUpdate();
    afx_msg void OnEditCopy();
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
    afx_msg void OnUpdateAttackSquat(CCmdUI *pCmdUI);
    afx_msg void OnAttackSquat();
    afx_msg void OnUpdateAttackSquatAll(CCmdUI *pCmdUI);
    afx_msg void OnAttackSquatAll();


    DECLARE_MESSAGE_MAP()

protected:

    BOOL ResetView();
    BOOL DisplayMutexHandles();

    BOOL HandleContextMenu( LPNMITEMACTIVATE pItem );
    BOOL SquatMutants();
    BOOL SquatAllMutants();
    BOOL SquatFreeform();

    INT FindMenuPosition( CMenu* pMenu, LPCWSTR pszText );
    BOOL GetMenuCoordinates(HWND hWnd, const POINT& client, POINT& screen);

private:
    INT m_vsb_width;
};

#ifndef _DEBUG// debug version in ProcMutexView.cpp
inline CPmvatDoc* CProcMutexView::GetDocument()
{ return reinterpret_cast<CPmvatDoc*>(m_pDocument); }
#endif