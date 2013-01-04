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
#include "xpAntiSpy.h"
#include "HelpBox.h"
#include "GlobalConstants.h"
#include "resource.h"
#include "AntiSpySettings.h"
#include "resource_helper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHelpBox

CHelpBox::CHelpBox():CRichEditCtrl()
{
}

CHelpBox::~CHelpBox()
{
}


BEGIN_MESSAGE_MAP(CHelpBox, CRichEditCtrl)
	//{{AFX_MSG_MAP(CHelpBox)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CHelpBox 

int CHelpBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CRichEditCtrl::OnCreate(lpCreateStruct);
	//set the color and font of the control
	SetBackgroundColor(false,0x00CEFFFF); //light yellow
	CHARFORMAT cf;
	cf.cbSize=sizeof(CHARFORMAT);
	cf.dwMask=CFM_BOLD|CFM_COLOR|CFM_FACE|CFM_ITALIC|CFM_SIZE|CFM_UNDERLINE;
	cf.dwEffects=0;
	cf.yHeight=180;
	cf.yOffset=0;
	cf.crTextColor=0x00000000;
	cf.bCharSet=ANSI_CHARSET;
	cf.bPitchAndFamily=DEFAULT_PITCH;
	strcpy(cf.szFaceName, "Arial");
	
	SetDefaultCharFormat(cf);

	return 0;
}

BOOL CHelpBox::PreCreateWindow(CREATESTRUCT &cs)
{
	//set some WindowStyles
	cs.style=ES_MULTILINE|WS_VSCROLL|WS_CHILD|WS_VISIBLE|WS_BORDER|ES_READONLY;
	return true;
}

void CHelpBox::SetHelpText(UINT i,bool isGroupItem)
{
    CString langStr;

	if(!isGroupItem)
	{
		langStr = CResource::getHelpText(i);
	}
	else
	{
		
		langStr = CResource::getGroupHelpText(i);
		
	}
	if(!isGroupItem)
	{
		//langStr+=CString("\n\n")+CAntiSpySettings::GetRegKeyAsString(i);
		this->SetWindowText(langStr);
	}
	else this->SetWindowText(langStr);
}

void CHelpBox::OnSetFocus(CWnd* pOldWnd) 
{
	CRichEditCtrl::OnSetFocus(pOldWnd);
	
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen
	
}

void CHelpBox::ClearHelpText()
{
	this->SetWindowText(_T(""));
}
