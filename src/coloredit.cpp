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
#include "coloredit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ColorEdit

ColorEdit::ColorEdit()
{
	m_crText = RGB(0,0,0);
	m_crBackGnd = 0x00CEFFFF;
	m_brBackGnd.CreateSolidBrush(0x00CEFFFF);
}

ColorEdit::~ColorEdit()
{
	if (m_brBackGnd.GetSafeHandle())
       m_brBackGnd.DeleteObject();
}


BEGIN_MESSAGE_MAP(ColorEdit, CEdit)
	//{{AFX_MSG_MAP(ColorEdit)
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

HBRUSH ColorEdit::CtlColor(CDC* pDC, UINT nCtlColor) 
{	
	pDC->SetTextColor(m_crText);
	pDC->SetBkColor(m_crBackGnd);
	return m_brBackGnd;
}


void ColorEdit::SetBackColor(COLORREF rgb)
{
	m_crBackGnd = rgb;
	
	if (m_brBackGnd.GetSafeHandle())
       m_brBackGnd.DeleteObject();
	m_brBackGnd.CreateSolidBrush(rgb);
	Invalidate(TRUE);
}


void ColorEdit::SetTextColor(COLORREF rgb)
{
	m_crText = rgb;
	Invalidate(TRUE);
}
