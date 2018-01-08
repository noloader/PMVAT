// CFreeformDlg.cpp : implementation file
//

#include "stdafx.h"

#include "PmvatApp.h"
#include "Common.h"
#include "FreeformDlg.h"

IMPLEMENT_DYNAMIC(CFreeformDlg, CDialog)

CFreeformDlg::CFreeformDlg(CWnd* pParent /*=NULL*/)
: CDialog(CFreeformDlg::IDD, pParent) { }

CFreeformDlg::~CFreeformDlg() { }

void CFreeformDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LABEL_INSTRUCTIONS, m_label);
    DDX_Control(pDX, IDC_EDIT_FREEFORM_NAME, m_edit);
}

BEGIN_MESSAGE_MAP(CFreeformDlg, CDialog)
    ON_BN_CLICKED(IDOK, &CFreeformDlg::OnBnClickedOk)
END_MESSAGE_MAP()

void CFreeformDlg::OnBnClickedOk()
{
    m_edit.GetWindowText( m_szName );

    OnOK();
}

BOOL CFreeformDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    static LPCWSTR pszText = L"Enter the name of a mutext below. If the mutex does not exist, " \
        L"the program will create the mutex. If the mutex does exist, the program will open the " \
        L"existing mutex (if permitted to do so)." \
        L"\r\n\r\n" \
        L"If a Copy/Paste is being performed from a program such as Process Explorer, " \
        L"note that the namespace prefix (for example,  \\Sessions\\1\\BaseNamedObjects) " \
        L"might cause the program to fail to open the mutex. " \
        L"In this case, the error is 161 (\'The specified path is invalid\').";

    m_label.SetWindowText( pszText );

    return TRUE;
}
