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
    along with xp-AntiSpy.  If not, see <http://www.gnu.org/licenses/
*/
#ifndef VS2015
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif 
#endif

#include "stdafx.h"
#include "resource.h"
#include "AntiSpySettings.h"
#include "ServicesControl.h"
#define IMPLEMENT_VISTA_TOOLS
#include "VistaTools.cpp"
#include <stdio.h>
#include <aclapi.h>
#include <sys/stat.h>
#include <Tlhelp32.h>

#include <commctrl.h>
#include <commdlg.h>
#include <tchar.h>
#include <sddl.h>
extern BOOL IsWow64();
extern BOOL MyWow64DisableWow64FsRedirection(PVOID OldValue);
extern BOOL MyWow64RevertWow64FsRedirection(PVOID OldValue);

typedef struct {
	  DWORD dwOSVersionInfoSize;
	  DWORD dwMajorVersion;
	  DWORD dwMinorVersion;
	  DWORD dwBuildNumber;
	  DWORD dwPlatformId;
	  TCHAR szCSDVersion[ 128 ];
	  WORD wServicePackMajor;
	  WORD wServicePackMinor;
	  WORD wSuiteMask;
	  BYTE wProductType;
	  BYTE wReserved;
} OSVERSIONINFOEX_NEW;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CAntiSpySettings::CAntiSpySettings()
{
	m_svcControl=new CServicesControl(false);
	//CString tmp = GetUninstallString(_T("MSN Messenger"));
	//TRACE1(_T("%s"),tmp);
}

CAntiSpySettings::~CAntiSpySettings()
{
	if(m_svcControl) delete m_svcControl;
}
//////////////////////////////////////////////////////////////////////
CAntiSpySettings::SUITE_TYPE CAntiSpySettings::GetSuiteType()
{
	//———————————————————————————————————
	// GetSuiteType - Returns the type of platform
	//
	// 0 - Older than NT SP6 or Win9x
	// 1 - Windows XP Home
	// 2 - Windows NT/2000
	// 3 - XP Professional
	// 4 - Vista
	// 5 - Windows 7
	// 6 - Windows 8
	// 7 - Windows 8.1
	// 8 - Windows 10
	//
    OSVERSIONINFOEX_NEW osVerEx;

    //Try the new Ex structure. This will fail on < Windows NT 4.0 SP6
    memset (&osVerEx, 0, sizeof (OSVERSIONINFOEX_NEW));
    osVerEx.dwOSVersionInfoSize = sizeof (OSVERSIONINFOEX_NEW);
	if (!GetVersionEx((OSVERSIONINFO *)&osVerEx))
		return DEPRECATED_WINDOWS;
    if(osVerEx.wSuiteMask==768) return WINDOWS_XP_HOME;
    else if(/*osVerEx.wSuiteMask==256 && */osVerEx.dwMajorVersion==5 && osVerEx.dwMinorVersion == 0) return WINDOWS_NT_2000;
    else if(/*osVerEx.wSuiteMask==256 && */osVerEx.dwMajorVersion==5 && osVerEx.dwMinorVersion > 0) return WINDOWS_XP_PROFESSIONAL;
    else if(/*osVerEx.wSuiteMask==256 && */osVerEx.dwMajorVersion==6 && osVerEx.dwMinorVersion == 0) return WINDOWS_VISTA;
    else if(/*osVerEx.wSuiteMask==256 && */osVerEx.dwMajorVersion==6 && osVerEx.dwMinorVersion == 1) return WINDOWS_7;
	else if (osVerEx.dwMajorVersion >= 6)
	{
		RTLGETVERSION RtlGetVersion = (RTLGETVERSION)GetProcAddress(GetModuleHandleA("ntdll.dll"), "RtlGetVersion");
		OSVERSIONINFO osv = { 0 };
		osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		RtlGetVersion(&osv);
		if (osv.dwMajorVersion == 6 && osv.dwMinorVersion == 2)
		{
			return WINDOWS_8;
		}
		else if (osv.dwMajorVersion == 6 && osv.dwMinorVersion == 3)
		{
			return WINDOWS_8_1;
		}
		else if (osv.dwMajorVersion == 10 && osv.dwMinorVersion == 0)
		{
			return WINDOWS_10;
		}
	}

	return UNKNOWN;
}


int CAntiSpySettings::GetSuiteType(CString* versionInfo)
{
//———————————————————————————————————
// GetSuiteType - Returns the type of platform
//
// 0 - Older than NT SP6 or Win9x
// 1 - Windows XP Home
// 2 - Windows NT/2000/XP Professional
//
    OSVERSIONINFOEX_NEW osVerEx;

    //Try the new Ex structure. This will fail on < Windows NT 4.0 SP6
    memset (&osVerEx, 0, sizeof (OSVERSIONINFOEX_NEW));
    osVerEx.dwOSVersionInfoSize = sizeof (OSVERSIONINFOEX_NEW);
    if (!GetVersionEx ((OSVERSIONINFO *)&osVerEx))
        return 0;
//	CString osVerInfo;
	if(versionInfo)	versionInfo->Format(_T("MajorVersion: %d \nMinorVersion: %d\nBuildNumber: %d\nPlatformId: %d\nCSDVersion: %s\nSuiteMask: %d"),osVerEx.dwMajorVersion,osVerEx.dwMinorVersion,osVerEx.dwBuildNumber,osVerEx.dwPlatformId,osVerEx.szCSDVersion,osVerEx.wSuiteMask);
//	AfxMessageBox(osVerInfo);
    switch(osVerEx.dwPlatformId)
	{
		case VER_PLATFORM_WIN32_WINDOWS:
			return -1;
		break;
		case VER_PLATFORM_WIN32_NT:
			if(osVerEx.dwMajorVersion==5 && osVerEx.dwMinorVersion==1)
			{
				if(osVerEx.wSuiteMask==768) return 1;
				if(osVerEx.wSuiteMask==256) return 2;
			}
		break;
	}

	return -1;
}


