#if !defined(AFX_EDITNUM_H__57F9D004_7592_4F74_8E40_2915D986D5D0__INCLUDED_)
#define AFX_EDITNUM_H__57F9D004_7592_4F74_8E40_2915D986D5D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditNum.h : header file
//
#define WM_EDITNUM_KEYDOWM		WM_USER + 9527

/////////////////////////////////////////////////////////////////////////////
// CEditNum window

class CEditNum : public CEdit
{
// Construction
public:
	CEditNum();

// Attributes
public:
	void SetPos( int nPos );

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditNum)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEditNum();

	// Generated message map functions
protected:
	int m_nMax, m_nMin, m_nPos, m_nPage;
	//{{AFX_MSG(CEditNum)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITNUM_H__57F9D004_7592_4F74_8E40_2915D986D5D0__INCLUDED_)
