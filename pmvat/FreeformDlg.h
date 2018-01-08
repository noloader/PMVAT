#pragma once

#include "Common.h"
#include "afxwin.h"

class CFreeformDlg : public CDialog
{
	DECLARE_DYNAMIC(CFreeformDlg)

public:
	CFreeformDlg( CWnd* pParent = NULL );   // standard constructor
	virtual ~CFreeformDlg();

    CString GetMutexName() const { return m_szName; }

// Dialog Data
	enum { IDD = IDD_FREEFORMDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support    
    virtual BOOL OnInitDialog();

    afx_msg void OnBnClickedOk();

	DECLARE_MESSAGE_MAP()

private:
    CString m_szName;

    CStatic m_label;
    CEdit m_edit;
};
