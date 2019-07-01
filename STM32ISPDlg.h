// STM32ISPDlg.h : header file
//

#if !defined(AFX_STM32ISPDLG_H__828A0BFF_71B9_46A6_9D80_35A5F9F5EAF0__INCLUDED_)
#define AFX_STM32ISPDLG_H__828A0BFF_71B9_46A6_9D80_35A5F9F5EAF0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ISP.h"
#include "IAP.h"

#define RX_BUF_SIZE 1024
#define FILE_SIZE_MAX	1024*1024*10

//用户定义
typedef struct
{
	bool flag;
	int Old_cnt;
	int New_cnt;
	char buffer[RX_BUF_SIZE];
}uart_t;

typedef struct
{
	unsigned char Update_Buffer[FILE_SIZE_MAX];
	int  UpdateLen;
	int  Cont;
}BIN_st;


typedef struct {
	DWORD		dwAddress;
	DWORD		dwDataLength;
	PBYTE		Data;
} IMAGEELEMENT, *PIMAGEELEMENT;


/////////////////////////////////////////////////////////////////////////////
// CSTM32ISPDlg dialog
//wifi-dmx.com
class CSTM32ISPDlg : public CDialog
{
// Construction
public:
	CISP m_myisp;
	CIAP m_myiap;
	int State;
	POINT old;
	CPoint OldTLPoint[3];//左上
	CPoint OldBRPoint[3];//右下
	CSTM32ISPDlg(CWnd* pParent = NULL);	// standard constructor
	void ScanPort();
	int GetSerialPort(CStringArray &arrCom);
	BYTE STBL_Init_BL();

	void ChangeFile(void); 
	void GetFile(void); 
	BOOL LoadBIN(CString pFilePath);
	BOOL LoadHEX(CString pFilePath);
	BOOL FileExist(LPCTSTR filename);
	//int __fastcall Convert(char *sfile, char *dfile, char *dbuf, int *Length);
	void LDisplayError(CString Str) {;}

// Dialog Data
	//{{AFX_DATA(CSTM32ISPDlg)
	enum { IDD = IDD_STM32ISP_DIALOG };
	CTabCtrl	m_tab1;
	CComboBox	m_ctrldtr;
	CEdit	m_ctrlfile;
	CComboBox	m_ctrlport;
	CComboBox	m_ctrlbps;
	CProgressCtrl	m_ctrlprogress;
	CEdit	m_output;
	CString	m_valuefile;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSTM32ISPDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CSTM32ISPDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBUTTONOpenFile();
	afx_msg void OnCheckGo();
	afx_msg void OnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	afx_msg void OnMyInfo(WPARAM pParam,LPARAM IParam);
	afx_msg void OnPcomm(); //这里是我们自定义的消息响应函数
	afx_msg void SendPcomm(); //这里是我们自定义的消息响应函数
	afx_msg void OnStartKeydowm(WPARAM wParam, LPARAM lParam); //这里是我们自定义的消息响应函数
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STM32ISPDLG_H__828A0BFF_71B9_46A6_9D80_35A5F9F5EAF0__INCLUDED_)
