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
#include "connectionlimitdlg.h"
#include "AntiSpySettings.h"
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CConnectionLimitDlg 


CConnectionLimitDlg::CConnectionLimitDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConnectionLimitDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConnectionLimitDlg)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}


void CConnectionLimitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConnectionLimitDlg)
	DDX_Control(pDX, ID_BACKUP_RESTORE, m_btnBackupRestore);
	DDX_Control(pDX, ID_BACKUP_CLEAR, m_btnBackupClear);
	DDX_Control(pDX, ID_BTN_CREATE_BACKUP, m_btnCreateBackup);
	DDX_Control(pDX, IDC_BACKUP_LIST, m_lstBackup);
	DDX_Control(pDX, IDC_MAX_CONNS, m_txtMaxConns);
	DDX_Control(pDX, IDC_ON, m_txtOn);
	DDX_Control(pDX, IDC_INFO_BOX, m_grpInfo);
	DDX_Control(pDX, IDC_CONNECTIONS, m_txtConn);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_PATCHED, m_txtPatched);
	DDX_Control(pDX, IDC_BUILD, m_txtBuild);
	DDX_Control(pDX, IDC_BTN_RESET, m_btnReset);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDC_COMBO, m_cboConnections);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConnectionLimitDlg, CDialog)
	//{{AFX_MSG_MAP(CConnectionLimitDlg)
	ON_LBN_SELCHANGE(IDC_BACKUP_LIST, OnSelchangeBackupList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CConnectionLimitDlg 

void CConnectionLimitDlg::OnBtnPatch() 
{
	int sel = m_cboConnections.GetCurSel();
	CString tmp,unlim;
	if(sel < 0)
	{
		m_cboConnections.GetWindowText(tmp);
	}
	else
	{
		m_cboConnections.GetLBText(sel,tmp);
	}
	DWORD conns;
	unlim.LoadString(IDS_UNLIMITED_CONNS);
	if(tmp == unlim)
	{
		conns = 0x00FFFFFE;
	}
	else conns = _wtoi(tmp);
	if(conns == 0 || conns>0x00FFFFFE || conns<10)
	{
		CString tmp;
		tmp.LoadString(IDS_CORRUPT_VALUE);
		MessageBox(tmp);
		return;
	}
	if(CAntiSpySettings::GetSuiteType() == CAntiSpySettings::SUITE_TYPE::WINDOWS_VISTA)
	{
		m_conns.PatchVista();
	}
	else
	{
		CString patchedFiles = m_conns.WriteConnections(conns);
		CString txt;
		txt.LoadString(IDS_PATCHED_FILES);
		txt+=patchedFiles;
		MessageBox(txt,_T("xp-AntiSpy"));
		checkFile();
	}
}

BOOL CConnectionLimitDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CRect rect;
	GetWindowRect(&rect);
	rect.InflateRect(80,0,80,0);
	this->MoveWindow(&rect);
	GetClientRect(&rect);
	m_warning.Create(WS_BORDER|WS_CHILD|WS_VISIBLE|WS_VSCROLL|ES_MULTILINE|ES_READONLY,CRect(rect.left, rect.top,rect.right,rect.bottom-23),this,0);
	m_warning.SetBackgroundColor(false,0x00CEFFFF); //light yellow
	rect.DeflateRect(0,rect.Height()-23,0,0);
	CString txt;
	txt.LoadString(IDS_AGREE);
	m_warningOk.Create(txt,WS_VISIBLE,rect,this,1);

	CHARFORMAT cf;
	cf.cbSize=sizeof(CHARFORMAT);
	cf.dwMask=CFM_BOLD|CFM_COLOR|CFM_FACE|CFM_ITALIC|CFM_SIZE|CFM_UNDERLINE;
	cf.dwEffects=0;
	cf.yHeight=190;
	cf.yOffset=0;
	cf.crTextColor=0x00000000;
	cf.bCharSet=ANSI_CHARSET;
	cf.bPitchAndFamily=DEFAULT_PITCH;
	
	lstrcpy((LPTSTR)cf.szFaceName, _T("Arial"));
	m_warning.SetDefaultCharFormat(cf);

	txt.LoadString(IDS_CONNECTION_LIMIT_WARNING);
	m_warning.SetWindowText(txt);

	int start = m_warning.LineIndex(0);
	int stop = start + m_warning.LineLength(0);
	m_warning.SetSel(start, stop);
	PARAFORMAT Pfm;
	Pfm.cbSize = sizeof(PARAFORMAT);
	Pfm.dwMask = PFM_ALIGNMENT;
	Pfm.wAlignment = PFA_CENTER;
	m_warning.SetParaFormat(Pfm);

	m_warning.GetSelectionCharFormat(cf);
	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask = CFM_COLOR;
	cf.dwEffects = 0;
	cf.crTextColor = RGB(255, 0, 0);

	m_warning.SetSelectionCharFormat(cf);
	
	int last = m_warning.GetLineCount()-1;
	start = m_warning.LineIndex(last);
	stop = start + m_warning.LineLength(start);
	m_warning.SetSel(start, stop);
	m_warning.SetParaFormat(Pfm);
	m_warning.SetSelectionCharFormat(cf);

	m_warning.SetSel(0,0);

	m_cboConnections.InsertString(0, _T("50"));
	m_cboConnections.InsertString(1, _T("100"));
	m_cboConnections.InsertString(2, _T("200"));
	m_cboConnections.InsertString(3, _T("500"));
	CString tmp;
	tmp.LoadString(IDS_UNLIMITED_CONNS);
	m_cboConnections.InsertString(4, tmp);
	m_cboConnections.SetCurSel(0);
	checkFile();
	m_btnOk.setHasBorder(true);
	m_btnReset.setHasBorder(true);
	m_btnCancel.setHasBorder(true);
	m_btnBackupClear.setHasBorder(true);
	m_btnCreateBackup.setHasBorder(true);
	m_btnBackupRestore.setHasBorder(true);
	tmp.LoadString(IDS_BACKUP_CREATE);
	m_btnCreateBackup.SetTipText(tmp);
	tmp.LoadString(IDS_BACKUP_DELETE);
	m_btnBackupClear.SetTipText(tmp);
	m_btnBackupClear.EnableWindow(FALSE);
	tmp.LoadString(IDS_BACKUP_RESTORE);
	m_btnBackupRestore.SetTipText(tmp);
	m_btnBackupRestore.EnableWindow(FALSE);
	initBackupList();
	return TRUE;
}

