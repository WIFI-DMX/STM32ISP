// STM32ISP.h : main header file for the STM32ISP application
//

#if !defined(AFX_STM32ISP_H__70DBC4BE_658D_4077_8182_0490C71F63B8__INCLUDED_)
#define AFX_STM32ISP_H__70DBC4BE_658D_4077_8182_0490C71F63B8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WM_PCOMM         WM_USER+500 //自定义消息
#define WM_PCOMMSEND     WM_USER+1000 //自定义消息
#define WM_INFO          WM_USER+1500
#define WM_START_KEYDOWM WM_USER+9527

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
//wifi-dmx.com
/////////////////////////////////////////////////////////////////////////////
// CSTM32ISPApp:
// See STM32ISP.cpp for the implementation of this class
//

class CSTM32ISPApp : public CWinApp
{
public:
	CSTM32ISPApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSTM32ISPApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSTM32ISPApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STM32ISP_H__70DBC4BE_658D_4077_8182_0490C71F63B8__INCLUDED_)
