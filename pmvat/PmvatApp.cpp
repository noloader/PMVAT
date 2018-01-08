// Pmvat.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"

#include "PmvatApp.h"
#include "EulaDlg.h"
#include "PmvatDoc.h"
#include "AboutDlg.h"
#include "MainFrame.h"
#include "UpdateCheck.h"
#include "ProcTreeView.h"

#ifdef _DEBUG
# define new DEBUG_NEW
#endif

#ifdef NDEBUG
# ifdef OutputDebugString
# undef OutputDebugString
# define OutputDebugString(x) (void)0;
# endif
#endif

BEGIN_MESSAGE_MAP(CPmvatApp, CWinApp)
    ON_COMMAND(ID_APP_ABOUT, &CPmvatApp::OnAppAbout)
    ON_COMMAND(ID_APP_LICENSE, &CPmvatApp::OnAppLicense)
END_MESSAGE_MAP()

// Lots of interesing stuff happens when we are shutting down
// Especially when we are still trying to do things such as log messages
volatile bool g_shutdown = false;
volatile bool g_bEulaAccepted = false;

CPmvatApp::CPmvatApp()
{
    g_shutdown = false;
}

// The one and only CPmvatApp object

CPmvatApp theApp;

// CPmvatApp initialization

BOOL CPmvatApp::InitInstance()
{
    // InitCommonControlsEx() is required on Windows XP if an application
    // manifest specifies use of ComCtl32.dll version 6 or later to enable
    // visual styles. Otherwise, any window creation will fail.
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    // Set this to include all the common control classes you want to use
    // in your application.
    InitCtrls.dwICC = ICC_WIN95_CLASSES | ICC_LISTVIEW_CLASSES | ICC_TREEVIEW_CLASSES ;
    InitCommonControlsEx(&InitCtrls);

    AfxInitRichEdit2();

    CWinApp::InitInstance();

    // Initialize OLE libraries
    //if (!AfxOleInit())
    //{
    //    AfxMessageBox(IDP_OLE_INIT_FAILED);
    //    return FALSE;
    //}
    //AfxEnableControlContainer();

    // Standard initialization
    // If you are not using these features and wish to reduce the size
    // of your final executable, you should remove from the following
    // the specific initialization routines you do not need
    // Change the registry key under which our settings are stored
    // TODO: You should modify this string to be something appropriate
    // such as the name of your company or organization
    SetRegistryKey(L"Software Integrity");

    // Does nothing if the EULA was previously accepted. If not
    // previously accepted, the function will display the EULA
    // and offer Accept/Decline. If Declined, the function
    // returns FALSE and we bail.
    if( FALSE == HandleEulaPreamble() ) { return FALSE; }

#ifndef WIN64
    HandleWow64();
#endif

    // Load standard INI file options (including MRU)
    LoadStdProfileSettings(0);

    // Register the application's document templates. Document templates
    // serve as the connection between documents, frame windows and views
    CSingleDocTemplate* pDocTemplate = NULL;
    pDocTemplate = new CSingleDocTemplate(
        IDR_MAINFRAME,
        RUNTIME_CLASS(CPmvatDoc),
        RUNTIME_CLASS(CMainFrame), // main SDI frame window
        RUNTIME_CLASS(CProcTreeView));

    ASSERT( NULL != pDocTemplate );
    if (!pDocTemplate) { return FALSE; }

    AddDocTemplate(pDocTemplate);

    // Parse command line for standard shell commands, DDE, file open
    CCommandLineInfo cmdInfo;
    ParseCommandLine(cmdInfo);

    // Dispatch commands specified on the command line. Will return FALSE if
    // app was launched with /RegServer, /Register, /Unregserver or /Unregister.
    if (!ProcessShellCommand(cmdInfo)) { return FALSE; }

    // The one and only window has been initialized, so show and update it
    m_pMainWnd->ShowWindow(SW_SHOW);
    m_pMainWnd->UpdateWindow();

    // call DragAcceptFiles only if there's a suffix
    // In an SDI app, this should occur after ProcessShellCommand
    return TRUE;
}

BOOL CPmvatApp::InitApplication()
{
    // TODO: Add your specialized code here and/or call the base class

    if( !CWinApp::InitApplication() )
        return FALSE;

    return TRUE;
}

// http://msdn.microsoft.com/en-us/library/3e077sxt(VS.80).aspx
BOOL CPmvatApp::OnIdle(LONG lCount)
{
    static bool launched = false;

    CWinApp::OnIdle(lCount);

    if( !launched && g_bEulaAccepted )
    {
        LaunchUpdateThread();
        launched = true;
    }

    // Nonzero to receive more idle processing time;
    //  0 if no more idle time is needed.
    return (INT)(launched != true);
}

#ifndef WIN64
void CPmvatApp::HandleWow64() const
{
    Wow64ProcFunctions funcs;
    ASSERT( NULL != funcs );

    if( NULL != funcs.pfnIsWow64Process )
    {
        BOOL bIsWow64 = FALSE;
        if( funcs.pfnIsWow64Process( GetCurrentProcess(), &bIsWow64 ) )
        {
            if( bIsWow64 )
            {
                AfxMessageBox( L"The x86 version of this program " \
                    L"does not produce correct results when run under Wow64. " \
                    L"An x64 version is available for download.", MB_ICONWARNING );
            }
        }
    }
}
#endif

BOOL CPmvatApp::HandleEulaPreamble() const
{
    BOOL bResult = FALSE;

    do
    {
        CWinApp* pApplication = AfxGetApp();
        ASSERT( NULL != pApplication );
        if( NULL == pApplication ) { break; }

        INT eula = 0;
        eula = pApplication->GetProfileInt( L"Settings", L"EULA", 0 );

        // Already accepted
        if( 1 == eula ) { bResult = TRUE; break; }

        CEulaDlg dlg;
        if( IDOK == dlg.DoModal() )
        {
            bResult = pApplication->WriteProfileInt( L"Settings", L"EULA", 1 );
        }
        else
        {
            pApplication->WriteProfileInt( L"Settings", L"EULA", 0 );
        }

    } while(false);

    g_bEulaAccepted = !!bResult;

    return bResult;
}

void CPmvatApp::OnAppAbout()
{
    CAboutDlg dlg;
    dlg.DoModal();
}

void CPmvatApp::OnAppLicense()
{
    CEulaDlg dlg;
    if( IDCANCEL == dlg.DoModal() )
    {
        AfxGetApp()->WriteProfileInt( L"Settings", L"EULA", 0 );
        AfxGetMainWnd()->PostMessage(WM_CLOSE, 0, 0);
    }
}