/*
CConnectionLimitDlg::ConnectionLimit::BUILD_ID CConnectionLimitDlg::GetBuildID()
{
	char windir[MAX_PATH];
	GetWindowsDirectory(windir, MAX_PATH);
	strcat(windir,DIR_LOC);
	CFile file;
	file.Open(windir,CFile::modeRead);
	CFileStatus stat;
	file.GetStatus(stat);
	if(stat.m_size == 356992) return ConnectionLimit::BUILD_2096;
	if(stat.m_size == 359424) return ConnectionLimit::BUILD_2149;
	if(stat.m_size == 360320) return ConnectionLimit::BUILD_2162;
	if(stat.m_size == 359040) return ConnectionLimit::BUILD_2180;
	if(stat.m_size == 328192) return ConnectionLimit::BUILD_1218;
	return ConnectionLimit::BUILD_UNKNOWN;
}
*/

void CConnectionLimitDlg::OnBtnReset() 
{
	if(CAntiSpySettings::GetSuiteType() == CAntiSpySettings::SUITE_TYPE::WINDOWS_VISTA)
	{
		m_conns.ResetVistaPatch();
	}
	else
	{
		CString patchedFiles = m_conns.WriteConnections(10);
		CString txt;
		txt.LoadString(IDS_RESET_FILES);
		txt+=patchedFiles;
		MessageBox(txt,_T("xp-AntiSpy"));
		checkFile();
	}
}

