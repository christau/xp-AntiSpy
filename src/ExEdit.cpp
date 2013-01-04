#include "stdafx.h"
#include "ExEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExEdit

CExEdit::CExEdit() : m_bkBrush( m_clrBk ), m_pFont( new CFont )
{
	m_nFontSize = 200;
	m_clrText = RGB( 0, 0, 0 );
	m_clrBk = RGB( 152, 228, 231 );
    m_bItalic = FALSE;
	m_bBold = FALSE;
	m_bFocus = FALSE;
	m_bOver = FALSE;
	m_bUnderLine = FALSE;
	m_strFontName = "Arial";

}

CExEdit::~CExEdit()
{
}

BEGIN_MESSAGE_MAP(CExEdit, CEdit)
	//{{AFX_MSG_MAP(CExEdit)
	ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillfocus)
	ON_CONTROL_REFLECT(EN_SETFOCUS, OnSetfocus)
	ON_WM_MOUSEMOVE()
	ON_MESSAGE( WM_MOUSEHOVER, OnMouseHover )
	ON_MESSAGE( WM_MOUSELEAVE, OnMouseLeave )
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExEdit message handlers
void CExEdit::setTextColor( COLORREF clrText )
{
	m_clrText = clrText;
}

void CExEdit::setFontSize( int nSize )
{
	m_nFontSize = nSize;
	setFontHelper();
}
void CExEdit::setFontStyle( BOOL bItalic, BOOL bBold, BOOL bUnderLine )
{
	m_bItalic = bItalic;
	m_bBold = bBold;
	m_bUnderLine = bUnderLine;
	setFontHelper();
}

void CExEdit::setBkColor( COLORREF clrBk )
{
	m_clrBk = clrBk;
	if( m_bkBrush.m_hObject )
		m_bkBrush.DeleteObject();

	m_bkBrush.CreateSolidBrush( m_clrBk );
	//m_bkBrush.CreateSolidBrush( RGB( 0, 255, 255 ) );
}

void CExEdit::setFontName( CString strFontName)
{
	m_strFontName = strFontName;
	setFontHelper();
}

void CExEdit::drawEditFrame()
{
	CRect rcItem;
	CDC* pDC=this->GetDC();
	this->GetClientRect(&rcItem);


	if( !this->IsWindowEnabled() )
	{
		rcItem.InflateRect( 1, 1 );
		pDC->Draw3dRect( rcItem, RGB( 96, 96, 96),RGB( 96, 96, 96));
		rcItem.InflateRect(1,1);
		pDC->Draw3dRect( rcItem, RGB( 128, 128, 128),RGB( 128, 128, 128 ) );
		ReleaseDC( pDC );

		return;
	}

	rcItem.InflateRect( 1, 1 );
	pDC->Draw3dRect(rcItem,RGB(102,102,153),RGB(102,102,153));
	rcItem.InflateRect( 1, 1);
	pDC->Draw3dRect(rcItem,RGB(174,174,202),RGB(174,174,202));


/*	if( m_bOver&& m_bFocus )
	{
		rcItem.InflateRect(1,1);
		pDC->Draw3dRect(rcItem,RGB( 255, 102, 51 ),RGB( 255, 102, 51 ) );
		rcItem.InflateRect(1,1);
		pDC->Draw3dRect(rcItem,RGB( 255, 163, 132 ),RGB( 255, 163, 132 ) );
	}
	if( m_bOver && !m_bFocus )
	{
		rcItem.InflateRect( 1, 1 );
		pDC->Draw3dRect( rcItem,RGB( 0, 176, 88 ),RGB( 0, 176, 88 ) );
		rcItem.InflateRect(1,1);
		pDC->Draw3dRect( rcItem,RGB( 122, 189, 32 ),RGB( 122, 189, 32 ) );
	}
	if( !m_bOver )
	{
		rcItem.InflateRect( 1, 1 );
		pDC->Draw3dRect(rcItem,RGB(102,102,153),RGB(102,102,153));
		rcItem.InflateRect( 1, 1);
		pDC->Draw3dRect(rcItem,RGB(174,174,202),RGB(174,174,202));
	}*/

	ReleaseDC(pDC);
}

void CExEdit::OnKillfocus()
{
	// TODO: Add your control notification handler code here
	m_bFocus = FALSE;
	drawEditFrame();
}

void CExEdit::OnSetfocus()
{
	// TODO: Add your control notification handler code here
	m_bFocus = TRUE;
	drawEditFrame();
}

void CExEdit::setFontHelper()
{
	if( m_pFont->m_hObject )
		m_pFont->DeleteObject();

	LOGFONT  lgFont;

	lgFont.lfCharSet = DEFAULT_CHARSET;
	lgFont.lfClipPrecision = 0;
	lgFont.lfEscapement = 0;
	lstrcpy( lgFont.lfFaceName, m_strFontName );
	lgFont.lfHeight = m_nFontSize;
	lgFont.lfItalic = m_bItalic;
	lgFont.lfOrientation = 0;
	lgFont.lfOutPrecision = 0;
	lgFont.lfPitchAndFamily = 2;
	lgFont.lfQuality = 0;
	lgFont.lfStrikeOut = 0;
	lgFont.lfUnderline = m_bUnderLine;
	lgFont.lfWidth = 0;
	if( m_bBold )
		lgFont.lfWeight = FW_BOLD;
	else
		lgFont.lfWeight = FW_NORMAL;

	m_pFont->CreatePointFontIndirect( &lgFont );
	SetFont( m_pFont );

}

void CExEdit::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	TRACKMOUSEEVENT mouseEvent;

	mouseEvent.cbSize = sizeof( TRACKMOUSEEVENT );
	mouseEvent.dwFlags = TME_HOVER | TME_LEAVE;
	mouseEvent.dwHoverTime = 0;
	mouseEvent.hwndTrack = m_hWnd;

	_TrackMouseEvent( &mouseEvent );

	CEdit::OnMouseMove(nFlags, point);
}

LRESULT CExEdit::OnMouseHover( WPARAM wParam, LPARAM lParam )
{
	m_bOver = TRUE;
	drawEditFrame();

	return 0;
}

LRESULT CExEdit::OnMouseLeave( WPARAM wParam, LPARAM lParam )
{
	m_bOver = FALSE;
	drawEditFrame();

	return 0;
}

HBRUSH CExEdit::CtlColor(CDC* pDC, UINT nCtlColor)
{
	// TODO: Change any attributes of the DC here
	drawEditFrame();


	pDC->SetTextColor( m_clrText );
	pDC->SetBkColor( m_clrBk );
	pDC->SetBkMode( TRANSPARENT );
	// TODO: Return a non-NULL brush if the parent's handler should not be called
	return m_bkBrush;
}

