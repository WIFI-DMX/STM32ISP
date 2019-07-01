#if !defined(AFX_ColorStatic_H__4CCD6BDB_9694_47EC_A643_42A2CEBF62DF__INCLUDED_)
#define AFX_ColorStatic_H__4CCD6BDB_9694_47EC_A643_42A2CEBF62DF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class CColorStatic : public CStatic   //可设置背景颜色的静态框
{

public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorStatic)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

public:
	void SetText(LPCTSTR lpszText);

protected:
	CFont m_font;
	CRect m_rc;
	CString m_strText;
	
	//{{AFX_MSG(CColorStatic)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ColorStatic_H__4CCD6BDB_9694_47EC_A643_42A2CEBF62DF__INCLUDED_)