void CConnectionLimitDlg::checkFile()
{
	TCHAR windir[MAX_PATH];
	GetWindowsDirectory(windir, MAX_PATH);
	wcscat(windir,DIR_LOC);
	int id = m_conns.GetBuildID(CString(windir));
	CString patched;
	CString yesNo;
	if(m_conns.IsPatched() == RET_YES)
	{
		patched.LoadString(IDS_PATCHED);
		yesNo.LoadString(IDS_YES);
		patched+=_T(" ") + yesNo;
		m_btnReset.EnableWindow();
		m_cboConnections.EnableWindow(false);
		m_btnOk.EnableWindow(false);
	}
	else if(m_conns.IsPatched() == RET_NO)
	{
		patched.LoadString(IDS_PATCHED);
		yesNo.LoadString(IDS_NO);
		patched+=_T(" ") + yesNo;
		m_btnReset.EnableWindow(false);
		m_cboConnections.EnableWindow(true);
		m_btnOk.EnableWindow();
	}
	else if(m_conns.IsPatched() == RET_ERROR)
	{
		CString tmp;
		tmp.Format(_T("Error: %d"), GetLastError());
		AfxMessageBox(tmp);
		patched.LoadString(IDS_PATCHED);
		yesNo.LoadString(IDS_ERROR);
		patched+=_T(" ") + yesNo;
		m_btnReset.EnableWindow(false);
		m_cboConnections.EnableWindow(false);
		m_btnOk.EnableWindow(FALSE);
	}

	if(id==0) //Unknown build
	{
		CString unknown;
		unknown.LoadString(IDS_UNKNOWN_BUILD);
		MessageBox(unknown,_T("xp-AntiSpy"));
		m_cboConnections.EnableWindow(false);
		m_btnOk.EnableWindow(false);
		m_btnReset.EnableWindow(false);
		CDialog::OnOK();
	}
	else
	{
		CString str;
		str.Format(_T("Build: %d"), id);
		m_txtBuild.SetWindowText(str);	
		m_txtPatched.SetWindowText(patched);
	}
	yesNo.LoadString(IDS_MAX_CONNS);
	DWORD dwOffset = m_conns.GetOffset(CString(windir));
	if(dwOffset != 0)
	{
		patched.Format(_T("%s %u\tOffset:0x0%x"),yesNo, m_conns.GetMaxConnections(windir), dwOffset);
		m_txtMaxConns.SetWindowText(patched);

	}
	else
	{
		patched.LoadString(IDS_CL_NOT_POSSIBLE);
		m_txtMaxConns.SetWindowText(patched);
		m_btnReset.EnableWindow(false);
		m_cboConnections.EnableWindow(false);
		m_btnOk.EnableWindow(false);
	}

}


BOOL CConnectionLimitDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if((HWND)lParam==m_warningOk.m_hWnd) 
	{
		m_warning.ShowWindow(false);
		m_warningOk.ShowWindow(false);
		m_txtBuild.ShowWindow(true);
		m_txtPatched.ShowWindow(true);
		m_btnOk.ShowWindow(true);
		m_btnReset.ShowWindow(true);
		m_btnCancel.ShowWindow(true);
		m_txtConn.ShowWindow(true);
		m_txtOn.ShowWindow(true);
		m_grpInfo.ShowWindow(true);
		m_cboConnections.ShowWindow(true);
		m_txtMaxConns.ShowWindow(true);
		m_btnBackupClear.ShowWindow(true);
		m_btnCreateBackup.ShowWindow(true);
		m_btnBackupRestore.ShowWindow(true);

		CRect rect;
		GetWindowRect(&rect);
		rect.DeflateRect(80,0,80,0);
		this->MoveWindow(&rect);
	}
	if((HWND)lParam==m_btnOk.m_hWnd) 
	{
		OnBtnPatch();
	}
	if((HWND)lParam==m_btnReset.m_hWnd) 
	{
		OnBtnReset();
	}
	if((HWND)lParam==m_btnCancel.m_hWnd) 
	{
		OnOK();
	}
	if((HWND)lParam==m_btnBackupClear.m_hWnd) 
	{
		OnBackupClear();
	}
	if((HWND)lParam==m_btnCreateBackup.m_hWnd) 
	{
		OnBtnCreateBackup();
	}
	if((HWND)lParam==m_btnBackupRestore.m_hWnd) 
	{
		OnBackupRestore();
	}
	return CDialog::OnCommand(wParam, lParam);
}