//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
const SETTING_STATE CAntiSpySettings::TestSetting(const UINT setting) const
{
	bool isWrite;
	unsigned int startType;
	unsigned int retVal,retVal1, retVal2;

	switch(setting)
	{
	case ST_MEDIAPLAYER_LICENSES:
		{
			int ver = GetMediaPlayerVersion();
			if(ver > 8 && ver < 13)
			{
				if(TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\MediaPlayer\\Preferences"),_T("SilentAcquisition"),REG_DWORD,0,NULL) == MATCH)
					return ACTIVE;
				else
					return INACTIVE;
			}
			else return NON_EXISTENT;
		}
		break;
	case ST_MEDIAPLAYER_SEND_GUID:
		{
			int ver = GetMediaPlayerVersion();
			if(ver > 8 && ver < 13)
			{
				if(TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\MediaPlayer\\Preferences"),_T("SendUserGUID"),REG_BINARY,0,_T("\0"))==MATCH)
					return ACTIVE;
				else
					return INACTIVE;
			}
			else return NON_EXISTENT;
		}
		break;
	case ST_MEDIAPLAYER_CODECS:
		{
			int ver = GetMediaPlayerVersion();
			if(ver > 8 && ver < 13)
			{
				if(TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\MediaPlayer\\Preferences"),_T("UpgradeCodecPrompt"),REG_DWORD,0,NULL) == MATCH)
					return ACTIVE;
				else
					return INACTIVE;
			}
			else return NON_EXISTENT;
		}
		break;
	case ST_MP9_ADDMEDIATOLIB:
		{
			int ver = GetMediaPlayerVersion();
			if(ver > 8 && ver < 13)
			{
				if(TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\MediaPlayer\\Preferences"),_T("AutoAddMusicToLibrary"),REG_DWORD,0,NULL) == MATCH)
					return ACTIVE;
				else
					return INACTIVE;
			}
			else return NON_EXISTENT;
		}
		break;
	case ST_MP9_RETRIEVEMETADATA:
		{
			int ver = GetMediaPlayerVersion();
			if(ver > 8 && ver < 13)
			{
				if(TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\MediaPlayer\\Preferences"),_T("MetadataRetrieval"),REG_DWORD,0,NULL) == MATCH)
					return ACTIVE;
				else
					return INACTIVE;
			}
			else return NON_EXISTENT;
			}
		break;
	case ST_MP9_USAGETRACKING:
		{
			int ver = GetMediaPlayerVersion();
			if(ver > 8 && ver < 13)
			{
				if(TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\MediaPlayer\\Preferences"),_T("UsageTracking"),REG_DWORD,0,NULL) == MATCH)
					return ACTIVE;
				else
					return INACTIVE;
			}
			else return NON_EXISTENT;
		}
		break;
	case ST_MP9_SAVEMRU:
		{
			int ver = GetMediaPlayerVersion();
			if(ver > 8 && ver < 12)
			{
				if(TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\MediaPlayer\\Preferences"),_T("DisableMRU"),REG_DWORD,1,NULL) == MATCH)
					return ACTIVE;
				else
					return INACTIVE;
			}
			else if(ver > 11)
			{
				if(TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\MediaPlayer\\Preferences"),_T("DisableMRUMusic"),REG_DWORD,1,NULL) == MATCH)
					return ACTIVE;
				else
					return INACTIVE;
			}
			else return NON_EXISTENT;
		}
		break;
	case ST_MP_START_IN_MEDIA_GUIDE:
		{
			int ver = GetMediaPlayerVersion();
			if(!TestRegKey(HKEY_CURRENT_USER,_T("Software\\Microsoft\\MediaPlayer\\Preferences")) || ver > 9)
				return NON_EXISTENT;
			if(TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\MediaPlayer\\Preferences"),_T("StartInMediaGuide"),REG_DWORD,0,NULL) == MATCH)
				return ACTIVE;
			else 
				return INACTIVE;
			break;
		}
	case ST_ERROR_REPORTS_SEND:
		if(IsWinXP() || GetSuiteType() == WINDOWS_VISTA)
		{
			if(TestRegKey(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\PCHealth\\ErrorReporting")))
			{
				isWrite = TestKeyWritePermission(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\PCHealth\\ErrorReporting"));
				if(TestRegKeyValue(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\PCHealth\\ErrorReporting"),_T("DoReport"),REG_DWORD,0,NULL) == MATCH)
				{
					if(!isWrite)
						return ACTIVE_NO_PERMISSION;
					return ACTIVE;
				}
				else
				{
					if(!isWrite)
						return INACTIVE_NO_PERMISSION;
					return INACTIVE;
				}
			}
		}
		else return NON_EXISTENT;
	break;
//	case ST_ERROR_REPORTS_CRITICAL:
//		if(IsWinXP())
//		{
//			return TestRegKeyValue(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\PCHealth\\ErrorReporting"),_T("ShowUI"),REG_DWORD,0,NULL)==1;
//		}
//		else return NON_EXISTENT;
//	break;
	case ST_REMOTEDESKTOP:
		isWrite = TestKeyWritePermission(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Control\\Terminal Server"));
		if(GetSuiteType() == SUITE_TYPE::WINDOWS_XP_HOME || GetSuiteType() >= SUITE_TYPE::WINDOWS_XP_PROFESSIONAL)
		{
			if(TestRegKeyValue(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Control\\Terminal Server"),_T("fDenyTSConnections"),REG_DWORD,0,NULL) == MATCH)
			{
				if(!isWrite)
					return INACTIVE_NO_PERMISSION;
				return INACTIVE;
			}
			else
			{
				if(!isWrite)
					return ACTIVE_NO_PERMISSION;
				return ACTIVE;
			}
		}
		else return NON_EXISTENT;
	break;

	case ST_IE_SCHEDULED_UPDATES:
		if(GetSuiteType() > SUITE_TYPE::WINDOWS_XP_PROFESSIONAL)
		{
			return NON_EXISTENT;
		}
		if(TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Webcheck"),_T("NoScheduledUpdates"),REG_DWORD,1,NULL) == MATCH)
			return ACTIVE;
		else
			return INACTIVE;
	break;
	case ST_IE_CHECK_4_UPDATES:
		if(TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Internet Explorer\\Main"),_T("NoUpdateCheck"),REG_DWORD,1,NULL) == MATCH)
			return ACTIVE;
		else
			return INACTIVE;
	break;
	case ST_IE_FORM_SUGGEST:
		if(!TestRegKey(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Internet Explorer\\Main")))
			return NON_EXISTENT;
		retVal = TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Internet Explorer\\Main"),_T("Use FormSuggest"),REG_SZ,0,_T("no"));
		retVal2 = TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Internet Explorer\\Main"),_T("FormSuggest Passwords"),REG_SZ,0,_T("no"));
		if(retVal == MATCH || retVal2 == MATCH) return ACTIVE;
		if(retVal == NOT_FOUND || retVal2 == NOT_FOUND) return INACTIVE;
		return INACTIVE;
	break;
	case ST_IE_JAVASCRIPT:
		if(GetIEVersion() >= 6 && GetIEVersion() <= 9)
		{
			retVal = TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Zones\\3"),_T("1400"),REG_DWORD,3,NULL);
			retVal1 = TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Zones\\3"),_T("1402"),REG_DWORD,3,NULL);
			retVal2 = TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Zones\\3"),_T("1407"),REG_DWORD,3,NULL);
			if(retVal == MATCH && retVal1 == MATCH && retVal2 == MATCH) return ACTIVE;
			//if(retVal == 2 || retVal1 == 2 || retVal2 == 2) return NON_EXISTENT;
			else return INACTIVE;
		}
		else return NON_EXISTENT;
	break;
	case ST_IE_ACTIVEX:
		if(GetIEVersion() >= 6 && GetIEVersion() <= 9)
		{
			retVal = TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Zones\\3"),_T("1200"),REG_DWORD,3,NULL);
			retVal1 = TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Zones\\3"),_T("1405"),REG_DWORD,3,NULL);
			//retVal2 = TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Zones\\3"),_T("1001"),REG_DWORD,1,NULL);
			if(retVal == MATCH && retVal1 == MATCH /*&& retVal2 == 1*/) return ACTIVE;
			//if(retVal == 2 || retVal1 == 2 /*|| retVal2 == 2*/) return NON_EXISTENT;
			else return INACTIVE;
		}
		else return NON_EXISTENT;
	break;
	case ST_IE_POPUP_BLOCKER:
		{
			int ver = GetIEVersion();
			if(ver == 6)
			{
				retVal = TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Zones\\3"),_T("1809"),REG_DWORD,0,NULL);
				if(retVal== MATCH) return ACTIVE;
				else if(retVal == NOT_FOUND) return NON_EXISTENT;
				else return INACTIVE;
			}
			else if(ver > 6)
			{
				retVal = TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Internet Explorer\\New Windows"),_T("PopupMgr"),REG_DWORD,1, NULL);
				if(retVal == NOT_FOUND)
					retVal = TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Internet Explorer\\New Windows"),_T("PopupMgr"),REG_SZ,1, _T("yes"));
				if(retVal== MATCH) return ACTIVE;
				else if(retVal == NOT_FOUND) return NON_EXISTENT;
				else return INACTIVE;
			}
			else return NON_EXISTENT;
			//on ie7 it's HKEY_CURRENT_USER\Software\Microsoft\Internet Explorer\New Windows PopupMgr no|yes
		}
	break;
	case ST_IE_PHISHING_FILTER:
		if(GetIEVersion() == 7)
		{
			retVal = TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Internet Explorer\\PhishingFilter"),_T("Enabled"),REG_DWORD,2,NULL);
			if(retVal== MATCH) return ACTIVE;
			//else if(retVal == NOT_FOUND) return NON_EXISTENT;
			else return INACTIVE;
		}
		else if(GetIEVersion() == 8)
		{
			retVal = TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Internet Explorer\\PhishingFilter"),_T("EnabledV8"),REG_DWORD,1,NULL);
			if(retVal== MATCH) return ACTIVE;
			//else if(retVal == NOT_FOUND) return NON_EXISTENT;
			else return INACTIVE;
		}
		else if(GetIEVersion() == 9)
		{
			retVal = TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Internet Explorer\\PhishingFilter"),_T("EnabledV9"),REG_DWORD,1,NULL);
			if(retVal== MATCH) return ACTIVE;
			//else if(retVal == NOT_FOUND) return NON_EXISTENT;
			else return INACTIVE;
		}
		else return NON_EXISTENT;
	break;
	case ST_TIME_SYNC:
		if(IsWinXP() || GetSuiteType() == WINDOWS_VISTA || GetSuiteType() == WINDOWS_7)
		{
			isWrite = TestKeyWritePermission(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Services\\W32Time\\Parameters"));
			if(TestRegKeyValue(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Services\\W32Time\\Parameters"),_T("Type"),REG_SZ,0,_T("NoSync")) == MATCH)
			{
				if(!isWrite)
					return ACTIVE_NO_PERMISSION;
				return ACTIVE;
			}
			else
			{
				if(!isWrite)
					return INACTIVE_NO_PERMISSION;
				return INACTIVE;
			}
		}
		else return NON_EXISTENT;
	break;
	case ST_IE_INTEGRATED_AUTHENTICATION:
		if(GetIEVersion() >= 6 && GetIEVersion() <= 9)
		{
			if(TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings"),_T("EnableNegotiate"),REG_DWORD,0,NULL) == MATCH)
				return ACTIVE;
			else
				return INACTIVE;
		}
		else 
			return NON_EXISTENT;
	break;
	case ST_IE_NO_SQM:
		if(!TestRegKey(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Internet Explorer\\SQM")))
			return NON_EXISTENT;
		if(GetIEVersion() == 7)
		{
			if(TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Internet Explorer\\SQM"),_T("SqmOptInForIE"),REG_DWORD,0,NULL) == MATCH)
				return ACTIVE;
			else
				return INACTIVE;
		}
		else if(GetIEVersion() == 8)
		{
			if(TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Internet Explorer\\SQM"),_T("SqmOptInForIE8"),REG_DWORD,0,NULL) == MATCH)
				return ACTIVE;
			else
				return INACTIVE;
		}
		else 
			return NON_EXISTENT;
	break;
	case ST_IE_SUGGESTED_SITES:
		if(!TestRegKey(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Internet Explorer\\Suggested Sites")) || GetIEVersion() < 8)
			return NON_EXISTENT;
		if(GetIEVersion() == 8 || GetIEVersion()==9)
		{
			if(TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Internet Explorer\\Suggested Sites"),_T("Enabled"),REG_DWORD,0,NULL) == MATCH)
				return ACTIVE;
			else
				return INACTIVE;
		}
		else 
			return NON_EXISTENT;
	break;
	case ST_REGDONE:
		isWrite = TestKeyWritePermission(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"));
		if(TestRegKeyValue(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"),_T("RegDone"),REG_SZ,0,_T("1")) == MATCH)
		{
			if(!isWrite)
				return ACTIVE_NO_PERMISSION;
			return ACTIVE;
		}
		else
		{
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}	
	break;
	case ST_ALEXA:
		if(!TestRegKey(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Internet Explorer\\Extensions\\{c95fe080-8f5d-11d2-a20b-00aa003c157a}")))
			return NON_EXISTENT;
		else 
		{
			return ACTIVE;
		}
	break;
	case ST_SERVICES_ERROR_REPORTS:
		if(GetSuiteType() != WINDOWS_VISTA && ServiceExist(_T("ERSvc")))
		{
			startType = m_svcControl->GetStartType(_T("ERSvc"));
			if(startType != 5)
			{
				isWrite = m_svcControl->IsChangeable(_T("ERSvc"));
			}
			if(startType<4) 
			{
				if(!isWrite)
					return INACTIVE_NO_PERMISSION;
				return INACTIVE;
			}
			if(startType==5) return NON_EXISTENT;

			if(!isWrite)
				return ACTIVE_NO_PERMISSION;
			return ACTIVE;
		}
		else if((GetSuiteType() == WINDOWS_VISTA || GetSuiteType() == WINDOWS_7) && ServiceExist(_T("WerSvc")))
		{
			startType = m_svcControl->GetStartType(_T("WerSvc"));
			if(startType != 5)
			{
				isWrite = m_svcControl->IsChangeable(_T("WerSvc"));
			}
			if(startType<4)
			{
				if(!isWrite)
					return INACTIVE_NO_PERMISSION;
				return INACTIVE;
			}
			if(startType==5) return NON_EXISTENT;
			if(!isWrite)
				return ACTIVE_NO_PERMISSION;
			return ACTIVE;
		}
		return NON_EXISTENT;
	break;
	case ST_SERVICES_AUTO_UPDATES:
		startType = m_svcControl->GetStartType(_T("wuauserv"));
		if(startType != 5)
		{
			isWrite = m_svcControl->IsChangeable(_T("wuauserv"));
		}
		if(startType<4)
		{
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}
		if(startType==5) return NON_EXISTENT;
		if(!isWrite)
			return ACTIVE_NO_PERMISSION;
		return ACTIVE;
	break;
	case ST_SERVICES_TIMESERVER:
		startType = m_svcControl->GetStartType(_T("W32Time"));
			if(startType != 5)
			{
				isWrite = m_svcControl->IsChangeable(_T("W32Time"));
			}
		if(startType<4) 
		{
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}
		if(startType==5) return NON_EXISTENT;
		if(!isWrite)
			return ACTIVE_NO_PERMISSION;
		return ACTIVE;
	break;
	case ST_SERVICES_TASKSCHEDULER:
		startType = m_svcControl->GetStartType(_T("Schedule"));
		if(startType != 5)
		{
			isWrite = TestKeyWritePermission(HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Services\\Schedule"));
			//isWrite = m_svcControl->IsChangeable(_T("Schedule"));
		}
		if(startType<4)
		{
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}
		if(startType==5) return NON_EXISTENT;
		if(!isWrite)
			return ACTIVE_NO_PERMISSION;
		return ACTIVE;
	break;
	case ST_SERVICES_UPNP:
		startType = m_svcControl->GetStartType(_T("upnphost"));
		if(startType != 5)
		{
			isWrite = m_svcControl->IsChangeable(_T("upnphost"));
		}
		if(startType<4)
		{
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}
		if(startType==5) return NON_EXISTENT;
		if(!isWrite)
			return ACTIVE_NO_PERMISSION;
		return ACTIVE;
	break;
	case ST_SERVICES_MSGSRV:
		startType = m_svcControl->GetStartType(_T("Messenger"));
		if(startType != 5)
		{
			isWrite = m_svcControl->IsChangeable(_T("Messenger"));
		}
		if(startType<4)
		{
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}
		if(startType==5) return NON_EXISTENT;
		if(!isWrite)
			return ACTIVE_NO_PERMISSION;
		return ACTIVE;
	break;
	case ST_SERVICES_FIREWALL:
		if((GetSuiteType() == WINDOWS_XP_HOME || GetSuiteType() == WINDOWS_XP_PROFESSIONAL) && ServiceExist(_T("SharedAccess")))
		{
			startType = m_svcControl->GetStartType(_T("SharedAccess"));
			if(startType != 5)
			{
				isWrite = m_svcControl->IsChangeable(_T("SharedAccess"));
			}
			if(startType<4)
			{
				if(!isWrite)
					return INACTIVE_NO_PERMISSION;
				return INACTIVE;
			}
			if(startType==5) return NON_EXISTENT;
			if(!isWrite)
				return ACTIVE_NO_PERMISSION;
			return ACTIVE;
		}
		else if((GetSuiteType() == WINDOWS_VISTA || GetSuiteType() == WINDOWS_7) && ServiceExist(_T("MpsSvc")))
		{
			startType = m_svcControl->GetStartType(_T("MpsSvc"));
			if(startType != 5)
			{
				isWrite = m_svcControl->IsChangeable(_T("MpsSvc"));
			}
			if(startType<4)
			{
				if(!isWrite)
					return INACTIVE_NO_PERMISSION;
				return INACTIVE;
			}
			if(startType==5) return NON_EXISTENT;
			if(!isWrite)
				return ACTIVE_NO_PERMISSION;
			return ACTIVE;
		}
		return NON_EXISTENT;
	break;
	case ST_SERVICES_WINDOWS_DEFENDER:
		if(GetSuiteType() >= WINDOWS_VISTA && ServiceExist(_T("WinDefend")))
		{
			startType = m_svcControl->GetStartType(_T("WinDefend"));
			if(startType != 5)
			{
				isWrite = m_svcControl->IsChangeable(_T("WinDefend"));
			}
			if(startType<4)
			{
				if(!isWrite)
					return ACTIVE_NO_PERMISSION;
				return ACTIVE;
			}
			if(startType==5) return NON_EXISTENT;
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}
		return NON_EXISTENT;
	break;
	case ST_SERVICES_SEC_CTR:
		startType = m_svcControl->GetStartType(_T("wscsvc"));
		if(startType != 5)
		{
			isWrite = m_svcControl->IsChangeable(_T("wscsvc"));
		}
		if(startType<4)
		{
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}
		if(startType==5) return NON_EXISTENT;
		if(!isWrite)
			return ACTIVE_NO_PERMISSION;
		return ACTIVE;
	break;
	case ST_SERVICES_RPCLOCATOR:
		startType = m_svcControl->GetStartType(_T("RpcLocator"));
		if(startType != 5)
		{
			isWrite = m_svcControl->IsChangeable(_T("RpcLocator"));
		}
		if(startType<4)
		{
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}
		if(startType==5) return NON_EXISTENT;
		if(!isWrite)
			return ACTIVE_NO_PERMISSION;
		return ACTIVE;
	break;
	case ST_SERVICES_INDEXER:
		startType = m_svcControl->GetStartType(_T("CiSvc"));
		if(startType != 5)
		{
			isWrite = m_svcControl->IsChangeable(_T("CiSvc"));
		}
		if(startType<4)
		{
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}
		if(startType==5) return NON_EXISTENT;
		if(!isWrite)
			return ACTIVE_NO_PERMISSION;
		return ACTIVE;
	break;
	case ST_REGSVR_REGWIZC:
//		if(!TestRegKey(HKEY_CLASSES_ROOT,_T("RegWizCtrl.RegWizCtrl\\CurVer")))
		if(!ExistFile(_T("regwizc.dll")))
			return NON_EXISTENT;
		isWrite = TestKeyWritePermission(HKEY_CLASSES_ROOT,_T(".zip"));
		if(TestRegKeyValue(HKEY_CLASSES_ROOT,_T("RegWizCtrl.RegWizCtrl\\CurVer"),_T(""),REG_SZ,0,_T("RegWizCtrl.RegWizCtrl.1")) == MATCH)
		{
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}
		else
		{
			if(!isWrite)
				return ACTIVE_NO_PERMISSION;
			return ACTIVE;
		}
	break;
	case ST_REGSVR_LICDLL:
		if(!ExistFile(_T("licdll.dll")))
			return NON_EXISTENT;
		isWrite = TestKeyWritePermission(HKEY_CLASSES_ROOT,_T(".zip"));
		if(TestRegKey(HKEY_CLASSES_ROOT,_T("Licdll.CLicenseAgent\\CurVer")))
		//if(TestRegKeyValue(HKEY_CLASSES_ROOT,_T("Licdll.CLicenseAgent\\CurVer"),_T(""),REG_SZ,0,_T("CLicenseAgent Class")) == MATCH)
		{
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}
		else
		{
			if(!isWrite)
				return ACTIVE_NO_PERMISSION;
			return ACTIVE;
		}
	break;
	case ST_REGSVR_ZIPFLDR:
		if(!ExistFile(_T("zipfldr.dll")))
			return NON_EXISTENT;
		isWrite = TestKeyWritePermission(HKEY_CLASSES_ROOT,_T(".zip"));
		if(TestRegKeyValue(HKEY_CLASSES_ROOT,_T(".zip"),_T("Content Type"),REG_SZ,0,_T("application/x-zip-compressed"))== MATCH||
			   TestRegKeyValue(HKEY_CLASSES_ROOT,_T(".zip"),_T(""),REG_SZ,0,_T("application/x-zip-compressed"))==MATCH)
		{
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}
		else
		{
			if(!isWrite)
				return ACTIVE_NO_PERMISSION;
			return ACTIVE;
		}
	break;
	case ST_MESSENGER_AUTOSTART:
		retVal = TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"),_T("MSMSGS"),REG_SZ,0,NULL);
		if(retVal == MATCH)	return INACTIVE;
		else if(retVal == 0) return ACTIVE;
	break;
	case ST_MESSENGER_6_1_AUTOSTART:
		retVal = TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"),_T("MsnMsgr"),REG_SZ,0,NULL);
		if(retVal == MATCH) return INACTIVE;
		else if(retVal == NO_MATCH) return ACTIVE;
		else return NON_EXISTENT;
	break;
	case ST_MESSENGER_DEINSTALL:
		retVal = TestRegKeyValue(HKEY_CLASSES_ROOT,_T("CLSID\\{B69003B3-C55E-4b48-836C-BC5946FC3B28}\\LocalServer32"),_T(""),REG_SZ,0,NULL);
		if(retVal == MATCH) return INACTIVE;
		else if(retVal == NO_MATCH) return ACTIVE;
		else return NON_EXISTENT;
	break;
	case ST_SUPPORTUSER:
		if((IsSupportuser()==true)&&(GetSuiteType()==2)) return INACTIVE;
		else return NON_EXISTENT;
	break;
	case ST_BALLOON_TIPS:
		if(TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced"),_T("EnableBalloonTips"),REG_DWORD,0,NULL) == MATCH)
			return ACTIVE;
		else
			return INACTIVE;
	break;
	case ST_CLEAR_PAGEFILE:
		isWrite = TestKeyWritePermission(HKEY_LOCAL_MACHINE,_T("System\\CurrentControlSet\\Control\\Session Manager\\Memory Management"));
		if(TestRegKeyValue(HKEY_LOCAL_MACHINE,_T("System\\CurrentControlSet\\Control\\Session Manager\\Memory Management"),_T("ClearPageFileAtShutdown"),REG_DWORD,1,NULL) == MATCH)
		{
			if(!isWrite)
				return ACTIVE_NO_PERMISSION;
			return ACTIVE;
		}
		else
		{
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}
	break;
	case ST_OE_START_MESSENGER:
		isWrite = TestKeyWritePermission(HKEY_LOCAL_MACHINE,_T("Software\\Microsoft\\Outlook Express"));
		retVal = TestRegKeyValue(HKEY_LOCAL_MACHINE,_T("Software\\Microsoft\\Outlook Express"),_T("Hide Messenger"),REG_DWORD,2,NULL);
		if(retVal == NOT_FOUND && TestRegKey(HKEY_LOCAL_MACHINE, _T("Software\\Microsoft\\Outlook Express")))
		{
			retVal = NO_MATCH;
		}
		if(retVal == MATCH)	
		{
			if(!isWrite)
				return ACTIVE_NO_PERMISSION;
			return ACTIVE;
		}
		else if(retVal == NO_MATCH)
		{
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}
	break;
	case ST_FAST_SHUTDOWN:
		isWrite = TestKeyWritePermission(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Control"));
		retVal = TestRegKeyValue(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Control"),_T("WaitToKillServiceTimeout"),REG_SZ,0,_T("2000"));
		retVal2 = TestRegKeyValue(HKEY_CURRENT_USER,_T("Control Panel\\Desktop"),_T("AutoEndTasks"),REG_SZ,0,_T("1"));
		if(retVal == MATCH)
		{
			if(!isWrite)
				return ACTIVE_NO_PERMISSION;
			return ACTIVE;
		}
		else if(retVal == NO_MATCH)
		{
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}
	break;
	case ST_MAX_CONNECTIONS:
		retVal=TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings"),_T("MaxConnectionsPer1_0Server"),REG_DWORD,10,NULL);
		//int ret2=TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings"),_T("MaxConnectionsPerServer"),REG_DWORD,10,NULL);
		if(retVal == NO_MATCH || retVal == NOT_FOUND)return INACTIVE;
		else return ACTIVE;
	break;
	case ST_OXP_ERRORREPORT:
		if (IsOfficeXpInstalled())
		{
			if((TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Office\\10.0\\Common"),_T("DWNeverUpload"),REG_DWORD,1,NULL) == MATCH)&&
			(TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Office\\10.0\\Common"),_T("DWNoExternalURL"),REG_DWORD,1,NULL) == MATCH)&&
			(TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Office\\10.0\\Common"),_T("DWNoFileCollection"),REG_DWORD,1,NULL) == MATCH)&&
			(TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Office\\10.0\\Common"),_T("DWNoSecondLevelCollection"),REG_DWORD,1,NULL) == MATCH))
				return ACTIVE;
			else
				return INACTIVE;
		}
	break;
	case ST_O2k3_ERRORREPORT:
		if (IsOffice2k3Installed())
		{
			if((TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Office\\11.0\\Common"),_T("DWNeverUpload"),REG_DWORD,1,NULL) == MATCH)&&
			(TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Office\\11.0\\Common"),_T("DWNoExternalURL"),REG_DWORD,1,NULL) == MATCH)&&
			(TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Office\\11.0\\Common"),_T("DWNoFileCollection"),REG_DWORD,1,NULL) == MATCH)&&
			(TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Office\\11.0\\Common"),_T("DWNoSecondLevelCollection"),REG_DWORD,1,NULL) == MATCH))
				return ACTIVE;
			else
				return INACTIVE;
		}
		else if (IsOffice2k7Installed())
		{
			if((TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Office\\12.0\\Common"),_T("DWNeverUpload"),REG_DWORD,1,NULL) == MATCH)&&
			(TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Office\\12.0\\Common"),_T("DWNoExternalURL"),REG_DWORD,1,NULL) == MATCH)&&
			(TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Office\\12.0\\Common"),_T("DWNoFileCollection"),REG_DWORD,1,NULL) == MATCH)&&
			(TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Office\\12.0\\Common"),_T("DWNoSecondLevelCollection"),REG_DWORD,1,NULL) == MATCH))
				return ACTIVE;
			else
				return INACTIVE;
		}
	break;
	case ST_IE_UPLDDRVINFO:
		if(GetSP()>=1) return NON_EXISTENT;
		FILE* file;
		TCHAR strFile[MAX_PATH];
		GetWindowsDirectory(strFile,sizeof(strFile));
		wcscat(strFile,_T("\\PCHealth\\HelpCtr\\System\\DFS\\uplddrvinfo.htm"));
		file = _wfopen(strFile,_T("r"));
		if(!file)
		{
			GetWindowsDirectory(strFile,sizeof(strFile));
			wcscat(strFile,_T("\\PCHealth\\HelpCtr\\System\\DFS\\uplddrvinfo.htm.bak"));
			file = _wfopen(strFile,_T("r"));
			if(file)
			{
				fclose(file);
				return ACTIVE;
			}
			return NON_EXISTENT;
		}
		else fclose(file);
		return INACTIVE;
	break;
	case ST_FIREWALL:
		isWrite = TestKeyWritePermission(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Services\\SharedAccess\\Parameters\\FirewallPolicy\\StandardProfile"));
		retVal=TestRegKeyValue(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Services\\SharedAccess\\Parameters\\FirewallPolicy\\StandardProfile"),_T("EnableFirewall"),REG_DWORD,0,NULL);
		if(retVal == NOT_FOUND)
		{
			if(TestRegKey(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Services\\SharedAccess\\Parameters\\FirewallPolicy\\StandardProfile")))
			{
				if(!isWrite)
					return INACTIVE_NO_PERMISSION;
				return INACTIVE;
			}
		}
		if(retVal == MATCH)
		{
			if(!isWrite)
					return ACTIVE_NO_PERMISSION;
			return ACTIVE;
		}
		if(retVal == NOT_FOUND)
			return NON_EXISTENT;
		else 
		{
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}
	break;
	case ST_SHOW_FILE_EXT:
		if(!TestRegKey(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced")))
			return NON_EXISTENT;
		if(TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced"),_T("HideFileExt"),REG_DWORD,0,NULL) == MATCH)
			return ACTIVE;
		else 
			return INACTIVE;
	break;
	case ST_HIDE_IN_NETWORK:
		if(!TestRegKey(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Services\\LanManServer\\Parameters")))
			return NON_EXISTENT;
		isWrite = TestKeyWritePermission(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Services\\LanManServer\\Parameters"));
		if(TestRegKeyValue(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Services\\LanManServer\\Parameters"),_T("hidden"),REG_DWORD,1,NULL) == MATCH)
		{
			if(!isWrite)
				return ACTIVE_NO_PERMISSION;
			return ACTIVE;
		}
		else 
		{
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}
	break;
	case ST_DENY_REGEDIT:
		{
			if(!TestRegKey(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies")))
				return NON_EXISTENT;
			const HKEY key = HKEY_CURRENT_USER; //chooseHKey(HKEY_LOCAL_MACHINE, HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies"));

			if(TestRegKeyValue(key, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System"),_T("DisableRegistryTools"),REG_DWORD,1,NULL) == MATCH)
				return ACTIVE;
			else 
				return INACTIVE;
		}
	break;
	case ST_SCRIPTING_HOST:
		if(!TestRegKey(HKEY_LOCAL_MACHINE,_T("Software\\Microsoft\\Windows Script Host\\Settings")))
			return NON_EXISTENT;
		isWrite = TestKeyWritePermission(HKEY_LOCAL_MACHINE,_T("Software\\Microsoft\\Windows Script Host\\Settings"));
		if(TestRegKeyValue(HKEY_LOCAL_MACHINE,_T("Software\\Microsoft\\Windows Script Host\\Settings"),_T("Enabled"),REG_DWORD,0,NULL) == MATCH)
		{
			if(!isWrite)
				return ACTIVE_NO_PERMISSION;
			return ACTIVE;
		}
		else 
		{
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}	
	break;
	case ST_SHOW_LAST_USERNAME:
		if(!TestRegKey(HKEY_LOCAL_MACHINE,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System")))
			return NON_EXISTENT;
		isWrite = TestKeyWritePermission(HKEY_LOCAL_MACHINE,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System"));
		if(TestRegKeyValue(HKEY_LOCAL_MACHINE,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System"),_T("DontDisplayLastUserName"),REG_DWORD,1,NULL) == MATCH)
		{
			if(!isWrite)
				return ACTIVE_NO_PERMISSION;
			return ACTIVE;
		}
		else 
		{
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}
	break;
	case ST_IE_WATSON:
		if(!TestRegKey(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Internet Explorer\\Main")))
			return NON_EXISTENT;
		if(TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Internet Explorer\\Main"),_T("IEWatsonDisabled"),REG_DWORD,1,NULL) == MATCH)
			return ACTIVE;
		else 
			return INACTIVE;
	break;
	case ST_IE_CLEAN_CACHE:
		if(!TestRegKey(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Cache")))
			return NON_EXISTENT;
		if(TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Cache"),_T("Persistent"),REG_DWORD,0,NULL) == MATCH)
			return ACTIVE;
		else 
			return INACTIVE;
	break;
	case ST_MEDIAPLAYER_UPDATE:
		//if(!TestRegKey(HKEY_LOCAL_MACHINE,_T("Software\\Policies\\Microsoft\\WindowsMediaPlayer")))
		//	return NON_EXISTENT;
		isWrite = TestKeyWritePermission(HKEY_LOCAL_MACHINE,_T("Software\\Policies\\Microsoft\\WindowsMediaPlayer"));
		if(TestRegKeyValue(HKEY_LOCAL_MACHINE,_T("Software\\Policies\\Microsoft\\WindowsMediaPlayer"),_T("DisableAutoUpdate"),REG_DWORD,1,NULL) == MATCH)
		{
			if(!isWrite)
				return ACTIVE_NO_PERMISSION;
			return ACTIVE;
		}
		else 
		{
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}
	break;
	case ST_LNK_SUFFIX:
		if(!TestRegKey(HKEY_CLASSES_ROOT,_T("lnkfile")))
			return NON_EXISTENT;
		isWrite = TestKeyWritePermission(HKEY_CLASSES_ROOT,_T("lnkfile"));
		if(TestRegKeyValue(HKEY_CLASSES_ROOT,_T("lnkfile"),_T("AlwaysShowExt"),REG_SZ,0,_T("")) == MATCH)
		{
			if(!isWrite)
				return ACTIVE_NO_PERMISSION;
			return ACTIVE;
		}
		else 
		{
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}
	break;
	case ST_PIF_SUFFIX:
		if(!TestRegKey(HKEY_CLASSES_ROOT,_T("piffile")))
			return NON_EXISTENT;
		isWrite = TestKeyWritePermission(HKEY_CLASSES_ROOT,_T("piffile"));
		if(TestRegKeyValue(HKEY_CLASSES_ROOT,_T("piffile"),_T("AlwaysShowExt"),REG_SZ,0,_T("")) == MATCH)
		{
			if(!isWrite)
				return ACTIVE_NO_PERMISSION;
			return ACTIVE;
		}
		else 
		{
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}
	break;
	case ST_SHC_SUFFIX:
		if(!TestRegKey(HKEY_CLASSES_ROOT,_T("SHCmdFile")))
			return NON_EXISTENT;
		isWrite = TestKeyWritePermission(HKEY_CLASSES_ROOT,_T("SHCmdFile"));
		if(TestRegKeyValue(HKEY_CLASSES_ROOT,_T("SHCmdFile"),_T("AlwaysShowExt"),REG_SZ,0,_T("")) == MATCH)
		{
			if(!isWrite)
				return ACTIVE_NO_PERMISSION;
			return ACTIVE;
		}
		else 
		{
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}
	break;
	case ST_SHB_SUFFIX:
		if(!TestRegKey(HKEY_CLASSES_ROOT,_T("DocShortcut")))
			return NON_EXISTENT;
		isWrite = TestKeyWritePermission(HKEY_CLASSES_ROOT,_T("DocShortcut"));
		if(TestRegKeyValue(HKEY_CLASSES_ROOT,_T("DocShortcut"),_T("AlwaysShowExt"),REG_SZ,0,_T("")) == MATCH)
		{
			if(!isWrite)
				return ACTIVE_NO_PERMISSION;
			return ACTIVE;
		}
		else 
		{
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}
	break;
	case ST_URL_SUFFIX:
		if(!TestRegKey(HKEY_CLASSES_ROOT,_T("InternetShortcut")))
			return NON_EXISTENT;
		isWrite = TestKeyWritePermission(HKEY_CLASSES_ROOT,_T("InternetShortcut"));
		if(TestRegKeyValue(HKEY_CLASSES_ROOT,_T("InternetShortcut"),_T("AlwaysShowExt"),REG_SZ,0,_T("")) == MATCH)
		{
			if(!isWrite)
				return ACTIVE_NO_PERMISSION;
			return ACTIVE;
		}
		else 
		{
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}
	break;
	case ST_CD_AUTORUN:
	{
		int type = GetSuiteType();

		if(type == WINDOWS_7)
		{
			const HKEY key = HKEY_CURRENT_USER; //chooseHKey(HKEY_LOCAL_MACHINE, HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies"));
			isWrite = TestKeyWritePermission(key, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\AutoplayHandlers"));
			if(TestRegKeyValue(key,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\AutoplayHandlers"),_T("DisableAutoplay"),REG_DWORD,1,NULL) == MATCH)
			{
				if(!isWrite)
					return ACTIVE_NO_PERMISSION;
				return ACTIVE;
			}
			else 
			{
				if(!isWrite)
					return INACTIVE_NO_PERMISSION;
				return INACTIVE;
			}
		}
		else
		{
			const HKEY key = HKEY_CURRENT_USER; //chooseHKey(HKEY_LOCAL_MACHINE, HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies"));
			isWrite = TestKeyWritePermission(key, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies"));
			if(!TestRegKey(key,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies")))
				return NON_EXISTENT;
			if(TestRegKeyValue(key,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer"),_T("NoDriveAutoRun"),REG_DWORD,0x3ffffff,NULL) == MATCH)
			{
				if(!isWrite)
					return ACTIVE_NO_PERMISSION;
				return ACTIVE;
			}
			else 
			{
				if(!isWrite)
					return INACTIVE_NO_PERMISSION;
				return INACTIVE;
			}
		}

	}

	break;
	case ST_CLEAN_RECENT_DOCS:
		isWrite = TestKeyWritePermission(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies"));
		if(!TestRegKey(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies")))
			return NON_EXISTENT;
		if(TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer"),_T("ClearRecentDocsOnExit"),REG_DWORD,1,NULL) == MATCH)
		{
			if(!isWrite)
				return ACTIVE_NO_PERMISSION;
			return ACTIVE;
		}
		else 
		{
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}
	break;
	case ST_NO_RECENT_DOCS:
		isWrite = TestKeyWritePermission(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies"));
		if(!TestRegKey(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies")))
			return NON_EXISTENT;
		if(TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer"),_T("NoRecentDocsHistory"),REG_DWORD,1,NULL) == MATCH)
		{
			if(!isWrite)
				return ACTIVE_NO_PERMISSION;
			return ACTIVE;
		}
		else 
		{
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}
	break;
	case ST_DONT_REPORT_INFECTION:
		if(!TestRegKey(HKEY_LOCAL_MACHINE,_T("Software\\Microsoft\\RemovalTools\\MRT"), IsWow64()))
			return NON_EXISTENT;
		isWrite = TestKeyWritePermission(HKEY_LOCAL_MACHINE,_T("Software\\Microsoft\\RemovalTools\\MRT"), IsWow64());
		if(TestRegKeyValue(HKEY_LOCAL_MACHINE,_T("Software\\Policies\\Microsoft\\MRT"),_T("DontReportInfectionInformation"),REG_DWORD,1,NULL, IsWow64()) == MATCH)
		{
			if(!isWrite)
				return ACTIVE_NO_PERMISSION;
			return ACTIVE;
		}
		else 
		{
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}
	break;
	case ST_RESTRICTANONYMOUS:
		if(!TestRegKey(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Control\\Lsa")))
			return NON_EXISTENT;
		isWrite = TestKeyWritePermission(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Control\\Lsa"));
		if(TestRegKeyValue(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Control\\Lsa"),_T("restrictanonymous"),REG_DWORD,2,NULL) == MATCH)
		{
			if(!isWrite)
				return ACTIVE_NO_PERMISSION;
			return ACTIVE;
		}
		else 
		{
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}	
	break;
	case ST_NONETCRAWL:
		if(!TestRegKey(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced")))
			return NON_EXISTENT;
		isWrite = TestKeyWritePermission(HKEY_LOCAL_MACHINE,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced"));
		if(TestRegKeyValue(HKEY_LOCAL_MACHINE,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced"),_T("NoNetCrawling"),REG_DWORD,1,NULL) == MATCH)
			{
				if(!isWrite)
					return ACTIVE_NO_PERMISSION;
				return ACTIVE;
			}
			else 
			{
				if(!isWrite)
					return INACTIVE_NO_PERMISSION;
				return INACTIVE;
			}	
	break;
	case ST_MEDIAPLAYER_BLOCKUNINSTALL:
		if(GetMediaPlayerVersion() < 9)
		{
			isWrite = TestKeyWritePermission(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\MediaPlayer"));
			if(TestRegKeyValue(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\MediaPlayer"),_T("BlockUninstall"),REG_SZ,0,_T("no")) == MATCH)
			{
				if(!isWrite)
					return ACTIVE_NO_PERMISSION;
				return ACTIVE;
			}
			else 
			{
				if(!isWrite)
					return INACTIVE_NO_PERMISSION;
				return INACTIVE;
			}	
		}
	break;
	case ST_WMP_PREVENT_LIBRARY_SHARING:
		{
			if(GetMediaPlayerVersion() >= 11)
			{
				isWrite = TestKeyWritePermission(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Policies\\Microsoft"), false);
				if(TestRegKeyValue(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Policies\\Microsoft\\WindowsMediaPlayer"),_T("PreventLibrarySharing"),REG_DWORD,1,NULL, false) == MATCH)
				{
					if(!isWrite)
						return ACTIVE_NO_PERMISSION;
					return ACTIVE;
				}
				else 
				{
					if(!isWrite)
						return INACTIVE_NO_PERMISSION;
					return INACTIVE;
				}	
			}
			else
				return NON_EXISTENT;
		}
	break;
	case ST_NOLMHASH:
		if(!TestRegKey(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Control\\Lsa")))
			return NON_EXISTENT;
		isWrite = TestKeyWritePermission(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Control\\Lsa"));
		if(TestRegKeyValue(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Control\\Lsa"),_T("NoLMHash"),REG_DWORD,1,NULL) == MATCH)
		{
			if(!isWrite)
				return ACTIVE_NO_PERMISSION;
			return ACTIVE;
		}
		else 
		{
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}	
	break;
	case ST_NO_WGA_CHECK:
		if(!TestRegKey(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon\\Notify\\wgalogon")))
		{
			return NON_EXISTENT;
		}
		else
		{
			isWrite = TestKeyWritePermission(HKEY_CLASSES_ROOT,_T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon\\Notify\\wgalogon"));
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}
	break;
	case ST_DO_NOT_ALLOW_IE70:
		if(GetIEVersion() >= 7)
			return NON_EXISTENT;
		isWrite = TestKeyWritePermission(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Internet Explorer"));
		if(TestRegKeyValue(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Internet Explorer\\Setup\\7.0"),_T("DoNotAllowIE70"),REG_DWORD,1,NULL) == MATCH)
		{
			if(!isWrite)
				return ACTIVE_NO_PERMISSION;
			return ACTIVE;
		}
		else
		{
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}
	break;
	case ST_READER_DISABLE_JS:
		//check if the acrobat reader is installed
		switch(GetAdobeReaderVersion())
		{
			case 7:
				if(TestRegKeyValue(HKEY_CURRENT_USER,_T("SOFTWARE\\Adobe\\Acrobat Reader\\7.0\\JSPrefs"),_T("bEnableJS"),REG_DWORD,0,NULL) == MATCH)
					return ACTIVE;
				else
					return INACTIVE;
			case 8:
				if(TestRegKeyValue(HKEY_CURRENT_USER,_T("SOFTWARE\\Adobe\\Acrobat Reader\\8.0\\JSPrefs"),_T("bEnableJS"),REG_DWORD,0,NULL) == MATCH)
					return ACTIVE;
				else
					return INACTIVE;
			case 9:
				if(TestRegKeyValue(HKEY_CURRENT_USER,_T("SOFTWARE\\Adobe\\Acrobat Reader\\9.0\\JSPrefs"),_T("bEnableJS"),REG_DWORD,0,NULL) == MATCH)
					return ACTIVE;
				else
					return INACTIVE;
			default:
				return NON_EXISTENT;

		}
	break;
	case ST_TWEAK_SVC_SUPPORT_CTR:
		startType = m_svcControl->GetStartType(_T("HelpSvc"));
		if(startType != 5)
		{
			isWrite = m_svcControl->IsChangeable(_T("HelpSvc"));
		}
		if(startType<4)
		{
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}
		if(startType==5) return NON_EXISTENT;
		if(!isWrite)
			return ACTIVE_NO_PERMISSION;
		return ACTIVE;
	break;
	case ST_TWEAK_SVC_WmdmPmSN:
		startType = m_svcControl->GetStartType(_T("WmdmPmSN"));
		if(startType != 5)
		{
			isWrite = m_svcControl->IsChangeable(_T("WmdmPmSN"));
		}
		if(startType<4)
		{
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}
		if(startType==5) return NON_EXISTENT;
		if(!isWrite)
			return ACTIVE_NO_PERMISSION;
		return ACTIVE;
	break;
	case ST_TWEAK_COMMAND_HERE:
		isWrite = TestKeyWritePermission(HKEY_CLASSES_ROOT,_T("Directory\\Shell"));
		if(TestRegKey(HKEY_CLASSES_ROOT,_T("Directory\\Shell\\OpenNew")) == true)
		{
			if(!isWrite)
				return ACTIVE_NO_PERMISSION;
			return ACTIVE;
		}
		else
		{
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}
	break;
	case ST_TWEAK_DEL_CTFMON:
		{
			if(!ExistFile(_T("ctfmon.exe")))
				return NON_EXISTENT;
			isWrite = TestKeyWritePermission(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"));
			
			if(TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), _T("ctfmon.exe"), REG_SZ, 0, NULL) != MATCH)
			{
				if(!isWrite)
					return ACTIVE_NO_PERMISSION;
				return ACTIVE;
			}
			else
			{
				if(!isWrite)
					return INACTIVE_NO_PERMISSION;
				return INACTIVE;
			}
		}
	break;
	case ST_TWEAK_CLEANUP_WIZ:
		if(!TestRegKey(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Desktop\\CleanupWiz")))
			return NON_EXISTENT;
		if(TestRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Desktop\\CleanupWiz"), _T("NoRun"), REG_DWORD, 1, _T("")) == MATCH)
		{
			return ACTIVE;
		}
		else
		{
			return INACTIVE;
		}
	break;
	case ST_TWEAK_WINUPDATE_NO_DRIVER:
		{
			isWrite = TestKeyWritePermission(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Policies\\Microsoft\\Windows"));
			if(TestRegKey(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Policies\\Microsoft\\Windows\\DriverSearching")) == true)
			{
				REGKEY_TEST_RESULT res = TestRegKeyValue(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Policies\\Microsoft\\Windows\\DriverSearching"), _T("DontSearchWindowsUpdate"),REG_DWORD, 1, _T(""));
				if(res == MATCH)
				{
					if(!isWrite)
						return ACTIVE_NO_PERMISSION;
					return ACTIVE;
				}
				else
				{
					if(!isWrite)
						return INACTIVE_NO_PERMISSION;
					return INACTIVE;
				}
			}
			else
			{
				if(!isWrite)
					return INACTIVE_NO_PERMISSION;
				return INACTIVE;
			}
		}
	break;
	case ST_TWEAK_WEBSERVICE:
		{
			int type = GetSuiteType();

			if(type == WINDOWS_7)
				isWrite = TestKeyWritePermission(HKEY_CURRENT_USER,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies"));
			else
				isWrite = TestKeyWritePermission(HKEY_CURRENT_USER,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer"));
			
			bool isRegKey;
			if(type == WINDOWS_7)
			{
				isRegKey = TestRegKey(HKEY_CURRENT_USER,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies"));
			}
			else
			{
				isRegKey = TestRegKey(HKEY_CURRENT_USER,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer"));
			}
			if(isRegKey)
			{
				REGKEY_TEST_RESULT res;
				if(type == WINDOWS_7)
				{
					res = TestRegKeyValue(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer"), _T("NoInternetOpenWith"),REG_DWORD, 1, _T(""));
				}
				else
				{
					res = TestRegKeyValue(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer"), _T("InternetOpenWith"),REG_DWORD, 0, _T(""));
				}
				if(res == MATCH)
				{
					if(!isWrite)
						return ACTIVE_NO_PERMISSION;
					return ACTIVE;
				}
				else
				{
					if(!isWrite)
						return INACTIVE_NO_PERMISSION;
					return INACTIVE;
				}
			}
			else
			{
				if(!isWrite)
					return INACTIVE_NO_PERMISSION;
				return INACTIVE;
			}
		}
	break;
	case ST_TWEAK_BSOD_AUTOREBOOT:
		{
			isWrite = TestKeyWritePermission(HKEY_LOCAL_MACHINE,_T("System\\CurrentControlSet\\Control\\CrashControl"));
			if(TestRegKey(HKEY_LOCAL_MACHINE,_T("System\\CurrentControlSet\\Control\\CrashControl")) == true)
			{
				REGKEY_TEST_RESULT res = TestRegKeyValue(HKEY_LOCAL_MACHINE, _T("System\\CurrentControlSet\\Control\\CrashControl"), _T("AutoReboot"),REG_DWORD, 0, _T(""));
				if(res == MATCH)
				{
					if(!isWrite)
						return ACTIVE_NO_PERMISSION;
					return ACTIVE;
				}
				else
				{
					if(!isWrite)
						return INACTIVE_NO_PERMISSION;
					return INACTIVE;
				}
			}
			else
			{
				return NON_EXISTENT;
			}
		}
	break;
	case ST_TWEAK_DEV_MANAGER:
		isWrite = TestKeyWritePermission(HKEY_CLASSES_ROOT,_T("CLSID\\{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\shell"), IsWow64());
		if(TestRegKey(HKEY_CLASSES_ROOT,_T("CLSID\\{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\shell\\DeviceManager\\Command"), IsWow64()) == true)
		{
			if(!isWrite)
				return ACTIVE_NO_PERMISSION;
			return ACTIVE;
		}
		else
		{
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}
	break;
	case ST_TWEAK_NO_WINTOUR:
		if(!IsWinXP())
			return NON_EXISTENT;
		
		if(TestRegKeyValue(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Applets\\Tour"), _T("RunCount"),REG_DWORD, 0, _T("")) == MATCH)
		{
			return ACTIVE;
		}
		else
		{
			return INACTIVE;
		}
	break;
	case ST_NET_NO_RECENT_DOCS:
		isWrite = TestKeyWritePermission(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies"));
		if(TestRegKeyValue(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer"), _T("NoRecentDocsNetHood"),REG_DWORD, 1, _T("")) == MATCH)
		{
			if(!isWrite)
				return ACTIVE_NO_PERMISSION;
			return ACTIVE;
		}
		else
		{
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}
	break;
	case ST_NO_THUMBNAIL_CACHE:
		if(GetSuiteType() >= SUITE_TYPE::WINDOWS_VISTA)
			return NON_EXISTENT;
		if(TestRegKeyValue(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced"), _T("DisableThumbnailCache"),REG_DWORD, 1, _T("")) == MATCH)
		{
			return ACTIVE;
		}
		else
		{
			return INACTIVE;
		}
	break;
	case ST_NO_SEARCH_ASSISTANT:
		if(GetSuiteType() >= SUITE_TYPE::WINDOWS_VISTA)
			return NON_EXISTENT;
		if(TestRegKeyValue(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\CabinetState"), _T("Use Search Asst"),REG_SZ, 0, _T("no")) == MATCH)
		{
			return ACTIVE;
		}
		else
		{
			return INACTIVE;
		}
	break;
	case ST_DISABLE_UAC:
		if(GetSuiteType() < SUITE_TYPE::WINDOWS_VISTA)
			return NON_EXISTENT;
		isWrite = TestKeyWritePermission(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System"));
		if(TestRegKeyValue(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System"), _T("ConsentPromptBehaviorAdmin"),REG_DWORD, 0, NULL) == MATCH)
		{
			if(!isWrite)
				return ACTIVE_NO_PERMISSION;
			return ACTIVE;
		}
		else
		{
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}
	break;
	case ST_NO_UNREAD_MAIL:
		if (!TestRegKey(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\UnreadMail")))
			return NON_EXISTENT;
		if(TestRegKeyValue(HKEY_CURRENT_USER,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\UnreadMail"), _T("MessageExpiryDays"),REG_DWORD, 0, NULL) == MATCH)
		{
			return ACTIVE;
		}
		else
		{
			return INACTIVE;
		}
	break;
	case ST_NO_VDM:
		if(IsWow64())
			return NON_EXISTENT;
		isWrite = TestKeyWritePermission(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Policies\\Microsoft\\Windows"));
		if(TestRegKeyValue(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Policies\\Microsoft\\Windows\\AppCompat"), _T("VDMDisallowed"),REG_DWORD, 1, NULL) == MATCH)
		{
			if(!isWrite)
				return ACTIVE_NO_PERMISSION;
			return ACTIVE;
		}
		else
		{
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}
	break;
	case ST_NO_TEREDO:
		if(GetSuiteType() < SUITE_TYPE::WINDOWS_VISTA)
			return NON_EXISTENT;
		isWrite = TestKeyWritePermission(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Enum\\Root"));
		if(!TestRegKey(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Enum\\Root\\*TEREDO")))
		{
			if(!isWrite)
				return ACTIVE_NO_PERMISSION;
			return ACTIVE;
		}
		else
		{
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}
	break;
	case ST_NO_ISATAP:
		if(GetSuiteType() < SUITE_TYPE::WINDOWS_VISTA)
			return NON_EXISTENT;
		isWrite = TestKeyWritePermission(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Enum\\Root"));
		if(!TestRegKey(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Enum\\Root\\*ISATAP")))
		{
			if(!isWrite)
				return ACTIVE_NO_PERMISSION;
			return ACTIVE;
		}
		else
		{
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}
	break;
	case ST_NO_NCSI:
		if(GetSuiteType() < SUITE_TYPE::WINDOWS_VISTA)
			return NON_EXISTENT;
		isWrite = TestKeyWritePermission(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Services\\NlaSvc\\Parameters\\Internet"));
		if(TestRegKeyValue(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Services\\NlaSvc\\Parameters\\Internet"), _T("EnableActiveProbing"),REG_DWORD, 0, NULL) == MATCH)
		{
			if(!isWrite)
				return ACTIVE_NO_PERMISSION;
			return ACTIVE;
		}
		else
		{
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}
	break;
	case ST_NO_ROOT_CERT:
		if(GetSuiteType() < SUITE_TYPE::WINDOWS_XP_PROFESSIONAL)
			return NON_EXISTENT;
		isWrite = TestKeyWritePermission(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Policies\\Microsoft\\SystemCertificates"));
		if(TestRegKeyValue(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Policies\\Microsoft\\SystemCertificates\\AuthRoot"), _T("DisableRootAutoUpdate"),REG_DWORD, 1, NULL) == MATCH)
		{
			if(!isWrite)
				return ACTIVE_NO_PERMISSION;
			return ACTIVE;
		}
		else
		{
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}
	break;
	case ST_CERT_REVOCATION:
		if(GetSuiteType() < SUITE_TYPE::WINDOWS_XP_PROFESSIONAL)
			return NON_EXISTENT;
		isWrite = TestKeyWritePermission(HKEY_CURRENT_USER,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Internet Settings"));
		if(TestRegKeyValue(HKEY_CURRENT_USER,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Internet Settings"), _T("CertificateRevocation"),REG_DWORD, 1, NULL) == MATCH)
		{
			if(!isWrite)
				return ACTIVE_NO_PERMISSION;
			return ACTIVE;
		}
		else
		{
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}
	break;
	case ST_CERT_TRUST:
		{
			if(GetSuiteType() < SUITE_TYPE::WINDOWS_XP_PROFESSIONAL)
				return NON_EXISTENT;
			isWrite = TestKeyWritePermission(HKEY_CURRENT_USER,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WinTrust\\Trust Providers\\Software Publishing"));
			DWORD val = GetDWORDRegKeyValue(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WinTrust\\Trust Providers\\Software Publishing"),_T("State"));
			val &= 0x200;
			if(val == 0)
			{
				if(!isWrite)
					return ACTIVE_NO_PERMISSION;
				return ACTIVE;
			}
			else if(val == 0x200)
			{
				if(!isWrite)
					return INACTIVE_NO_PERMISSION;
				return INACTIVE;
			}
		}
	break;
	case ST_NO_HTTPS_CACHE:
	{
		if(GetSuiteType() < SUITE_TYPE::WINDOWS_XP_PROFESSIONAL)
			return NON_EXISTENT;
		isWrite = TestKeyWritePermission(HKEY_CURRENT_USER,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Internet Settings"));
		if(TestRegKeyValue(HKEY_CURRENT_USER,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Internet Settings"), _T("DisableCachingOfSSLPages"),REG_DWORD, 1, NULL) == MATCH)
		{
			if(!isWrite)
				return ACTIVE_NO_PERMISSION;
			return ACTIVE;
		}
		else
		{
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}

	}
	case ST_NO_FORCEREBOOT:
	{
		if(GetSuiteType() < SUITE_TYPE::WINDOWS_XP_PROFESSIONAL)
			return NON_EXISTENT;
		isWrite = TestKeyWritePermission(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\\WindowsUpdate\\Auto Update"));
		if(TestRegKeyValue(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\\WindowsUpdate\\Auto Update"), _T("NoAutoRebootWithLoggedOnUsers"),REG_DWORD, 1, NULL) == MATCH)
		{
			if(!isWrite)
				return ACTIVE_NO_PERMISSION;
			return ACTIVE;
		}
		else
		{
			if(!isWrite)
				return INACTIVE_NO_PERMISSION;
			return INACTIVE;
		}

	}
	case ST_TWEAK_NO_SHORTCUT_ARROW:
	{
		if(GetSuiteType() <= SUITE_TYPE::WINDOWS_XP_PROFESSIONAL)
		{
			isWrite = TestKeyWritePermission(HKEY_CLASSES_ROOT,_T("lnkfile"), IsWow64());
			if(TestRegKeyValue(HKEY_CLASSES_ROOT,_T("lnkfile"), _T("IsShortcut"),REG_SZ, 0, NULL, IsWow64()) == MATCH)
			{
				return  isWrite?INACTIVE:INACTIVE_NO_PERMISSION;
			}
			else
			{
				return  isWrite?ACTIVE:ACTIVE_NO_PERMISSION;
			}
		}
		else if(GetSuiteType() >= SUITE_TYPE::WINDOWS_VISTA)
		{
			isWrite = TestKeyWritePermission(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer"), IsWow64());
			if(!TestRegKey(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Icons")))
				return  isWrite?INACTIVE:INACTIVE_NO_PERMISSION;
			
			if(TestRegKeyValue(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Icons"), _T("29"),REG_SZ, 0, NULL, IsWow64()) == MATCH)
			{
				return  isWrite?ACTIVE:ACTIVE_NO_PERMISSION;
			}
			else return isWrite?INACTIVE:INACTIVE_NO_PERMISSION;

		}
	}
	case ST_TWEAK_SVC_BITS:
	{
		if(GetSuiteType() >= SUITE_TYPE::WINDOWS_VISTA)
		{
			startType = m_svcControl->GetStartType(_T("BITS"));
			if(startType != 5)
			{
				isWrite = m_svcControl->IsChangeable(_T("BITS"));
			}
			if(startType<4)
			{
				if(!isWrite)
					return INACTIVE_NO_PERMISSION;
				return INACTIVE;
			}
			if(startType==5) return NON_EXISTENT;
			if(!isWrite)
				return ACTIVE_NO_PERMISSION;
			return ACTIVE;
		}
	}
	case ST_TELEMETRY:
	{
		if (ServiceExist(_T("DiagTrack")))
		{
			isWrite = TestKeyWritePermission(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\DataCollection"), IsWow64());
			if (TestRegKeyValue(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\DataCollection"), _T("AllowTelemetry"), REG_DWORD, 0, NULL, IsWow64()) == MATCH)
			{
				return  isWrite ? ACTIVE : ACTIVE_NO_PERMISSION;
			}
			else
			{
				return  isWrite ? INACTIVE : INACTIVE_NO_PERMISSION;
			}
		}
	}
	case ST_INVENTORY:
	{
		if (GetSuiteType() >= SUITE_TYPE::WINDOWS_10)
		{
			isWrite = TestKeyWritePermission(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Policies\\Microsoft\\Windows\\AppCompat"), IsWow64());
			if (TestRegKeyValue(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Policies\\Microsoft\\Windows\\AppCompat"), _T("DisableInventory"), REG_DWORD, 1, NULL, IsWow64()) == MATCH)
			{
				return  isWrite ? ACTIVE : ACTIVE_NO_PERMISSION;
			}
			else
			{
				return  isWrite ? INACTIVE : INACTIVE_NO_PERMISSION;
			}
		}
	}
	case ST_PERSONALIZE:
	{
		if (GetSuiteType() >= SUITE_TYPE::WINDOWS_10)
		{
			REGKEY_TEST_RESULT res1 = TestRegKeyValue(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\InputPersonalization"), _T("RestrictImplicitInkCollection"), REG_DWORD, 1, NULL, IsWow64());
			REGKEY_TEST_RESULT res2 = TestRegKeyValue(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\InputPersonalization"), _T("RestrictImplicitTextCollection"), REG_DWORD, 1, NULL, IsWow64());
			REGKEY_TEST_RESULT res3 = TestRegKeyValue(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\InputPersonalization\\TrainedDataStore"), _T("HarvestContacts"), REG_DWORD, 0, NULL, IsWow64());
			if(res1==MATCH && res2 == MATCH && res3 == MATCH)
			{
				return  ACTIVE;
			}
			else
			{
				return  INACTIVE;
			}
		}
	}
	case ST_APP_TELEMETRY:
	{
		if (GetSuiteType() >= SUITE_TYPE::WINDOWS_10)
		{
			isWrite = TestKeyWritePermission(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Policies\\Microsoft\\Windows\\AppCompat"), IsWow64());
			if (TestRegKeyValue(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Policies\\Microsoft\\Windows\\AppCompat"), _T("AITEnable"), REG_DWORD, 0, NULL, IsWow64()) == MATCH)
			{
				return  isWrite ? ACTIVE : ACTIVE_NO_PERMISSION;
			}
			else
			{
				return  isWrite ? INACTIVE : INACTIVE_NO_PERMISSION;
			}
		}
	}
	case ST_AD_ID:
	{
		if (GetSuiteType() >= SUITE_TYPE::WINDOWS_10)
		{
			REGKEY_TEST_RESULT res = TestRegKeyValue(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\AdvertisingInfo"), _T("Enabled"), REG_DWORD, 1, NULL, IsWow64());
			if (res == NO_MATCH)
			{
				return  ACTIVE;
			}
			else
			{
				return  INACTIVE;
			}
		}
	}
	case ST_WRITING_BEHAVIOR:
	{
		if (GetSuiteType() >= SUITE_TYPE::WINDOWS_10)
		{
			isWrite = TestKeyWritePermission(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\Input\\TIPC"), IsWow64());
			if (TestRegKeyValue(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Input\\TIPC"), _T("Enabled"), REG_DWORD, 0, NULL, IsWow64()) == MATCH)
			{
				return  isWrite ? ACTIVE : ACTIVE_NO_PERMISSION;
			}
			else
			{
				return  isWrite ? INACTIVE : INACTIVE_NO_PERMISSION;
			}
		}
	}
	case ST_FEEDBACK:
	{
		if (GetSuiteType() >= SUITE_TYPE::WINDOWS_10)
		{
			isWrite = TestKeyWritePermission(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Siuf\\Rules"), IsWow64());
			REGKEY_TEST_RESULT res = TestRegKeyValue(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Siuf\\Rules"), _T("NumberOfSIUFInPeriod"), REG_DWORD, 0, NULL, IsWow64());
			if ( res == MATCH)
			{
				return  isWrite ? ACTIVE : ACTIVE_NO_PERMISSION;
			}
			else
			{
				return  isWrite ? INACTIVE : INACTIVE_NO_PERMISSION;
			}
		}
	}
	case ST_BIOMETRY:
	{
		if (GetSuiteType() >= SUITE_TYPE::WINDOWS_10)
		{
			isWrite = TestKeyWritePermission(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Policies\\Microsoft\\Biometrics"), IsWow64());
			REGKEY_TEST_RESULT res = TestRegKeyValue(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Policies\\Microsoft\\Biometrics"), _T("Enabled"), REG_DWORD, 1, NULL, IsWow64());
			if (res == MATCH || res == NOT_FOUND)
			{
				return  isWrite ? INACTIVE : INACTIVE_NO_PERMISSION;
			}
			else
			{
				return  isWrite ? ACTIVE : ACTIVE_NO_PERMISSION;
			}
		}
	}
	case ST_WEBSEARCH:
	{
		if (GetSuiteType() >= SUITE_TYPE::WINDOWS_10)
		{
			isWrite = TestKeyWritePermission(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Search"), IsWow64());
			if (TestRegKeyValue(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Search"), _T("BingSearchEnabled"), REG_DWORD, 0, NULL, IsWow64()) == MATCH)
			{
				return  isWrite ? ACTIVE : ACTIVE_NO_PERMISSION;
			}
			else
			{
				return  isWrite ? INACTIVE : INACTIVE_NO_PERMISSION;
			}
		}
	}
	case ST_UPDATE_SHARING:
	{
		if (GetSuiteType() >= SUITE_TYPE::WINDOWS_10)
		{
			isWrite = TestKeyWritePermission(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\DeliveryOptimization\\Config"), IsWow64());
			if (TestRegKeyValue(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\DeliveryOptimization\\Config"), _T("DODownloadMode"), REG_DWORD, 0, NULL, IsWow64()) == MATCH)
			{
				return  isWrite ? ACTIVE : ACTIVE_NO_PERMISSION;
			}
			else
			{
				return  isWrite ? INACTIVE : INACTIVE_NO_PERMISSION;
			}
		}
	}
	case ST_LOCATION_PROVIDER:
	{
		if (GetSuiteType() >= SUITE_TYPE::WINDOWS_10)
		{
			isWrite = TestKeyWritePermission(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Policies\\Microsoft\\Windows\\LocationAndSensors"), IsWow64());
			if (TestRegKeyValue(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Policies\\Microsoft\\Windows\\LocationAndSensors"), _T("DisableLocation"), REG_DWORD, 1, NULL, IsWow64()) == MATCH)
			{
				return  isWrite ? ACTIVE : ACTIVE_NO_PERMISSION;
			}
			else
			{
				return  isWrite ? INACTIVE : INACTIVE_NO_PERMISSION;
			}
		}
	}
	case ST_SENSORS:
	{
		if (GetSuiteType() >= SUITE_TYPE::WINDOWS_10)
		{
			isWrite = TestKeyWritePermission(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Policies\\Microsoft\\Windows\\LocationAndSensors"), IsWow64());
			if (TestRegKeyValue(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Policies\\Microsoft\\Windows\\LocationAndSensors"), _T("DisableSensors"), REG_DWORD, 1,NULL, IsWow64()) == MATCH)
			{
				return  isWrite ? ACTIVE : ACTIVE_NO_PERMISSION;
			}
			else
			{
				return  isWrite ? INACTIVE : INACTIVE_NO_PERMISSION;
			}
		}
	}
	case ST_WIFI_SENSE:
	{
		if (GetSuiteType() >= SUITE_TYPE::WINDOWS_10)
		{
			const CString sid = getCurrentUserSID();
			isWrite = TestKeyWritePermission(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\WcmSvc\\wifinetworkmanager\\features\\"+sid), IsWow64());
			REGKEY_TEST_RESULT res = TestRegKeyValue(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\WcmSvc\\wifinetworkmanager\\features\\"+sid), _T("FeatureStates"), REG_DWORD, 828, NULL, IsWow64());
			if(res == MATCH)
			{
				return  isWrite ? ACTIVE : ACTIVE_NO_PERMISSION;
			}
			else
			{
				return  isWrite ? INACTIVE : INACTIVE_NO_PERMISSION;
			}
		}
	}
	case ST_SITE_PREDICTION:
	{
		if (GetSuiteType() >= SUITE_TYPE::WINDOWS_10)
		{
			isWrite = TestKeyWritePermission(HKEY_CURRENT_USER, _T("Software\\Classes\\Local Settings\\Software\\Microsoft\\Windows\\CurrentVersion\\AppContainer\\Storage\\microsoft.microsoftedge_8wekyb3d8bbwe\\MicrosoftEdge\\FlipAhead"), IsWow64());
			REGKEY_TEST_RESULT res = TestRegKeyValue(HKEY_CURRENT_USER, _T("Software\\Classes\\Local Settings\\Software\\Microsoft\\Windows\\CurrentVersion\\AppContainer\\Storage\\microsoft.microsoftedge_8wekyb3d8bbwe\\MicrosoftEdge\\FlipAhead"), _T("FPEnabled"), REG_DWORD, 1, NULL, IsWow64());
			if (res == MATCH || res == NOT_FOUND)
			{
				return  isWrite ? INACTIVE : INACTIVE_NO_PERMISSION;
			}
			else
			{
				return  isWrite ? ACTIVE : ACTIVE_NO_PERMISSION;
			}
		}
	}
	}
	return NON_EXISTENT;
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

const bool CAntiSpySettings::EnableSetting(const UINT setting) const
{
	HKEY hKey;
	HKEY hSubKey;
	DWORD	KeyDisposition;
	bool retVal;
	switch(setting)
	{
	case ST_MEDIAPLAYER_LICENSES:
		if(IsMediaPlayer())return DeleteRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\MediaPlayer\\Preferences"),_T("SilentAcquisition"));
		if(IsMediaPlayer9() || 
			IsMediaPlayer11() || 
			IsMediaPlayer12())
			return SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\MediaPlayer\\Preferences"),_T("SilentAcquisition"),1);//Mediaplayer: Lizenzen nicht automatisch erwerben
	break;
	case ST_MEDIAPLAYER_SEND_GUID:
		return DeleteRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\MediaPlayer\\Preferences"),_T("SendUserGUID"));
	break;
	case ST_MEDIAPLAYER_CODECS:
		if(IsMediaPlayer())return DeleteRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\MediaPlayer\\Preferences"),_T("UpgradeCodecPrompt"));
		if(IsMediaPlayer9() || 
			IsMediaPlayer11() || 
			IsMediaPlayer12())
			return SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\MediaPlayer\\Preferences"),_T("UpgradeCodecPrompt"),1);//Mediaplayer: Automatischen Codecdownload deaktivieren
	break;
	case ST_MP9_ADDMEDIATOLIB:
		return	SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\MediaPlayer\\Preferences"),_T("AutoAddMusicToLibrary"),1);
	break;
	case ST_MP9_RETRIEVEMETADATA:
		return	SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\MediaPlayer\\Preferences"),_T("MetadataRetrieval"),3);
	break;
	case ST_MP9_USAGETRACKING:
		return	SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\MediaPlayer\\Preferences"),_T("UsageTracking"),1);
	break;
	case ST_MP9_SAVEMRU:
		if(GetMediaPlayerVersion() >= 12)
		{
			SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\MediaPlayer\\Preferences"),_T("DisableMRUMusic"),0);
			SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\MediaPlayer\\Preferences"),_T("DisableMRUPictures"),0);
			SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\MediaPlayer\\Preferences"),_T("DisableMRUVideo"),0);
			SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\MediaPlayer\\Preferences"),_T("DisableMRUPlaylists"),0);
			
		}
		else
		{
			return	SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\MediaPlayer\\Preferences"),_T("DisableMRU"),0);
		}
	break;
	case ST_ERROR_REPORTS_SEND:
		return SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\PCHealth\\ErrorReporting"),_T("DoReport"),1);
	break;
//	case ST_ERROR_REPORTS_CRITICAL:
//		return SetRegKeyValue(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\PCHealth\\ErrorReporting"),_T("ShowUI"),_T("\1\0\0\0"),REG_DWORD,4,4);
//	break;
	case ST_REMOTEDESKTOP:
		return SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Control\\Terminal Server"),_T("fDenyTSConnections"),0);
	break;
	case ST_IE_SCHEDULED_UPDATES:
		return SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Webcheck"),_T("NoScheduledUpdates"),0);
	break;
	case ST_IE_CHECK_4_UPDATES:
		return SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Internet Explorer\\Main"),_T("NoUpdateCheck"),0);
	break;
	case ST_IE_FORM_SUGGEST:
		return SetRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Internet Explorer\\Main"),_T("Use FormSuggest"),_T("yes"),REG_SZ,8,4) &&
			   SetRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Internet Explorer\\Main"),_T("FormSuggest Passwords"),_T("yes"),REG_SZ,8,4);
	break;
	case ST_IE_UPLDDRVINFO:
		return SetUpldDrvInfo(true);
	break;
	case ST_TIME_SYNC:
		return SetRegKeyValue(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Services\\W32Time\\Parameters"),_T("Type"),_T("Ntp"),REG_SZ,8,7);
	break;
	case ST_IE_INTEGRATED_AUTHENTICATION:
		return SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings"),_T("EnableNegotiate"),1);
	break;
	case ST_IE_NO_SQM:
		if(GetIEVersion()==7)
		{
			DeleteRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Internet Explorer\\SQM"),_T("DisableCustomerImprovementProgram"));
			return SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Internet Explorer\\SQM"),_T("SqmOptInForIE"),1);
		}
		else if(GetIEVersion()==8 || GetIEVersion()==9)
		{
			DeleteRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Internet Explorer\\SQM"),_T("DisableCustomerImprovementProgram"));
			return SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Internet Explorer\\SQM"),_T("SqmOptInForIE8"),1);
		}
	break;
	case ST_IE_SUGGESTED_SITES:
		if(GetIEVersion()==8 || GetIEVersion()==9)
		{
			return SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Internet Explorer\\Suggested Sites"),_T("Enabled"), 1);
		}
	break;

	case ST_IE_JAVASCRIPT:
		return	SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Zones\\3"),_T("1400"),0)&&
				SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Zones\\3"),_T("1402"),0)&&
				SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Zones\\3"),_T("1407"),0);
	break;
	case ST_IE_ACTIVEX:
		return	SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Zones\\3"),_T("1200"),0)&&
				SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Zones\\3"),_T("1201"),3)&&
				SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Zones\\3"),_T("1001"),1)&&
				SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Zones\\3"),_T("1004"),3)&&
				SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Zones\\3"),_T("1405"),0);
	break;
	case ST_IE_POPUP_BLOCKER:
		if(GetIEVersion()==6)
			return	SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Zones\\3"),_T("1809"),3);
		else if(GetIEVersion()>6)
			return	SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Internet Explorer\\New Windows"),_T("PopupMgr"),0);

	break;
	case ST_IE_PHISHING_FILTER:
		if(GetIEVersion()==7)
			return	SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Internet Explorer\\PhishingFilter"),_T("Enabled"),0);
		else if(GetIEVersion()==8)
			return	SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Internet Explorer\\PhishingFilter"),_T("EnabledV8"),0);
		else if(GetIEVersion()==9)
			return	SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Internet Explorer\\PhishingFilter"),_T("EnabledV9"),0);

	break;
	case ST_REGDONE:
		return DeleteRegKeyValue(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"),_T("RegDone"));
	break;
	case ST_BALLOON_TIPS:
		return DeleteRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced"),_T("EnableBalloonTips"));
	break;
	case ST_OE_START_MESSENGER:
		return DeleteRegKeyValue(HKEY_LOCAL_MACHINE,_T("Software\\Microsoft\\Outlook Express"),_T("Hide Messenger"));
	break;
	case ST_CLEAR_PAGEFILE:
		return SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE,_T("System\\CurrentControlSet\\Control\\Session Manager\\Memory Management"),_T("ClearPageFileAtShutdown"),0);
	break;
	case ST_FAST_SHUTDOWN:
		return SetRegKeyValue(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Control"),_T("WaitToKillServiceTimeout"),_T("20000"),REG_SZ,10,5) &&
		SetRegKeyValue(HKEY_CURRENT_USER,_T("Control Panel\\Desktop"),_T("AutoEndTasks"),_T("0"),REG_SZ,10,4);
	break;
	case ST_MAX_CONNECTIONS:
		return SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings"),_T("MaxConnectionsPer1_0Server"),4) &&
		SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings"),_T("MaxConnectionsPerServer"),4);
	break;
	case ST_OXP_ERRORREPORT:
			return (DeleteRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Office\\10.0\\Common"),_T("DWNeverUpload"))&&
			DeleteRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Office\\10.0\\Common"),_T("DWNoExternalURL"))&&
			DeleteRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Office\\10.0\\Common"),_T("DWNoFileCollection"))&&
			DeleteRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Office\\10.0\\Common"),_T("DWNoSecondLevelCollection")));
		break;
	case ST_O2k3_ERRORREPORT:
		if (IsOffice2k3Installed())
		{
			return (DeleteRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Office\\11.0\\Common"),_T("DWNeverUpload"))&&
			DeleteRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Office\\11.0\\Common"),_T("DWNoExternalURL"))&&
			DeleteRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Office\\11.0\\Common"),_T("DWNoFileCollection"))&&
			DeleteRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Office\\11.0\\Common"),_T("DWNoSecondLevelCollection")));
		}
		else if (IsOffice2k7Installed())
		{
			return (DeleteRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Office\\12.0\\Common"),_T("DWNeverUpload"))&&
			DeleteRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Office\\12.0\\Common"),_T("DWNoExternalURL"))&&
			DeleteRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Office\\12.0\\Common"),_T("DWNoFileCollection"))&&
			DeleteRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Office\\12.0\\Common"),_T("DWNoSecondLevelCollection")));
		}
		break;
	case ST_MESSENGER_AUTOSTART:
		//get the MSMessenger Dir, if installed
		{
			HKEY hKey;
			TCHAR cMSGDir[128];
			unsigned long type = REG_SZ;
			DWORD dwType=REG_SZ;
			DWORD kSize = sizeof(cMSGDir);
			if(RegOpenKeyEx(HKEY_CLASSES_ROOT,_T("CLSID\\{B69003B3-C55E-4b48-836C-BC5946FC3B28}\\LocalServer32"),0,KEY_READ,&hKey)==ERROR_SUCCESS)
			{
				RegQueryValueEx(hKey,_T(""),NULL,&dwType,(LPBYTE)cMSGDir,&kSize);
				RegCloseKey(hKey);
				CString lpszVal(cMSGDir);
				return SetRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"),_T("MSMSGS"),lpszVal,REG_SZ,0,sizeof(cMSGDir));
			}
#ifdef __ENGLISH__
			AfxMessageBox(_T("You don't have the MS Messeger installed, so this function can't be reset."));
#endif
#ifdef __GERMAN__
			AfxMessageBox(_T("Der MS Messenger ist auf diesem System nicht installiert, somit kann diese Funktion nicht rückgängig gemacht werden."));
#endif
		}
		return 0;
	break;
	case ST_SERVICES_ERROR_REPORTS:
		if(GetSuiteType() != WINDOWS_VISTA && GetSuiteType() == WINDOWS_7 && ServiceExist(_T("ERSvc")))
		{
			m_svcControl->SetStartType(2,_T("ERSvc"));
			m_svcControl->StartService(_T("ERSvc"));
		}
		else if((GetSuiteType() == WINDOWS_VISTA || GetSuiteType() == WINDOWS_7) && ServiceExist(_T("WerSvc")))
		{
			m_svcControl->SetStartType(2,_T("WerSvc"));
			m_svcControl->StartService(_T("WerSvc"));
		}
		return true;
	break;
	case ST_SERVICES_AUTO_UPDATES:
		m_svcControl->SetStartType(2,_T("wuauserv"));
		m_svcControl->StartService(_T("wuauserv"));
		return true;
	break;
	case ST_SERVICES_TIMESERVER:
		m_svcControl->SetStartType(2,_T("W32Time"));
		m_svcControl->StartService(_T("W32Time"));
		return true;
	break;
	case ST_SERVICES_TASKSCHEDULER:
		if(GetSuiteType() == WINDOWS_VISTA || GetSuiteType() == WINDOWS_7)
		{
			SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Services\\Schedule"),_T("Start"),2);
		}
		else
			m_svcControl->SetStartType(2,_T("Schedule"));
		m_svcControl->StartService(_T("Schedule"));
		return true;
	break;
	case ST_SERVICES_UPNP:
		m_svcControl->SetStartType(2,_T("upnphost"));
		m_svcControl->StartService(_T("upnphost"));
		return true;
	break;
	case ST_SERVICES_MSGSRV:
		m_svcControl->SetStartType(2,_T("Messenger"));
		m_svcControl->StartService(_T("Messenger"));
		return true;
	break;
	case ST_SERVICES_FIREWALL:
		if((GetSuiteType() == WINDOWS_XP_HOME || GetSuiteType() == WINDOWS_XP_PROFESSIONAL) && ServiceExist(_T("SharedAccess")))
		{
			m_svcControl->SetStartType(2,_T("SharedAccess"));
			m_svcControl->StartService(_T("SharedAccess"));
		}
		else if(GetSuiteType() >= WINDOWS_VISTA && ServiceExist(_T("MpsSvc")))
		{
			m_svcControl->SetStartType(2,_T("MpsSvc"));
			m_svcControl->StartService(_T("MpsSvc"));
		}
		return true;
	break;
	case ST_SERVICES_WINDOWS_DEFENDER:
		if(GetSuiteType() >= WINDOWS_VISTA && ServiceExist(_T("WinDefend")))
		{
			m_svcControl->SetStartType(4,_T("WinDefend"));
			m_svcControl->StopService(_T("WinDefend"));
		}
	break;
	case ST_SERVICES_SEC_CTR:
		
/*		if(GetSuiteType() == WINDOWS_VISTA)
		{
			HKEY hSubKey;
			HKEY hSubSubKey;
			LONG lnReturn = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Microsoft\\Security Center"), 0, KEY_ENUMERATE_SUB_KEYS | KEY_SET_VALUE, &hSubKey);
			//LONG lnReturn = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Microsoft\\Security Center\\Svc"), 0, KEY_ENUMERATE_SUB_KEYS | KEY_SET_VALUE, &hSubKey);

			while (ERROR_SUCCESS == lnReturn) 
			{
				TCHAR szSubKey[MAX_PATH + 1] = { 0 };
				DWORD cbSubkeySize(sizeof(szSubKey));

				lnReturn = ::RegEnumKeyEx(hSubKey, 0, szSubKey, &cbSubkeySize, 0, 0, 0, 0);
				//RegOpenKeyEx(hSubKey, szSubKey, 0, KEY_SET_VALUE | KEY_WRITE, &hSubSubKey);
				if(TestRegKeyValue(hSubKey, szSubKey, _T("EnableNotifications"), REG_DWORD, 0, NULL) == NO_MATCH
				//RegCloseKey(hSubKey);
				if (ERROR_NO_MORE_ITEMS == lnReturn) 
				{
					break;
				}
				else if (ERROR_SUCCESS != lnReturn) 
				{
					//return lnReturn;
				}

				//lnReturn = RecursiveDeleteRegKey(hSubKey, szSubKey);

				if (ERROR_SUCCESS != lnReturn) 
				{
					break;
				}
			}

			// Close (but may not have been there).
			if (hSubKey) 
			{
				::RegCloseKey(hSubKey);
			}

		}
*/		
		//HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Security Center\\Svc\\S-1-5-21-4286161049-2308747925-4068152196-1000\\EnableNotifications
		//HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Security Center\\Svc\\S-1-5-21-4286161049-2308747925-4068152196-1000\\EnableNotificationsRef
		m_svcControl->SetStartType(2,_T("wscsvc"));
		m_svcControl->StartService(_T("wscsvc"));
		return true;
	break;
	case ST_SERVICES_RPCLOCATOR:
		m_svcControl->SetStartType(2,_T("RpcLocator"));
		m_svcControl->StartService(_T("RpcLocator"));
		return true;
	break;
	case ST_SERVICES_INDEXER:
		m_svcControl->SetStartType(2,_T("CiSvc"));
		m_svcControl->StartService(_T("CiSvc"));
		return true;
	break;
	case ST_REGSVR_REGWIZC:
		RegisterDlls(_T("regwizc.dll"));
		return true;
	break;
	case ST_REGSVR_LICDLL:
		RegisterDlls(_T("licdll.dll"));
		return true;
	break;
	case ST_REGSVR_ZIPFLDR:
		RegisterDlls(_T("zipfldr.dll"));
		return true;
	break;
	case ST_FIREWALL:
		return	SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Services\\SharedAccess\\Parameters\\FirewallPolicy\\StandardProfile"),_T("EnableFirewall"),1);
	break;
	case ST_SHOW_FILE_EXT:
		retVal = SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced"),_T("HideFileExt"),1);
		SendMessage(HWND_BROADCAST, WM_SETTINGCHANGE, 0, 0);
		return retVal;
	break;
	case ST_HIDE_IN_NETWORK:
		DeleteRegKeyValue(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Services\\LanManServer\\Parameters"),_T("hidden"));
		return true;
	break;
	case ST_DENY_REGEDIT:
		{
//			const HKEY key = chooseHKey(HKEY_LOCAL_MACHINE, HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies"));
			const HKEY key = HKEY_CURRENT_USER;
			DeleteRegKeyValue(key, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System"),_T("DisableRegistryTools"));
			return true;
		}
	break;
	case ST_SCRIPTING_HOST:
		DeleteRegKeyValue(HKEY_LOCAL_MACHINE,_T("Software\\Microsoft\\Windows Script Host\\Settings"),_T("Enabled"));
		return true;
	break;
	case ST_SHOW_LAST_USERNAME:
		SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System"),_T("DontDisplayLastUserName"),0);
		return true;
	break;
	case ST_IE_WATSON:
		DeleteRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Internet Explorer\\Main"),_T("IEWatsonDisabled"));
		return true;
	break;
	case ST_IE_CLEAN_CACHE:
		SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Cache"),_T("Persistent"),1);
		return true;
	break;
	case ST_MEDIAPLAYER_UPDATE:
		DeleteRegKeyValue(HKEY_LOCAL_MACHINE,_T("Software\\Microsoft\\MediaPlayer\\PlayerUpgrade"),_T("AskMeAgain"));
		DeleteRegKeyValue(HKEY_LOCAL_MACHINE,_T("Software\\Policies\\Microsoft\\WindowsMediaPlayer"),_T("DisableAutoUpdate"));
		return true;
	break;
	case ST_LNK_SUFFIX:
		SetRegKeyValue(HKEY_CLASSES_ROOT,_T("lnkfile"),_T("NeverShowExt"),_T(""),REG_SZ,7,0);
		DeleteRegKeyValue(HKEY_CLASSES_ROOT,_T("lnkfile"),_T("AlwaysShowExt"));
		SendMessage(HWND_BROADCAST, WM_SETTINGCHANGE, 0, 0);
		return true;
	break;
	case ST_PIF_SUFFIX:
		SetRegKeyValue(HKEY_CLASSES_ROOT,_T("piffile"),_T("NeverShowExt"),_T(""),REG_SZ,7,0);
		DeleteRegKeyValue(HKEY_CLASSES_ROOT,_T("piffile"),_T("AlwaysShowExt"));
		SendMessage(HWND_BROADCAST, WM_SETTINGCHANGE, 0, 0);
		return true;
	break;
	case ST_SHC_SUFFIX:
		SetRegKeyValue(HKEY_CLASSES_ROOT,_T("SHCmdFile"),_T("NeverShowExt"),_T(""),REG_SZ,7,0);
		DeleteRegKeyValue(HKEY_CLASSES_ROOT,_T("SHCmdFile"),_T("AlwaysShowExt"));
		SendMessage(HWND_BROADCAST, WM_SETTINGCHANGE, 0, 0);
		return true;
	break;
	case ST_SHB_SUFFIX:
		SetRegKeyValue(HKEY_CLASSES_ROOT,_T("DocShortcut"),_T("NeverShowExt"),_T(""),REG_SZ,7,0);
		DeleteRegKeyValue(HKEY_CLASSES_ROOT,_T("DocShortcut"),_T("AlwaysShowExt"));
		SendMessage(HWND_BROADCAST, WM_SETTINGCHANGE, 0, 0);
		return true;
	case ST_URL_SUFFIX:
		SetRegKeyValue(HKEY_CLASSES_ROOT,_T("InternetShortcut"),_T("NeverShowExt"),_T(""),REG_SZ,7,0);
		DeleteRegKeyValue(HKEY_CLASSES_ROOT,_T("InternetShortcut"),_T("AlwaysShowExt"));
		SendMessage(HWND_BROADCAST, WM_SETTINGCHANGE, 0, 0);
		return true;
	break;
	case ST_CD_AUTORUN:
		{
			int type = GetSuiteType();
			if(type == WINDOWS_7)
			{
				SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\AutoplayHandlers"),_T("DisableAutoplay"),0);
			}
			else
			{
				HKEY key = HKEY_CURRENT_USER; //chooseHKey(HKEY_LOCAL_MACHINE, HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer"));
				DeleteRegKeyValue(key,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer"),_T("NoDriveAutoRun"));
				DeleteRegKeyValue(key,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer"),_T("HonorAutorunSetting"));
				DeleteRegKeyValue(key,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer"),_T("NoDriveTypeAutoRun"));
				//and agin with HKLM
				key = HKEY_LOCAL_MACHINE;
				DeleteRegKeyValue(key,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer"),_T("NoDriveAutoRun"));
				DeleteRegKeyValue(key,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer"),_T("HonorAutorunSetting"));
				DeleteRegKeyValue(key,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer"),_T("NoDriveTypeAutoRun"));
			}
			return true;
		}
	break;
	case ST_CLEAN_RECENT_DOCS:
		DeleteRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer"),_T("ClearRecentDocsOnExit"));
		return true;
	break;
	case ST_NO_RECENT_DOCS:
		DeleteRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer"),_T("NoRecentDocsHistory"));
		DeleteRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer"),_T("NoRecentDocsMenu"));
		return true;
	break;
	case ST_DONT_REPORT_INFECTION:
		DeleteRegKeyValue(HKEY_LOCAL_MACHINE,_T("Software\\Policies\\Microsoft\\MRT"),_T("DontReportInfectionInformation"), IsWow64());
		return true;
	break;
	case ST_MP_START_IN_MEDIA_GUIDE:
		SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\MediaPlayer\\Preferences"),_T("StartInMediaGuide"),1);
		return true;
	break;
	case ST_RESTRICTANONYMOUS:
		SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Control\\Lsa"),_T("restrictanonymous"),0);
		return true;
	break;
	case ST_NONETCRAWL:
		SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced"),_T("NoNetCrawling"),0);
		return true;
	break;
	case ST_MEDIAPLAYER_BLOCKUNINSTALL:
		SetRegKeyValue(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\MediaPlayer"),_T("BlockUninstall"),_T("yes"),REG_SZ,7,0);
	break;
	case ST_WMP_PREVENT_LIBRARY_SHARING:
		SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Policies\\Microsoft\\WindowsMediaPlayer"),_T("PreventLibrarySharing"),0, false);
	break;
	case ST_NOLMHASH:
		SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Control\\Lsa"),_T("NoLMHash"),0);
	break;
	case ST_DO_NOT_ALLOW_IE70:
		SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Internet Explorer\\Setup\\7.0"),_T("DoNotAllowIE70"),0);
	break;
	case ST_READER_DISABLE_JS:
		if(GetAdobeReaderVersion() == 7)
		{
				RecursiveDeleteRegKey(HKEY_CURRENT_USER, "SOFTWARE\\Adobe\\Acrobat Reader\\7.0\\JSPrefs");
		}
		else if(GetAdobeReaderVersion() == 8)
		{
				RecursiveDeleteRegKey(HKEY_CURRENT_USER, "SOFTWARE\\Adobe\\Acrobat Reader\\8.0\\JSPrefs");
		}
		else if(GetAdobeReaderVersion() == 8)
		{
				RecursiveDeleteRegKey(HKEY_CURRENT_USER, "SOFTWARE\\Adobe\\Acrobat Reader\\8.0\\JSPrefs");
		}
	break;
	case ST_TWEAK_SVC_SUPPORT_CTR:
		m_svcControl->SetStartType(2,_T("HelpSvc"));
		m_svcControl->StartService(_T("HelpSvc"));
		return true;
	break;
	case ST_TWEAK_SVC_WmdmPmSN:
		m_svcControl->SetStartType(2,_T("WmdmPmSN"));
		m_svcControl->StartService(_T("WmdmPmSN"));
		return true;
	break;
	case ST_TWEAK_COMMAND_HERE:
		return RecursiveDeleteRegKey(HKEY_CLASSES_ROOT,_T("Directory\\Shell\\OpenNew"));
	break;
	case ST_TWEAK_DEL_CTFMON:
		{
			TCHAR windir[MAX_PATH];
			GetWindowsDirectory(windir, MAX_PATH);
			
			CString tmpSrc, tmpDest;

			tmpSrc.Format(_T("%s\\system32\\dllcache\\ctfmon.exe.backup"), windir);
			tmpDest.Format(_T("%s\\system32\\dllcache\\ctfmon.exe"), windir);
			CopyFile(tmpSrc, tmpDest, FALSE);
			DeleteFile(tmpSrc);

			tmpSrc.Format(_T("%s\\system32\\ctfmon.exe.backup"), windir);
			tmpDest.Format(_T("%s\\system32\\ctfmon.exe"), windir);
			CopyFile(tmpSrc, tmpDest, FALSE);
			DeleteFile(tmpSrc);
			
			if(ExistFile(_T("msctf.dll")))
				RegisterDlls(_T("msctf.dll"));
			if(ExistFile(_T("msimtf.dll")))
				RegisterDlls(_T("msimtf.dll"));

			CString tmp;
			tmp.Format(_T("\"%s\\system32\\ctfmon.exe\""), windir);
			SetRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"),_T("ctfmon.exe"), tmp,REG_SZ,4,tmp.GetLength());
			
			SHELLEXECUTEINFO seInfo;
			ZeroMemory(&seInfo, sizeof(SHELLEXECUTEINFO));
			seInfo.cbSize=sizeof(SHELLEXECUTEINFO);
			seInfo.nShow = SW_HIDE;
			seInfo.lpFile = _T("ctfmon.exe");
			ShellExecuteEx(&seInfo);
			Sleep(2000);
			
			SendMessage(HWND_BROADCAST, WM_SETTINGCHANGE, 0, 0);
			return true;
		}
	break;
	case ST_TWEAK_CLEANUP_WIZ:
		return DeleteRegKeyValue(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Desktop\\CleanupWiz"), _T("NoRun"));
	break;
	case ST_TWEAK_WINUPDATE_NO_DRIVER:
		return DeleteRegKeyValue(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Policies\\Microsoft\\Windows\\DriverSearching"), _T("DontSearchWindowsUpdate"));
	break;
	case ST_TWEAK_WEBSERVICE:
		{
			int type = GetSuiteType();
			if(type == WINDOWS_7)
				return DeleteRegKeyValue(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer"), _T("NoInternetOpenWith"));
			else
				return DeleteRegKeyValue(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer"), _T("InternetOpenWith"));
		}
	break;
	case ST_TWEAK_BSOD_AUTOREBOOT:
		return SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE,_T("System\\CurrentControlSet\\Control\\CrashControl"),_T("AutoReboot"),1);
	break;
	case ST_TWEAK_DEV_MANAGER:
		RecursiveDeleteRegKey(HKEY_CLASSES_ROOT,_T("CLSID\\{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\shell\\DeviceManager"), IsWow64());
		return DeleteRegKey(HKEY_CLASSES_ROOT, _T("CLSID\\{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\shell\\"),_T("DeviceManager"), IsWow64());
		 
	break;
	case ST_TWEAK_NO_WINTOUR:
		DeleteRegKeyValue(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Applets\\Tour"), _T("RunCount"));
	break;
	case ST_NET_NO_RECENT_DOCS:
		return DeleteRegKeyValue(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer"), _T("NoRecentDocsNetHood"));
	break;
	case ST_NO_THUMBNAIL_CACHE:
		return SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced"),_T("DisableThumbnailCache"),0);
	break;
	case ST_NO_SEARCH_ASSISTANT:
		return DeleteRegKeyValue(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\CabinetState"), _T("Use Search Asst"));
	break;
	case ST_DISABLE_UAC:
		return SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System"), _T("ConsentPromptBehaviorAdmin"), 2);
	break;
	case ST_NO_UNREAD_MAIL:
		return DeleteRegKeyValue(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\UnreadMail"), _T("MessageExpiryDays"));
	break;
	case ST_NO_VDM:
	{
		return DeleteRegKeyValue(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Policies\\Microsoft\\Windows\\AppCompat"), _T("VDMDisallowed"));
	}
	break;
	case ST_NO_TEREDO:
		{
			SHELLEXECUTEINFO seInfo;
			ZeroMemory(&seInfo, sizeof(SHELLEXECUTEINFO));
			seInfo.cbSize=sizeof(SHELLEXECUTEINFO);
			seInfo.nShow = SW_HIDE;
			seInfo.lpFile = _T("netsh");
			seInfo.lpParameters = _T("interface ipv6 set teredo client");
			ShellExecuteEx(&seInfo);
			Sleep(2000);

			return true;
		}
	break;
	case ST_NO_ISATAP:
		{
			SHELLEXECUTEINFO seInfo;
			ZeroMemory(&seInfo, sizeof(SHELLEXECUTEINFO));
			seInfo.cbSize=sizeof(SHELLEXECUTEINFO);
			seInfo.nShow = SW_HIDE;
			seInfo.lpFile = _T("netsh");
			seInfo.lpParameters = _T("interface isatap set state enabled");
			ShellExecuteEx(&seInfo);
			Sleep(2000);

			return true;
		}
	break;
	case ST_NO_NCSI:
		return SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Services\\NlaSvc\\Parameters\\Internet"),_T("EnableActiveProbing"),1);

	case ST_NO_ROOT_CERT:
		return DeleteRegKeyValue(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Policies\\Microsoft\\SystemCertificates\\AuthRoot"), _T("DisableRootAutoUpdate"));
	
	case ST_CERT_REVOCATION:
		return SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Internet Settings"), _T("CertificateRevocation"),0);

	case ST_CERT_TRUST:
		{
			DWORD oldVal = GetDWORDRegKeyValue(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WinTrust\\Trust Providers\\Software Publishing"),_T("State"));
			if((oldVal & 0x200) == 0)
			{
				DWORD newVal = oldVal + 0x200;
				return SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WinTrust\\Trust Providers\\Software Publishing"), _T("State"),newVal);
			}
		}
	break;
	
	case ST_NO_HTTPS_CACHE:
		return SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Internet Settings"), _T("DisableCachingOfSSLPages"),0);

	case ST_NO_FORCEREBOOT:
		return DeleteRegKeyValue(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\\WindowsUpdate\\Auto Update"), _T("NoAutoRebootWithLoggedOnUsers")) &&
			DeleteRegKeyValue(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Policies\\Microsoft\\Windows\\WindowsUpdate\\AU"), _T("NoAutoRebootWithLoggedOnUsers"));


	case ST_TWEAK_NO_SHORTCUT_ARROW:
		{
			if(GetSuiteType() <= SUITE_TYPE::WINDOWS_XP_PROFESSIONAL)
			{
				return 	SetRegKeyValue(HKEY_CLASSES_ROOT,_T("lnkfile"),_T("IsShortcut"), _T(""),REG_SZ,4,2, IsWow64());

			}
			else if(GetSuiteType() >= SUITE_TYPE::WINDOWS_VISTA)
			{

				return DeleteRegKeyValue(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Icons"),_T("29"), IsWow64());
			}
			SendMessage(HWND_BROADCAST, WM_SETTINGCHANGE, SPI_SETNONCLIENTMETRICS, (LPARAM)_T("WindowMetrics"));
			return true;
		}
		case ST_TWEAK_SVC_BITS:
		{
			m_svcControl->SetStartType(2,_T("BITS"));
			m_svcControl->StartService(_T("BITS"));
			return true;

		}
		case ST_TELEMETRY:
		{
			if (ServiceExist(_T("DiagTrack")))
			{
				m_svcControl->SetStartType(2, _T("DiagTrack"));
				m_svcControl->StartService(_T("DiagTrack"));
			}
			if (ServiceExist(_T("dmwappushservice")))
			{
				m_svcControl->SetStartType(2, _T("dmwappushservice"));
				m_svcControl->StartService(_T("dmwappushservice"));
			}

			//			return SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\DataCollection"), _T("AllowTelemetry"), 3);
			return DeleteRegKeyValue(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\DataCollection"), _T("AllowTelemetry"), IsWow64());

		}
		case ST_INVENTORY:
		{
			
			//			return SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\DataCollection"), _T("AllowTelemetry"), 3);
			return DeleteRegKeyValue(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Policies\\Microsoft\\Windows\\AppCompat"), _T("DisableInventory"), IsWow64());

		}
		case ST_PERSONALIZE:
		{
			return DeleteRegKeyValue(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\InputPersonalization"), _T("RestrictImplicitInkCollection"), IsWow64()) &&
				DeleteRegKeyValue(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\InputPersonalization"), _T("RestrictImplicitTextCollection"), IsWow64()) &&
				DeleteRegKeyValue(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\InputPersonalization\\TrainedDataStore"), _T("HarvestContacts"), IsWow64());


		}
		case ST_APP_TELEMETRY:
		{
			
			return SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Policies\\Microsoft\\Windows\\AppCompat"), _T("AITEnable"), 1);

		}
		case ST_AD_ID:
		{
			
			return SetDWORDRegKeyValue(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\AdvertisingInfo"), _T("Enabled"), 1);

		}
		case ST_WRITING_BEHAVIOR:
		{
			
			return SetDWORDRegKeyValue(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Input\\TIPC"), _T("Enabled"), 1);

		}
		case ST_FEEDBACK:
		{
			return DeleteRegKeyValue(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Siuf\\Rules"), _T("NumberOfSIUFInPeriod"), IsWow64());
		
	
		}
		case ST_BIOMETRY:
		{
			return SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Policies\\Microsoft\\Biometrics"), _T("Enabled"), 1);
	
		}
		case ST_WEBSEARCH:
		{
			return DeleteRegKeyValue(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Search"), _T("BingSearchEnabled"),IsWow64());
	
		}
		case ST_UPDATE_SHARING:
		{
			const bool ret = SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\DeliveryOptimization\\Config"), _T("DODownloadMode"), 3);
			return ret;
		}
		case ST_LOCATION_PROVIDER:
		{
			m_svcControl->StopService(_T("lfsvc"));
			m_svcControl->SetStartType(4,_T("lfsvc"));
			return SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Policies\\Microsoft\\Windows\\LocationAndSensors"), _T("DisableLocation"), 0);
		}
		case ST_SENSORS:
		{
				return SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Policies\\Microsoft\\Windows\\LocationAndSensors"), _T("DisableSensors"), 0);
		}
		case ST_WIFI_SENSE:
		{
			const bool ret = SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\WcmSvc\\wifinetworkmanager\\features\\" + getCurrentUserSID()), _T("FeatureStates"), 829);
			DeleteRegKeyValue(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\WcmSvc\\wifinetworkmanager\\features"), _T("WiFiSenseCredShared"), IsWow64());
			DeleteRegKeyValue(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\WcmSvc\\wifinetworkmanager\\features"), _T("WiFiSenseOpen"), IsWow64());
			m_svcControl->StopService(_T("WcmSvc"));
			m_svcControl->StartService(_T("WcmSvc"));
			return ret;
		}
		case ST_SITE_PREDICTION:
		{
			const bool ret = SetDWORDRegKeyValue(HKEY_CURRENT_USER, _T("Software\\Classes\\Local Settings\\Software\\Microsoft\\Windows\\CurrentVersion\\AppContainer\\Storage\\microsoft.microsoftedge_8wekyb3d8bbwe\\MicrosoftEdge\\FlipAhead"), _T("FPEnabled"), 1);
			return ret;
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
const bool CAntiSpySettings::DisableSetting(const UINT setting) const
{
	UINT errCode=0;
	HKEY hKey;
	HKEY hSubKey;
	DWORD	KeyDisposition;
	bool retVal;
	switch(setting)
	{
	case ST_MEDIAPLAYER_LICENSES:
		if(IsMediaPlayer9() || 
			IsMediaPlayer() || 
			IsMediaPlayer11() || 
			IsMediaPlayer12())
			return SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\MediaPlayer\\Preferences"),_T("SilentAcquisition"),0);
	break;
	case ST_MEDIAPLAYER_SEND_GUID:
		return SetBINARYRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\MediaPlayer\\Preferences"),_T("SendUserGUID"),0);//Mediaplayer: Identifikation des Players durch Internetsites nicht zulassen
	break;
	case ST_MEDIAPLAYER_CODECS:
		if(IsMediaPlayer())
			return SetBINARYRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\MediaPlayer\\Preferences"),_T("UpgradeCodecPrompt"),0);//Mediaplayer: Automatischen Codecdownload deaktivieren
		if(IsMediaPlayer9() || IsMediaPlayer11() || IsMediaPlayer12())
			return SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\MediaPlayer\\Preferences"),_T("UpgradeCodecPrompt"),0);//Mediaplayer: Automatischen Codecdownload deaktivieren
	break;
	case ST_MP9_ADDMEDIATOLIB:
		return	SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\MediaPlayer\\Preferences"),_T("AutoAddMusicToLibrary"),0);
	break;
	case ST_MP9_RETRIEVEMETADATA:
		return	SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\MediaPlayer\\Preferences"),_T("MetadataRetrieval"),0);
	break;
	case ST_MP9_USAGETRACKING:
		return	SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\MediaPlayer\\Preferences"),_T("UsageTracking"),0);
	break;
	case ST_MP9_SAVEMRU:
		if(GetMediaPlayerVersion() >= 12)
		{
			SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\MediaPlayer\\Preferences"),_T("DisableMRUMusic"),1);
			SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\MediaPlayer\\Preferences"),_T("DisableMRUPictures"),1);
			SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\MediaPlayer\\Preferences"),_T("DisableMRUVideo"),1);
			SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\MediaPlayer\\Preferences"),_T("DisableMRUPlaylists"),1);
		}
		else
		{
			return	SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\MediaPlayer\\Preferences"),_T("DisableMRU"),1);
		}

	break;
	case ST_ERROR_REPORTS_SEND:
		return SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\PCHealth\\ErrorReporting"),_T("DoReport"),0);//Fehlerberichterstattung ausschalten
	break;
//	case ST_ERROR_REPORTS_CRITICAL:
//		return SetRegKeyValue(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\PCHealth\\ErrorReporting"),_T("ShowUI"),_T("\0\0\0\0"),REG_DWORD,4,4);//Fehlerberichte: auch nicht bei schwerwiegenden fehlern
//	break;
	case ST_REMOTEDESKTOP:
		return SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Control\\Terminal Server"),_T("fDenyTSConnections"),1);//Remotedesktopunterstützung ausschalten
	break;
	case ST_IE_SCHEDULED_UPDATES:
		return SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Webcheck"),_T("NoScheduledUpdates"),1);//Webcheck: Keine automatischen updates
	break;
	case ST_IE_UPLDDRVINFO:
		return SetUpldDrvInfo(false);
	break;
	case ST_IE_CHECK_4_UPDATES:
		return SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Internet Explorer\\Main"),_T("NoUpdateCheck"),1);//IE6: Keine überprüfung auf Updates
	break;
	case ST_IE_FORM_SUGGEST:
		return SetRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Internet Explorer\\Main"),_T("Use FormSuggest"),_T("no"),REG_SZ,8,3) &&
			   SetRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Internet Explorer\\Main"),_T("FormSuggest Passwords"),_T("no"),REG_SZ,8,3) &&
			   SetRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\AutoComplete"),_T("AutoSuggest"),_T("no"),REG_SZ,8,3);
	break;
	case ST_IE_JAVASCRIPT:
		return	SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Zones\\3"),_T("1400"),3)&&
				SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Zones\\3"),_T("1402"),3)&&
				SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Zones\\3"),_T("1407"),3);
	break;
	case ST_IE_ACTIVEX:
		return	SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Zones\\3"),_T("1200"),3)&&
				SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Zones\\3"),_T("1201"),3)&&
				SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Zones\\3"),_T("1001"),3)&&
				SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Zones\\3"),_T("1004"),3)&&
				SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Zones\\3"),_T("1405"),3);
	break;
	case ST_IE_POPUP_BLOCKER:
		if(GetIEVersion()==6)
			return	SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Zones\\3"),_T("1809"),0);
		else if(GetIEVersion()>6)
			return	SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Internet Explorer\\New Windows"),_T("PopupMgr"),1);
	case ST_IE_PHISHING_FILTER:
	{
		int access = KEY_ALL_ACCESS | (IsWow64()?KEY_WOW64_64KEY:0);
		if(RegOpenKeyEx(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Internet Explorer\\PhishingFilter"),0,access,&hKey)!=ERROR_SUCCESS)
		{
			RegOpenKeyEx(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Internet Explorer"),0,access,&hKey);
			RegCreateKeyEx(hKey,_T("PhishingFilter"),NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_CREATE_SUB_KEY,NULL,&hSubKey,&KeyDisposition);
			RegCloseKey(hKey);
		}
		if(GetIEVersion()==7)
		{
			return	SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Internet Explorer\\PhishingFilter"),_T("Enabled"),2);
		}
		else if(GetIEVersion()==8)
		{
			return	SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Internet Explorer\\PhishingFilter"),_T("EnabledV8"),1);
		}
		else if(GetIEVersion()==9)
		{
			return	SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Internet Explorer\\PhishingFilter"),_T("EnabledV9"),1);
		}
	break;
	}
	case ST_TIME_SYNC:
		return SetRegKeyValue(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Services\\W32Time\\Parameters"),_T("Type"),_T("NoSync"),REG_SZ,8,7);//Internetzeit nicht automatisch synchronisieren
	break;
	case ST_IE_INTEGRATED_AUTHENTICATION:
		return SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings"),_T("EnableNegotiate"),0);//Integrierte Windows Authentifizierung deaktivieren
	break;
	case ST_IE_NO_SQM:
		if(GetIEVersion()==7)
		{
			SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Internet Explorer\\SQM"),_T("DisableCustomerImprovementProgram"), 1);
			return SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Internet Explorer\\SQM"),_T("SqmOptInForIE"),0);
		}
		else if(GetIEVersion()==8)
		{
			SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Internet Explorer\\SQM"),_T("DisableCustomerImprovementProgram"), 1);
			return SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Internet Explorer\\SQM"),_T("SqmOptInForIE8"),0);
		}
	break;
	case ST_IE_SUGGESTED_SITES:
		if(GetIEVersion()==8 || GetIEVersion()==9)
		{
			return SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Internet Explorer\\Suggested Sites"),_T("Enabled"), 0);
		}
	break;
	case ST_REGDONE:
		return SetRegKeyValue(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"),_T("RegDone"),_T("1"),REG_SZ,10,2);//Regdone auf 1 setzen
	break;
	case ST_ALEXA:
		return DeleteRegKey(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Internet Explorer\\Extensions"),_T("{c95fe080-8f5d-11d2-a20b-00aa003c157a}"));
	break;
	case ST_BALLOON_TIPS:
		return SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced"),_T("EnableBalloonTips"),0);
	break;
	case ST_CLEAR_PAGEFILE:
		return SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE,_T("System\\CurrentControlSet\\Control\\Session Manager\\Memory Management"),_T("ClearPageFileAtShutdown"),1);
	break;
	case ST_OE_START_MESSENGER:
		return SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE,_T("Software\\Microsoft\\Outlook Express"),_T("Hide Messenger"),2);
	break;
	case ST_FAST_SHUTDOWN:
		return SetRegKeyValue(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Control"),_T("WaitToKillServiceTimeout"),_T("2000"),REG_SZ,10,4) &&
		SetRegKeyValue(HKEY_CURRENT_USER,_T("Control Panel\\Desktop"),_T("AutoEndTasks"),_T("1"),REG_SZ,10,4);
	break;
	case ST_MAX_CONNECTIONS:
		return SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings"),_T("MaxConnectionsPer1_0Server"),10) &&
		SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings"),_T("MaxConnectionsPerServer"),10);
	break;
	case ST_SERVICES_ERROR_REPORTS:
		if(GetSuiteType() < WINDOWS_VISTA && ServiceExist(_T("ERSvc")))
		{
			m_svcControl->SetStartType(4,_T("ERSvc"));
			m_svcControl->StopService(_T("ERSvc"));
		}
		else if((GetSuiteType() == WINDOWS_VISTA || GetSuiteType() == WINDOWS_7) && ServiceExist(_T("WerSvc")))
		{
			m_svcControl->SetStartType(4,_T("WerSvc"));
			m_svcControl->StopService(_T("WerSvc"));
		}
		return true;
	break;
	case ST_SERVICES_AUTO_UPDATES:
		m_svcControl->SetStartType(4,_T("wuauserv"));
		m_svcControl->StopService(_T("wuauserv"));
		return true;
	break;
	case ST_SERVICES_TIMESERVER:
		m_svcControl->SetStartType(4,_T("W32Time"));
		m_svcControl->StopService(_T("W32Time"));
		return true;
	break;
	case ST_SERVICES_TASKSCHEDULER:
		if(GetSuiteType() == WINDOWS_VISTA || GetSuiteType() == WINDOWS_7)
		{
			SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Services\\Schedule"),_T("Start"),4);
		}
		else
			m_svcControl->SetStartType(4,_T("Schedule"));
		m_svcControl->StopService(_T("Schedule"));
		return true;
	break;
	case ST_SERVICES_UPNP:
		m_svcControl->SetStartType(4,_T("upnphost"));
		m_svcControl->StopService(_T("upnphost"));
		return true;
	break;
	case ST_SERVICES_MSGSRV:
		m_svcControl->SetStartType(4,_T("Messenger"));
		m_svcControl->StopService(_T("Messenger"));
		return true;
	break;
	case ST_SERVICES_FIREWALL:
		if((GetSuiteType() == WINDOWS_XP_HOME || GetSuiteType() == WINDOWS_XP_PROFESSIONAL) && ServiceExist(_T("SharedAccess")))
		{
			m_svcControl->SetStartType(4,_T("SharedAccess"));
			m_svcControl->StopService(_T("SharedAccess"));
		}
		else if((GetSuiteType() == WINDOWS_VISTA || GetSuiteType() == WINDOWS_7) && ServiceExist(_T("MpsSvc")))
		{
			m_svcControl->SetStartType(4,_T("MpsSvc"));
			m_svcControl->StopService(_T("MpsSvc"));
		}
		return true;
	break;
	case ST_SERVICES_WINDOWS_DEFENDER:
		m_svcControl->SetStartType(2,_T("WinDefend"));
		m_svcControl->StartService(_T("WinDefend"));
		return true;
	break;
	case ST_SERVICES_SEC_CTR:
		m_svcControl->SetStartType(4,_T("wscsvc"));
		m_svcControl->StopService(_T("wscsvc"));
		return true;
	break;
	case ST_SERVICES_RPCLOCATOR:
		m_svcControl->SetStartType(4,_T("RpcLocator"));
		m_svcControl->StopService(_T("RpcLocator"));
		return true;
	break;
	case ST_SERVICES_INDEXER:
		m_svcControl->SetStartType(4,_T("CiSvc"));
		m_svcControl->StopService(_T("CiSvc"));
		return true;
	break;
	case ST_REGSVR_REGWIZC:
		UnregisterDlls(_T("regwizc.dll"));
		return true;
	break;
	case ST_REGSVR_LICDLL:
		UnregisterDlls(_T("licdll.dll"));
		return true;
	break;
	case ST_REGSVR_ZIPFLDR:
		UnregisterDlls(_T("zipfldr.dll"));
		return true;
	break;
	case ST_MESSENGER_AUTOSTART:
		return DeleteRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"),_T("MSMSGS"));
	break;
	case ST_MESSENGER_6_1_AUTOSTART:
		return DeleteRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"),_T("MsnMsgr"));
	break;
	case ST_MESSENGER_DEINSTALL:
		DelMsMsg();
		return true;
	break;
	case ST_SUPPORTUSER:
	if(IsSupportuser()&&(GetSuiteType()==2))
			DelSupportUser();
		return true;
	break;
	case ST_FIREWALL:
		return	SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Services\\SharedAccess\\Parameters\\FirewallPolicy\\StandardProfile"),_T("EnableFirewall"),0);
	break;
	case ST_SHOW_FILE_EXT:
		retVal = SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced"),_T("HideFileExt"),0);
		SendMessage(HWND_BROADCAST, WM_SETTINGCHANGE, 0, 0);
		return retVal;
	break;
	case ST_OXP_ERRORREPORT:
	{

		int access = IsWow64()?KEY_WOW64_64KEY:0;

		if(RegOpenKeyEx(HKEY_CURRENT_USER,_T("Software\\Microsoft"),0,access | KEY_ALL_ACCESS,&hKey)!=ERROR_SUCCESS) errCode = 1;
		if(RegCreateKeyEx(hKey,_T("Office"),NULL,NULL,REG_OPTION_NON_VOLATILE,access | KEY_CREATE_SUB_KEY,NULL,&hSubKey,&KeyDisposition)!=ERROR_SUCCESS) errCode = 2;
		if(RegCreateKeyEx(hSubKey,_T("10.0"),NULL,NULL,REG_OPTION_NON_VOLATILE,access | KEY_CREATE_SUB_KEY,NULL,&hSubKey,&KeyDisposition)!=ERROR_SUCCESS) errCode = 2;
		if(RegCreateKeyEx(hSubKey,_T("Common"),NULL,NULL,REG_OPTION_NON_VOLATILE,access | KEY_CREATE_SUB_KEY,NULL,&hSubKey,&KeyDisposition)!=ERROR_SUCCESS) errCode = 2;
		RegCloseKey(hKey);
		RegCloseKey(hSubKey);
		if(RegOpenKeyEx(HKEY_USERS,_T(".DEFAULT\\Software\\Microsoft"),0,access | KEY_ALL_ACCESS,&hKey)!=ERROR_SUCCESS) errCode = 1;
		if(RegCreateKeyEx(hKey,_T("Office"),NULL,NULL,REG_OPTION_NON_VOLATILE,access | KEY_CREATE_SUB_KEY,NULL,&hSubKey,&KeyDisposition)!=ERROR_SUCCESS) errCode = 2;
		if(RegCreateKeyEx(hSubKey,_T("10.0"),NULL,NULL,REG_OPTION_NON_VOLATILE,access | KEY_CREATE_SUB_KEY,NULL,&hSubKey,&KeyDisposition)!=ERROR_SUCCESS) errCode = 2;
		if(RegCreateKeyEx(hSubKey,_T("Common"),NULL,NULL,REG_OPTION_NON_VOLATILE,access | KEY_CREATE_SUB_KEY,NULL,&hSubKey,&KeyDisposition)!=ERROR_SUCCESS) errCode = 2;
		RegCloseKey(hKey);
		RegCloseKey(hSubKey);
		return (SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Office\\10.0\\Common"),_T("DWNeverUpload"),1)&&
		SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Office\\10.0\\Common"),_T("DWNoExternalURL"),1)&&
		SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Office\\10.0\\Common"),_T("DWNoFileCollection"),1)&&
		SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Office\\10.0\\Common"),_T("DWNoSecondLevelCollection"),1));
	break;
	}		
	case ST_O2k3_ERRORREPORT:
	{
		if (IsOffice2k3Installed())
		{
			int access = IsWow64()?KEY_WOW64_64KEY:0;
			if(RegOpenKeyEx(HKEY_CURRENT_USER,_T("Software\\Microsoft"),0,access | KEY_ALL_ACCESS,&hKey)!=ERROR_SUCCESS) errCode = 1;
			if(RegCreateKeyEx(hKey,_T("Office"),NULL,NULL,REG_OPTION_NON_VOLATILE,access | KEY_CREATE_SUB_KEY,NULL,&hSubKey,&KeyDisposition)!=ERROR_SUCCESS) errCode = 2;
			if(RegCreateKeyEx(hSubKey,_T("11.0"),NULL,NULL,REG_OPTION_NON_VOLATILE,access | KEY_CREATE_SUB_KEY,NULL,&hSubKey,&KeyDisposition)!=ERROR_SUCCESS) errCode = 2;
			if(RegCreateKeyEx(hSubKey,_T("Common"),NULL,NULL,REG_OPTION_NON_VOLATILE,access | KEY_CREATE_SUB_KEY,NULL,&hSubKey,&KeyDisposition)!=ERROR_SUCCESS) errCode = 2;
			RegCloseKey(hKey);
			RegCloseKey(hSubKey);
			if(RegOpenKeyEx(HKEY_USERS,_T(".DEFAULT\\Software\\Microsoft"),0,access | KEY_ALL_ACCESS,&hKey)!=ERROR_SUCCESS) errCode = 1;
			if(RegCreateKeyEx(hKey,_T("Office"),NULL,NULL,REG_OPTION_NON_VOLATILE,access | KEY_CREATE_SUB_KEY,NULL,&hSubKey,&KeyDisposition)!=ERROR_SUCCESS) errCode = 2;
			if(RegCreateKeyEx(hSubKey,_T("11.0"),NULL,NULL,REG_OPTION_NON_VOLATILE,access | KEY_CREATE_SUB_KEY,NULL,&hSubKey,&KeyDisposition)!=ERROR_SUCCESS) errCode = 2;
			if(RegCreateKeyEx(hSubKey,_T("Common"),NULL,NULL,REG_OPTION_NON_VOLATILE,access | KEY_CREATE_SUB_KEY,NULL,&hSubKey,&KeyDisposition)!=ERROR_SUCCESS) errCode = 2;
			RegCloseKey(hKey);
			RegCloseKey(hSubKey);
			return (SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Office\\11.0\\Common"),_T("DWNeverUpload"),1)&&
			SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Office\\11.0\\Common"),_T("DWNoExternalURL"),1)&&
			SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Office\\11.0\\Common"),_T("DWNoFileCollection"),1)&&
			SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Office\\11.0\\Common"),_T("DWNoSecondLevelCollection"),1));
		}
		else if (IsOffice2k7Installed())
		{
			int access = IsWow64()?KEY_WOW64_64KEY:0;
			if(RegOpenKeyEx(HKEY_CURRENT_USER,_T("Software\\Microsoft"),0,access | KEY_ALL_ACCESS,&hKey)!=ERROR_SUCCESS) errCode = 1;
			if(RegCreateKeyEx(hKey,_T("Office"),NULL,NULL,REG_OPTION_NON_VOLATILE,access | KEY_CREATE_SUB_KEY,NULL,&hSubKey,&KeyDisposition)!=ERROR_SUCCESS) errCode = 2;
			if(RegCreateKeyEx(hSubKey,_T("12.0"),NULL,NULL,REG_OPTION_NON_VOLATILE,access | KEY_CREATE_SUB_KEY,NULL,&hSubKey,&KeyDisposition)!=ERROR_SUCCESS) errCode = 2;
			if(RegCreateKeyEx(hSubKey,_T("Common"),NULL,NULL,REG_OPTION_NON_VOLATILE,access | KEY_CREATE_SUB_KEY,NULL,&hSubKey,&KeyDisposition)!=ERROR_SUCCESS) errCode = 2;
			RegCloseKey(hKey);
			RegCloseKey(hSubKey);
			if(RegOpenKeyEx(HKEY_USERS,_T(".DEFAULT\\Software\\Microsoft"),0,access | KEY_ALL_ACCESS,&hKey)!=ERROR_SUCCESS) errCode = 1;
			if(RegCreateKeyEx(hKey,_T("Office"),NULL,NULL,REG_OPTION_NON_VOLATILE,access | KEY_CREATE_SUB_KEY,NULL,&hSubKey,&KeyDisposition)!=ERROR_SUCCESS) errCode = 2;
			if(RegCreateKeyEx(hSubKey,_T("12.0"),NULL,NULL,REG_OPTION_NON_VOLATILE,access | KEY_CREATE_SUB_KEY,NULL,&hSubKey,&KeyDisposition)!=ERROR_SUCCESS) errCode = 2;
			if(RegCreateKeyEx(hSubKey,_T("Common"),NULL,NULL,REG_OPTION_NON_VOLATILE,access | KEY_CREATE_SUB_KEY,NULL,&hSubKey,&KeyDisposition)!=ERROR_SUCCESS) errCode = 2;
			RegCloseKey(hKey);
			RegCloseKey(hSubKey);
			return (SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Office\\12.0\\Common"),_T("DWNeverUpload"),1)&&
			SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Office\\12.0\\Common"),_T("DWNoExternalURL"),1)&&
			SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Office\\12.0\\Common"),_T("DWNoFileCollection"),1)&&
			SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Office\\12.0\\Common"),_T("DWNoSecondLevelCollection"),1));
		}

	break;
	}
	case ST_HIDE_IN_NETWORK:
		return SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Services\\LanManServer\\Parameters"),_T("hidden"),1);
	break;
	case ST_DENY_REGEDIT:
		{
//			const HKEY key = chooseHKey(HKEY_LOCAL_MACHINE, HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies"));
			int access = IsWow64()?KEY_WOW64_64KEY:0;
			const HKEY key = HKEY_CURRENT_USER;
			if(!TestRegKey(key,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System")))
			{
				//create System sub key if not existent
				RegOpenKeyEx(key,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies"),0,access | KEY_ALL_ACCESS,&hKey);
				RegCreateKeyEx(hKey,_T("System"),NULL,NULL,REG_OPTION_NON_VOLATILE,access | KEY_CREATE_SUB_KEY,NULL,&hSubKey,&KeyDisposition);
				RegCloseKey(hKey);
				RegCloseKey(hSubKey);
			}
			return SetDWORDRegKeyValue(key,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System"),_T("DisableRegistryTools"),1);
		}
	break;
	case ST_SCRIPTING_HOST:
		return SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE,_T("Software\\Microsoft\\Windows Script Host\\Settings"),_T("Enabled"),0, IsWow64());
		return true;
	break;
	case ST_SHOW_LAST_USERNAME:
		SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System"),_T("DontDisplayLastUserName"),1);
		return true;
	break;
	case ST_IE_WATSON:
		SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Internet Explorer\\Main"),_T("IEWatsonDisabled"),1);
		return true;
	break;
	case ST_IE_CLEAN_CACHE:
		SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Cache"),_T("Persistent"),0);
		return true;
	break;
	case ST_MEDIAPLAYER_UPDATE:
	{

		HKEY hKey, hSubKey;
		DWORD	KeyDisposition;
		int access = IsWow64()?KEY_WOW64_64KEY:0;

		if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Policies\\Microsoft"),0,access |KEY_ALL_ACCESS,&hKey)==ERROR_SUCCESS)
		{
			if(RegCreateKeyEx(hKey,_T("WindowsMediaPlayer"),NULL,NULL,REG_OPTION_NON_VOLATILE,access |KEY_CREATE_SUB_KEY,NULL,&hSubKey,&KeyDisposition)!=ERROR_SUCCESS)
				RegCloseKey(hSubKey);
			RegCloseKey(hKey);
		}
		SetRegKeyValue(HKEY_LOCAL_MACHINE,_T("Software\\Microsoft\\MediaPlayer\\PlayerUpgrade"),_T("AskMeAgain"),_T("no"),REG_SZ,7,4);
		SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE,_T("Software\\Policies\\Microsoft\\WindowsMediaPlayer"),_T("DisableAutoUpdate"), 1);
		return true;
	}
	case ST_LNK_SUFFIX:
		SetRegKeyValue(HKEY_CLASSES_ROOT,_T("lnkfile"),_T("AlwaysShowExt"),_T(""),REG_SZ,7,0);
		DeleteRegKeyValue(HKEY_CLASSES_ROOT,_T("lnkfile"),_T("NeverShowExt"));
		return true;
	break;
	case ST_PIF_SUFFIX:
		SetRegKeyValue(HKEY_CLASSES_ROOT,_T("piffile"),_T("AlwaysShowExt"),_T(""),REG_SZ,7,0);
		DeleteRegKeyValue(HKEY_CLASSES_ROOT,_T("piffile"),_T("NeverShowExt"));
		SendMessage(HWND_BROADCAST, WM_SETTINGCHANGE, 0, 0);
		return true;
	break;
	case ST_SHC_SUFFIX:
		SetRegKeyValue(HKEY_CLASSES_ROOT,_T("SHCmdFile"),_T("AlwaysShowExt"),_T(""),REG_SZ,7,0);
		DeleteRegKeyValue(HKEY_CLASSES_ROOT,_T("SHCmdFile"),_T("NeverShowExt"));
		SendMessage(HWND_BROADCAST, WM_SETTINGCHANGE, 0, 0);
		return true;
	break;
	case ST_URL_SUFFIX:
		SetRegKeyValue(HKEY_CLASSES_ROOT,_T("InternetShortcut"),_T("AlwaysShowExt"),_T(""),REG_SZ,7,0);
		DeleteRegKeyValue(HKEY_CLASSES_ROOT,_T("InternetShortcut"),_T("NeverShowExt"));
		SendMessage(HWND_BROADCAST, WM_SETTINGCHANGE, 0, 0);
		return true;
	break;
	case ST_CD_AUTORUN:
		{
			int type = GetSuiteType();

			if(type == WINDOWS_7)
			{
				SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\AutoplayHandlers"),_T("DisableAutoplay"),1);
			}
			else
			{
				int access = IsWow64()?KEY_WOW64_64KEY:0;
				HKEY key = HKEY_CURRENT_USER; //chooseHKey(HKEY_LOCAL_MACHINE, HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies"));
				if(!TestRegKey(key,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer")))
				{
					//create Explorer sub key if not existent
					RegOpenKeyEx(key,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies"),0,access | KEY_ALL_ACCESS,&hKey);
					RegCreateKeyEx(hKey,_T("Explorer"),NULL,NULL,REG_OPTION_NON_VOLATILE,access | KEY_CREATE_SUB_KEY,NULL,&hSubKey,&KeyDisposition);
					RegCloseKey(hKey);
				}
				SetDWORDRegKeyValue(key,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer"),_T("NoDriveAutoRun"),0x3ffffff);
				SetDWORDRegKeyValue(key,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer"),_T("HonorAutorunSetting"),1);
				SetDWORDRegKeyValue(key,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer"),_T("NoDriveTypeAutoRun"),0xFF);
				
	//			RecursiveDeleteRegKey(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\MountPoints2"));

				//and again try to do that for HKLM
				key = HKEY_LOCAL_MACHINE;
				if(!TestRegKey(key,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer")))
				{
					//create Explorer sub key if not existent
					RegOpenKeyEx(key,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies"),0,access | KEY_ALL_ACCESS,&hKey);
					RegCreateKeyEx(hKey,_T("Explorer"),NULL,NULL,REG_OPTION_NON_VOLATILE,access | KEY_CREATE_SUB_KEY,NULL,&hSubKey,&KeyDisposition);
					RegCloseKey(hKey);
				}
				SetDWORDRegKeyValue(key,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer"),_T("NoDriveAutoRun"),0x3ffffff);
				SetDWORDRegKeyValue(key,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer"),_T("HonorAutorunSetting"),1);
				SetDWORDRegKeyValue(key,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer"),_T("NoDriveTypeAutoRun"),0xFF);

				//for fixing the bug from the last versions
				SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Services\\Cdrom"),_T("AutoRun"),1);
				return true;

			}

		}
	break;
	case ST_CLEAN_RECENT_DOCS:
		{
			HKEY key = HKEY_CURRENT_USER;
			int access = IsWow64()?KEY_WOW64_64KEY:0;
			if(!TestRegKey(key,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer")))
			{
				//create Explorer sub key if not existent
				RegOpenKeyEx(key,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies"),0,access | KEY_ALL_ACCESS,&hKey);
				RegCreateKeyEx(hKey,_T("Explorer"),NULL,NULL,REG_OPTION_NON_VOLATILE,access | KEY_CREATE_SUB_KEY,NULL,&hSubKey,&KeyDisposition);
				RegCloseKey(hKey);
			}

			SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer"),_T("ClearRecentDocsOnExit"),1);
			return true;
		}
	break;
	case ST_NO_RECENT_DOCS:
		{
			HKEY key = HKEY_CURRENT_USER;
			int access = IsWow64()?KEY_WOW64_64KEY:0;
			if(!TestRegKey(key,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer")))
			{
				//create Explorer sub key if not existent
				RegOpenKeyEx(key,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies"),0,access | KEY_ALL_ACCESS,&hKey);
				RegCreateKeyEx(hKey,_T("Explorer"),NULL,NULL,REG_OPTION_NON_VOLATILE,access | KEY_CREATE_SUB_KEY,NULL,&hSubKey,&KeyDisposition);
				RegCloseKey(hKey);
			}
			SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer"),_T("NoRecentDocsHistory"),1);
			SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer"),_T("NoRecentDocsMenu"),1);
			return true;
		}
	break;
	case ST_DONT_REPORT_INFECTION:
		RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("Software\\Policies\\Microsoft"),0,KEY_ALL_ACCESS|(IsWow64()?KEY_WOW64_64KEY:0),&hKey);
		RegCreateKeyEx(hKey,_T("MRT"),NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_CREATE_SUB_KEY|(IsWow64()?KEY_WOW64_64KEY:0),NULL,&hSubKey,&KeyDisposition);
		RegCloseKey(hKey);
		SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE,_T("Software\\Policies\\Microsoft\\MRT"),_T("DontReportInfectionInformation"),1, IsWow64());
		return true;
	break;
	case ST_MP_START_IN_MEDIA_GUIDE:
		SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\MediaPlayer\\Preferences"),_T("StartInMediaGuide"),0);
	break;
	case ST_RESTRICTANONYMOUS:
		SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Control\\Lsa"),_T("restrictanonymous"),2);
	break;
	case ST_NONETCRAWL:
		SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced"),_T("NoNetCrawling"),1);
	break;
	case ST_MEDIAPLAYER_BLOCKUNINSTALL:
		SetRegKeyValue(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\MediaPlayer"),_T("BlockUninstall"),_T("no"),REG_SZ,7,0);
	break;
	case ST_NOLMHASH:
		SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Control\\Lsa"),_T("NoLMHash"),1);
	break;
	case ST_WMP_PREVENT_LIBRARY_SHARING:
		if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Policies\\Microsoft"),0,KEY_ALL_ACCESS,&hKey)==ERROR_SUCCESS)
		{
			if(RegCreateKeyEx(hKey,_T("WindowsMediaPlayer"),NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_CREATE_SUB_KEY,NULL,&hSubKey,&KeyDisposition)!=ERROR_SUCCESS)
				RegCloseKey(hSubKey);
			RegCloseKey(hKey);
		}
		SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Policies\\Microsoft\\WindowsMediaPlayer"),_T("PreventLibrarySharing"),1, false);
	break;
	case ST_NO_WGA_CHECK:
		RecursiveDeleteRegKey(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon\\Notify\\wgalogon"));
	break;
	case ST_DO_NOT_ALLOW_IE70:
		if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Internet Explorer"),0,KEY_ALL_ACCESS|(IsWow64()?KEY_WOW64_64KEY:0),&hKey)!=ERROR_SUCCESS) errCode = 1;
		if(RegCreateKeyEx(hKey,_T("Setup"),NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_CREATE_SUB_KEY|(IsWow64()?KEY_WOW64_64KEY:0),NULL,&hSubKey,&KeyDisposition)!=ERROR_SUCCESS) errCode = 2;
		if(RegCreateKeyEx(hSubKey,_T("7.0"),NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_CREATE_SUB_KEY|(IsWow64()?KEY_WOW64_64KEY:0),NULL,&hSubKey,&KeyDisposition)!=ERROR_SUCCESS) errCode = 2;
		RegCloseKey(hKey);
		RegCloseKey(hSubKey);
		SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Internet Explorer\\Setup\\7.0"),_T("DoNotAllowIE70"),1);
	break;
	case ST_READER_DISABLE_JS:
		if(GetAdobeReaderVersion() == 7)
		{
			if(!TestRegKey(HKEY_CURRENT_USER, _T("SOFTWARE\\Adobe\\Acrobat Reader\\7.0\\JSPrefs")))
			{
				if(RegOpenKeyEx(HKEY_CURRENT_USER,_T("SOFTWARE\\Adobe\\Acrobat Reader\\7.0"),0,KEY_ALL_ACCESS|(IsWow64()?KEY_WOW64_64KEY:0),&hKey)==ERROR_SUCCESS)
				{
					RegCreateKeyEx(hKey,_T("JSPrefs"),NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_CREATE_SUB_KEY|(IsWow64()?KEY_WOW64_64KEY:0),NULL,&hSubKey,&KeyDisposition);
					RegCloseKey(hKey);
				}
			}
			SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("SOFTWARE\\Adobe\\Acrobat Reader\\7.0\\JSPrefs"),_T("bEnableJS"),0);
		}
		else if(GetAdobeReaderVersion() == 8)
		{
			if(!TestRegKey(HKEY_CURRENT_USER, _T("SOFTWARE\\Adobe\\Acrobat Reader\\8.0\\JSPrefs")))
			{
				if(RegOpenKeyEx(HKEY_CURRENT_USER,_T("SOFTWARE\\Adobe\\Acrobat Reader\\8.0"),0,KEY_ALL_ACCESS|(IsWow64()?KEY_WOW64_64KEY:0),&hKey)==ERROR_SUCCESS)
				{
					RegCreateKeyEx(hKey,_T("JSPrefs"),NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_CREATE_SUB_KEY|(IsWow64()?KEY_WOW64_64KEY:0),NULL,&hSubKey,&KeyDisposition);
					RegCloseKey(hKey);
				}
			}
			SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("SOFTWARE\\Adobe\\Acrobat Reader\\8.0\\JSPrefs"),_T("bEnableJS"),0);
		}
		else if(GetAdobeReaderVersion() == 9)
		{
			if(!TestRegKey(HKEY_CURRENT_USER, _T("SOFTWARE\\Adobe\\Acrobat Reader\\9.0\\JSPrefs")))
			{
				if(RegOpenKeyEx(HKEY_CURRENT_USER,_T("SOFTWARE\\Adobe\\Acrobat Reader\\9.0"),0,KEY_ALL_ACCESS|(IsWow64()?KEY_WOW64_64KEY:0),&hKey)==ERROR_SUCCESS)
				{
					RegCreateKeyEx(hKey,_T("JSPrefs"),NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_CREATE_SUB_KEY|(IsWow64()?KEY_WOW64_64KEY:0),NULL,&hSubKey,&KeyDisposition);
					RegCloseKey(hKey);
				}
			}
			SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("SOFTWARE\\Adobe\\Acrobat Reader\\9.0\\JSPrefs"),_T("bEnableJS"),0);
		}
	break;
	case ST_TWEAK_SVC_SUPPORT_CTR:
		m_svcControl->SetStartType(4,_T("HelpSvc"));
		m_svcControl->StopService(_T("HelpSvc"));
		return true;
	break;
	case ST_TWEAK_SVC_WmdmPmSN:
		m_svcControl->SetStartType(4,_T("WmdmPmSN"));
		m_svcControl->StopService(_T("WmdmPmSN"));
		return true;
	break;
	case ST_TWEAK_COMMAND_HERE:
		{
			if(!TestRegKey(HKEY_CLASSES_ROOT, _T("Directory\\Shell\\OpenNew\\Command")))
			{
				if(RegOpenKeyEx(HKEY_CLASSES_ROOT,_T("Directory\\Shell"),0,KEY_ALL_ACCESS|(IsWow64()?KEY_WOW64_64KEY:0),&hKey)==ERROR_SUCCESS)
				{
					HKEY hTmpKey;
					RegCreateKeyEx(hKey,_T("OpenNew"),NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_CREATE_SUB_KEY|(IsWow64()?KEY_WOW64_64KEY:0),NULL,&hSubKey,&KeyDisposition);
					RegCreateKeyEx(hSubKey,_T("Command"),NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_CREATE_SUB_KEY|(IsWow64()?KEY_WOW64_64KEY:0),NULL,&hTmpKey,&KeyDisposition);
					RegCloseKey(hTmpKey);
					RegCloseKey(hSubKey);
				}
			}
			CString openStr;
			openStr.LoadString(IDS_OPEN_COMMAND_PROMPT);
			SetRegKeyValue(HKEY_CLASSES_ROOT,_T("Directory\\Shell\\OpenNew"),_T(""),openStr,REG_SZ,4,openStr.GetLength());
			SetRegKeyValue(HKEY_CLASSES_ROOT,_T("Directory\\Shell\\OpenNew\\Command"),_T(""),_T("cmd.exe /k cd %1"),REG_SZ,4,16);
		}
	break;
	case ST_TWEAK_DEL_CTFMON:
		{
			HANDLE hndl = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
			DWORD dwsma = GetLastError();

			DWORD dwExitCode = 0;

			PROCESSENTRY32  procEntry={0};
			procEntry.dwSize = sizeof( PROCESSENTRY32 );
			Process32First(hndl,&procEntry);
			bool found = false;
			do
			{
				if(!_wcsicmp(procEntry.szExeFile,_T("ctfmon.exe")))
				{
					found = true;
					break;
				}
			}while(Process32Next(hndl,&procEntry));
			if(found)
			{
				HANDLE hHandle;
				hHandle = ::OpenProcess(PROCESS_ALL_ACCESS,0,procEntry.th32ProcessID);
				
				::GetExitCodeProcess(hHandle,&dwExitCode);
				::TerminateProcess(hHandle,dwExitCode);

			}

			TCHAR windir[MAX_PATH];
			GetWindowsDirectory(windir, MAX_PATH);
			CString tmpSrc, tmpDest;
			
			tmpSrc.Format(_T("%s\\system32\\ctfmon.exe"), windir);
			tmpDest.Format(_T("%s\\system32\\ctfmon.exe.backup"), windir);
			CopyFile(tmpSrc, tmpDest, TRUE);
			
			tmpSrc.Format(_T("%s\\system32\\dllcache\\ctfmon.exe"), windir);
			tmpDest.Format(_T("%s\\system32\\dllcache\\ctfmon.exe.backup"), windir);
			CopyFile(tmpSrc, tmpDest, TRUE);

			bool b = DeleteFile(tmpSrc);
			CreateCTFMonDummy(tmpSrc);

			tmpSrc.Format(_T("%s\\system32\\ctfmon.exe"), windir);
			b = DeleteFile(tmpSrc);
			CreateCTFMonDummy(tmpSrc);
			
			DeleteRegKeyValue(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), _T("ctfmon.exe"));

			if(ExistFile(_T("msctf.dll")))
				UnregisterDlls(_T("msctf.dll"));
			if(ExistFile(_T("msimtf.dll")))
				UnregisterDlls(_T("msimtf.dll"));
			SendMessage(HWND_BROADCAST, WM_SETTINGCHANGE, 0, 0);
			return true;
		}
	break;
	case ST_TWEAK_CLEANUP_WIZ:
		SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Desktop\\CleanupWiz"),_T("NoRun"), 1);
	break;
	case ST_TWEAK_WINUPDATE_NO_DRIVER:
		if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Policies\\Microsoft\\Windows"),0,KEY_ALL_ACCESS|(IsWow64()?KEY_WOW64_64KEY:0),&hKey)!=ERROR_SUCCESS) errCode = 1;
		if(RegCreateKeyEx(hKey,_T("DriverSearching"),NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_CREATE_SUB_KEY|(IsWow64()?KEY_WOW64_64KEY:0),NULL,&hSubKey,&KeyDisposition)!=ERROR_SUCCESS) errCode = 2;
		RegCloseKey(hSubKey);
		RegCloseKey(hKey);
		return SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Policies\\Microsoft\\Windows\\DriverSearching"),_T("DontSearchWindowsUpdate"),1);
	break;
	case ST_TWEAK_WEBSERVICE:
		{
			int type = GetSuiteType();
			if(type == WINDOWS_7)
			{
				if(RegOpenKeyEx(HKEY_CURRENT_USER,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer"),0,KEY_ALL_ACCESS|(IsWow64()?KEY_WOW64_64KEY:0),&hKey)!=ERROR_SUCCESS)
				{
					RegOpenKeyEx(HKEY_CURRENT_USER,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies"),0,KEY_ALL_ACCESS|(IsWow64()?KEY_WOW64_64KEY:0),&hKey);
					HKEY hSubKey;
					RegCreateKeyEx(hKey,_T("Explorer"),NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_CREATE_SUB_KEY|(IsWow64()?KEY_WOW64_64KEY:0),NULL,&hSubKey,&KeyDisposition);
					RegCloseKey(hSubKey);
					RegCloseKey(hKey);
				}

				if(RegOpenKeyEx(HKEY_CURRENT_USER,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer"),0,KEY_ALL_ACCESS|(IsWow64()?KEY_WOW64_64KEY:0),&hKey)==ERROR_SUCCESS)
				{
					return SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer"),_T("NoInternetOpenWith"),1);
				}
			}
			else
			{
				if(RegOpenKeyEx(HKEY_CURRENT_USER,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer"),0,KEY_ALL_ACCESS|(IsWow64()?KEY_WOW64_64KEY:0),&hKey)==ERROR_SUCCESS)
				{
					RegCloseKey(hKey);
					return SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer"),_T("InternetOpenWith"),0);
				}
			}

		}	
	break;
	case ST_TWEAK_BSOD_AUTOREBOOT:
		return SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE,_T("System\\CurrentControlSet\\Control\\CrashControl"),_T("AutoReboot"),0);
	break;
	case ST_TWEAK_DEV_MANAGER:
		{
			if(!TestRegKey(HKEY_CLASSES_ROOT, _T("CLSID\\{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\shell\\DeviceManager"),IsWow64()))
			{
				int regsam = KEY_ALL_ACCESS;
				if(IsWow64()) regsam |= KEY_WOW64_64KEY;
				if(RegOpenKeyEx(HKEY_CLASSES_ROOT,_T("CLSID\\{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\shell"),0,regsam,&hKey)==ERROR_SUCCESS)
				{
					int regsam = KEY_CREATE_SUB_KEY;
					if(IsWow64()) regsam |= KEY_WOW64_64KEY;
					HKEY hTmpKey;
					RegCreateKeyEx(hKey,_T("DeviceManager"),NULL,NULL,REG_OPTION_NON_VOLATILE, regsam,NULL,&hSubKey,&KeyDisposition);
					RegCreateKeyEx(hSubKey,_T("Command"),NULL,NULL,REG_OPTION_NON_VOLATILE,regsam,NULL,&hTmpKey,&KeyDisposition);
					RegCloseKey(hTmpKey);
					RegCloseKey(hSubKey);
				}
			}
			CString openStr;
			openStr.LoadString(IDS_OPEN_DEVICE_MANAGER);
			SetRegKeyValue(HKEY_CLASSES_ROOT,_T("CLSID\\{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\shell\\DeviceManager"),_T(""),openStr,REG_EXPAND_SZ,4,openStr.GetLength(), IsWow64());
			SetDWORDRegKeyValue(HKEY_CLASSES_ROOT,_T("CLSID\\{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\shell\\DeviceManager"),_T("SuppressionPolicy"),0x4000003c, IsWow64());
			SetRegKeyValue(HKEY_CLASSES_ROOT,_T("CLSID\\{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\shell\\DeviceManager\\Command"),_T(""),_T("%windir%\\system32\\mmc.exe /s %SystemRoot%\\system32\\devmgmt.msc /s"),REG_EXPAND_SZ,4,16, IsWow64());
		}
	break;
	case ST_TWEAK_NO_WINTOUR:
		return SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Applets\\Tour"),_T("RunCount"),0);
	break;
	case ST_NET_NO_RECENT_DOCS:
		{
				if(RegOpenKeyEx(HKEY_CURRENT_USER,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer"),0,KEY_ALL_ACCESS|(IsWow64()?KEY_WOW64_64KEY:0),&hKey)!=ERROR_SUCCESS)
				{
					RegOpenKeyEx(HKEY_CURRENT_USER,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies"),0,KEY_ALL_ACCESS|(IsWow64()?KEY_WOW64_64KEY:0),&hKey);
					HKEY hSubKey;
					RegCreateKeyEx(hKey,_T("Explorer"),NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_CREATE_SUB_KEY|(IsWow64()?KEY_WOW64_64KEY:0),NULL,&hSubKey,&KeyDisposition);
					RegCloseKey(hSubKey);
					RegCloseKey(hKey);
				}

//			if(!TestRegKey(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer")))
//			{
//				if(RegOpenKeyEx(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies"),0,KEY_ALL_ACCESS,&hKey)!=ERROR_SUCCESS)
//				{
//					HKEY hTmpKey;
//					RegCreateKeyEx(hKey,_T("Explorer"),NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_CREATE_SUB_KEY,NULL,&hSubKey,&KeyDisposition);
//					RegCloseKey(hSubKey);
//				}
//			}
			SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer"),_T("NoRecentDocsNetHood"),1);
		}
	break;
	case ST_NO_THUMBNAIL_CACHE:
		return SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced"),_T("DisableThumbnailCache"),1);
	break;
	case ST_NO_SEARCH_ASSISTANT:
		return SetRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\CabinetState"),_T("Use Search Asst"), _T("no"),REG_SZ,4,2);
	break;
	case ST_DISABLE_UAC:
		return SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System"), _T("ConsentPromptBehaviorAdmin"), 0);
	break;
	case ST_NO_UNREAD_MAIL:
		return SetDWORDRegKeyValue(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\UnreadMail"), _T("MessageExpiryDays"), 0);
	break;
	case ST_NO_VDM:
		{
				if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Policies\\Microsoft\\Windows\\AppCompat"),0,KEY_ALL_ACCESS|(IsWow64()?KEY_WOW64_64KEY:0),&hKey)!=ERROR_SUCCESS)
				{
					if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Policies\\Microsoft\\Windows"),0,KEY_ALL_ACCESS|(IsWow64()?KEY_WOW64_64KEY:0),&hKey)==ERROR_SUCCESS)
					{
						HKEY hTmpKey;
						RegCreateKeyEx(hKey,_T("AppCompat"),NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_CREATE_SUB_KEY|(IsWow64()?KEY_WOW64_64KEY:0),NULL,&hSubKey,&KeyDisposition);
						RegCloseKey(hSubKey);
					}
				}

			SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Policies\\Microsoft\\Windows\\AppCompat"),_T("VDMDisallowed"),1);
		}
	break;
	case ST_NO_TEREDO:
		{
			SHELLEXECUTEINFO seInfo;
			ZeroMemory(&seInfo, sizeof(SHELLEXECUTEINFO));
			seInfo.cbSize=sizeof(SHELLEXECUTEINFO);
			seInfo.nShow = SW_HIDE;
			seInfo.lpFile = _T("netsh");
			seInfo.lpParameters = _T("interface ipv6 set teredo disable");
			ShellExecuteEx(&seInfo);
			Sleep(2000);

			return true;
		}
	break;
	case ST_NO_ISATAP:
		{
			SHELLEXECUTEINFO seInfo;
			ZeroMemory(&seInfo, sizeof(SHELLEXECUTEINFO));
			seInfo.cbSize=sizeof(SHELLEXECUTEINFO);
			seInfo.nShow = SW_HIDE;
			seInfo.lpFile = _T("netsh");
			seInfo.lpParameters = _T("interface isatap set state disabled");
			ShellExecuteEx(&seInfo);
			Sleep(2000);

			return true;
		}
	break;
	case ST_NO_NCSI:
		return SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Services\\NlaSvc\\Parameters\\Internet"),_T("EnableActiveProbing"),0);
	break;
	case ST_NO_ROOT_CERT:
		if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Policies\\Microsoft\\SystemCertificates\\AuthRoot"),0,KEY_ALL_ACCESS|(IsWow64()?KEY_WOW64_64KEY:0),&hKey)!=ERROR_SUCCESS)
		{
			if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Policies\\Microsoft\\SystemCertificates"),0,KEY_ALL_ACCESS|(IsWow64()?KEY_WOW64_64KEY:0),&hKey)==ERROR_SUCCESS)
			{
				HKEY hTmpKey;
				RegCreateKeyEx(hKey,_T("AuthRoot"),NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_CREATE_SUB_KEY|(IsWow64()?KEY_WOW64_64KEY:0),NULL,&hSubKey,&KeyDisposition);
				RegCloseKey(hSubKey);
			}
		}

		return SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Policies\\Microsoft\\SystemCertificates\\AuthRoot"), _T("DisableRootAutoUpdate"),1);
	break;
	case ST_CERT_REVOCATION:
		return SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Internet Settings"), _T("CertificateRevocation"),1);
	
	case ST_CERT_TRUST:
	{
		DWORD oldVal = GetDWORDRegKeyValue(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WinTrust\\Trust Providers\\Software Publishing"),_T("State"));
		if((oldVal & 0x200) == 0x200)
		{
			DWORD newVal = oldVal - 0x200;
			return SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WinTrust\\Trust Providers\\Software Publishing"), _T("State"),newVal);
		}
	}
	break;
	case ST_NO_HTTPS_CACHE:
		return SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Internet Settings"), _T("DisableCachingOfSSLPages"),1);
	case ST_NO_FORCEREBOOT:
		{
			if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Policies\\Microsoft\\Windows\\WindowsUpdate"),0,KEY_ALL_ACCESS|(IsWow64()?KEY_WOW64_64KEY:0),&hKey)!=ERROR_SUCCESS)
			{
				if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Policies\\Microsoft\\Windows"),0,KEY_ALL_ACCESS|(IsWow64()?KEY_WOW64_64KEY:0),&hKey)==ERROR_SUCCESS)
				{
					HKEY hTmpKey;
					RegCreateKeyEx(hKey,_T("WindowsUpdate"),NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_CREATE_SUB_KEY,NULL,&hSubKey,&KeyDisposition);
					RegCloseKey(hSubKey);
				}
			}
			if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Policies\\Microsoft\\Windows\\WindowsUpdate\\AU"),0,KEY_ALL_ACCESS|(IsWow64()?KEY_WOW64_64KEY:0),&hKey)!=ERROR_SUCCESS)
			{
				if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Policies\\Microsoft\\Windows\\WindowsUpdate"),0,KEY_ALL_ACCESS|(IsWow64()?KEY_WOW64_64KEY:0),&hKey)==ERROR_SUCCESS)
				{
					HKEY hTmpKey;
					RegCreateKeyEx(hKey,_T("AU"),NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_CREATE_SUB_KEY|(IsWow64()?KEY_WOW64_64KEY:0),NULL,&hSubKey,&KeyDisposition);
					RegCloseKey(hSubKey);
				}
			}


			return SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate\\Auto Update"), _T("NoAutoRebootWithLoggedOnUsers"),1) &&
				SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Policies\\Microsoft\\Windows\\WindowsUpdate\\AU"), _T("NoAutoRebootWithLoggedOnUsers"),1);
		}
	case ST_TWEAK_NO_SHORTCUT_ARROW:
		{

		/*if(GetSuiteType() <= SUITE_TYPE::WINDOWS_XP_PROFESSIONAL)
		{
			return DeleteRegKeyValue(HKEY_CLASSES_ROOT,_T("lnkfile"), _T("IsShortcut"), IsWow64());
		}
		else */
		{
			if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Icons"),0,KEY_ALL_ACCESS|(IsWow64()?KEY_WOW64_64KEY:0),&hKey)!=ERROR_SUCCESS)
			{
				if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer"),0,KEY_ALL_ACCESS|(IsWow64()?KEY_WOW64_64KEY:0),&hKey)==ERROR_SUCCESS)
				{
					HKEY hTmpKey;
					RegCreateKeyEx(hKey,_T("Shell Icons"),NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_CREATE_SUB_KEY|(IsWow64()?KEY_WOW64_64KEY:0),NULL,&hSubKey,&KeyDisposition);
					RegCloseKey(hSubKey);
				}
			}

			TCHAR windir[MAX_PATH];
			GetWindowsDirectory(windir, MAX_PATH);
			CString tmpSrc;
			tmpSrc.Format(_T("%s\\system32\\empty.ico"), windir);
			bool isWow = IsWow64();
			PVOID oldValue = NULL;
			if(isWow)
			{
				MyWow64DisableWow64FsRedirection(&oldValue);
			}

			CreateEmptyIcon(tmpSrc);
			if(isWow)
			{
				MyWow64RevertWow64FsRedirection(oldValue);
			}

			bool ret = SetRegKeyValue(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Icons"),_T("29"), _T("empty.ico,0"),REG_SZ,4,2, IsWow64());
			HMODULE hShell32 = LoadLibrary(_T("shell32.dll"));
			if (hShell32 != NULL)
			{
				typedef BOOL (WINAPI * FII_PROC)(BOOL fFullInit);
				FII_PROC FileIconInit = (FII_PROC)GetProcAddress(hShell32, (LPCSTR)660);
				if (FileIconInit != NULL)
				{
					FileIconInit(TRUE);
				}
			}
			SendMessage(HWND_BROADCAST, WM_SETTINGCHANGE, SPI_SETNONCLIENTMETRICS, (LPARAM)_T("WindowMetrics"));
			return ret;

		}
	}
	break;
	case ST_TWEAK_SVC_BITS:
	{
		m_svcControl->SetStartType(4,_T("BITS"));
		m_svcControl->StopService(_T("BITS"));
		return true;
	}

	case ST_TELEMETRY:
	{
		if (ServiceExist(_T("DiagTrack")))
		{
			m_svcControl->SetStartType(4, _T("DiagTrack"));
			m_svcControl->StopService(_T("DiagTrack"));
		}
		if (ServiceExist(_T("dmwappushservice")))
		{
			m_svcControl->SetStartType(4, _T("dmwappushservice"));
			m_svcControl->StopService(_T("dmwappushservice"));
		}

		return SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\DataCollection"), _T("AllowTelemetry"), 0);
	}
	case ST_INVENTORY:
	{
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Policies\\Microsoft\\Windows\\AppCompat"), 0, KEY_ALL_ACCESS | (IsWow64() ? KEY_WOW64_64KEY : 0), &hKey) != ERROR_SUCCESS)
		{
			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Policies\\Microsoft\\Windows"), 0, KEY_ALL_ACCESS | (IsWow64() ? KEY_WOW64_64KEY : 0), &hKey) == ERROR_SUCCESS)
			{
				RegCreateKeyEx(hKey, _T("AppCompat"), NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_CREATE_SUB_KEY | (IsWow64() ? KEY_WOW64_64KEY : 0), NULL, &hSubKey, &KeyDisposition);
				RegCloseKey(hKey);
			}
		}
		return SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Policies\\Microsoft\\Windows\\AppCompat"), _T("DisableInventory"), 1);
	}
	case ST_PERSONALIZE:
	{
		if (RegOpenKeyEx(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\InputPersonalization"), 0, KEY_ALL_ACCESS | (IsWow64() ? KEY_WOW64_64KEY : 0), &hKey) != ERROR_SUCCESS)
		{
			if (RegOpenKeyEx(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft"), 0, KEY_ALL_ACCESS | (IsWow64() ? KEY_WOW64_64KEY : 0), &hKey) == ERROR_SUCCESS)
			{
				RegCreateKeyEx(hKey, _T("InputPersonalization"), NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_CREATE_SUB_KEY | (IsWow64() ? KEY_WOW64_64KEY : 0), NULL, &hSubKey, &KeyDisposition);
				RegCreateKeyEx(hSubKey, _T("TrainedDataStore"), NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_CREATE_SUB_KEY | (IsWow64() ? KEY_WOW64_64KEY : 0), NULL, &hSubKey, &KeyDisposition);
				RegCloseKey(hSubKey);
			}
		}

		return SetDWORDRegKeyValue(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\InputPersonalization"), _T("RestrictImplicitInkCollection"), 1) &&
			SetDWORDRegKeyValue(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\InputPersonalization"), _T("RestrictImplicitTextCollection"), 1) &&
			SetDWORDRegKeyValue(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\InputPersonalization\\TrainedDataStore"), _T("HarvestContacts"), 0);
	}
	case ST_APP_TELEMETRY:
	{

		return SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Policies\\Microsoft\\Windows\\AppCompat"), _T("AITEnable"), 0);

	}
	case ST_AD_ID:
	{
		if (RegOpenKeyEx(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\AdvertisingInfo"), 0, KEY_ALL_ACCESS | (IsWow64() ? KEY_WOW64_64KEY : 0), &hKey) != ERROR_SUCCESS)
		{
			if (RegOpenKeyEx(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion"), 0, KEY_ALL_ACCESS | (IsWow64() ? KEY_WOW64_64KEY : 0), &hKey) == ERROR_SUCCESS)
			{
				RegCreateKeyEx(hKey, _T("AdvertisingInfo"), NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_CREATE_SUB_KEY | (IsWow64() ? KEY_WOW64_64KEY : 0), NULL, &hSubKey, &KeyDisposition);
				RegCloseKey(hKey);
			}
		}
		DeleteRegKeyValue(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\AdvertisingInfo"), _T("Id"));
		return SetDWORDRegKeyValue(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\AdvertisingInfo"), _T("Enabled"), 0);

	}
	case ST_WRITING_BEHAVIOR:
	{
		if (RegOpenKeyEx(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Input\\TIPC"), 0, KEY_ALL_ACCESS | (IsWow64() ? KEY_WOW64_64KEY : 0), &hKey) != ERROR_SUCCESS)
		{
			if (RegOpenKeyEx(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft"), 0, KEY_ALL_ACCESS | (IsWow64() ? KEY_WOW64_64KEY : 0), &hKey) == ERROR_SUCCESS)
			{
				HKEY hTmpKey;
				RegCreateKeyEx(hKey, _T("Input"), NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_CREATE_SUB_KEY | (IsWow64() ? KEY_WOW64_64KEY : 0), NULL, &hSubKey, &KeyDisposition);
				RegCreateKeyEx(hSubKey, _T("TIPC"), NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_CREATE_SUB_KEY | (IsWow64() ? KEY_WOW64_64KEY : 0), NULL, &hSubKey, &KeyDisposition);
				RegCloseKey(hSubKey);
			}
		}
		return SetDWORDRegKeyValue(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Input\\TIPC"), _T("Enabled"), 0);

	}
	case ST_FEEDBACK:
	{
		if (RegOpenKeyEx(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Siuf\\Rules"), 0, KEY_ALL_ACCESS | (IsWow64() ? KEY_WOW64_64KEY : 0), &hKey) != ERROR_SUCCESS)
		{
			if (RegOpenKeyEx(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft"), 0, KEY_ALL_ACCESS | (IsWow64() ? KEY_WOW64_64KEY : 0), &hKey) == ERROR_SUCCESS)
			{
				HKEY hTmpKey;
				RegCreateKeyEx(hKey, _T("Siuf"), NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_CREATE_SUB_KEY | (IsWow64() ? KEY_WOW64_64KEY : 0), NULL, &hSubKey, &KeyDisposition);
				RegCreateKeyEx(hSubKey, _T("Rules"), NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_CREATE_SUB_KEY | (IsWow64() ? KEY_WOW64_64KEY : 0), NULL, &hSubKey, &KeyDisposition);
				RegCloseKey(hSubKey);
			}
		}
		return SetDWORDRegKeyValue(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Siuf\\Rules"), _T("NumberOfSIUFInPeriod"), 0);


	}
	case ST_BIOMETRY:
	{
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Policies\\Microsoft\\Biometrics"), 0, KEY_ALL_ACCESS | (IsWow64() ? KEY_WOW64_64KEY : 0), &hKey) != ERROR_SUCCESS)
		{
			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Policies\\Microsoft"), 0, KEY_ALL_ACCESS | (IsWow64() ? KEY_WOW64_64KEY : 0), &hKey) == ERROR_SUCCESS)
			{
				HKEY hTmpKey;
				RegCreateKeyEx(hKey, _T("Biometrics"), NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_CREATE_SUB_KEY | (IsWow64() ? KEY_WOW64_64KEY : 0), NULL, &hSubKey, &KeyDisposition);
				RegCloseKey(hSubKey);
			}
		}
		return SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Policies\\Microsoft\\Biometrics"), _T("Enabled"), 0);

	}
	case ST_WEBSEARCH:
	{
		return SetDWORDRegKeyValue(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Search"), _T("BingSearchEnabled"), 0);

	}
	case ST_UPDATE_SHARING:
	{
		const bool ret = SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\DeliveryOptimization\\Config"), _T("DODownloadMode"), 0);
		return ret;
	}
	case ST_LOCATION_PROVIDER:
	{
		m_svcControl->StopService(_T("lfsvc"));
		m_svcControl->SetStartType(3, _T("lfsvc"));
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Policies\\Microsoft\\Windows\\LocationAndSensors"), 0, KEY_ALL_ACCESS | (IsWow64() ? KEY_WOW64_64KEY : 0), &hKey) != ERROR_SUCCESS)
		{
			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Policies\\Microsoft\\Windows"), 0, KEY_ALL_ACCESS | (IsWow64() ? KEY_WOW64_64KEY : 0), &hKey) == ERROR_SUCCESS)
			{
				HKEY hTmpKey;
				RegCreateKeyEx(hKey, _T("LocationAndSensors"), NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_CREATE_SUB_KEY | (IsWow64() ? KEY_WOW64_64KEY : 0), NULL, &hSubKey, &KeyDisposition);
				RegCloseKey(hSubKey);
			}
		}
		return SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Policies\\Microsoft\\Windows\\LocationAndSensors"), _T("DisableLocation"), 1);
	}
	case ST_SENSORS:
	{
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Policies\\Microsoft\\Windows\\LocationAndSensors"), 0, KEY_ALL_ACCESS | (IsWow64() ? KEY_WOW64_64KEY : 0), &hKey) != ERROR_SUCCESS)
		{
			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Policies\\Microsoft\\Windows"), 0, KEY_ALL_ACCESS | (IsWow64() ? KEY_WOW64_64KEY : 0), &hKey) == ERROR_SUCCESS)
			{
				HKEY hTmpKey;
				RegCreateKeyEx(hKey, _T("LocationAndSensors"), NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_CREATE_SUB_KEY | (IsWow64() ? KEY_WOW64_64KEY : 0), NULL, &hSubKey, &KeyDisposition);
				RegCloseKey(hSubKey);
			}
		}
		return SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Policies\\Microsoft\\Windows\\LocationAndSensors"), _T("DisableSensors"), 1);
	}
	case ST_WIFI_SENSE:
	{
		CString sid = getCurrentUserSID();
		if (!TestRegKey(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\WcmSvc\\wifinetworkmanager\\features\\"+sid)))
		{
			//create System sub key if not existent
			int access = KEY_CREATE_SUB_KEY;
			if (IsWow64()) access |= KEY_WOW64_64KEY;
			long ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\WcmSvc\\wifinetworkmanager\\features\\"), 0, access, &hKey);
			RegCreateKeyEx(hKey, sid, NULL, NULL, REG_OPTION_NON_VOLATILE, access, NULL, &hSubKey, &KeyDisposition);
			RegCloseKey(hKey);
			RegCloseKey(hSubKey);
		}

		bool ret = SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\WcmSvc\\wifinetworkmanager\\features\\"+getCurrentUserSID()), _T("FeatureStates"), 828);
		ret |= SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\WcmSvc\\wifinetworkmanager\\features"), _T("WiFiSenseCredShared"), 0);
		ret |= SetDWORDRegKeyValue(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\WcmSvc\\wifinetworkmanager\\features"), _T("WiFiSenseOpen"), 0);
		
		m_svcControl->StopService(_T("WcmSvc"));
		m_svcControl->StartService(_T("WcmSvc"));
		return ret;
	}
	case ST_SITE_PREDICTION:
	{
		return SetDWORDRegKeyValue(HKEY_CURRENT_USER, _T("Software\\Classes\\Local Settings\\Software\\Microsoft\\Windows\\CurrentVersion\\AppContainer\\Storage\\microsoft.microsoftedge_8wekyb3d8bbwe\\MicrosoftEdge\\FlipAhead"), _T("FPEnabled"), 0);
	}

	}
	return false;
}
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
const bool CAntiSpySettings::SetRegKeyValue(const HKEY parentKey, LPCTSTR lpszKeyName,
									  LPCTSTR lpszValueName,LPCTSTR lpszValue,
									  const DWORD type,const UINT index,const DWORD size, bool isWow64) const
{
	HKEY hKey;
	unsigned int errCode = 0;
	CString errStr;
	CString lpszDesc;
	TCHAR lpReadVal[128];
	LPDWORD lpType=0;
	DWORD KeyDisposition;
	DWORD kSize = sizeof(lpReadVal);
	int access = KEY_ALL_ACCESS;
	if(isWow64) access |= KEY_WOW64_64KEY;

//	if(RegOpenKeyEx(parentKey,lpszKeyName,0,KEY_ALL_ACCESS,&hKey)!=ERROR_SUCCESS) errCode = 1;
	if(RegOpenKeyEx(parentKey,lpszKeyName,0,access,&hKey)!=ERROR_SUCCESS)
	{
		access = KEY_CREATE_SUB_KEY;
		if(isWow64) access |= KEY_WOW64_64KEY;

		if(RegCreateKeyEx(hKey,lpszValueName,NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_CREATE_SUB_KEY,NULL,&hKey,&KeyDisposition)!=ERROR_SUCCESS)
			errCode=1;
	}
	int i = wcslen(lpszValue);
	if(RegSetValueEx(hKey,lpszValueName,NULL,type,(LPBYTE)lpszValue,wcslen(lpszValue)*2)!=ERROR_SUCCESS) errCode = 3;
	if(RegQueryValueEx(hKey,lpszValueName,NULL,lpType,(LPBYTE)lpReadVal,&kSize)!=ERROR_SUCCESS) errCode = 4;
	if(wcscmp(lpReadVal,lpszValue)!=0) errCode = 5;
	RegCloseKey(hKey);
	if(errCode!=0) return 1;//if successful
	return 1;
}


const bool CAntiSpySettings::SetRegKeyValue(const HKEY parentKey, LPCTSTR lpszKeyName,
									  LPCTSTR lpszValueName,LPCTSTR lpszValue,
									  const DWORD type,const UINT index,const DWORD size) const
{
	return SetRegKeyValue(parentKey, lpszKeyName, lpszValueName, lpszValue, type, index, size, IsWow64());
}

const bool CAntiSpySettings::SetDWORDRegKeyValue(const HKEY parentKey, LPCTSTR lpszKeyName, LPCTSTR lpszValueName,DWORD dwValue, bool isWow64) const
{
	HKEY hKey;
	unsigned int errCode = 0;
	CString errStr;
	CString lpszDesc;
	DWORD dwReadVal;
	LPDWORD lpType=0;
	DWORD KeyDisposition;
	DWORD kSize = sizeof(DWORD);
	DWORD dwType = REG_DWORD;
	int access = KEY_ALL_ACCESS;
	if(isWow64) access |= KEY_WOW64_64KEY;
	if(RegOpenKeyEx(parentKey,lpszKeyName,0,access,&hKey)!=ERROR_SUCCESS)
	{
		access = KEY_CREATE_SUB_KEY;
		if(isWow64) access |= KEY_WOW64_64KEY;

		if(RegCreateKeyEx(hKey,lpszValueName,NULL,NULL,REG_OPTION_NON_VOLATILE,access,NULL,&hKey,&KeyDisposition)!=ERROR_SUCCESS)
			errCode=1;
	}
	if(RegSetValueEx(hKey,lpszValueName,NULL, REG_DWORD,(LPBYTE)&dwValue,sizeof(DWORD))!=ERROR_SUCCESS) errCode = 3;
	if(RegQueryValueEx(hKey,lpszValueName,NULL,&dwType,(LPBYTE)&dwReadVal,&kSize)!=ERROR_SUCCESS) errCode = 4;
	if(dwValue != dwReadVal) errCode = 5;
	RegCloseKey(hKey);

	if(errCode!=0) return 1;//if successful
	return 1;
}

const bool CAntiSpySettings::SetDWORDRegKeyValue(const HKEY parentKey, LPCTSTR lpszKeyName, LPCTSTR lpszValueName,DWORD dwValue) const
{
	return SetDWORDRegKeyValue(parentKey, lpszKeyName, lpszValueName, dwValue, IsWow64());
}

const bool CAntiSpySettings::SetBINARYRegKeyValue(const HKEY parentKey, LPCTSTR lpszKeyName, LPCTSTR lpszValueName,TCHAR dwValue, bool isWow64) const
{
	HKEY hKey;
	unsigned int errCode = 0;
	CString errStr;
	CString lpszDesc;
	DWORD dwReadVal;
	LPDWORD lpType=0;
	DWORD KeyDisposition;
	DWORD kSize = sizeof(DWORD);
	DWORD dwType = REG_BINARY;
	int access = KEY_ALL_ACCESS;
	if(isWow64) access |= KEY_WOW64_64KEY;

	if(RegOpenKeyEx(parentKey,lpszKeyName,0,access,&hKey)!=ERROR_SUCCESS)
	{
		int access = KEY_CREATE_SUB_KEY;
		if(isWow64) access |= KEY_WOW64_64KEY;
		if(RegCreateKeyEx(hKey,lpszValueName,NULL,NULL,REG_OPTION_NON_VOLATILE,access,NULL,&hKey,&KeyDisposition)!=ERROR_SUCCESS)
			errCode=1;
	}
	if(RegSetValueEx(hKey,lpszValueName,NULL, REG_BINARY,(LPBYTE)&dwValue,1)!=ERROR_SUCCESS) errCode = 3;
	if(RegQueryValueEx(hKey,lpszValueName,NULL,&dwType,(LPBYTE)&dwReadVal,&kSize)!=ERROR_SUCCESS) errCode = 4;
	if(dwValue != dwReadVal) errCode = 5;
	RegCloseKey(hKey);

	if(errCode!=0) return 1;//if successful
	return 1;
}

const bool CAntiSpySettings::SetBINARYRegKeyValue(const HKEY parentKey, LPCTSTR lpszKeyName, LPCTSTR lpszValueName,TCHAR dwValue) const
{
	return SetBINARYRegKeyValue(parentKey, lpszKeyName, lpszValueName, dwValue, IsWow64());
}

CString CAntiSpySettings::GetRegKeyValue(const HKEY parentKey,LPCTSTR lpszKeyName, LPCTSTR lpszValueName,const DWORD type, bool isWow64)
{
	HKEY hKey;
	DWORD kSize;
	DWORD dwReadVal=0;
	LPDWORD lpType=0;
	TCHAR lpszReadVal[128];

	int access = KEY_READ;
	if(isWow64) access |= KEY_WOW64_64KEY;

	if(RegOpenKeyEx(parentKey,lpszKeyName,0,KEY_READ,&hKey)!=ERROR_SUCCESS)
	{
//		RegCloseKey(hKey);
		return "";
	}
	CString str;
	switch(type)
	{
		case REG_SZ:
			kSize = sizeof(lpszReadVal);
			if(RegQueryValueEx(hKey,lpszValueName,NULL,lpType,(LPBYTE)lpszReadVal,&kSize)!=ERROR_SUCCESS)
			{
				RegCloseKey(hKey);
				return _T("");
			}
			str.Format(_T("%s"),lpszReadVal);
			return str;
		break;
		case REG_DWORD:
			kSize = sizeof(dwReadVal);
			if(RegQueryValueEx(hKey,lpszValueName,NULL,lpType,(LPBYTE)&dwReadVal,&kSize)!=ERROR_SUCCESS)
			{
				RegCloseKey(hKey);
				return "";
			}
			str.Format(_T("%d"),lpszReadVal);
			return str;
			
		break;
	}
	RegCloseKey(hKey);

	return "";
}

CString CAntiSpySettings::GetRegKeyValue(const HKEY parentKey,LPCTSTR lpszKeyName, LPCTSTR lpszValueName,const DWORD type)
{
	return GetRegKeyValue(parentKey, lpszKeyName, lpszValueName, type, IsWow64());
}

DWORD CAntiSpySettings::GetDWORDRegKeyValue(const HKEY parentKey,LPCTSTR lpszKeyName, LPCTSTR lpszValueName, const bool isWow64)
{
	HKEY hKey;
	DWORD kSize;
	DWORD dwReadVal=0;
	DWORD lpType= REG_DWORD;
	int access = KEY_READ;
	if(isWow64) access |= KEY_WOW64_64KEY;

	if(RegOpenKeyEx(parentKey,lpszKeyName,0,access,&hKey)!=ERROR_SUCCESS)
	{
		return -1;
	}
	kSize = sizeof(dwReadVal);
	if(RegQueryValueEx(hKey,lpszValueName,NULL,&lpType,(LPBYTE)&dwReadVal,&kSize)!=ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return -1;
	}			
	RegCloseKey(hKey);

	return dwReadVal;
}

DWORD CAntiSpySettings::GetDWORDRegKeyValue(const HKEY parentKey,LPCTSTR lpszKeyName, LPCTSTR lpszValueName)
{
	return GetDWORDRegKeyValue(parentKey, lpszKeyName, lpszValueName, IsWow64());
}


const CAntiSpySettings::REGKEY_TEST_RESULT CAntiSpySettings::TestRegKeyValue(const HKEY parentKey, LPCTSTR lpszKeyName,
										LPCTSTR lpszValueName,const DWORD type,
										const DWORD dwReqVal,LPCTSTR lpszReqVal, const bool isWow64) const
{
	HKEY hKey;
	DWORD kSize;
	DWORD dwReadVal=0;
	LPDWORD lpType=0;
	TCHAR lpszReadVal[128] = _T("N/A");
	DWORD readVal;

	int access = KEY_READ;
	if(isWow64) access |= KEY_WOW64_64KEY;


	if(RegOpenKeyEx(parentKey,lpszKeyName,0,access,&hKey)!=ERROR_SUCCESS)
	{
//		RegCloseKey(hKey);
		return NOT_FOUND;
	}
	switch(type)
	{
		case REG_SZ:
			kSize = sizeof(lpszReadVal);
			if(RegQueryValueEx(hKey,lpszValueName,NULL,lpType,(LPBYTE)lpszReadVal,&kSize)!=ERROR_SUCCESS)
			{
				RegCloseKey(hKey);
				return NOT_FOUND;
			}
			if(kSize == 0 && dwReqVal == 0)
				return MATCH;
			if(lpszReqVal!=NULL)
			{
				if(wcscmp(lpszReadVal,lpszReqVal)==0)
					return MATCH;//success
				else
					return NO_MATCH;
			}
			else
				return MATCH;
		break;
		case REG_DWORD:
			kSize = sizeof(dwReadVal);
			if(RegQueryValueEx(hKey,lpszValueName,NULL,lpType,(LPBYTE)&dwReadVal,&kSize)!=ERROR_SUCCESS)
			{
				RegCloseKey(hKey);
				return NOT_FOUND;
			}
			if(dwReadVal==dwReqVal) 
				return MATCH;//success
		break;
		case REG_BINARY:
			kSize = sizeof(lpszReadVal);
			if(RegQueryValueExW(hKey,lpszValueName,NULL,lpType,(LPBYTE)lpszReadVal,&kSize)!=ERROR_SUCCESS)
			{
				RegCloseKey(hKey);
				return NOT_FOUND;
			}
			TRACE1("deb: %s", lpszReadVal);
			if(lpszReqVal[0] == lpszReadVal[0])
				return MATCH;
			if(wcscmp(lpszReadVal,lpszReqVal)==0) 
				return MATCH;//success
		break;
	}
	RegCloseKey(hKey);

	return NO_MATCH;
}

const CAntiSpySettings::REGKEY_TEST_RESULT CAntiSpySettings::TestRegKeyValue(const HKEY parentKey, LPCTSTR lpszKeyName,
										LPCTSTR lpszValueName,const DWORD type,
										const DWORD dwReqVal,LPCTSTR lpszReqVal) const
{
	return TestRegKeyValue(parentKey, lpszKeyName, lpszValueName, type, dwReqVal, lpszReqVal, IsWow64());
}

/**
* See http://msdn.microsoft.com/en-us/library/aa384253%28v=VS.85%29.aspx what hives WOW64 should be used for
*/
bool CAntiSpySettings::TestRegKey(const HKEY parentKey,LPCTSTR lpszKeyName, const bool isWow64)
{
	int access = KEY_READ;
	if(isWow64) access |= KEY_WOW64_64KEY;

	HKEY hKey;
	if(RegOpenKeyEx(parentKey,lpszKeyName,0,access,&hKey)!=ERROR_SUCCESS)
	{
		return false;
	}
	RegCloseKey(hKey);

	return true;
}

bool CAntiSpySettings::TestRegKey(const HKEY parentKey,LPCTSTR lpszKeyName)
{
	return TestRegKey(parentKey, lpszKeyName, IsWow64());
}

bool CAntiSpySettings::TestKeyWritePermission(const HKEY parentKey, LPCTSTR lpszKeyName, const bool isWow64)
{
	if(GetSuiteType() == WINDOWS_VISTA || GetSuiteType() == WINDOWS_7 || GetSuiteType() == WINDOWS_10)
	{
		if(parentKey == HKEY_LOCAL_MACHINE || parentKey == HKEY_CLASSES_ROOT)
		{
			BOOL b;
			IsElevated(&b);
			if(b)
				return true;
			return false;
		}
	}
	HKEY hKey;
	long ret;
	if(isWow64)
		ret = RegOpenKeyEx(parentKey,lpszKeyName,0, KEY_ALL_ACCESS,&hKey);
	else
		ret = RegOpenKeyEx(parentKey,lpszKeyName,0, KEY_WOW64_64KEY | KEY_ALL_ACCESS,&hKey);
	RegCloseKey(hKey);
	if(ret == ERROR_ACCESS_DENIED)
	{
		return false;
	}
	return true;
}
bool CAntiSpySettings::TestKeyWritePermission(const HKEY parentKey, LPCTSTR lpszKeyName)
{
	return TestKeyWritePermission(parentKey, lpszKeyName, IsWow64());
}

const bool CAntiSpySettings::RecursiveDeleteRegKey(const HKEY hKeyRecurse, const CString& sSubKey) const
{
	return RecursiveDeleteRegKey(hKeyRecurse, sSubKey, IsWow64());
}

const bool CAntiSpySettings::RecursiveDeleteRegKey(const HKEY hKeyRecurse, const CString& sSubKey, const bool isWow64) const
{
	int access = KEY_ENUMERATE_SUB_KEYS | DELETE;
	if(isWow64)
		access |= KEY_WOW64_64KEY;

	HKEY hSubKey;
	LONG lnReturn = ::RegOpenKeyEx(hKeyRecurse, sSubKey, /* dwReserved */ 0, access, &hSubKey);

	while (ERROR_SUCCESS == lnReturn) 
	{
		TCHAR szSubKey[MAX_PATH + 1] = { 0 };
		DWORD cbSubkeySize(sizeof(szSubKey));

		lnReturn = ::RegEnumKeyEx(hSubKey, /* dwIndex */ 0, szSubKey, 
		  &cbSubkeySize, 0, 0, 0, 0);
		
		if (ERROR_NO_MORE_ITEMS == lnReturn) 
		{
			break;
		}
		else if (ERROR_SUCCESS != lnReturn) 
		{
			return lnReturn;
		}

		lnReturn = RecursiveDeleteRegKey(hSubKey, szSubKey, isWow64);

		if (ERROR_SUCCESS != lnReturn) 
		{
			break;
		}
	}

	// Close (but may not have been there).
	if (hSubKey) 
	{

		if (ERROR_SUCCESS == (lnReturn = ::RegCloseKey(hSubKey))) 
		{
			lnReturn = ::RegDeleteKey(hKeyRecurse, sSubKey);

			if (ERROR_SUCCESS != lnReturn) 
			{
			}
		}
		else 
		{
		}
	}


}

const bool CAntiSpySettings::DeleteRegKey(const HKEY parentKey,LPCTSTR lpszKeyName,
										  LPCTSTR lpszSubKey) const
{
	return DeleteRegKey(parentKey, lpszKeyName, lpszSubKey, IsWow64());
}

const bool CAntiSpySettings::DeleteRegKey(const HKEY parentKey,LPCTSTR lpszKeyName,
										  LPCTSTR lpszSubKey, const bool isWow64) const
{
	HKEY hKey;
	UINT errCode = 0;
    DWORD Size = _MAX_PATH;
    DWORD i = 0;
	int access = KEY_ALL_ACCESS;
	if(isWow64)
		access |= KEY_WOW64_64KEY;
	if(RegOpenKeyEx(parentKey,lpszKeyName,0,access,&hKey)!=ERROR_SUCCESS) errCode = 1;
	if(RegDeleteKey(hKey,lpszSubKey)!=ERROR_SUCCESS) errCode =2;
	RegCloseKey(hKey);
	if(errCode!=0) return 1;//if successful
	return 0;
}

const bool CAntiSpySettings::DeleteRegKeyValue(const HKEY parentKey,LPCTSTR lpszKeyName,
										       LPCTSTR lpszValueName) const
{	
	return DeleteRegKeyValue(parentKey, lpszKeyName, lpszValueName, IsWow64());
}

const bool CAntiSpySettings::DeleteRegKeyValue(const HKEY parentKey,LPCTSTR lpszKeyName,
										       LPCTSTR lpszValueName, bool isWow64) const
{
	HKEY hKey;
	UINT errCode = 0;
	int access = KEY_ALL_ACCESS;
	if(isWow64)
		access |= KEY_WOW64_64KEY;
	if(RegOpenKeyEx(parentKey,lpszKeyName,0,access,&hKey)!=ERROR_SUCCESS) errCode = 1;
	if(RegDeleteValue(hKey,lpszValueName)!=ERROR_SUCCESS) errCode = 2;
	RegCloseKey(hKey);
	if(errCode!=0) return 0;//if not successful
	return 1;
}

void CAntiSpySettings::RegisterDlls(LPCTSTR dllName) const
{
	CString fileStr;
	SHELLEXECUTEINFO seInfo;
	ZeroMemory(&seInfo, sizeof(SHELLEXECUTEINFO));
	seInfo.cbSize=sizeof(SHELLEXECUTEINFO);
	seInfo.nShow = SW_HIDE;
	seInfo.lpFile = _T("regsvr32");
	fileStr.Format(_T("/s %s"),dllName);
	seInfo.lpParameters = fileStr;
	ShellExecuteEx(&seInfo);
	Sleep(3000);
}

void CAntiSpySettings::UnregisterDlls(LPCTSTR dll) const
{
	CString fileStr;
	SHELLEXECUTEINFO seInfo;
	ZeroMemory(&seInfo, sizeof(SHELLEXECUTEINFO));
	seInfo.cbSize=sizeof(SHELLEXECUTEINFO);
	seInfo.nShow = SW_HIDE;
	seInfo.lpFile = _T("regsvr32");
	fileStr.Format(_T("/s /u %s"),dll);
	seInfo.lpParameters = fileStr;
	ShellExecuteEx(&seInfo);
	Sleep(3000);
}

void CAntiSpySettings::DelMsMsg() const
{
	HKEY hKey;
	TCHAR cWinDir[128];
	unsigned long type = REG_SZ;
	CString lpszParameters;
	DWORD kSize = sizeof(cWinDir);
	//get the systemdir
	RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"),0,KEY_READ,&hKey);
	RegQueryValueEx(hKey,_T("SystemRoot"),NULL,&type,(LPBYTE)cWinDir,&kSize);
	RegCloseKey(hKey);
	//uninstall the msmessenger
	SHELLEXECUTEINFO seInfo;
	ZeroMemory(&seInfo, sizeof(SHELLEXECUTEINFO));
	seInfo.cbSize=sizeof(SHELLEXECUTEINFO);
	seInfo.nShow = SW_HIDE;
	lpszParameters.Format(_T("advpack.dll,LaunchINFSection %s\\INF\\msmsgs.inf,BLC.Remove"),cWinDir);
	seInfo.lpParameters = lpszParameters;
	seInfo.lpDirectory = (LPCTSTR)cWinDir;
	seInfo.lpFile = _T("RunDll32.exe");
	seInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShellExecuteEx(&seInfo);
	Sleep(10000);
}

void CAntiSpySettings::DelSupportUser() const
{
	SHELLEXECUTEINFO seInfo;
	ZeroMemory(&seInfo, sizeof(SHELLEXECUTEINFO));
	seInfo.cbSize=sizeof(SHELLEXECUTEINFO);
	seInfo.nShow = SW_HIDE;
	seInfo.lpFile = _T("net");
	seInfo.lpParameters=_T(" user SUPPORT_388945a0 /delete");
	seInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShellExecuteEx(&seInfo);
}

const bool CAntiSpySettings::IsSupportuser() const
{
	TCHAR sid_buffer[1024];
	DWORD sid_size = sizeof(sid_buffer);
	PSID sid = (PSID)sid_buffer;
	TCHAR domain_buffer[1024];
	DWORD domain_size = sizeof(domain_buffer);
	LPTSTR domain = domain_buffer;
	SID_NAME_USE snu; // dummy

	if(LookupAccountName(NULL, _T("SUPPORT_388945a0"),sid, &sid_size,domain, &domain_size,&snu))return true;

	return false;
}

const bool CAntiSpySettings::SetBandwidthLimit(bool setting) const
{
	UINT errCode=0;
	HKEY hKey;
	HKEY hSubKey;
	DWORD	KeyDisposition;

	switch(setting)
	{
		case true: //delete regKeys
			DeleteRegKey(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Group Policy Objects\\LocalMachine\\Software\\Policies\\Microsoft\\Windows"),_T("Psched"));
			DeleteRegKey(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Group Policy Objects\\LokalMachine\\Software\\Policies\\Microsoft\\Windows"),_T("Psched"));
			return true;
		break;
		case false: //create regKeys
			RegOpenKeyEx(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion"),0,KEY_ALL_ACCESS|(IsWow64()?KEY_WOW64_64KEY:0),&hKey);
			RegCreateKeyEx(hKey,_T("Group Policy Objects"),NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_CREATE_SUB_KEY|(IsWow64()?KEY_WOW64_64KEY:0),NULL,&hSubKey,&KeyDisposition);
			RegCreateKeyEx(hSubKey,_T("LocalMachine"),NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_CREATE_SUB_KEY|(IsWow64()?KEY_WOW64_64KEY:0),NULL,&hSubKey,&KeyDisposition);
			RegCreateKeyEx(hSubKey,_T("Software"),NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_CREATE_SUB_KEY|(IsWow64()?KEY_WOW64_64KEY:0),NULL,&hSubKey,&KeyDisposition);
			RegCreateKeyEx(hSubKey,_T("Policies"),NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_CREATE_SUB_KEY|(IsWow64()?KEY_WOW64_64KEY:0),NULL,&hSubKey,&KeyDisposition);
			RegCreateKeyEx(hSubKey,_T("Microsoft"),NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_CREATE_SUB_KEY|(IsWow64()?KEY_WOW64_64KEY:0),NULL,&hSubKey,&KeyDisposition);
			RegCreateKeyEx(hSubKey,_T("Windows"),NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_CREATE_SUB_KEY|(IsWow64()?KEY_WOW64_64KEY:0),NULL,&hSubKey,&KeyDisposition);
			RegCreateKeyEx(hSubKey,_T("Psched"),NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_CREATE_SUB_KEY|(IsWow64()?KEY_WOW64_64KEY:0),NULL,&hSubKey,&KeyDisposition);
			RegCloseKey(hKey);
			RegCloseKey(hSubKey);
			SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Group Policy Objects\\LocalMachine\\Software\\Policies\\Microsoft\\Windows\\Psched"),_T("NonBestEffortLimit"),0);
//for german version
			RegOpenKeyEx(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion"),0,KEY_ALL_ACCESS|(IsWow64()?KEY_WOW64_64KEY:0),&hKey);
			RegCreateKeyEx(hKey,_T("Group Policy Objects"),NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_CREATE_SUB_KEY|(IsWow64()?KEY_WOW64_64KEY:0),NULL,&hSubKey,&KeyDisposition);
			RegCreateKeyEx(hSubKey,_T("LokalMachine"),NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_CREATE_SUB_KEY|(IsWow64()?KEY_WOW64_64KEY:0),NULL,&hSubKey,&KeyDisposition);
			RegCreateKeyEx(hSubKey,_T("Software"),NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_CREATE_SUB_KEY|(IsWow64()?KEY_WOW64_64KEY:0),NULL,&hSubKey,&KeyDisposition);
			RegCreateKeyEx(hSubKey,_T("Policies"),NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_CREATE_SUB_KEY|(IsWow64()?KEY_WOW64_64KEY:0),NULL,&hSubKey,&KeyDisposition);
			RegCreateKeyEx(hSubKey,_T("Microsoft"),NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_CREATE_SUB_KEY|(IsWow64()?KEY_WOW64_64KEY:0),NULL,&hSubKey,&KeyDisposition);
			RegCreateKeyEx(hSubKey,_T("Windows"),NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_CREATE_SUB_KEY|(IsWow64()?KEY_WOW64_64KEY:0),NULL,&hSubKey,&KeyDisposition);
			RegCreateKeyEx(hSubKey,_T("Psched"),NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_CREATE_SUB_KEY|(IsWow64()?KEY_WOW64_64KEY:0),NULL,&hSubKey,&KeyDisposition);
			RegCloseKey(hKey);
			RegCloseKey(hSubKey);
			SetDWORDRegKeyValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Group Policy Objects\\LokalMachine\\Software\\Policies\\Microsoft\\Windows\\Psched"),_T("NonBestEffortLimit"),0);
			return true;
		break;
	}
	return false;
}

bool CAntiSpySettings::IsQOSEnabled() const
{
	return	TestRegKeyValue(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Services\\PSched\\Enum"),_T("Count"),REG_DWORD,1,NULL)!=NO_MATCH||
			TestRegKeyValue(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\Services\\PSched\\Enum"),_T("Count"),REG_DWORD,2,NULL)!=NO_MATCH;

}

const bool CAntiSpySettings::IsOfficeXpInstalled() const
{
	//check for the existence of Path in that hive. This means, it's installed
	return TestRegKeyValue(HKEY_LOCAL_MACHINE,_T("Software\\Microsoft\\Office\\10.0\\Common\\InstallRoot"),_T("Path"),REG_SZ,1,_T(""))!= NOT_FOUND;

}

const bool CAntiSpySettings::IsOffice2k3Installed() const
{
	//check for the existence of Path in that hive. This means, it's installed
	return TestRegKeyValue(HKEY_LOCAL_MACHINE,_T("Software\\Microsoft\\Office\\11.0\\Common\\InstallRoot"),_T("Path"),REG_SZ,1,_T(""), false) != NOT_FOUND;
}

const bool CAntiSpySettings::IsOffice2k7Installed() const
{
	//check for the existence of Path in that hive. This means, it's installed
	return TestRegKeyValue(HKEY_LOCAL_MACHINE,_T("Software\\Microsoft\\Office\\12.0\\Common\\InstallRoot"),_T("Path"),REG_SZ,1,_T(""), false) != NOT_FOUND;
}

const bool CAntiSpySettings::IsMediaPlayer() const
{
	int ver = GetMediaPlayerVersion();
	return ver >= 8;
}

const int CAntiSpySettings::GetMediaPlayerVersion() const
{
	CString str = GetRegKeyValue(HKEY_LOCAL_MACHINE,_T("Software\\Microsoft\\MediaPlayer\\PlayerUpgrade"),_T("PlayerVersion"),REG_SZ);
	//str.Format(_T("%d"), str);
	return _wtoi(str);
}

const bool CAntiSpySettings::IsMediaPlayer11() const
{
	return GetMediaPlayerVersion() == 11;
}

const bool CAntiSpySettings::IsMediaPlayer12() const
{
	return GetMediaPlayerVersion() == 12;
}

const bool CAntiSpySettings::IsMediaPlayer9() const
{
	return GetMediaPlayerVersion() == 9;
}

const bool CAntiSpySettings::IsMS_Messenger() const
{
	int val1 = TestSetting(ST_MESSENGER_DEINSTALL);
	//int val2 = TestSetting(ST_MESSENGER_6_1_DEINSTALL);
	return (val1 != 2);// || (val2 != 2);
}

const unsigned int CAntiSpySettings::GetIEVersion() const
{
	CString str = GetRegKeyValue(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Internet Explorer"),_T("Version"),REG_SZ);
	int ver = _wtoi(str.Left(1));
	if(ver > 5 && ver < 10)
		return ver;
	return 0;
}

CString CAntiSpySettings::GetUninstallString(CString startString)
{
	HKEY parentKey=HKEY_LOCAL_MACHINE;
	LPTSTR lpszKeyName=_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall");
	HKEY hKey;
	DWORD dwSize=MAX_PATH;;
	DWORD dwIndex=0;
	DWORD dwType=REG_SZ;
	LPDWORD lpType=0;
	TCHAR lpszReadVal[MAX_PATH+1];
	FILETIME lastWrite;
	if(RegOpenKeyEx(parentKey,lpszKeyName,0,KEY_READ,&hKey)!=ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return "";
	}

	//int errVal=RegQueryInfoKey(hKey,NULL,NULL,NULL,&dwIndex,&dwSize,NULL,NULL,NULL,NULL,NULL,&lastWrite);
	std::vector<CString> tmpVec;
	int errVal=RegEnumKeyEx(hKey,dwIndex,lpszReadVal,&dwSize,NULL,NULL,NULL,&lastWrite);
	while(errVal==ERROR_SUCCESS||errVal==ERROR_MORE_DATA)
	{
		CString tmpStr = CString(lpszReadVal);
		tmpVec.push_back(tmpStr);
		errVal=RegEnumKeyEx(hKey,++dwIndex,lpszReadVal,&dwSize,NULL,NULL,NULL,&lastWrite);
	}
	RegCloseKey(hKey);
	for(std::vector<CString>::iterator it=tmpVec.begin(); it!=tmpVec.end(); ++it)
	{
		if(RegQueryValueEx(parentKey,lpszKeyName + CString(_T("\\")) + *it + CString(_T("\\DisplayName")),NULL,&dwType,(LPBYTE)lpszReadVal,&dwSize)==ERROR_SUCCESS)
		{
			CString tmp(lpszReadVal);
			if(tmp.Find(startString)!=-1)
			{
				return lpszKeyName + CString(_T("\\")) + *it;
			}

		}
	} 
	return _T("");
		
} 

const bool CAntiSpySettings::ExistFile(CString file) const
{
	TCHAR windir[MAX_PATH];
	GetWindowsDirectory(windir, MAX_PATH);
	CString tmp;
	tmp.Format(_T("%s\\system32\\%s"), windir, file);
	struct _stat buffer ;
	if(_wstat(tmp, &buffer) == 0)
		return true;
	return false;
}

const bool CAntiSpySettings::IsWinXP() const
{
    OSVERSIONINFOEX osVerEx;
    memset (&osVerEx, 0, sizeof (OSVERSIONINFO));
    osVerEx.dwOSVersionInfoSize = sizeof (OSVERSIONINFOEX);
    GetVersionEx ((OSVERSIONINFO *)&osVerEx);
	if(osVerEx.dwMajorVersion==5&&osVerEx.dwMinorVersion==1) return true;
	return false;
}

const HKEY CAntiSpySettings::chooseHKey(const HKEY choice1, HKEY choice2, LPCTSTR key) const
{
	bool ret1 = TestKeyWritePermission(choice1, key);	
	bool ret2 = TestKeyWritePermission(choice2, key);
	if(ret1 == true)
		return choice1;
	else if(ret2 == true)
		return choice2;
	else
		return NULL;

}

//bool CAntiSpySettings::ServiceExist(const LPSTR svc)
//{
//	return m_svcControl->GetStartType(svc)!=5;
//}

const unsigned int CAntiSpySettings::ScanUserAssist(std::vector<CString>& vec)
{
	HKEY parentKey=HKEY_CURRENT_USER;
	LPTSTR lpszKeyName=_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\UserAssist");
	HKEY hKey;
	DWORD dwSize=MAX_PATH;;
	DWORD dwIndex=0;
	LPDWORD lpType=0;
	TCHAR lpszReadVal[MAX_PATH+1];
	FILETIME lastWrite;
	if(RegOpenKeyEx(parentKey,lpszKeyName,0,KEY_READ,&hKey)!=ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return 2;
	}

	//int errVal=RegQueryInfoKey(hKey,NULL,NULL,NULL,&dwIndex,&dwSize,NULL,NULL,NULL,NULL,NULL,&lastWrite);
	std::vector<CString> tmpVec;
	int errVal=RegEnumKeyEx(hKey,dwIndex,lpszReadVal,&dwSize,NULL,NULL,NULL,&lastWrite);
	while(errVal==ERROR_SUCCESS||errVal==ERROR_MORE_DATA)
	{
		tmpVec.push_back(CString(lpszReadVal));
		errVal=RegEnumKeyEx(hKey,++dwIndex,lpszReadVal,&dwSize,NULL,NULL,NULL,&lastWrite);
	}

	RegCloseKey(hKey);
}

UINT CAntiSpySettings::GetSP() const
{
    OSVERSIONINFOEX osVerEx;
    memset (&osVerEx, 0, sizeof (OSVERSIONINFO));
    osVerEx.dwOSVersionInfoSize = sizeof (OSVERSIONINFOEX);
    GetVersionEx ((OSVERSIONINFO *)&osVerEx);
	if(osVerEx.dwMajorVersion==5 && osVerEx.dwMinorVersion>0) 
		return osVerEx.wServicePackMajor;
	return 0;
}

bool CAntiSpySettings::SetUpldDrvInfo(const bool set) const
{
	FILE* file;
	TCHAR oldFile[MAX_PATH];
	TCHAR newFile[MAX_PATH];
	GetWindowsDirectory(oldFile,sizeof(oldFile));
	wcscat(oldFile,_T("\\PCHealth\\HelpCtr\\System\\DFS\\uplddrvinfo.htm"));
	GetWindowsDirectory(newFile,sizeof(newFile));
	wcscat(newFile,_T("\\PCHealth\\HelpCtr\\System\\DFS\\uplddrvinfo.htm.bak"));
	switch(set)
	{
		case false:
			file=_wfopen(oldFile,_T("r"));
			if(file)
			{
				fclose(file);
				_wrename(oldFile,newFile);
			}
		break;
		case true:
			file=_wfopen(newFile,_T("r"));
			if(file)
			{
				fclose(file);
				return !_wrename(newFile,oldFile);
			}
		break;
	}
	return false;
}

const bool CAntiSpySettings::ServiceExist(LPCTSTR svc)const
{
	return m_svcControl->GetStartType(svc)!=5;
}

const int CAntiSpySettings::GetAdobeReaderVersion() const
{
	if(TestRegKey(HKEY_CURRENT_USER, _T("SOFTWARE\\Adobe\\Acrobat Reader\\7.0")))
		return 7;
	else if(TestRegKey(HKEY_CURRENT_USER, _T("SOFTWARE\\Adobe\\Acrobat Reader\\8.0")))
		return 8;
	else if(TestRegKey(HKEY_CURRENT_USER, _T("SOFTWARE\\Adobe\\Acrobat Reader\\9.0")))
		return 9;
	else 
		return -1;
}

bool CAntiSpySettings::CreateCTFMonDummy(CString file)
{

	HRSRC hResource = FindResource(NULL, MAKEINTRESOURCE(IDR_CTFMON_DUMMY), _T("binary")); 
	HGLOBAL hFileResource = LoadResource(NULL, hResource);
	// Now open and map this to a disk file           
	LPVOID lpFile = LockResource(hFileResource);           
	DWORD dwSize = SizeofResource(NULL, hResource);            

	// Open the file and filemap           
	HANDLE hFile = CreateFile(file, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);           
	HANDLE hFilemap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, dwSize, NULL);   
	LPVOID lpBaseAddress = MapViewOfFile(hFilemap, FILE_MAP_WRITE, 0, 0, 0);            

	// Write the file 
	CopyMemory(lpBaseAddress, lpFile, dwSize);            

	// Unmap the file and close the handles

	UnmapViewOfFile(lpBaseAddress);           
	CloseHandle(hFilemap);           
	CloseHandle(hFile);     
	return false;

}

bool CAntiSpySettings::CreateEmptyIcon(CString file)
{

	HRSRC hResource = FindResource(NULL, MAKEINTRESOURCE(IDR_EMPTY_ICON), _T("binary")); 
	HGLOBAL hFileResource = LoadResource(NULL, hResource);
	// Now open and map this to a disk file           
	LPVOID lpFile = LockResource(hFileResource);           
	DWORD dwSize = SizeofResource(NULL, hResource);            

	// Open the file and filemap           
	HANDLE hFile = CreateFile(file, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);           
	HANDLE hFilemap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, dwSize, NULL);   
	LPVOID lpBaseAddress = MapViewOfFile(hFilemap, FILE_MAP_WRITE, 0, 0, 0);            

	// Write the file 
	CopyMemory(lpBaseAddress, lpFile, dwSize);            

	// Unmap the file and close the handles

	UnmapViewOfFile(lpBaseAddress);           
	CloseHandle(hFilemap);           
	CloseHandle(hFile);     
	return false;

}

CString ConvertSidToString(PSID pSID)
{
	// Check input pointer
	//assert(pSID!=null);
	// Get string corresponding to SID
	LPTSTR pszSID = NULL;
	if (!ConvertSidToStringSid(pSID, &pszSID))
	{
		return _T("");
	}

	// Deep copy result in a CString instance
	CString strSID(pszSID);

	// Release buffer allocated by ConvertSidToStringSid API
	LocalFree(pszSID);
	pszSID = NULL;

	// Return string representation of the SID
	return strSID;
}

const CString CAntiSpySettings::getCurrentUserSID() const
{
	HANDLE hTok = NULL;
	CString sid = "";
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hTok))
	{
		// get user info size
		LPBYTE buf = NULL;
		DWORD  dwSize = 0;
		GetTokenInformation(hTok, TokenUser, NULL, 0, &dwSize);
		if (dwSize)
			buf = (LPBYTE)LocalAlloc(LPTR, dwSize);

		// get user info
		if (GetTokenInformation(hTok, TokenUser, buf, dwSize, &dwSize))
		{
			// here's the SID we've looked for
			PSID pSid = ((PTOKEN_USER)buf)->User.Sid;

			// check user name for SID
			TCHAR user[20], domain[20];
			DWORD cbUser = 20, cbDomain = 20;
			SID_NAME_USE nu;
			LookupAccountSid(NULL, pSid, user, &cbUser, domain, &cbDomain, &nu);
			sid = ConvertSidToString(pSid);
		}
		LocalFree(buf);
		CloseHandle(hTok);
	}
	return sid;
}


void CAntiSpySettings::SetRegPermission(CString keyStr)
{
	HKEY myKey;
	unsigned char *p = new unsigned char[9000];
	PSECURITY_DESCRIPTOR psecdesc = (PSECURITY_DESCRIPTOR)p;
	DWORD sts =::RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Microsoft Antimalware\\SpyNet"),0,KEY_ALL_ACCESS|(IsWow64()?KEY_WOW64_64KEY:0),&myKey);
	if(sts == ERROR_SUCCESS)
	{
		sts =::InitializeSecurityDescriptor(psecdesc,SECURITY_DESCRIPTOR_REVISION);
		sts = ::SetSecurityDescriptorDacl(psecdesc,TRUE,NULL,TRUE);
		sts = ::RegSetKeySecurity (myKey,DACL_SECURITY_INFORMATION,psecdesc);
		sts = ::RegCloseKey(myKey);
	}
	if(p) delete [] p;
}


CString CAntiSpySettings::GetRegKeyAsString(const unsigned int id)
{
	CString strRet;
/*
	CString strDsc;
	strDsc.LoadString(IDS_REGKEY_DESCRIPTION);
	CString strAct;
	strAct.LoadString(IDS_REGKEY_ACTIVATE);
	CString strDeAct;
	strDeAct.LoadString(IDS_REGKEY_SYSTEM_DEFAULT);
	switch(id)
	{
	case ST_MEDIAPLAYER_LICENSES:
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
						"HKEY_CURRENT_USER\\Software\\Microsoft\\MediaPlayer\\Preferences\\SilentAcquisition",
						strAct,
						"0",
						strDeAct,
						"1");
		break;
	case ST_MEDIAPLAYER_SEND_GUID:
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
						"HKEY_CURRENT_USER\\Software\\Microsoft\\MediaPlayer\\Preferences\\SendUserGUID",
						strAct,
						"0",
						strDeAct,
						"1");
		break;
	case ST_MEDIAPLAYER_CODECS:
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
						"HKEY_CURRENT_USER\\Software\\Microsoft\\MediaPlayer\\Preferences\\UpgradeCodecPrompt",
						strAct,
						"0",
						strDeAct,
						"1");
		break;
	case ST_MP9_ADDMEDIATOLIB:
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
	"HKEY_CURRENT_USER\\Software\\Microsoft\\MediaPlayer\\Preferences\\AutoAddMusicToLibrary",
						strAct,
						"0",
						strDeAct,
						"1");
		break;
	case ST_MP9_RETRIEVEMETADATA:
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_CURRENT_USER\\Software\\Microsoft\\MediaPlayer\\Preferences\\MetadataRetrieval",
						strAct,
						"0",
						strDeAct,
						"1");
		break;
	case ST_MP9_USAGETRACKING:
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_CURRENT_USER\\Software\\Microsoft\\MediaPlayer\\Preferences\\UsageTracking",
						strAct,
						"0",
						strDeAct,
						"1");
		break;
	case ST_MP9_SAVEMRU:
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_CURRENT_USER\\Software\\Microsoft\\MediaPlayer\\Preferences\\DisableMRU",
						strAct,
						"0",
						strDeAct,
						"1");
		break;
	case ST_ERROR_REPORTS_SEND:
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\PCHealth\\ErrorReporting\\DoReport",
						strAct,
						"0",
						strDeAct,
						"1");
	break;
	case ST_REMOTEDESKTOP:
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Terminal Server\\fAllowToGetHelp",
						strAct,
						"0",
						strDeAct,
						"1");
	break;
	case ST_IE_SCHEDULED_UPDATES:
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Webcheck\\NoScheduledUpdates",
						strAct,
						"0",
						strDeAct,
						"1");
	break;
	case ST_IE_CHECK_4_UPDATES:
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_CURRENT_USER\\Software\\Microsoft\\Internet Explorer\\Main\\NoUpdateCheck",
						strAct,
						"0",
						strDeAct,
						"1");
	break;
	case ST_IE_FORM_SUGGEST:
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_CURRENT_USER\\Software\\Microsoft\\Internet Explorer\\Main\\Use FormSuggest",
						strAct,
						"0",
						strDeAct,
						"1");
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_CURRENT_USER\\Software\\Microsoft\\Internet Explorer\\Main\\FormSuggest Passwords",
						strAct,
						"0",
						strDeAct,
						"1");
	break;
	case ST_IE_JAVASCRIPT:
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Zones\\3\\1400",
						strAct,
						"0",
						strDeAct,
						"1");
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Zones\\3\\1402",
						strAct,
						"0",
						strDeAct,
						"1");
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Zones\\3\\1407",
						strAct,
						"0",
						strDeAct,
						"1");
	break;
	case ST_IE_ACTIVEX:
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Zones\\3\\1200",
						strAct,
						"0",
						strDeAct,
						"1");
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Zones\\3\\1405",
						strAct,
						"0",
						strDeAct,
						"1");
	break;
	case ST_IE_POPUP_BLOCKER:
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Zones\\3\\1809",
						strAct,
						"0",
						strDeAct,
						"1");
	break;
	case ST_TIME_SYNC:
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\W32Time\\Parameters\\Type""NoSync",
						strAct,
						"0",
						strDeAct,
						"1");
	break;
	case ST_IE_INTEGRATED_AUTHENTICATION:
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\EnableNegotiate",
						strAct,
						"0",
						strDeAct,
						"1");
	break;
	case ST_REGDONE:
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\RegDone",
						strAct,
						"0",
						strDeAct,
						"1");
	break;
	case ST_ALEXA:
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Internet Explorer\\Extensions\\{c95fe080-8f5d-11d2-a20b-00aa003c157a}",
						strAct,
						"0",
						strDeAct,
						"1");
	break;
	case ST_MESSENGER_6_1_AUTOSTART:
	case ST_MESSENGER_AUTOSTART:
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run\\MSMSGS",
						strAct,
						"0",
						strDeAct,
						"1");
	break;
	case ST_BALLOON_TIPS:
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced\\EnableBalloonTips",
						strAct,
						"0",
						strDeAct,
						"1");
	break;
	case ST_CLEAR_PAGEFILE:
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_LOCAL_MACHINE\\System\\CurrentControlSet\\Control\\Session Manager\\Memory Management\\ClearPageFileAtShutdown",
						strAct,
						"0",
						strDeAct,
						"1");
	break;
	case ST_OE_START_MESSENGER:
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Outlook Express\\Hide Messenger",
						strAct,
						"0",
						strDeAct,
						"1");
	break;
	case ST_FAST_SHUTDOWN:
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_LOCAL_MACHINE\\SYSTEM\\ControlSet001\\Control\\WaitToKillServiceTimeout",
						strAct,
						"0",
						strDeAct,
						"1");
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_CURRENT_USER\\Control Panel\\Desktop\\AutoEndTasks",
						strAct,
						"0",
						strDeAct,
						"1");
	break;
	case ST_MAX_CONNECTIONS:
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\MaxConnectionsPer1_0Server",
						strAct,
						"0",
						strDeAct,
						"1");
	break;
	case ST_OXP_ERRORREPORT:
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_CURRENT_USER\\Software\\Microsoft\\Office\\10.0\\Common\\DWNeverUpload",
						strAct,
						"0",
						strDeAct,
						"1");
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_CURRENT_USER\\Software\\Microsoft\\Office\\10.0\\Common\\DWNoExternalURL",
						strAct,
						"0",
						strDeAct,
						"1");
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_CURRENT_USER\\Software\\Microsoft\\Office\\10.0\\Common\\DWNoFileCollection",
						strAct,
						"0",
						strDeAct,
						"1");
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_CURRENT_USER\\Software\\Microsoft\\Office\\10.0\\Common\\DWNoSecondLevelCollection",
						strAct,
						"0",
						strDeAct,
						"1");
	break;
	case ST_O2k3_ERRORREPORT:
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_CURRENT_USER\\Software\\Microsoft\\Office\\11.0\\Common\\DWNeverUpload",
						strAct,
						"0",
						strDeAct,
						"1");
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_CURRENT_USER\\Software\\Microsoft\\Office\\11.0\\Common\\DWNoExternalURL",
						strAct,
						"0",
						strDeAct,
						"1");
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_CURRENT_USER\\Software\\Microsoft\\Office\\11.0\\Common\\DWNoFileCollection",
						strAct,
						"0",
						strDeAct,
						"1");
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_CURRENT_USER\\Software\\Microsoft\\Office\\11.0\\Common\\DWNoSecondLevelCollection",
						strAct,
						"0",
						strDeAct,
						"1");
	break;
	case ST_FIREWALL:
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\SharedAccess\\Parameters\\FirewallPolicy\\StandardProfile\\EnableFirewall",
						strAct,
						"0",
						strDeAct,
						"1");
	break;
	case ST_SHOW_FILE_EXT:
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced\\HideFileExt",
						strAct,
						"0",
						strDeAct,
						"1");
	break;
	case ST_HIDE_IN_NETWORK:
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\LanManServer\\Parameters\\Hide",
						strAct,
						"0",
						strDeAct,
						"1");
	break;
	case ST_DENY_REGEDIT:
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System\\DisableRegistryTools",
						strAct,
						"0",
						strDeAct,
						"1");
	break;
	case ST_SCRIPTING_HOST:
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows Script Host\\Settings\\Enabled",
						strAct,
						"0",
						strDeAct,
						"1");
	break;
	case ST_SHOW_LAST_USERNAME:
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System\\DontDisplayLastUserName",
						strAct,
						"0",
						strDeAct,
						"1");
	break;
	case ST_IE_WATSON:
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Internet Explorer\\Main\\IEWatsonDisabled",
						strAct,
						"0",
						strDeAct,
						"1");
	break;
	case ST_IE_CLEAN_CACHE:
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Cache\\Persistent",
						strAct,
						"0",
						strDeAct,
						"1");
	break;
	case ST_MEDIAPLAYER_UPDATE:
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_CURRENT_USER\\Software\\Microsoft\\MediaPlayer\\Preferences\\UpgradeCheckFrequency",
						strAct,
						"0",
						strDeAct,
						"1");
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_CURRENT_USER\\Software\\Microsoft\\MediaPlayer\\Preferences\\AskMeAgain",	
						strAct,
						"0",
						strDeAct,
						"1");
	break;
	case ST_LNK_SUFFIX:
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_CLASSES_ROOT\\lnkfile\\NeverShowExt",
						strAct,
						"0",
						strDeAct,
						"1");
	break;
	case ST_PIF_SUFFIX:
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_CLASSES_ROOT\\piffile\\NeverShowExt",
						strAct,
						"0",
						strDeAct,
						"1");
	break;
	case ST_SHC_SUFFIX:
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_CLASSES_ROOT\\SHCmdFile\\NeverShowExt",
						strAct,
						"0",
						strDeAct,
						"1");
	break;
	case ST_URL_SUFFIX:
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_CLASSES_ROOT\\InternetShortcut\\NeverShowExt",
						strAct,
						"0",
						strDeAct,
						"1");
	break;
	case ST_CD_AUTORUN:
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_LOCAL_MACHINESYSTEM\\CurrentControlSet\\Services\\Cdrom\\AutoRun",
						strAct,
						"0",
						strDeAct,
						"1");
	break;
	case ST_CLEAN_RECENT_DOCS:
		strRet.Format("%s: %s %s: %s %s: %s",
						strDsc,
"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer\\ClearRecentDocsOnExit",
						strAct,
						"0",
						strDeAct,
						"1");
	}
*/
	return strRet;
}
