#if !defined(AFX_ISP_H__340B2206_324D_4DD9_8D57_899958BD920C__INCLUDED_)
#define AFX_ISP_H__340B2206_324D_4DD9_8D57_899958BD920C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ISP.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CISP dialog

class CISP : public CDialog
{
// Construction
public:
	CISP(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CISP)
	enum { IDD = IDD_DIALOG_ISP };
		// NOTE: the ClassWizard will add data members here
	CString	m_wrp0;
	CString	m_wrp1;
	CString	m_wrp2;
	CString	m_wrp3;
	CString	m_data0;
	CString	m_data1;
	CString	m_prd;
	CString	m_user;
	CString	m_savefile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CISP)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CISP)
	afx_msg void OnBUTTONStart();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ISP_H__340B2206_324D_4DD9_8D57_899958BD920C__INCLUDED_)