void CConnectionLimitDlg::OnBtnCreateBackup() 
{
	TCHAR windir[MAX_PATH];
	GetWindowsDirectory(windir, MAX_PATH);
	wcscat(windir,DIR_LOC);
	TCHAR szDirectory[MAX_PATH] = _T("");
	if(!::GetCurrentDirectory(sizeof(szDirectory) - 1, szDirectory))
	{
		return;
	}
	CString dest(szDirectory);
	CTime time = CTime::GetCurrentTime();
	CString tmp;
	tmp = time.Format(_T("%d%m%Y-%H%M%S.xpas_bak"));
	CString tmp2;
	tmp2.Format(_T("\\%d-%s"),  m_conns.GetMaxConnections(windir) , tmp);
	dest += tmp2;
	if(CopyFile(windir, dest, TRUE))
	{
		m_lstBackup.ResetContent();
		initBackupList();
	}

}

void CConnectionLimitDlg::OnBackupClear() 
{
	signed int idx = m_lstBackup.GetCurSel();
	if(idx >= 0 && idx <= m_lstBackup.GetCount())
	{
		TCHAR szDirectory[MAX_PATH] = _T("");
		if(!::GetCurrentDirectory(sizeof(szDirectory) - 1, szDirectory))
		{
			return;
		}
		CString file;
		CString path(szDirectory);
		m_lstBackup.GetText(idx, file);
		file = path + _T("\\") + file;
		if(::DeleteFile(file))
		{
			m_lstBackup.ResetContent();
			initBackupList();
		}

	}
}

void CConnectionLimitDlg::OnBackupRestore() 
{
	signed int idx = m_lstBackup.GetCurSel();
	if(idx >= 0 && idx <= m_lstBackup.GetCount())
	{
		TCHAR szDirectory[MAX_PATH] = _T("");
		if(!::GetCurrentDirectory(sizeof(szDirectory) - 1, szDirectory))
		{
			return;
		}
		CString file;
		CString path(szDirectory);
		m_lstBackup.GetText(idx, file);
		file = path + _T("\\") + file;
		
		TCHAR windir[MAX_PATH];
		GetWindowsDirectory(windir, MAX_PATH);
		CString dir;
		dir.Format(_T("%s%s"), windir, DIR_LOC);
		if(!CopyFile(file, dir, FALSE))
		{
			MessageBox(_T("Error"));
		}
		dir.Format(_T("%s%s"), windir, DIR_LOC_1);
		if(!CopyFile(file, dir, FALSE))
		{
		}
		dir.Format(_T("%s%s"), windir, DIR_LOC_2);
		if(!CopyFile(file, dir, FALSE))
		{
		}
		
		checkFile();
	}
}

void CConnectionLimitDlg::initBackupList() 
{
	TCHAR szDirectory[MAX_PATH] = _T("");
	if(!::GetCurrentDirectory(sizeof(szDirectory) - 1, szDirectory))
	{
		return;
	}
	std::wstring file(szDirectory);
	file += _T("\\*.xpas_bak");
	struct _wfinddata_t c_file;
	long hFile;
	hFile = _wfindfirst((LPTSTR)file.c_str(), &c_file);
	if(hFile != -1L)
	{
		m_lstBackup.AddString(c_file.name);
		while (_wfindnext(hFile, &c_file) == 0)
		{
			m_lstBackup.AddString(c_file.name);
		}
	}
}

void CConnectionLimitDlg::OnSelchangeBackupList() 
{
	if(m_lstBackup.GetCurSel() > -1)
	{
		m_btnBackupClear.EnableWindow(TRUE);
		m_btnBackupRestore.EnableWindow(TRUE);
	}
	else
	{
		m_btnBackupClear.EnableWindow(FALSE);
		m_btnBackupRestore.EnableWindow(FALSE);
	}
}
