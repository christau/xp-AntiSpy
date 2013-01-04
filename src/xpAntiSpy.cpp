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
#include "XPAntiSpyDlg.h"
#include "ConnectionLimitNG.h"
#include "ProfileDlg.h"

#include "checkprofiledlg.h"

#include <sstream>
#include "CMDLineDlg.h"

static const WORD MAX_CONSOLE_LINES = 500;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXPAntiSpyApp

BEGIN_MESSAGE_MAP(CXPAntiSpyApp, CWinApp)
	//{{AFX_MSG_MAP(CXPAntiSpyApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

template <class T>
T strToValue(std::wstring strValue)
{
	T value;
	std::wistringstream iss(strValue);
	iss >> value;
	return value;
}

/////////////////////////////////////////////////////////////////////////////
// CXPAntiSpyApp Konstruktion

CXPAntiSpyApp::CXPAntiSpyApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// Das einzige CXPAntiSpyApp-Objekt

CXPAntiSpyApp theApp;


/////////////////////////////////////////////////////////////////////////////
// CXPAntiSpyApp Initialisierung

BOOL CXPAntiSpyApp::InitInstance()
{
	AfxEnableControlContainer();
	AfxInitRichEdit();
	

	// Standardinitialisierung

#ifdef _AFXDLL
	Enable3dControls();			// Diese Funktion bei Verwendung von MFC in gemeinsam genutzten DLLs aufrufen
#else
	Enable3dControlsStatic();	// Diese Funktion bei statischen MFC-Anbindungen aufrufen
#endif
	//this is to get them commandline arguments for unattended running

	CString cmdLine(m_lpCmdLine); 

	if(cmdLine.Find(_T("/apply"), 0) == 0)
	{
		CString profile = cmdLine.Mid(7, cmdLine.GetLength() - 7);
		profile.TrimLeft();
		profile.TrimRight();
		SettingVec settings =ProfileDlg::GetProfile(profile);
		CAntiSpySettings xpas;
		for(int i=0; i<settings.size(); ++i)
		{
			SETTING set = settings[i];
			const SETTING_STATE state= xpas.TestSetting(set.settingID);
			if(state == NON_EXISTENT)
				continue;
			BOOL actChecked = state == ACTIVE || state == ACTIVE_NO_PERMISSION;
			if(set.checked != actChecked)
			{
				if(set.checked)
					xpas.DisableSetting(set.settingID);
				else
					xpas.EnableSetting(set.settingID);
			}
		
		}

		return true;
	}
	else if(wcscmp(m_lpCmdLine,_T("/check")) == 0)
	{
		CString profileName;
		SettingVec settings;
		int id = ProfileDlg::GetStartProfile();// - ID_PROFILE_SYSTEM_DEFAULT - 1;
		if(id == ID_PROFILE_SYSTEM_DEFAULT)
		{
			profileName.LoadString(IDS_PROFILE_SYSTEM_DEFAULT);
			settings = ProfileDlg::GetProfile(PROFILE_SYSTEM_DEFAULT_ON, PROFILE_SYSTEM_DEFAULT_OFF);
			//CheckProfile(PROFILE_SYSTEM_DEFAULT_ON, PROFILE_SYSTEM_DEFAULT_OFF, wParam);
		}
		else if(id == ID_PROFILE_SUGGESTED)
		{
			profileName.LoadString(IDS_PROFILE_SUGGESTED);
		settings = ProfileDlg::GetProfile(PROFILE_SUGGESTED_ON, PROFILE_SUGGESTED_OFF);
		}
		else if(id == ID_PROFILE_WINDOWS_UPDATE)
		{
			profileName.LoadString(IDS_PROFILE_WINDOWS_UPDATE);
			settings = ProfileDlg::GetProfile(PROFILE_WINDOWS_UPDATE_ON, PROFILE_WINDOWS_UPDATE_OFF);
		}
		else
		{
			id = id - ID_PROFILE_SYSTEM_DEFAULT - 1;
			ProfileDlg::StringVec profiles = ProfileDlg::GetExistingProfiles();
			//do check, if id is out of range
			if(id < 0 || profiles.size() < id)
			{
				return false;
			}
			settings = ProfileDlg::GetProfile(profiles[id]);
			profileName = profiles[id];
		}
		
		
		CAntiSpySettings xpas;
		SettingVec bad_settings;
		int cnt = 0;
		bool needsAdmin = false;
		for(int i = 0; i < settings.size(); ++i)
		{
			SETTING set = settings[i];
			const SETTING_STATE state= xpas.TestSetting(set.settingID);
			if(state == NON_EXISTENT)
				continue;
			BOOL actChecked = state == ACTIVE || state == ACTIVE_NO_PERMISSION;
			if(set.checked != actChecked)
			{
				if(state == ACTIVE_NO_PERMISSION || state == INACTIVE_NO_PERMISSION)
					needsAdmin = true;
				//++cnt;
				bad_settings.push_back(set);
			}
		}
		if(bad_settings.size() > 0)
		{
			//inform the user that some settings do not comply to the selected profile
			CCheckProfileDlg dlg(profileName, bad_settings, needsAdmin);
			dlg.DoModal();
		}

		
		return false;
	}
	else if(wcscmp(m_lpCmdLine,_T("/?")) == 0)
	{
		CCMDLineDlg cmd;
		cmd.DoModal();
		return false;
	}
	else if (m_lpCmdLine[0] != _T('\0'))
	{
		CAntiSpySettings settings;
		std::wstringstream cmdStr;
		cmdStr<<m_lpCmdLine;
		while (!cmdStr.eof())
		{
			std::wstring s;
			cmdStr>>s;
			int val = strToValue<int>(s);
			if(val<((-1)*ITEMCOUNT)||val>ITEMCOUNT) 
			{
				if(s.at(0) == 'l')
				{
					std::wstring str = s.substr(1);
					unsigned int limit = strToValue<unsigned int>(str);
					ConnectionLimit cl;
					TCHAR windir[MAX_PATH];
					GetWindowsDirectory(windir, MAX_PATH);
					wcscat(windir,DIR_LOC);
					if(cl.GetBuildID(windir)!=0)
						cl.WriteConnections(limit);
				}
				else
				{
					CString err;
					err.Format(_T("You mistyped some of the commandline aguments!\nThe wrong argument is %s"),s);
					AfxMessageBox(err);
					return false;
				}
			}
			if(val>=0 && val<=ITEMCOUNT)
			{
				val=abs(val);
				settings.DisableSetting(val);
			}
			else if(val<=0 && val>=((-1)*ITEMCOUNT))
			{
				val=abs(val);
				settings.EnableSetting(val);
			}
		}
		return false;
	}
	CXPAntiSpyDlg dlg;
	m_pMainWnd = &dlg;
	//dlg.m_CommandLine = this->m_lpCmdLine;

	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}

	// Da das Dialogfeld geschlossen wurde, FALSE zurückliefern, so dass wir die
	//  Anwendung verlassen, anstatt das Nachrichtensystem der Anwendung zu starten.
	return FALSE;
}

