// ISP.cpp : implementation file
//
//wifi-dmx.com
#include "stdafx.h"
#include "STM32ISP.h"
#include "ISP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CISP dialog


CISP::CISP(CWnd* pParent /*=NULL*/)
	: CDialog(CISP::IDD, pParent)
{
	//{{AFX_DATA_INIT(CISP)
		// NOTE: the ClassWizard will add member initialization here
	m_wrp0 = _T("00");
	m_wrp1 = _T("00");
	m_wrp2 = _T("00");
	m_wrp3 = _T("00");
	m_data0 = _T("FF");
	m_data1 = _T("FF");
	m_prd = _T("A5");
	m_user = _T("FF");
	m_savefile = _T("");
	//}}AFX_DATA_INIT
}


void CISP::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CISP)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Text(pDX, IDC_EDIT_WRP0, m_wrp0);
	DDV_MaxChars(pDX, m_wrp0, 2);
	DDX_Text(pDX, IDC_EDIT_WRP1, m_wrp1);
	DDX_Text(pDX, IDC_EDIT_WRP2, m_wrp2);
	DDX_Text(pDX, IDC_EDIT_WRP3, m_wrp3);
	DDX_Text(pDX, IDC_EDIT_Data0, m_data0);
	DDX_Text(pDX, IDC_EDIT_Data1, m_data1);
	DDX_Text(pDX, IDC_EDIT_RPD, m_prd);
	DDX_Text(pDX, IDC_EDIT_User, m_user);
	DDX_Text(pDX, IDC_EDIT_SAVE, m_savefile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CISP, CDialog)
	//{{AFX_MSG_MAP(CISP)
	ON_BN_CLICKED(IDC_BUTTON_Start, OnBUTTONStart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CISP message handlers

void CISP::OnBUTTONStart() 
{
	// TODO: Add your control notification handler code here
	::PostMessage(/*GetParent()->GetSafeHwnd()*/::AfxGetMainWnd()->m_hWnd,	WM_START_KEYDOWM,0,0);
}
//wifi-dmx.com