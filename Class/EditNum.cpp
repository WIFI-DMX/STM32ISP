// EditNum.cpp : implementation file
//

#include "stdafx.h"
//#include "MyIAP.h"
#include "EditNum.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditNum

CEditNum::CEditNum()
{
	m_nPos = m_nMin = 0;
	m_nMax = 255;
	m_nPage = 20;
}

CEditNum::~CEditNum()
{
}


BEGIN_MESSAGE_MAP(CEditNum, CEdit)
	//{{AFX_MSG_MAP(CEditNum)
	ON_WM_KEYDOWN()
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CEditNum message handlers


void CEditNum::SetPos(int nPos)
{
	m_nPos = nPos;

	// Boundary check
	if( m_nPos >= m_nMax )
		m_nPos = m_nMax;
	if( m_nPos <= m_nMin )
		m_nPos = m_nMin;

	Invalidate();
}

void CEditNum::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	switch( nChar ) {

	// Left & up
	case VK_LEFT :
	case VK_UP :

		SetPos( m_nPos-1 );
		break;

	// Right & down
	case VK_RIGHT :
	case VK_DOWN :

		SetPos( m_nPos+1 );
		break;

	// Home
	case VK_HOME :

		SetPos( m_nMin );
		break;

	// End
	case VK_END :

		SetPos( m_nMax );
		break;

	// Page up
	case VK_PRIOR :

		SetPos( m_nPos - m_nPage );
		break;

	// Page down
	case VK_NEXT :

		SetPos( m_nPos + m_nPage );
		break;
	case VK_NUMPAD0:
	case VK_NUMPAD1:
	case VK_NUMPAD2:
	case VK_NUMPAD3:
	case VK_NUMPAD4:
	case VK_NUMPAD5:
	case VK_NUMPAD6:
	case VK_NUMPAD7:
	case VK_NUMPAD8:
	case VK_NUMPAD9:
		break;

	default :

		CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
		return;
	}	
	::PostMessage(
		GetParent()->GetSafeHwnd(),	WM_EDITNUM_KEYDOWM,
		GetDlgCtrlID(), nChar );	
	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CEditNum::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// TODO: Add your message handler code here and/or call default
	
	return CEdit::OnMouseWheel(nFlags, zDelta, pt);
}
