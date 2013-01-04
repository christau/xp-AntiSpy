/*
	Copyright (C) 2001 by Christian Taubenheim <mail at xp-antispy.org>
	This file is part of xp-AntiSpy http://www.xp-AntiSpy.org.

    xp-AntiSpy is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    xp-AntiSpy is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with xp-AntiSpy. If not, see <http://www.gnu.org/licenses/
*/

#include "stdafx.h"
#include "resource.h"
#include "HoverButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHoverButton

CHoverButton::CHoverButton()
{
    m_ButtonState           = BUTTON_OFF;
    m_bMouseTracking        = FALSE;
	m_ButtonPressed			= false;
	m_hasBorder = false;
	m_useBitmap = false;
	m_bmpHover = NULL;
}

CHoverButton::~CHoverButton()
{
}


BEGIN_MESSAGE_MAP(CHoverButton, CButton)
	//{{AFX_MSG_MAP(CHoverButton)
        ON_WM_MOUSEMOVE()
	ON_WM_DRAWITEM()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseEnter)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CHoverButton 
void CHoverButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bMouseTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE|TME_HOVER;
		tme.dwHoverTime = 1;
		m_bMouseTracking = _TrackMouseEvent(&tme);
	}
	CButton::OnMouseMove(nFlags, point);
}

LRESULT CHoverButton::OnMouseEnter(WPARAM wparam, LPARAM lparam)
{
    m_ButtonState = BUTTON_OVER;
	if(wparam == 0) 
	{
		m_ButtonPressed = false;
	}
    Invalidate();
	return 0;
}

LRESULT CHoverButton::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
    m_ButtonState = BUTTON_OFF;
    m_bMouseTracking = FALSE;
    Invalidate();
	return 0;
}

void CHoverButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CRect rect;
	GetClientRect(&rect);
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	if(m_useBitmap)
	{
		if(m_ButtonState == BUTTON_OVER)
		{
			CBitmap* pBMP = CBitmap::FromHandle(m_bmpHover);
			CDC memDC;
			memDC.CreateCompatibleDC(pDC);
			CBitmap* pOldBmp = memDC.SelectObject(pBMP);
			pDC->BitBlt(0,0,rect.Width(),rect.Height(),&memDC,0,0,SRCCOPY);
			memDC.SelectObject(pOldBmp);
		}
		else
		{
			CBitmap* pBMP = CBitmap::FromHandle(GetBitmap());
			CDC memDC;
			memDC.CreateCompatibleDC(pDC);
			CBitmap* pOldBmp = memDC.SelectObject(pBMP);
			pDC->BitBlt(0,0,rect.Width(),rect.Height(),&memDC,0,0,SRCCOPY);
			memDC.SelectObject(pOldBmp);
		}
	}
	else
	{


	//	const int oldROP = pDC->GetROP2();
	//	pDC->SetROP2(SRCCOPY);
		CPen border;
			CBrush fill;
		if(m_ButtonState == BUTTON_OFF)
		{
			if(!m_hasBorder)
				pDC->FillSolidRect(rect,0x00D1D8DB);
			else
			{
				
				border.CreatePen(PS_SOLID, 1, 0x00444444);
				fill.CreateSolidBrush(0x00D1D8DB);
				CPen* pOldPen=pDC->SelectObject(&border);
				CBrush* pOldBrush=pDC->SelectObject(&fill);
				pDC->Rectangle(&rect);
				pDC->SelectObject(pOldPen);
				pDC->SelectObject(pOldBrush);
			}
		}
		else if(m_ButtonState == BUTTON_OVER && m_ButtonPressed  == false)
		{
			border.CreatePen(PS_SOLID, 1, 0x006A240A);
			fill.CreateSolidBrush(0x00D2BDB6);
			CPen* pOldPen=pDC->SelectObject(&border);
			CBrush* pOldBrush=pDC->SelectObject(&fill);
			pDC->Rectangle(&rect);
			pDC->SelectObject(pOldPen);
			pDC->SelectObject(pOldBrush);
		}
		else if(m_ButtonState == BUTTON_OVER && m_ButtonPressed  == true)
		{
			border.CreatePen(PS_SOLID, 1, 0x006A240A);
			fill.CreateSolidBrush(0x00B59285);
			CPen* pOldPen=pDC->SelectObject(&border);
			CBrush* pOldBrush=pDC->SelectObject(&fill);
			pDC->Rectangle(&rect);
			pDC->SelectObject(pOldPen);
			pDC->SelectObject(pOldBrush);
		}
	}

//	pDC->SetROP2(oldROP);



	TCHAR txt[50];
	this->SendMessage(WM_GETTEXT,(WPARAM)sizeof(txt),(LPARAM)txt);
	if(wcscmp(txt,_T(""))!=0)
	{
		pDC->SetBkMode(TRANSPARENT);
		if(IsWindowEnabled())
		{
			if(m_ButtonState == BUTTON_OVER && m_ButtonPressed  == true) pDC->SetTextColor(0x00FFFFFF);
			else pDC->SetTextColor(0x00000000);
		}
		else
		{
			pDC->SetTextColor(0x00AAAAAA);
		}
			pDC->DrawText(txt,&rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	}
}

BOOL CHoverButton::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style|=BS_OWNERDRAW;
	return CButton::PreCreateWindow(cs);
}

void CHoverButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_ButtonPressed=true;
	Invalidate();
}

void CHoverButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if(m_ButtonPressed == false) return;
	::SendMessage(GetParent()->m_hWnd,WM_COMMAND,0,(LPARAM)this->m_hWnd);
	m_ButtonPressed=false;
	Invalidate();
}

void CHoverButton::setHasBorder(bool flag)
{
	m_hasBorder = flag;
}

void CHoverButton::PreSubclassWindow()
{
     CButton::PreSubclassWindow();
}
//-------------------------------------------------------------
BOOL CHoverButton::PreTranslateMessage(MSG* pMsg)
{
     switch(pMsg->message) {
          case WM_LBUTTONDOWN:
          case WM_LBUTTONUP:
          case WM_MOUSEMOVE:
               if ( m_tooltip.m_hWnd ) {
                    m_tooltip.RelayEvent(pMsg);
               }
          break;
     }
     return CButton::PreTranslateMessage(pMsg);
}