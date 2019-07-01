// STM32ISP.cpp : Defines the class behaviors for the application.
//
//wifi-dmx.com
#include "stdafx.h"
#include "STM32ISP.h"
#include "STM32ISPDlg.h"
#include "Class\ClassXP.h"
#include <Windows.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSTM32ISPApp

BEGIN_MESSAGE_MAP(CSTM32ISPApp, CWinApp)
	//{{AFX_MSG_MAP(CSTM32ISPApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSTM32ISPApp construction

CSTM32ISPApp::CSTM32ISPApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSTM32ISPApp object

CSTM32ISPApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSTM32ISPApp initialization

BOOL CSTM32ISPApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	SetDialogBkColor(RGB(251,252,251), RGB(0, 64, 255));

	CSTM32ISPDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
//wifi-dmx.com