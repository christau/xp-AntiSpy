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
#include "checkprofiledlg.h"
#include "resource_helper.h"
#include "AntiSpySettings.h"
#include "XPAntiSpyDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CCheckProfileDlg 


CCheckProfileDlg::CCheckProfileDlg(CString profileName, SettingVec settings, bool needsAdmin, CWnd* pParent /*=NULL*/)
	: CDialog(CCheckProfileDlg::IDD, pParent), m_profileName(profileName), m_settings(settings), m_needsAdmin(needsAdmin)
{
	//{{AFX_DATA_INIT(CCheckProfileDlg)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}


void CCheckProfileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCheckProfileDlg)
	DDX_Control(pDX, IDREAPPLY, m_btnReApply);
	DDX_Control(pDX, IDOK, m_btnIgnore);
	DDX_Control(pDX, IDC_INFO, m_txtInfo);
	DDX_Control(pDX, IDC_LIST, m_lstSettings);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCheckProfileDlg, CDialog)
	//{{AFX_MSG_MAP(CCheckProfileDlg)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CCheckProfileDlg 

void CCheckProfileDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	//m_lstSettings.MoveWindow(0,0, cx, cy, true);
	
}

BOOL CCheckProfileDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CString str;
	CString tr;
	tr.LoadString(IDS_PRF);
	str.Format(_T("xp-AntiSpy %s: %s"), tr, m_profileName);
	SetWindowText(str);

	for(int i=0; i<m_settings.size(); ++i)
	{
		SETTING set = m_settings[i];

		m_lstSettings.AddString(CResource::getText(set.settingID));

	}

	
	// TODO: Zusätzliche Initialisierung hier einfügen
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

BOOL CCheckProfileDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if((HWND)lParam == m_btnReApply.m_hWnd)
	{
		CString str;
		str = "/apply " + m_profileName;
		CXPAntiSpyDlg::ReRunApp(m_hWnd, str, CAntiSpySettings::GetSuiteType() >= CAntiSpySettings::SUITE_TYPE::WINDOWS_VISTA);
		return false;
	}
	if((HWND)lParam == m_btnIgnore.m_hWnd)
	{
		AfxGetMainWnd()->SendMessage(WM_CLOSE);
		//CloseWindow();
		return true;
	}
	return CDialog::OnCommand(wParam, lParam);
}
