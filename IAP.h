#if !defined(AFX_IAP_H__666E7EE8_D6FF_47E5_A0DF_13FA2DEC6EB8__INCLUDED_)
#define AFX_IAP_H__666E7EE8_D6FF_47E5_A0DF_13FA2DEC6EB8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IAP.h : header file
//
//wifi-dmx.com
/////////////////////////////////////////////////////////////////////////////
// CIAP dialog

class CIAP : public CDialog
{
// Construction
public:
	CIAP(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CIAP)
	enum { IDD = IDD_DIALOG_IAP };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIAP)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CIAP)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IAP_H__666E7EE8_D6FF_47E5_A0DF_13FA2DEC6EB8__INCLUDED_)
