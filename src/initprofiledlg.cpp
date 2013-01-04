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
#include "initprofiledlg.h"
#include <srrestoreptapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CinitProfileDlg 


CinitProfileDlg::CinitProfileDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CinitProfileDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CinitProfileDlg)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}

void SetSystemRestorPoint()
{
	RESTOREPOINTINFO RstPt;
	STATEMGRSTATUS MgrStat; 
	RstPt.dwRestorePtType = APPLICATION_INSTALL;
	wcscpy(RstPt.szDescription, _T("Demonstration Restore Point"));
	RstPt.dwEventType = BEGIN_SYSTEM_CHANGE;
	RstPt.dwRestorePtType = APPLICATION_INSTALL;
	wcscpy(RstPt.szDescription, _T("Demonstration Restore Point"));
	if (!SRSetRestorePoint(&RstPt, &MgrStat))
	{
		printf("Unable to set restore point. Error %ld\n",
			MgrStat.nStatus);
	}


}

void CinitProfileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CinitProfileDlg)
	DDX_Control(pDX, IDC_BTN_NEVER, m_btnNever);
	DDX_Control(pDX, IDC_BTN_LATER, m_btnLater);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDC_TEXT, m_dlgText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CinitProfileDlg, CDialog)
	//{{AFX_MSG_MAP(CinitProfileDlg)
	ON_BN_CLICKED(IDC_BTN_LATER, OnBtnLater)
	ON_BN_CLICKED(IDC_BTN_NEVER, OnBtnNever)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CinitProfileDlg 

BOOL CinitProfileDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Zusätzliche Initialisierung hier einfügen
	m_dlgText.FmtLines(true);
	CString txt;
	txt.LoadString(IDS_CREATE_DEFAULT_PROFILE);
	m_dlgText.SetWindowText(txt);
	txt.LoadString(IDS_YES);
	m_btnOk.SetWindowText(txt);
	txt.LoadString(IDS_NEVER);
	m_btnNever.SetWindowText(txt);
	txt.LoadString(IDS_LATER);
	m_btnLater.SetWindowText(txt);
	SetWindowText(_T("xp-AntiSpy"));

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CinitProfileDlg::OnBtnLater() 
{
	EndDialog(11);	
}

void CinitProfileDlg::OnBtnNever() 
{
	EndDialog(10);	
}
