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
#include "TimeServerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CTimeServerDlg 


CTimeServerDlg::CTimeServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTimeServerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTimeServerDlg)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}


void CTimeServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTimeServerDlg)
	DDX_Control(pDX, IDC_OK, m_btnOk);
	DDX_Control(pDX, IDC_ADD, m_NewTimeServerApply);
	DDX_Control(pDX, IDC_EDIT1, m_TimeServerEdit);
	DDX_Control(pDX, IDC_APPLY, m_TimeServerApply);
	DDX_Control(pDX, IDC_COMBO1, m_TimeServerCombo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTimeServerDlg, CDialog)
	//{{AFX_MSG_MAP(CTimeServerDlg)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeTimeServerCombo)
	ON_EN_CHANGE(IDC_EDIT1, OnChangeTimeServerEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CTimeServerDlg 

BOOL CTimeServerDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_TimeServerApply.EnableWindow(FALSE);	
	m_NewTimeServerApply.EnableWindow(FALSE);
	m_NewTimeServerApply.setHasBorder(true);
	m_TimeServerApply.setHasBorder(true);
	m_btnOk.setHasBorder(true);
	EnumTimeServers();	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CTimeServerDlg::EnumTimeServers()
{
	HKEY hKey;
	const int DATA_SIZE = 1024;
	TCHAR lpszReadVal[DATA_SIZE];
	TCHAR lpszReadData[DATA_SIZE];
	DWORD dwValSize = DATA_SIZE;
	DWORD dwDataSize = DATA_SIZE;
	DWORD dwIndex = 0;
	DWORD dwDefault = 0;
	DWORD dwValType = REG_SZ;
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\DateTime\\Servers"),0,KEY_READ,&hKey)!=ERROR_SUCCESS) 
	{
		RegCloseKey(hKey);
		return;
	}

	int retVal = RegEnumValue(hKey, dwIndex, lpszReadVal, &dwValSize,NULL, &dwValType, (LPBYTE)lpszReadData, &dwDataSize);
	while (retVal == ERROR_SUCCESS || retVal == ERROR_MORE_DATA)
    {
		if (dwValSize != 0) 
			m_TimeServerCombo.InsertString(-1,lpszReadData);
		else
			dwDefault = _wtoi(lpszReadData);
//			wscanf(lpszReadData,_T("%d"),&dwDefault);
		dwValSize = DATA_SIZE;
		dwDataSize = DATA_SIZE;
		++dwIndex;
		retVal = RegEnumValue(hKey, dwIndex, lpszReadVal, &dwValSize,NULL, &dwValType, (LPBYTE)lpszReadData, &dwDataSize);
    }
	m_TimeServerCombo.SetCurSel(dwDefault-1);
	RegCloseKey(hKey);


}

void CTimeServerDlg::TimeServerApply() 
{
	HKEY hKey;
	TCHAR lpszVal[128];
	TCHAR lpszServName[128];
	DWORD dwValSize = 128;
	DWORD dwDataSize = 128;
	DWORD dwIndex = 0;
	DWORD dwDefault = 0;
	DWORD dwValType = REG_SZ;
	CString tmp;
	tmp.LoadString(IDS_TIME_SERVER_START);
	if(AfxMessageBox(tmp,MB_YESNO)==IDYES)
	{
		m_TimeServerCombo.GetLBText(m_TimeServerCombo.GetCurSel(),lpszServName);
		RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Services\\W32Time\\Parameters"),0,KEY_WRITE,&hKey);
		wcscat(lpszServName,_T(",0x1"));
		//set address of timeserver
		RegSetValueEx(hKey,_T("NtpServer"),NULL,REG_SZ,(LPBYTE)lpszServName,sizeof(lpszServName));
		wcscpy(lpszVal,_T("NTP"));
		//set timesynchronisation
		RegSetValueEx(hKey,_T("Type"),NULL,REG_SZ,(LPBYTE)lpszVal,sizeof(lpszVal));	
		RegCloseKey(hKey);
		//write the defaultValue
		RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\DateTime\\Servers"),0,KEY_WRITE,&hKey);
		swprintf(lpszVal,_T("%d"),m_TimeServerCombo.GetCurSel()+1);
		RegSetValueEx(hKey,_T(""),NULL,REG_SZ,(LPBYTE)lpszVal,sizeof(lpszVal));	
		m_TimeServerCombo.GetLBText(m_TimeServerCombo.GetCurSel(),lpszServName);//add the TS to the list
		RegSetValueEx(hKey,lpszVal,NULL,REG_SZ,(LPBYTE)lpszServName,sizeof(lpszServName));	
		RegCloseKey(hKey);
		RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Services\\W32Time"),0,KEY_WRITE,&hKey);
		//start timerservice
		RegSetValueEx(hKey,_T("Start"),NULL,REG_DWORD,(LPBYTE)_T("\2\0\0\0"),4);
		RegCloseKey(hKey);
	}
}

void CTimeServerDlg::OnSelchangeTimeServerCombo() 
{
	m_TimeServerApply.EnableWindow(TRUE);	
}

void CTimeServerDlg::NewTimeServerApply() 
{
	TCHAR data[128];
	m_TimeServerEdit.GetWindowText(data,sizeof(data));
	m_TimeServerCombo.InsertString(-1,data);	
}

void CTimeServerDlg::OnChangeTimeServerEdit() 
{
	m_NewTimeServerApply.EnableWindow(TRUE);
}

BOOL CTimeServerDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if((HWND)lParam == m_NewTimeServerApply.m_hWnd)
	{
		NewTimeServerApply();
	}
	if((HWND)lParam == m_btnOk.m_hWnd)
	{
		OnOK();
	}
	if((HWND)lParam == m_TimeServerApply.m_hWnd)
	{
		TimeServerApply();
	}

	return CDialog::OnCommand(wParam, lParam);
}
