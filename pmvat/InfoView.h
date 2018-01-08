#pragma once

#include "PmvatApp.h"
#include "PmvatDoc.h"
#include "Common.h"

class CInfoView: public CEditView
{
    DECLARE_DYNCREATE(CInfoView)

public:
    static CInfoView* GetView();

protected:
    CInfoView();
    virtual ~CInfoView();

protected:
    CPmvatDoc* GetDocument();

#ifdef _DEBUG
    void AssertValid() const;
    void Dump(CDumpContext& dc) const;
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual void OnInitialUpdate();
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);

    afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor); 
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);       
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
    afx_msg void OnEditCopy();
    afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);

    DECLARE_MESSAGE_MAP()

protected:
    BOOL ResetView();    
    BOOL UpdateVersionInformation();
    BOOL DisplayComputerAndUserInformation();
    BOOL DisplayModuleInformation();

    void HandleScrollBar(int cx, int cy);

private:
    ProcessFunctions m_pfuncs;

    INT m_fontHeight;
    CBrush m_whiteBrush;
    CFont m_font;
};

#ifndef _DEBUG// debug version in InfoView.cpp
inline CPmvatDoc* CInfoView::GetDocument()
{ return reinterpret_cast<CPmvatDoc*>(m_pDocument); }
#endif
