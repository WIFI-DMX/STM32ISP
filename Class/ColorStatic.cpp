
#include "stdafx.h"
#include "..\resource.h"
#include "ColorStatic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CColorStatic, CStatic)
	//{{AFX_MSG_MAP(CColorStatic)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CColorStatic::OnPaint() 
{
	CPaintDC dc(this); 

	dc.FillSolidRect(m_rc,RGB(123,156,235)); //绘制背景

	dc.SelectObject(&m_font);	 		
	dc.SetBkMode(TRANSPARENT); //设置背景透明
	dc.SetTextColor(RGB(255,255,255)); //设置文字颜色为白色
	dc.DrawText(m_strText.GetBuffer(0),m_strText.GetLength(),&m_rc,DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_PATH_ELLIPSIS);
}



void CColorStatic::SetText(LPCTSTR lpszText)
{
	m_strText = lpszText;
	Invalidate();  
}


/********************************************************************/
/*																	*/
/* Function name : PreSubclassWindow								*/
/* Description   : Initialize control variables						*/
/*																	*/
/********************************************************************/
void CColorStatic::PreSubclassWindow() 
{
	GetClientRect(m_rc);

	m_font.CreateFont(16, 0,0,0,FW_BOLD, 0,0,0,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "宋体");
	
	CStatic::PreSubclassWindow();		
}
