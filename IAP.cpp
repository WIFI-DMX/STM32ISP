// IAP.cpp : implementation file
//
//wifi-dmx.com
#include "stdafx.h"
#include "STM32ISP.h"
#include "IAP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIAP dialog


CIAP::CIAP(CWnd* pParent /*=NULL*/)
	: CDialog(CIAP::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIAP)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CIAP::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIAP)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIAP, CDialog)
	//{{AFX_MSG_MAP(CIAP)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIAP message handlers
//wifi-dmx.com