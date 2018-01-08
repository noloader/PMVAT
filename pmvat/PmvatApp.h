// Pmvat.h : main header file for the Pmvat application
//
#pragma once

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h" // main symbols


// Lots of interesing stuff happens when we are shutting down
// Especially when we are still trying to do things such as log messages
extern volatile bool g_shutdown;

class CPmvatApp: public CWinApp
{
public:
    CPmvatApp();
    DECLARE_MESSAGE_MAP()

protected:
    virtual BOOL InitInstance();        
    virtual BOOL InitApplication();
    virtual BOOL OnIdle(LONG lCount); // return TRUE if more idle processing

    afx_msg void OnAppAbout();
    afx_msg void OnAppLicense();

    BOOL HandleEulaPreamble() const;

#ifndef WIN64
    void HandleWow64() const;
#endif
};

extern CPmvatApp theApp;