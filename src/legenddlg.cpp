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
#include "legenddlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld LegendDlg 


LegendDlg::LegendDlg(CWnd* pParent /*=NULL*/)
	: CDialog(LegendDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(LegendDlg)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}


void LegendDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(LegendDlg)
	DDX_Control(pDX, IDC_INFO_UW, m_unckeckedWrongInfo);
	DDX_Control(pDX, IDC_INFO_UR, m_unckeckedRightInfo);
	DDX_Control(pDX, IDC_INFO_UC, m_unckeckedChgInfo);
	DDX_Control(pDX, IDC_INFO_U, m_unckeckedInfo);
	DDX_Control(pDX, IDC_INFO_LCK, m_lockedInfo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(LegendDlg, CDialog)
	//{{AFX_MSG_MAP(LegendDlg)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten LegendDlg 

BOOL LegendDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CString str;
	str.LoadString(IDS_LEGEND_TITLE);
	SetWindowText(str);
	
	str.LoadString(IDS_LEGEND_UNCHECKED);
	SetDlgItemText(IDC_INFO_U, str);	
	str.LoadString(IDS_LEGEND_UNCHECKED_RIGHT);
	SetDlgItemText(IDC_INFO_UR, str);	
	str.LoadString(IDS_LEGEND_UNCHECKED_WRONG);
	SetDlgItemText(IDC_INFO_UW, str);	
	str.LoadString(IDS_LEGEND_UNCHECKED_CHANGE);
	SetDlgItemText(IDC_INFO_UC, str);	
	str.LoadString(IDS_LEGEND_LOCKED);
	SetDlgItemText(IDC_INFO_LCK, str);	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void LegendDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CPen border;
	CPen* pOldPen;
	CBrush fill;
	CBrush* pOldBrush;
	border.CreatePen(PS_SOLID,1,(COLORREF)0x0096E4FE);
	pOldPen=dc.SelectObject(&border);
	fill.CreateSolidBrush(0x00CEFFFF);
	pOldBrush = dc.SelectObject(&fill);

	CRect rect;
	GetClientRect(rect);
	int right = rect.right;
	CalcRect(rect, m_unckeckedInfo, 5, right);
	dc.Rectangle(&rect);
	CalcRect(rect, m_unckeckedRightInfo, 5, right);
	dc.Rectangle(&rect);
	CalcRect(rect, m_unckeckedWrongInfo, 5, right);
	dc.Rectangle(&rect);
	CalcRect(rect, m_unckeckedChgInfo, 5, right);
	dc.Rectangle(&rect);
	CalcRect(rect, m_lockedInfo, 5, right);
	dc.Rectangle(&rect);


	dc.SelectObject(pOldBrush);
	dc.SelectObject(*pOldPen);

	// Kein Aufruf von CDialog::OnPaint() für Zeichnungsnachrichten
}

void LegendDlg::CalcRect(CRect &rect, ColorEdit &edit, int xOffs, int right)
{
	CRect eRect;
	edit.GetWindowRect(eRect);
	ScreenToClient(eRect);
	rect.SetRect(xOffs, eRect.top - 4, right - xOffs, eRect.top + eRect.Height() + 4);
}
