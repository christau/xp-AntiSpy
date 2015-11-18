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

#include "stdafx.h"
#include "xpAntiSpy.h"
#include "XPAntiSpyDlg.h"
#include "TimeServerDlg.h"
#include "ServicesControl.h"
#include "connectionlimitdlg.h"
#include "ProfileDlg.h"
#include "initprofiledlg.h"
#include "resource.h"
#include "settings_helper.h"
#include "resource_helper.h"


#include <atlbase.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define GERMAN 1
#define ENGLISH 60

/////////////////////////////////////////////////////////////////////////////
// CXPAntiSpyDlg Dialogfeld
typedef BOOL (WINAPI *PFUNC_GetProductInfo)(DWORD, DWORD, DWORD, DWORD, PDWORD);
PFUNC_GetProductInfo pGetProductInfo = NULL;

CXPAntiSpyDlg::CXPAntiSpyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CXPAntiSpyDlg::IDD, pParent), m_split(2,SSP_VERT), m_HelpBox()
{
	m_showSearchControl = false;
	m_isTreeFiltered = false;
	//{{AFX_DATA_INIT(CXPAntiSpyDlg)
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON2);
	m_profile_on[0] = ST_END_ENTRY;
	m_profile_off[0] = ST_END_ENTRY;
	m_applyBtnState = true;
//	memset(m_entryItem,0,sizeof(TreeItem)*ITEMCOUNT);
//	memset(m_hGroupItem,0,sizeof(GroupItem)*GROUPCOUNT);
	m_entryItem = new TreeItem[ITEMCOUNT];
	m_hGroupItem = new GroupItem[GROUPCOUNT];
	//m_profileManager.setAntiSpySettings(&m_AntiSpySettings);
	m_pSymbolReference = NULL;
}

CXPAntiSpyDlg::~CXPAntiSpyDlg()
{
	delete[] m_entryItem;
	delete[] m_hGroupItem;
	if(m_pSymbolReference != NULL)
	{
		delete m_pSymbolReference;
	}
	m_txtFont.DeleteObject();
}


BEGIN_MESSAGE_MAP(CXPAntiSpyDlg, CDialog)
	//{{AFX_MSG_MAP(CXPAntiSpyDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_FWD, OnBtnFwd)
	ON_COMMAND(ID_MENU_ABOUT, OnMenuAbout)
	ON_COMMAND(ID_SPECIAL_CHECKSYSTEMSTATUS, OnSpecialRefresh)
	ON_COMMAND(ID_SPEZIAL_TIMESERVERFESTLEGEN, OnSpecialSetTimeServer)
	ON_COMMAND(ID_MNU_RUNAS_ADMIN, OnRunAsAdmin)
	ON_COMMAND(ID__VISITTHEXPANTISPYHOMEPAGE, OnMenuVisitHP)
	ON_COMMAND(ID__VIEWQUICKLEGEND, OnMenuQuickReference)
	ON_WM_SIZE()
	ON_COMMAND(ID_RESET, OnPopUpResetSetting)
	ON_COMMAND(ID_SET,	 OnPopUpSetSetting)
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID__INDEX, OnHelpIndex)
	ON_COMMAND(ID_SPEZIAL_VERBINDUNGSLIMIT, OnConnectionLimit)
	//}}AFX_MSG_MAP
//#ifdef __GERMAN__
//#endif
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXPAntiSpyDlg Nachrichten-Handler

BOOL CXPAntiSpyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
 
	m_helpBoxHeight=70;

	InitCommonControls();
	SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
	menu.LoadMenu(IDR_MENU1);
	m_hPopUp.LoadMenu(IDR_POPUP);
	bool success = m_searchIcon.Create(_T("icnSearch"),WS_CHILD|WS_VISIBLE|SS_BITMAP|SS_CENTERIMAGE, CRect(0,0,22,SEARCH_HEIGHT + 2), this);
	HBITMAP hbmp =(HBITMAP)::LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_ICN_SEARCH), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE );

	success = m_searchIcon.SetBitmap(hbmp);
	m_searchField.Create(ES_LEFT|WS_CHILD|WS_VISIBLE|WS_TABSTOP, CRect(10,1,200,SEARCH_HEIGHT), this, 98);
	m_searchField.setBkColor(RGB(255,255,255));

	m_btnClearSearch.Create(_T(""),WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_BITMAP, CRect(10,1,200,SEARCH_HEIGHT), this,5);
	m_btnClearSearch.setHasBorder(false);
	hbmp =(HBITMAP)::LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_EDIT_CLEAR), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE );
	m_btnClearSearch.SetBitmap(hbmp);
	hbmp =(HBITMAP)::LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_EDIT_CLEAR_HOVER), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE );
	m_btnClearSearch.setHoverBitmap(hbmp);
	m_btnClearSearch.setUseBitmap(true);
	
	
	m_searchIcon.ShowWindow(m_showSearchControl);
	m_searchField.ShowWindow(m_showSearchControl);
	m_btnClearSearch.ShowWindow(m_showSearchControl);
	CRect rect(10,250,100,350);
	m_split.Create(this);
	m_HelpBox.Create(WS_CHILD|WS_VISIBLE,rect,&m_split,2);
//////////////////////////////////	
	m_TreeCtrl.Create(WS_CHILD|WS_VISIBLE|WS_BORDER|WS_TABSTOP|TVS_SHOWSELALWAYS,rect,&m_split,0);

	//#endif
	m_split.SetPane(0,&m_TreeCtrl);
	m_split.SetPane(1,&m_HelpBox);
	CString tmp;
	tmp.LoadString(IDS_BTN_APPLY);
	m_BtnFwd.Create(tmp,WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_BITMAP,rect,this,4);
	m_BtnFwd.setHasBorder(true);

	this->SetMenu(&menu);
	
	//set Windowtitle
	CString title=APPVERSION;
	OS_INFO osInfo;
	GetSuiteType(osInfo);
	title+=" - " + osInfo.osName;
	if(osInfo.servicePack>0) 
	{
		CString tmp;
		tmp.Format(_T(" Service Pack %d "),osInfo.servicePack);
		title+=tmp;
	}
	if(ConnectionLimit::Wow64Capable())
	{
		title += _T(" 64Bit");
	}
	else title+=_T(" -");
	this->SetWindowText(title);
	
	//pass our own address to the treeview, for popup-messages
	m_TreeCtrl.SetAppDlgPointer(this);

	memset(m_entryItem, 0, sizeof(TreeItem)*ITEMCOUNT);
	InitTreeView();
	m_TreeCtrl.SelectItem(m_hGroupItem[GI_DATASAFETY].hItem);
	CheckForSpyFunctions();

	const int i[]={90,30};
	m_split.SetPaneSizes(i);
	m_split.SetActivePane(0);

	//deactivate connection limit menu entry if we're on vista
	if(CAntiSpySettings::GetSuiteType() >= CAntiSpySettings::SUITE_TYPE::WINDOWS_VISTA)
	{
		GetMenu()->GetSubMenu(1)->RemoveMenu(2, MF_BYPOSITION);
	}
	if(CAntiSpySettings::GetSuiteType() < CAntiSpySettings::SUITE_TYPE::WINDOWS_VISTA)
	{
		GetMenu()->GetSubMenu(1)->RemoveMenu(3, MF_BYPOSITION);
	}

	checkForInitialProfile();

	UpdateProfiles();
	int id = ProfileDlg::GetStartProfile();
	CMenu* pSubMenu=GetMenu()->GetSubMenu(0);
	if(!id||(id>(ID_PROFILE_SYSTEM_DEFAULT + m_userDefinedProfiles))||(id<ID_PROFILE_NORMAL))
		pSubMenu->CheckMenuRadioItem(ID_PROFILE_NORMAL, ID_PROFILE_SYSTEM_DEFAULT, ID_PROFILE_NORMAL, MF_BYCOMMAND);
	else
	{
		pSubMenu->CheckMenuRadioItem(ID_PROFILE_NORMAL, ID_PROFILE_SYSTEM_DEFAULT + m_userDefinedProfiles, id, MF_BYCOMMAND);
		SendMessage(WM_COMMAND, (WPARAM)id);
	}

	//manage menu item for auto profile check on login
	pSubMenu=GetMenu()->GetSubMenu(0);
	MENUITEMINFO mi;
	mi.cbSize = sizeof (mi);
	mi.fMask = MIIM_STATE; 
	bool ret = pSubMenu->GetMenuItemInfo(ID_PROFILE_AUTOSTART, &mi);
	if(ProfileDlg::IsAutoCheckProfile())
		mi.fState = MFS_CHECKED;
	else
		mi.fState = MFS_UNCHECKED;
	if(id == ID_PROFILE_NORMAL)
	{
		mi.fState |= MFS_DISABLED;
	}
	SetMenuItemInfo(pSubMenu->m_hMenu, ID_PROFILE_AUTOSTART, false, &mi);

	
	SetApplyButtonState(false);
	setMode(getStartMode());
	restoreWindowSize();
	return TRUE;
}

void CXPAntiSpyDlg::checkForInitialProfile()
{
	CRegKey regKey;
	regKey.Open(HKEY_CURRENT_USER, ProfileDlg::REGISTRY_LOC, KEY_QUERY_VALUE | KEY_SET_VALUE);
	DWORD cnt = MAX_PATH;
	TCHAR tmp[MAX_PATH];
	regKey.QueryValue(tmp, _T("DefaultProfileCreated"), &cnt);
	CString retVal(tmp);
	if(retVal == _T("true"))
	{
		return;
	}
	CinitProfileDlg dlg;
	int res = dlg.DoModal();
	if(res == IDOK)
	{
		CTime time = CTime::GetCurrentTime();
		CString name;
		retVal.LoadString(IDS_STANDARD_PROFILE);
		name = retVal + time.Format(_T(" %d.%m.%Y"));

		SettingVec settings = GetSettings();
		CRegKey tmp;
		if(tmp.Open(HKEY_CURRENT_USER, ProfileDlg::REGISTRY_LOC, KEY_READ) != ERROR_SUCCESS)
		{
			tmp.Create(HKEY_CURRENT_USER, ProfileDlg::REGISTRY_LOC, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_WRITE);
		}
		tmp.Close();
		if(tmp.Open(HKEY_CURRENT_USER, ProfileDlg::PROFILES_LOC, KEY_READ) != ERROR_SUCCESS)
		{
			tmp.Create(HKEY_CURRENT_USER, ProfileDlg::PROFILES_LOC, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_WRITE);
		}
		tmp.Close();
		
		CString profileName = ProfileDlg::PROFILES_LOC + CString(_T("\\")) + name;
		CRegKey regKey2;
		if(regKey2.Create(HKEY_CURRENT_USER,profileName, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_WRITE)== ERROR_SUCCESS)
		{
			for(int i=0; i<settings.size(); ++i)
			{
				CString name;
				name.Format(_T("%d"), settings[i].settingID);
				regKey2.SetValue((settings[i].checked)?1:0, name);
			}
			regKey2.Close();
		}
		regKey.SetValue(_T("true"), _T("DefaultProfileCreated"));
	}
	else if(res == 10)
	{
		regKey.SetValue(_T("true"), _T("DefaultProfileCreated"));		
	}
	regKey.Close();

}

void CXPAntiSpyDlg::restoreWindowSize()
{
	HKEY hKey;
	DWORD dwReadVal;
	LPDWORD lpType=0;
	DWORD KeyDisposition;
	DWORD kSize = sizeof(DWORD);
	DWORD dwType = REG_DWORD;
	int winX = -1;
	int winY = -1;
	int winH = -1;
	int winW = -1;
	if(RegOpenKeyEx(HKEY_CURRENT_USER,ProfileDlg::REGISTRY_LOC,0,KEY_ALL_ACCESS,&hKey) == ERROR_SUCCESS)
	{
		int ret = RegQueryValueEx(hKey,_T("window_x"),NULL,&dwType,(LPBYTE)&dwReadVal,&kSize);
		if(RegQueryValueEx(hKey,_T("window_x"),NULL,&dwType,(LPBYTE)&dwReadVal,&kSize) == ERROR_SUCCESS)
			winX = dwReadVal;
		if(RegQueryValueEx(hKey,_T("window_y"),NULL,&dwType,(LPBYTE)&dwReadVal,&kSize) == ERROR_SUCCESS)
			winY = dwReadVal;
		if(RegQueryValueEx(hKey,_T("window_w"),NULL,&dwType,(LPBYTE)&dwReadVal,&kSize) == ERROR_SUCCESS)
			winW = dwReadVal;
		if(RegQueryValueEx(hKey,_T("window_h"),NULL,&dwType,(LPBYTE)&dwReadVal,&kSize) == ERROR_SUCCESS)
			winH = dwReadVal;
		// Get Width/Height
		long width = (LONG)::GetSystemMetrics( SM_CXFULLSCREEN );
		long height = (LONG)::GetSystemMetrics( SM_CYFULLSCREEN );
		if(winX > width || winY > height || winW > width || winH > height)
			return;
		if(winH > 0 && winW > 0 && winX > -1 && winY > -1)
		{
			MoveWindow(winX, winY, winW, winH);
		}
	}
	RegCloseKey(hKey);

}

void CXPAntiSpyDlg::storeWindowSize()
{
	CRect rect;
	GetWindowRect(&rect);
	HKEY hKey;
	DWORD dwReadVal;
	LPDWORD lpType=0;
	DWORD KeyDisposition;
	DWORD kSize = sizeof(DWORD);
	DWORD dwType = REG_DWORD;

	CRegKey regKey;
	if(regKey.Open(HKEY_CURRENT_USER, ProfileDlg::REGISTRY_LOC, KEY_WRITE) == ERROR_SUCCESS)
	{
		DWORD winX = rect.left;
		DWORD winY = rect.top;
		DWORD winW = rect.Width();
		DWORD winH = rect.Height();
		regKey.SetValue(winX, _T("window_x"));
		regKey.SetValue(winY, _T("window_y"));
		regKey.SetValue(winW, _T("window_w"));
		regKey.SetValue(winH, _T("window_h"));
	}
	regKey.Close();
}


void CXPAntiSpyDlg::OnPaint() 
{
	static bool isInit = true;

	if(isInit)
	{
		ChangeFocusTo(m_TreeCtrl.m_hWnd);
		isInit = false;
	}
	if (IsIconic())
	{
		CPaintDC dc(this); 

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CXPAntiSpyDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CXPAntiSpyDlg::OnBtnFwd() 
{
	SetCursor(LoadCursor(NULL, IDC_WAIT));
	for(int i=0;i<ITEMCOUNT;++i)
	{
		if(m_entryItem[i].enabled)
		{
			int profileSuggestion = GetProfileSuggestion((SETTINGS)i);
			if(profileSuggestion == PROFILE_SUGGESTS_NOTHING)
			{
				if(m_TreeCtrl.IsChecked(m_entryItem[i].treeItem) && (m_AntiSpySettings.TestSetting((SETTINGS)i) == INACTIVE))
				{
					m_AntiSpySettings.DisableSetting(i);
				}
				else if((!m_TreeCtrl.IsChecked(m_entryItem[i].treeItem) && (m_AntiSpySettings.TestSetting((SETTINGS)i) == ACTIVE)))
				{
					m_AntiSpySettings.EnableSetting(i);
				}
			}
			else if(profileSuggestion == PROFILE_SUGGESTS_OFF)
			{
				if(m_AntiSpySettings.TestSetting((SETTINGS)i) == ACTIVE)
				{
					m_AntiSpySettings.EnableSetting(i);
				}
			}
			else if(profileSuggestion == PROFILE_SUGGESTS_ON)
			{
				if(m_AntiSpySettings.TestSetting((SETTINGS)i) == INACTIVE)
				{
					m_AntiSpySettings.DisableSetting(i);
				}
				
			}
		}
	}

	CheckForSpyFunctions();
	SetApplyButtonState(false);
	SetCursor(LoadCursor(NULL, IDC_ARROW));
	setMode(m_TreeCtrl.getMode());
	m_TreeCtrl.Invalidate();
	::SetFocus(m_TreeCtrl.m_hWnd);
}

int CXPAntiSpyDlg::GetProfileSuggestion(SETTINGS item)
{
	unsigned int i = 0;
	while(m_profile_on[i] != ST_END_ENTRY)
	{
		if(m_profile_on[i] == item)
			return PROFILE_SUGGESTS_ON;
		++i;
	}

	i = 0;
	while(m_profile_off[i] != ST_END_ENTRY)
	{
		if(m_profile_off[i] == item)
			return PROFILE_SUGGESTS_OFF;
		++i;
	}
	return PROFILE_SUGGESTS_NOTHING;
}

void CXPAntiSpyDlg::CheckForSpyFunctions()
{
	ResetAlerts();
	for(int i=0; i<ITEMCOUNT; ++i)
	{
		if(m_entryItem[i].enabled && m_TreeCtrl.IsCheckBox(m_entryItem[i].treeItem))
		{
			SETTING_STATE state = m_AntiSpySettings.TestSetting((SETTINGS)i);
			switch(state)
			{
				case ACTIVE:
					m_TreeCtrl.SetChecked(m_entryItem[i].treeItem,1);
					break;
				case NON_EXISTENT:
					m_entryItem[i].enabled = false;
					m_TreeCtrl.DeleteItem(m_entryItem[i].treeItem);
					break;
				case INACTIVE:
					m_TreeCtrl.SetChecked(m_entryItem[i].treeItem,0);
					break;
				case ACTIVE_NO_PERMISSION:
					m_TreeCtrl.SetChecked(m_entryItem[i].treeItem,1);
					m_TreeCtrl.SetLocked(m_entryItem[i].treeItem);
					break;
				case INACTIVE_NO_PERMISSION:
					m_TreeCtrl.SetChecked(m_entryItem[i].treeItem,0);
					m_TreeCtrl.SetLocked(m_entryItem[i].treeItem);
					break;
			}
		}
	}
	SetProfile(true);
	SetProfile(false);
	return;

SetApplyButtonState(false);
}

void CXPAntiSpyDlg::checkState(TreeItem &item, SETTING_STATE state)
{

}


void CXPAntiSpyDlg::InitTreeView()
{
    CString langStr;
	//Remove any items which may be in the control already
    m_TreeCtrl.DeleteAllItems();

	/////////////////////////////////////////////////////////////////////////////
	//Data safety
	/////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	langStr.LoadString(IDS_GROUP_DATASAFETY);
	m_hGroupItem[GI_DATASAFETY].hItem = m_TreeCtrl.InsertGroup(langStr, DISABLED, NULL);
	m_hGroupItem[GI_DATASAFETY].text = langStr;
	if (m_AntiSpySettings.TestSetting(ST_TELEMETRY) != NON_EXISTENT)
	{
		langStr.LoadString(IDS_TELEMETRY);
		m_entryItem[ST_TELEMETRY].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_DATASAFETY].hItem, 1);
		m_entryItem[ST_TELEMETRY].text = langStr;
		m_entryItem[ST_TELEMETRY].enabled = true;
		m_entryItem[ST_TELEMETRY].pGroupItem = &m_hGroupItem[GI_DATASAFETY];
	}
	else
	{
		m_entryItem[ST_TELEMETRY].enabled = false;
	}
	///////////////////////////////////////////////////////////////////////////
	/*if (m_AntiSpySettings.TestSetting(ST_APP_TELEMETRY) != NON_EXISTENT)
	{
		langStr.LoadString(IDS_APP_TELEMETRY);
		m_entryItem[ST_APP_TELEMETRY].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_DATASAFETY].hItem, 1);
		m_entryItem[ST_APP_TELEMETRY].text = langStr;
		m_entryItem[ST_APP_TELEMETRY].enabled = true;
		m_entryItem[ST_APP_TELEMETRY].pGroupItem = &m_hGroupItem[GI_DATASAFETY];
	}
	else
	{
		m_entryItem[ST_APP_TELEMETRY].enabled = false;
	}*/
	///////////////////////////////////////////////////////////////////////////
	if (m_AntiSpySettings.TestSetting(ST_AD_ID) != NON_EXISTENT)
	{
		langStr.LoadString(IDS_AD_ID);
		m_entryItem[ST_AD_ID].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_DATASAFETY].hItem, 1);
		m_entryItem[ST_AD_ID].text = langStr;
		m_entryItem[ST_AD_ID].enabled = true;
		m_entryItem[ST_AD_ID].pGroupItem = &m_hGroupItem[GI_DATASAFETY];
	}
	else
	{
		m_entryItem[ST_AD_ID].enabled = false;
	}
	///////////////////////////////////////////////////////////////////////////
	if (m_AntiSpySettings.TestSetting(ST_WRITING_BEHAVIOR) != NON_EXISTENT)
	{
		langStr.LoadString(IDS_WRITING_BEHAVIOR);
		m_entryItem[ST_WRITING_BEHAVIOR].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_DATASAFETY].hItem, 1);
		m_entryItem[ST_WRITING_BEHAVIOR].text = langStr;
		m_entryItem[ST_WRITING_BEHAVIOR].enabled = true;
		m_entryItem[ST_WRITING_BEHAVIOR].pGroupItem = &m_hGroupItem[GI_DATASAFETY];
	}
	else
	{
		m_entryItem[ST_WRITING_BEHAVIOR].enabled = false;
	}
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	if (m_AntiSpySettings.TestSetting(ST_FEEDBACK) != NON_EXISTENT)
	{
		langStr.LoadString(IDS_FEEDBACK);
		m_entryItem[ST_FEEDBACK].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_DATASAFETY].hItem, 1);
		m_entryItem[ST_FEEDBACK].text = langStr;
		m_entryItem[ST_FEEDBACK].enabled = true;
		m_entryItem[ST_FEEDBACK].pGroupItem = &m_hGroupItem[GI_DATASAFETY];
	}
	else
	{
		m_entryItem[ST_FEEDBACK].enabled = false;
	}
	///////////////////////////////////////////////////////////////////////////
	if (m_AntiSpySettings.TestSetting(ST_BIOMETRY) != NON_EXISTENT)
	{
		langStr.LoadString(IDS_BIOMETRY);
		m_entryItem[ST_BIOMETRY].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_DATASAFETY].hItem, 1);
		m_entryItem[ST_BIOMETRY].text = langStr;
		m_entryItem[ST_BIOMETRY].enabled = true;
		m_entryItem[ST_BIOMETRY].pGroupItem = &m_hGroupItem[GI_DATASAFETY];
	}
	else
	{
		m_entryItem[ST_BIOMETRY].enabled = false;
	}
	///////////////////////////////////////////////////////////////////////////
	if (m_AntiSpySettings.TestSetting(ST_WEBSEARCH) != NON_EXISTENT)
	{
		langStr.LoadString(IDS_WEBSEARCH);
		m_entryItem[ST_WEBSEARCH].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_DATASAFETY].hItem, 1);
		m_entryItem[ST_WEBSEARCH].text = langStr;
		m_entryItem[ST_WEBSEARCH].enabled = true;
		m_entryItem[ST_WEBSEARCH].pGroupItem = &m_hGroupItem[GI_DATASAFETY];
	}
	else
	{
		m_entryItem[ST_WEBSEARCH].enabled = false;
	}
	///////////////////////////////////////////////////////////////////////////
	if (m_AntiSpySettings.TestSetting(ST_LOCATION_PROVIDER) != NON_EXISTENT)
	{
		langStr.LoadString(IDS_LOCATION_PROVIDER);
		m_entryItem[ST_LOCATION_PROVIDER].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_DATASAFETY].hItem, 1);
		m_entryItem[ST_LOCATION_PROVIDER].text = langStr;
		m_entryItem[ST_LOCATION_PROVIDER].enabled = true;
		m_entryItem[ST_LOCATION_PROVIDER].pGroupItem = &m_hGroupItem[GI_DATASAFETY];
	}
	else
	{
		m_entryItem[ST_LOCATION_PROVIDER].enabled = false;
	}
	///////////////////////////////////////////////////////////////////////////
	if (m_AntiSpySettings.TestSetting(ST_SENSORS) != NON_EXISTENT)
	{
		langStr.LoadString(IDS_SENSORS);
		m_entryItem[ST_SENSORS].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_DATASAFETY].hItem, 1);
		m_entryItem[ST_SENSORS].text = langStr;
		m_entryItem[ST_SENSORS].enabled = true;
		m_entryItem[ST_SENSORS].pGroupItem = &m_hGroupItem[GI_DATASAFETY];
	}
	else
	{
		m_entryItem[ST_SENSORS].enabled = false;
	}
	///////////////////////////////////////////////////////////////////////////
	if (m_AntiSpySettings.TestSetting(ST_WIFI_SENSE) != NON_EXISTENT)
	{
		langStr.LoadString(IDS_WIFI_SENSE);
		m_entryItem[ST_WIFI_SENSE].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_DATASAFETY].hItem, 1);
		m_entryItem[ST_WIFI_SENSE].text = langStr;
		m_entryItem[ST_WIFI_SENSE].enabled = true;
		m_entryItem[ST_WIFI_SENSE].pGroupItem = &m_hGroupItem[GI_DATASAFETY];
	}
	else
	{
		m_entryItem[ST_WIFI_SENSE].enabled = false;
	}
	///////////////////////////////////////////////////////////////////////////
	if (m_AntiSpySettings.TestSetting(ST_SITE_PREDICTION) != NON_EXISTENT)
	{
		langStr.LoadString(IDS_SITE_PREDICTION);
		m_entryItem[ST_SITE_PREDICTION].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_DATASAFETY].hItem, 1);
		m_entryItem[ST_SITE_PREDICTION].text = langStr;
		m_entryItem[ST_SITE_PREDICTION].enabled = true;
		m_entryItem[ST_SITE_PREDICTION].pGroupItem = &m_hGroupItem[GI_DATASAFETY];
	}
	else
	{
		m_entryItem[ST_SITE_PREDICTION].enabled = false;
	}
	///////////////////////////////////////////////////////////////////////////
	if (m_AntiSpySettings.TestSetting(ST_UPDATE_SHARING) != NON_EXISTENT)
	{
		langStr.LoadString(IDS_UPDATE_SHARING);
		m_entryItem[ST_UPDATE_SHARING].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_DATASAFETY].hItem, 1);
		m_entryItem[ST_UPDATE_SHARING].text = langStr;
		m_entryItem[ST_UPDATE_SHARING].enabled = true;
		m_entryItem[ST_UPDATE_SHARING].pGroupItem = &m_hGroupItem[GI_DATASAFETY];
	}
	else
	{
		m_entryItem[ST_UPDATE_SHARING].enabled = false;
	}
	///////////////////////////////////////////////////////////////////////////
	if (m_AntiSpySettings.TestSetting(ST_INVENTORY) != NON_EXISTENT)
	{
		langStr.LoadString(IDS_INVENTORY);
		m_entryItem[ST_INVENTORY].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_DATASAFETY].hItem, 1);
		m_entryItem[ST_INVENTORY].text = langStr;
		m_entryItem[ST_INVENTORY].enabled = true;
		m_entryItem[ST_INVENTORY].pGroupItem = &m_hGroupItem[GI_DATASAFETY];
	}
	else
	{
		m_entryItem[ST_INVENTORY].enabled = false;
	}
	///////////////////////////////////////////////////////////////////////////
	if (m_AntiSpySettings.TestSetting(ST_PERSONALIZE) != NON_EXISTENT)
	{
		langStr.LoadString(IDS_PERSONALIZE);
		m_entryItem[ST_PERSONALIZE].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_DATASAFETY].hItem, 1);
		m_entryItem[ST_PERSONALIZE].text = langStr;
		m_entryItem[ST_PERSONALIZE].enabled = true;
		m_entryItem[ST_PERSONALIZE].pGroupItem = &m_hGroupItem[GI_DATASAFETY];
	}
	else
	{
		m_entryItem[ST_PERSONALIZE].enabled = false;
	}
	///////////////////////////////////////////////////////////////////////////
	if(m_TreeCtrl.ItemHasChildren(m_hGroupItem[GI_DATASAFETY].hItem))
	{
		m_TreeCtrl.Expand(m_hGroupItem[GI_DATASAFETY].hItem, TVE_EXPAND);	
	}
	else
	{
		m_TreeCtrl.RemoveGroup(m_hGroupItem[GI_DATASAFETY].hItem);
	}


/////////////////////////////////////////////////////////////////////////////
//Mediaplayer entries
/////////////////////////////////////////////////////////////////////////////
    bool hasMediaPlayer = m_AntiSpySettings.IsMediaPlayer()||m_AntiSpySettings.IsMediaPlayer9()||m_AntiSpySettings.IsMediaPlayer11()||m_AntiSpySettings.IsMediaPlayer12();
	if(hasMediaPlayer)
	{
		langStr.LoadString(IDS_GROUP_MEDIAPLAYER);
		m_hGroupItem[GI_MEDIAPLAYER].hItem = m_TreeCtrl.InsertGroup(langStr, DISABLED, NULL);
		m_hGroupItem[GI_MEDIAPLAYER].text = langStr;
    }

	SETTING_STATE state;
	
	state = m_AntiSpySettings.TestSetting(ST_MEDIAPLAYER_LICENSES);
	if(hasMediaPlayer && (state != NON_EXISTENT))
	{
		if(state == ACTIVE_NO_PERMISSION || state == INACTIVE_NO_PERMISSION)
		{
			m_entryItem[ST_MEDIAPLAYER_LICENSES].restricted = true;
		}
		langStr.LoadString(IDS_MEDIAPLAYER_LICENSES);
		m_entryItem[ST_MEDIAPLAYER_LICENSES].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_MEDIAPLAYER].hItem, 1);
		m_entryItem[ST_MEDIAPLAYER_LICENSES].text = langStr;//langStr.operator LPCTSTR;
		m_entryItem[ST_MEDIAPLAYER_LICENSES].enabled=	true;
		m_entryItem[ST_MEDIAPLAYER_LICENSES].pGroupItem =	&m_hGroupItem[GI_MEDIAPLAYER];
	} else
	{
		m_entryItem[ST_MEDIAPLAYER_LICENSES].enabled=	false;		
	}
	
	state = m_AntiSpySettings.TestSetting(ST_MEDIAPLAYER_UPDATE);
	if(hasMediaPlayer && (state != NON_EXISTENT))
	{
		langStr.LoadString(IDS_MEDIAPLAYER_UPDATE);
		if(state == ACTIVE_NO_PERMISSION || state == INACTIVE_NO_PERMISSION)
		{
			m_entryItem[ST_MEDIAPLAYER_UPDATE].restricted = true;
		}
		m_entryItem[ST_MEDIAPLAYER_UPDATE].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_MEDIAPLAYER].hItem, 1);
		m_entryItem[ST_MEDIAPLAYER_UPDATE].text = langStr;
		m_entryItem[ST_MEDIAPLAYER_UPDATE].enabled=	true;
		m_entryItem[ST_MEDIAPLAYER_UPDATE].pGroupItem =	&m_hGroupItem[GI_MEDIAPLAYER];
	} else
	{
		m_entryItem[ST_MEDIAPLAYER_UPDATE].enabled=	false;		
	}

	if(hasMediaPlayer && m_AntiSpySettings.TestSetting(ST_MEDIAPLAYER_SEND_GUID)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_MEDIAPLAYER_SEND_GUID);
		m_entryItem[ST_MEDIAPLAYER_SEND_GUID].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_MEDIAPLAYER].hItem, 1);
		m_entryItem[ST_MEDIAPLAYER_SEND_GUID].text = langStr;
		m_entryItem[ST_MEDIAPLAYER_SEND_GUID].enabled=	true;
		m_entryItem[ST_MEDIAPLAYER_SEND_GUID].pGroupItem =	&m_hGroupItem[GI_MEDIAPLAYER];
	} else
	{
		m_entryItem[ST_MEDIAPLAYER_SEND_GUID].enabled=	false;		
	}

	if(hasMediaPlayer && m_AntiSpySettings.TestSetting(ST_MEDIAPLAYER_CODECS)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_MEDIAPLAYER_CODECS);
		m_entryItem[ST_MEDIAPLAYER_CODECS].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_MEDIAPLAYER].hItem, 1);
		m_entryItem[ST_MEDIAPLAYER_CODECS].text = langStr;
		m_entryItem[ST_MEDIAPLAYER_CODECS].enabled=	true;
		m_entryItem[ST_MEDIAPLAYER_CODECS].pGroupItem =	&m_hGroupItem[GI_MEDIAPLAYER];
	} else
	{
		m_entryItem[ST_MEDIAPLAYER_CODECS].enabled=	false;		
	}

	if(hasMediaPlayer && m_AntiSpySettings.TestSetting(ST_MP9_ADDMEDIATOLIB)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_MP9_ADDMEDIATOLIB);
		m_entryItem[ST_MP9_ADDMEDIATOLIB].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_MEDIAPLAYER].hItem, 1);
		m_entryItem[ST_MP9_ADDMEDIATOLIB].text = langStr;
		m_entryItem[ST_MP9_ADDMEDIATOLIB].enabled=	true;
		m_entryItem[ST_MP9_ADDMEDIATOLIB].pGroupItem =	&m_hGroupItem[GI_MEDIAPLAYER];
	} else
	{
		m_entryItem[ST_MP9_ADDMEDIATOLIB].enabled=	false;		
	}

	if(hasMediaPlayer && m_AntiSpySettings.TestSetting(ST_MP9_RETRIEVEMETADATA)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_MP9_RETRIEVEMETADATA);
		m_entryItem[ST_MP9_RETRIEVEMETADATA].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_MEDIAPLAYER].hItem, 1);
		m_entryItem[ST_MP9_RETRIEVEMETADATA].text = langStr;
		m_entryItem[ST_MP9_RETRIEVEMETADATA].enabled=	true;
		m_entryItem[ST_MP9_RETRIEVEMETADATA].pGroupItem =	&m_hGroupItem[GI_MEDIAPLAYER];
	} else
	{
		m_entryItem[ST_MP9_RETRIEVEMETADATA].enabled=	false;		
	}

	if(hasMediaPlayer && m_AntiSpySettings.TestSetting(ST_MP9_USAGETRACKING)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_MP9_USAGETRACKING);
		m_entryItem[ST_MP9_USAGETRACKING].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_MEDIAPLAYER].hItem, 1);
		m_entryItem[ST_MP9_USAGETRACKING].text = langStr;
		m_entryItem[ST_MP9_USAGETRACKING].enabled=	true;
		m_entryItem[ST_MP9_USAGETRACKING].pGroupItem =	&m_hGroupItem[GI_MEDIAPLAYER];
	} else
	{
		m_entryItem[ST_MP9_USAGETRACKING].enabled=	false;		
	}

	if(hasMediaPlayer && m_AntiSpySettings.TestSetting(ST_MP9_SAVEMRU)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_MP9_SAVEMRU);
		m_entryItem[ST_MP9_SAVEMRU].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_MEDIAPLAYER].hItem, 1);
		m_entryItem[ST_MP9_SAVEMRU].text = langStr;
		m_entryItem[ST_MP9_SAVEMRU].enabled=	true;
		m_entryItem[ST_MP9_SAVEMRU].pGroupItem =	&m_hGroupItem[GI_MEDIAPLAYER];
	} else
	{
		m_entryItem[ST_MP9_SAVEMRU].enabled=	false;		
	}

	if(hasMediaPlayer && m_AntiSpySettings.TestSetting(ST_MP_START_IN_MEDIA_GUIDE)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_MP_START_IN_MEDIA_GUIDE);
		m_entryItem[ST_MP_START_IN_MEDIA_GUIDE].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_MEDIAPLAYER].hItem, 1);
		m_entryItem[ST_MP_START_IN_MEDIA_GUIDE].text = langStr;
		m_entryItem[ST_MP_START_IN_MEDIA_GUIDE].enabled=	true;
		m_entryItem[ST_MP_START_IN_MEDIA_GUIDE].pGroupItem =	&m_hGroupItem[GI_MEDIAPLAYER];
	} else
	{
		m_entryItem[ST_MP_START_IN_MEDIA_GUIDE].enabled=	false;		
	}
	state = m_AntiSpySettings.TestSetting(ST_MEDIAPLAYER_BLOCKUNINSTALL);
	if(hasMediaPlayer && (!m_AntiSpySettings.IsMediaPlayer11()) && (state != NON_EXISTENT))
	{
		if(state == ACTIVE_NO_PERMISSION || state == INACTIVE_NO_PERMISSION)
		{
			m_entryItem[ST_MEDIAPLAYER_BLOCKUNINSTALL].restricted = true;
		}
		langStr.LoadString(IDS_MEDIAPLAYER_BLOCKUNINSTALL);
		m_entryItem[ST_MEDIAPLAYER_BLOCKUNINSTALL].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_MEDIAPLAYER].hItem, 1);
		m_entryItem[ST_MEDIAPLAYER_BLOCKUNINSTALL].text = langStr;
		m_entryItem[ST_MEDIAPLAYER_BLOCKUNINSTALL].enabled=	true;
		m_entryItem[ST_MEDIAPLAYER_BLOCKUNINSTALL].pGroupItem =	&m_hGroupItem[GI_MEDIAPLAYER];
	} else
	{
		m_entryItem[ST_MEDIAPLAYER_BLOCKUNINSTALL].enabled=	false;		
	}

	state = m_AntiSpySettings.TestSetting(ST_WMP_PREVENT_LIBRARY_SHARING);
	if(m_AntiSpySettings.GetMediaPlayerVersion() >= 11 && (state != NON_EXISTENT))
	{
		if(state == ACTIVE_NO_PERMISSION || state == INACTIVE_NO_PERMISSION)
		{
			m_entryItem[ST_WMP_PREVENT_LIBRARY_SHARING].restricted = true;
		}
		langStr.LoadString(IDS_WMP_PREVENT_LIBRARY_SHARING);
		m_entryItem[ST_WMP_PREVENT_LIBRARY_SHARING].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_MEDIAPLAYER].hItem, 1);
		m_entryItem[ST_WMP_PREVENT_LIBRARY_SHARING].text = langStr;
		m_entryItem[ST_WMP_PREVENT_LIBRARY_SHARING].enabled=	true;
		m_entryItem[ST_WMP_PREVENT_LIBRARY_SHARING].pGroupItem =	&m_hGroupItem[GI_MEDIAPLAYER];
	} else
	{
		m_entryItem[ST_WMP_PREVENT_LIBRARY_SHARING].enabled=	false;		
	}

    if(hasMediaPlayer)
	{
		m_TreeCtrl.Expand(m_hGroupItem[GI_MEDIAPLAYER].hItem, TVE_EXPAND);
    }
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//Error reports Entries
/////////////////////////////////////////////////////////////////////////////
    bool hasErrorReports = false;
	//if(m_AntiSpySettings.IsWinXP())
	{
		langStr.LoadString(IDS_GROUP_ERROR_REPORTS);
		m_hGroupItem[GI_ERROR_REPORTS].hItem = m_TreeCtrl.InsertGroup(langStr, DISABLED, NULL);
		m_hGroupItem[GI_ERROR_REPORTS].text = langStr;
	}
	state = m_AntiSpySettings.TestSetting(ST_ERROR_REPORTS_SEND);
	if(m_AntiSpySettings.GetSuiteType() != CAntiSpySettings::SUITE_TYPE::WINDOWS_VISTA && (state != NON_EXISTENT))
	{
		langStr.LoadString(IDS_ERROR_REPORTS_SEND);
		m_entryItem[ST_ERROR_REPORTS_SEND].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_ERROR_REPORTS].hItem, 1);
		if(state == ACTIVE_NO_PERMISSION || state == INACTIVE_NO_PERMISSION)
		{
			m_entryItem[ST_ERROR_REPORTS_SEND].restricted = true;
		}
		m_entryItem[ST_ERROR_REPORTS_SEND].text = langStr;
		m_entryItem[ST_ERROR_REPORTS_SEND].enabled=		true;
		m_entryItem[ST_ERROR_REPORTS_SEND].pGroupItem =	&m_hGroupItem[GI_ERROR_REPORTS];
		hasErrorReports = true;
	}
	else
	{
		m_entryItem[ST_ERROR_REPORTS_SEND].enabled=		false;
	}
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	//OfficeXP errorreports
	/////////////////////////////////////////////////////////////////////////////
    if(m_AntiSpySettings.TestSetting(ST_OXP_ERRORREPORT)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_OXP_ERRORREPORT);
		m_entryItem[ST_OXP_ERRORREPORT].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_ERROR_REPORTS].hItem, 1);
		m_entryItem[ST_OXP_ERRORREPORT].text = langStr;
		m_entryItem[ST_OXP_ERRORREPORT].enabled=	true;
		m_entryItem[ST_OXP_ERRORREPORT].pGroupItem =	&m_hGroupItem[GI_ERROR_REPORTS];
		hasErrorReports = true;
	} else
	{
		m_entryItem[ST_OXP_ERRORREPORT].enabled=	false;		
	}
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	//Office2003 errorreports
	/////////////////////////////////////////////////////////////////////////////
    if(m_AntiSpySettings.TestSetting(ST_O2k3_ERRORREPORT)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_O2K3_ERRORREPORT);
		m_entryItem[ST_O2k3_ERRORREPORT].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_ERROR_REPORTS].hItem, 1);
		m_entryItem[ST_O2k3_ERRORREPORT].text = langStr;
		m_entryItem[ST_O2k3_ERRORREPORT].enabled=	true;
		m_entryItem[ST_O2k3_ERRORREPORT].pGroupItem =	&m_hGroupItem[GI_ERROR_REPORTS];
		hasErrorReports = true;
	} else
	{
		m_entryItem[ST_O2k3_ERRORREPORT].enabled=	false;		
	}

	if(!hasErrorReports)
	{
		m_TreeCtrl.RemoveGroup(m_hGroupItem[GI_ERROR_REPORTS].hItem);
		m_hGroupItem[GI_ERROR_REPORTS].hItem = NULL;
	}
	else
	{
		m_TreeCtrl.Expand(m_hGroupItem[GI_ERROR_REPORTS].hItem, TVE_EXPAND);
	}
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//Diverse
/////////////////////////////////////////////////////////////////////////////
    langStr.LoadString(IDS_GROUP_MISCELLANOUS);
	m_hGroupItem[GI_MISCELLANOUS].hItem = m_TreeCtrl.InsertGroup(langStr, DISABLED, NULL);
	m_hGroupItem[GI_MISCELLANOUS].text = langStr;
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	//remotedesktop
	/////////////////////////////////////////////////////////////////////////////
	if(m_AntiSpySettings.TestSetting(ST_REMOTEDESKTOP)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_REMOTEDESKTOP);
		m_entryItem[ST_REMOTEDESKTOP].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_MISCELLANOUS].hItem, 1);
		m_entryItem[ST_REMOTEDESKTOP].text = langStr;
		m_entryItem[ST_REMOTEDESKTOP].enabled=	true;
		m_entryItem[ST_REMOTEDESKTOP].pGroupItem =	&m_hGroupItem[GI_MISCELLANOUS];
	} else
	{
		m_entryItem[ST_REMOTEDESKTOP].enabled=	false;		
	}
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	//timesync
	/////////////////////////////////////////////////////////////////////////////
	state = m_AntiSpySettings.TestSetting(ST_TIME_SYNC);
	if(state != NON_EXISTENT)
	{
		if(state == ACTIVE_NO_PERMISSION || state == INACTIVE_NO_PERMISSION)
		{
			m_entryItem[ST_TIME_SYNC].restricted = true;
		}
		langStr.LoadString(IDS_TIME_SYNC);
		m_entryItem[ST_TIME_SYNC].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_MISCELLANOUS].hItem, 1);
		m_entryItem[ST_TIME_SYNC].text = langStr;
		m_entryItem[ST_TIME_SYNC].enabled=	true;
		m_entryItem[ST_TIME_SYNC].pGroupItem =	&m_hGroupItem[GI_MISCELLANOUS];
	} else
	{
		m_entryItem[ST_TIME_SYNC].enabled=	false;		
	}
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	//regdone
	/////////////////////////////////////////////////////////////////////////////
    state = m_AntiSpySettings.TestSetting(ST_REGDONE);
	if(state != NON_EXISTENT)
	{
		if(state == ACTIVE_NO_PERMISSION || state == INACTIVE_NO_PERMISSION)
		{
			m_entryItem[ST_REGDONE].restricted = true;
		}
		langStr.LoadString(IDS_REGDONE);
		m_entryItem[ST_REGDONE].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_MISCELLANOUS].hItem, 1);
		m_entryItem[ST_REGDONE].text = langStr;
		m_entryItem[ST_REGDONE].enabled=	true;
		m_entryItem[ST_REGDONE].pGroupItem =	&m_hGroupItem[GI_MISCELLANOUS];
	} else
	{
		m_entryItem[ST_REGDONE].enabled=	false;		
	}
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	//supportuser only if not exists
	/////////////////////////////////////////////////////////////////////////////
    if(m_AntiSpySettings.TestSetting(ST_SUPPORTUSER)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_SUPPORTUSER);
		m_entryItem[ST_SUPPORTUSER].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_MISCELLANOUS].hItem, 1);
		m_entryItem[ST_SUPPORTUSER].text = langStr;
		m_entryItem[ST_SUPPORTUSER].enabled=	true;
		m_entryItem[ST_SUPPORTUSER].pGroupItem =	&m_hGroupItem[GI_MISCELLANOUS];
	
	} else
	{
		m_entryItem[ST_SUPPORTUSER].enabled=	false;		
	}
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	//Alexa
	/////////////////////////////////////////////////////////////////////////////
    if(m_AntiSpySettings.TestSetting(ST_ALEXA)==1)
	{
		langStr.LoadString(IDS_ALEXA);
 		m_entryItem[ST_ALEXA].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_MISCELLANOUS].hItem, 1);
		m_entryItem[ST_ALEXA].text = langStr;
		m_entryItem[ST_ALEXA].enabled=	true;
		m_entryItem[ST_ALEXA].pGroupItem =	&m_hGroupItem[GI_MISCELLANOUS];
	} else
	{
		m_entryItem[ST_ALEXA].enabled=	false;		
	}
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	//BalloonTips
	/////////////////////////////////////////////////////////////////////////////
    if(m_AntiSpySettings.TestSetting(ST_BALLOON_TIPS)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_BALLOON_TIPS);
		m_entryItem[ST_BALLOON_TIPS].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_MISCELLANOUS].hItem, 1);
		m_entryItem[ST_BALLOON_TIPS].text = langStr;
		m_entryItem[ST_BALLOON_TIPS].enabled=	true;
		m_entryItem[ST_BALLOON_TIPS].pGroupItem =	&m_hGroupItem[GI_MISCELLANOUS];
	} else
	{
		m_entryItem[ST_BALLOON_TIPS].enabled=	false;		
	}
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	//ClearPageFile
	/////////////////////////////////////////////////////////////////////////////
    state = m_AntiSpySettings.TestSetting(ST_CLEAR_PAGEFILE);
	if(state != NON_EXISTENT)
	{
		if(state == ACTIVE_NO_PERMISSION || state == INACTIVE_NO_PERMISSION)
		{
			m_entryItem[ST_CLEAR_PAGEFILE].restricted = true;
		}
		langStr.LoadString(IDS_CLEAR_PAGEFILE);
		m_entryItem[ST_CLEAR_PAGEFILE].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_MISCELLANOUS].hItem, 1);
		m_entryItem[ST_CLEAR_PAGEFILE].text = langStr;
		m_entryItem[ST_CLEAR_PAGEFILE].enabled=	true;
		m_entryItem[ST_CLEAR_PAGEFILE].pGroupItem =	&m_hGroupItem[GI_MISCELLANOUS];
	} else
	{
		m_entryItem[ST_CLEAR_PAGEFILE].enabled=	false;		
	}
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	//OE start Messenger
	/////////////////////////////////////////////////////////////////////////////
    state = m_AntiSpySettings.TestSetting(ST_OE_START_MESSENGER);
	if(state != NON_EXISTENT)
	{
		if(state == ACTIVE_NO_PERMISSION || state == INACTIVE_NO_PERMISSION)
		{
			m_entryItem[ST_OE_START_MESSENGER].restricted = true;
		}
		langStr.LoadString(IDS_OE_START_MESSENGER);
		m_entryItem[ST_OE_START_MESSENGER].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_MISCELLANOUS].hItem, 1);
		m_entryItem[ST_OE_START_MESSENGER].text = langStr;
		m_entryItem[ST_OE_START_MESSENGER].enabled=	true;
		m_entryItem[ST_OE_START_MESSENGER].pGroupItem =	&m_hGroupItem[GI_MISCELLANOUS];
	} else
	{
		m_entryItem[ST_OE_START_MESSENGER].enabled=	false;		
	}
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	//fast shutdown
	/////////////////////////////////////////////////////////////////////////////
    state = m_AntiSpySettings.TestSetting(ST_FAST_SHUTDOWN);
	if(state != NON_EXISTENT)
	{
		if(state == ACTIVE_NO_PERMISSION || state == INACTIVE_NO_PERMISSION)
		{
			m_entryItem[ST_FAST_SHUTDOWN].restricted = true;
		}
		langStr.LoadString(IDS_FAST_SHUTDOWN);
		m_entryItem[ST_FAST_SHUTDOWN].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_MISCELLANOUS].hItem, 1);
		m_entryItem[ST_FAST_SHUTDOWN].text = langStr;
		m_entryItem[ST_FAST_SHUTDOWN].enabled=	true;
		m_entryItem[ST_FAST_SHUTDOWN].pGroupItem =	&m_hGroupItem[GI_MISCELLANOUS];
	} else
	{
		m_entryItem[ST_FAST_SHUTDOWN].enabled=	false;		
	}
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	//Deny regedit
	/////////////////////////////////////////////////////////////////////////////
    if(m_AntiSpySettings.TestSetting(ST_DENY_REGEDIT)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_DENY_REGEDIT);
		m_entryItem[ST_DENY_REGEDIT].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_MISCELLANOUS].hItem, 1);
		m_entryItem[ST_DENY_REGEDIT].text = langStr;
		m_entryItem[ST_DENY_REGEDIT].enabled=	true;
		m_entryItem[ST_DENY_REGEDIT].pGroupItem =	&m_hGroupItem[GI_MISCELLANOUS];
	} else
	{
		m_entryItem[ST_DENY_REGEDIT].enabled=	false;		
	}
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	//ST_SCRIPTING_HOST
	/////////////////////////////////////////////////////////////////////////////
    state = m_AntiSpySettings.TestSetting(ST_SCRIPTING_HOST);
	if(state != NON_EXISTENT)
	{
		if(state == ACTIVE_NO_PERMISSION || state == INACTIVE_NO_PERMISSION)
		{
			m_entryItem[ST_SCRIPTING_HOST].restricted = true;
		}
		langStr.LoadString(IDS_SCRIPTING_HOST);
		m_entryItem[ST_SCRIPTING_HOST].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_MISCELLANOUS].hItem, 1);
		m_entryItem[ST_SCRIPTING_HOST].text = langStr;
		m_entryItem[ST_SCRIPTING_HOST].enabled=	true;
		m_entryItem[ST_SCRIPTING_HOST].pGroupItem =	&m_hGroupItem[GI_MISCELLANOUS];
	} else
	{
		m_entryItem[ST_SCRIPTING_HOST].enabled=	false;		
	}
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	//ST_SHOW_LAST_USERNAME
	/////////////////////////////////////////////////////////////////////////////
    state = m_AntiSpySettings.TestSetting(ST_SHOW_LAST_USERNAME);
	if(state != NON_EXISTENT)
	{
		if(state == ACTIVE_NO_PERMISSION || state == INACTIVE_NO_PERMISSION)
		{
			m_entryItem[ST_SHOW_LAST_USERNAME].restricted = true;
		}
		langStr.LoadString(IDS_SHOW_LAST_USERNAME);
		m_entryItem[ST_SHOW_LAST_USERNAME].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_MISCELLANOUS].hItem, 1);
		m_entryItem[ST_SHOW_LAST_USERNAME].text = langStr;
		m_entryItem[ST_SHOW_LAST_USERNAME].enabled=	true;
		m_entryItem[ST_SHOW_LAST_USERNAME].pGroupItem =	&m_hGroupItem[GI_MISCELLANOUS];
	} else
	{
		m_entryItem[ST_SHOW_LAST_USERNAME].enabled=	false;		
	}
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	//ST_LNK_SUFFIX
	/////////////////////////////////////////////////////////////////////////////
    state = m_AntiSpySettings.TestSetting(ST_LNK_SUFFIX);
	if(state != NON_EXISTENT)
	{
		if(state == ACTIVE_NO_PERMISSION || state == INACTIVE_NO_PERMISSION)
		{
			m_entryItem[ST_LNK_SUFFIX].restricted = true;
		}
		langStr.LoadString(IDS_LNK_SUFFIX);
		m_entryItem[ST_LNK_SUFFIX].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_MISCELLANOUS].hItem, 1);
		m_entryItem[ST_LNK_SUFFIX].text = langStr;
		m_entryItem[ST_LNK_SUFFIX].enabled=	true;
		m_entryItem[ST_LNK_SUFFIX].pGroupItem =	&m_hGroupItem[GI_MISCELLANOUS];
	} else
	{
		m_entryItem[ST_LNK_SUFFIX].enabled=	false;		
	}
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	//ST_PIF_SUFFIX
	/////////////////////////////////////////////////////////////////////////////
    state = m_AntiSpySettings.TestSetting(ST_PIF_SUFFIX);
	if(state != NON_EXISTENT)
	{
		if(state == ACTIVE_NO_PERMISSION || state == INACTIVE_NO_PERMISSION)
		{
			m_entryItem[ST_PIF_SUFFIX].restricted = true;
		}
		langStr.LoadString(IDS_PIF_SUFFIX);
		m_entryItem[ST_PIF_SUFFIX].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_MISCELLANOUS].hItem, 1);
		m_entryItem[ST_PIF_SUFFIX].text = langStr;
		m_entryItem[ST_PIF_SUFFIX].enabled=	true;
		m_entryItem[ST_PIF_SUFFIX].pGroupItem =	&m_hGroupItem[GI_MISCELLANOUS];
	} else
	{
		m_entryItem[ST_PIF_SUFFIX].enabled=	false;		
	}
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	//ST_SHC_SUFFIX
	/////////////////////////////////////////////////////////////////////////////
    state = m_AntiSpySettings.TestSetting(ST_SHC_SUFFIX);
	if(state != NON_EXISTENT)
	{
		if(state == ACTIVE_NO_PERMISSION || state == INACTIVE_NO_PERMISSION)
		{
			m_entryItem[ST_SHC_SUFFIX].restricted = true;
		}
		langStr.LoadString(IDS_SHC_SUFFIX);
		m_entryItem[ST_SHC_SUFFIX].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_MISCELLANOUS].hItem, 1);
		m_entryItem[ST_SHC_SUFFIX].text = langStr;
		m_entryItem[ST_SHC_SUFFIX].enabled=	true;
		m_entryItem[ST_SHC_SUFFIX].pGroupItem =	&m_hGroupItem[GI_MISCELLANOUS];
	} else
	{
		m_entryItem[ST_SHC_SUFFIX].enabled=	false;		
	}
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	//ST_URL_SUFFIX
	/////////////////////////////////////////////////////////////////////////////
    state = m_AntiSpySettings.TestSetting(ST_URL_SUFFIX);
	if(state != NON_EXISTENT)
	{
		if(state == ACTIVE_NO_PERMISSION || state == INACTIVE_NO_PERMISSION)
		{
			m_entryItem[ST_URL_SUFFIX].restricted = true;
		}
		langStr.LoadString(IDS_URL_SUFFIX);
		m_entryItem[ST_URL_SUFFIX].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_MISCELLANOUS].hItem, 1);
		m_entryItem[ST_URL_SUFFIX].text = langStr;
		m_entryItem[ST_URL_SUFFIX].enabled=	true;
		m_entryItem[ST_URL_SUFFIX].pGroupItem =	&m_hGroupItem[GI_MISCELLANOUS];
	} else
	{
		m_entryItem[ST_URL_SUFFIX].enabled=	false;		
	}
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	//ST_CD_AUTORUN
	/////////////////////////////////////////////////////////////////////////////
    state = m_AntiSpySettings.TestSetting(ST_CD_AUTORUN);
	if(state != NON_EXISTENT)
	{
		if(state == ACTIVE_NO_PERMISSION || state == INACTIVE_NO_PERMISSION)
		{
			m_entryItem[ST_CD_AUTORUN].restricted = true;
		}
		langStr.LoadString(IDS_CD_AUTORUN);
		m_entryItem[ST_CD_AUTORUN].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_MISCELLANOUS].hItem, 1);
		m_entryItem[ST_CD_AUTORUN].text = langStr;
		m_entryItem[ST_CD_AUTORUN].enabled=	true;
		m_entryItem[ST_CD_AUTORUN].pGroupItem =	&m_hGroupItem[GI_MISCELLANOUS];
	} else
	{
		m_entryItem[ST_CD_AUTORUN].enabled=	false;		
	}
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	//ST_NO_RECENT_DOCS
	/////////////////////////////////////////////////////////////////////////////
    if(m_AntiSpySettings.TestSetting(ST_NO_RECENT_DOCS)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_NO_RECENT_DOCS);
		m_entryItem[ST_NO_RECENT_DOCS].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_MISCELLANOUS].hItem, 1);
		m_entryItem[ST_NO_RECENT_DOCS].text = langStr;
		m_entryItem[ST_NO_RECENT_DOCS].enabled=	true;
		m_entryItem[ST_NO_RECENT_DOCS].pGroupItem =	&m_hGroupItem[GI_MISCELLANOUS];
	} else
	{
		m_entryItem[ST_NO_RECENT_DOCS].enabled=	false;		
	}
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	//ST_CLEAN_RECENT_DOCS
	/////////////////////////////////////////////////////////////////////////////
    if(m_AntiSpySettings.TestSetting(ST_CLEAN_RECENT_DOCS)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_CLEAN_RECENT_DOCS);
		m_entryItem[ST_CLEAN_RECENT_DOCS].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_MISCELLANOUS].hItem, 1);
		m_entryItem[ST_CLEAN_RECENT_DOCS].text = langStr;
		m_entryItem[ST_CLEAN_RECENT_DOCS].enabled=	true;
		m_entryItem[ST_CLEAN_RECENT_DOCS].pGroupItem =	&m_hGroupItem[GI_MISCELLANOUS];
	} else
	{
		m_entryItem[ST_CLEAN_RECENT_DOCS].enabled=	false;		
	}
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	//File extensions
	/////////////////////////////////////////////////////////////////////////////
    if(m_AntiSpySettings.TestSetting(ST_SHOW_FILE_EXT)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_SHOW_FILE_EXT);
		m_entryItem[ST_SHOW_FILE_EXT].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_MISCELLANOUS].hItem, 1);
		m_entryItem[ST_SHOW_FILE_EXT].text = langStr;
		m_entryItem[ST_SHOW_FILE_EXT].enabled=	true;
		m_entryItem[ST_SHOW_FILE_EXT].pGroupItem =	&m_hGroupItem[GI_MISCELLANOUS];
	} else
	{
		m_entryItem[ST_SHOW_FILE_EXT].enabled=	false;		
	}
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	//Dont repost infections via the removal tool
	/////////////////////////////////////////////////////////////////////////////
    state = m_AntiSpySettings.TestSetting(ST_DONT_REPORT_INFECTION);
	if(state != NON_EXISTENT)
	{
		if(state == ACTIVE_NO_PERMISSION || state == INACTIVE_NO_PERMISSION)
		{
			m_entryItem[ST_DONT_REPORT_INFECTION].restricted = true;
		}
		langStr.LoadString(IDS_DONT_REPORT_INFECTION);
		m_entryItem[ST_DONT_REPORT_INFECTION].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_MISCELLANOUS].hItem, 1);
		m_entryItem[ST_DONT_REPORT_INFECTION].text = langStr;
		m_entryItem[ST_DONT_REPORT_INFECTION].enabled=	true;
		m_entryItem[ST_DONT_REPORT_INFECTION].pGroupItem =	&m_hGroupItem[GI_MISCELLANOUS];
	} else
	{
		m_entryItem[ST_DONT_REPORT_INFECTION].enabled=	false;		
	}
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	//Dont use lm hashes
	/////////////////////////////////////////////////////////////////////////////
    if(m_AntiSpySettings.TestSetting(ST_NOLMHASH)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_NOLMHASH);
		m_entryItem[ST_NOLMHASH].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_MISCELLANOUS].hItem, 1);
		m_entryItem[ST_NOLMHASH].text = langStr;
		m_entryItem[ST_NOLMHASH].enabled=	true;
		m_entryItem[ST_NOLMHASH].pGroupItem =	&m_hGroupItem[GI_MISCELLANOUS];
	} else
	{
		m_entryItem[ST_NOLMHASH].enabled=	false;		
	}

	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	//Disable java script in adobe reader
	/////////////////////////////////////////////////////////////////////////////
    if(m_AntiSpySettings.TestSetting(ST_READER_DISABLE_JS)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_READER_DISABLE_JS);
		m_entryItem[ST_READER_DISABLE_JS].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_MISCELLANOUS].hItem, 1);
		m_entryItem[ST_READER_DISABLE_JS].text = langStr;
		m_entryItem[ST_READER_DISABLE_JS].enabled=	true;
		m_entryItem[ST_READER_DISABLE_JS].pGroupItem =	&m_hGroupItem[GI_MISCELLANOUS];
	} else
	{
		m_entryItem[ST_READER_DISABLE_JS].enabled=	false;		
	}

	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	//zip funcionality
	/////////////////////////////////////////////////////////////////////////////
	m_entryItem[ST_REGSVR_ZIPFLDR].enabled=	false;		
    /*if(m_AntiSpySettings.TestSetting(ST_REGSVR_ZIPFLDR)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_REGSVR_ZIPFLDR);
		m_entryItem[ST_REGSVR_ZIPFLDR].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_MISCELLANOUS].hItem,0);
		m_entryItem[ST_REGSVR_ZIPFLDR].text = langStr;
		m_entryItem[ST_REGSVR_ZIPFLDR].enabled=	true;
		m_entryItem[ST_REGSVR_ZIPFLDR].pGroupItem =	&m_hGroupItem[GI_MISCELLANOUS];
	} else
	{
		m_entryItem[ST_REGSVR_ZIPFLDR].enabled=	false;		
	}*/

	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	//No Virtual Machine for DOS
	/////////////////////////////////////////////////////////////////////////////
    if(m_AntiSpySettings.TestSetting(ST_NO_VDM)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_NO_VDM);
		m_entryItem[ST_NO_VDM].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_MISCELLANOUS].hItem, 1);
		m_entryItem[ST_NO_VDM].text = langStr;
		m_entryItem[ST_NO_VDM].enabled=	true;
		m_entryItem[ST_NO_VDM].pGroupItem =	&m_hGroupItem[GI_MISCELLANOUS];
	} else
	{
		m_entryItem[ST_NO_VDM].enabled=	false;		
	}


	//expand tree
	m_TreeCtrl.Expand(m_hGroupItem[GI_MISCELLANOUS].hItem, TVE_EXPAND);
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//Network
/////////////////////////////////////////////////////////////////////////////
	langStr.LoadString(IDS_GROUP_NETWORK);
	m_hGroupItem[GI_NETWORK].hItem = m_TreeCtrl.InsertGroup(langStr, DISABLED, NULL);
	m_hGroupItem[GI_NETWORK].text = langStr;
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	//Firewall
	/////////////////////////////////////////////////////////////////////////////
	state = m_AntiSpySettings.TestSetting(ST_FIREWALL);
    if(state != NON_EXISTENT)
	{
		if(state == ACTIVE_NO_PERMISSION || state == INACTIVE_NO_PERMISSION)
		{
			m_entryItem[ST_FIREWALL].restricted = true;
		}
		langStr.LoadString(IDS_FIREWALL);
		m_entryItem[ST_FIREWALL].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_NETWORK].hItem, 1);
		m_entryItem[ST_FIREWALL].text = langStr;
		m_entryItem[ST_FIREWALL].enabled=	true;
		m_entryItem[ST_FIREWALL].pGroupItem =	&m_hGroupItem[GI_NETWORK];
	} else
	{
		m_entryItem[ST_FIREWALL].enabled=	false;		
	}
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	//ST_HIDE_IN_NETWORK
	/////////////////////////////////////////////////////////////////////////////
    state = m_AntiSpySettings.TestSetting(ST_HIDE_IN_NETWORK);
	if(state != NON_EXISTENT)
	{
		if(state == ACTIVE_NO_PERMISSION || state == INACTIVE_NO_PERMISSION)
		{
			m_entryItem[ST_HIDE_IN_NETWORK].restricted = true;
		}
		langStr.LoadString(IDS_HIDE_IN_NETWORK);
		m_entryItem[ST_HIDE_IN_NETWORK].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_NETWORK].hItem, 1);
		m_entryItem[ST_HIDE_IN_NETWORK].text = langStr;
		m_entryItem[ST_HIDE_IN_NETWORK].enabled=	true;
		m_entryItem[ST_HIDE_IN_NETWORK].pGroupItem =	&m_hGroupItem[GI_NETWORK];
	} else
	{
		m_entryItem[ST_HIDE_IN_NETWORK].enabled=	false;		
	}
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	//No network crawling
	/////////////////////////////////////////////////////////////////////////////
    if(m_AntiSpySettings.TestSetting(ST_NONETCRAWL)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_NONETCRAWL);
		m_entryItem[ST_NONETCRAWL].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_NETWORK].hItem, 1);
		m_entryItem[ST_NONETCRAWL].text = langStr;
		m_entryItem[ST_NONETCRAWL].enabled=	true;
		m_entryItem[ST_NONETCRAWL].pGroupItem =	&m_hGroupItem[GI_NETWORK];
	} else
	{
		m_entryItem[ST_NONETCRAWL].enabled=	false;		
	}
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	//Restrict anonymous network access
	/////////////////////////////////////////////////////////////////////////////
    state = m_AntiSpySettings.TestSetting(ST_RESTRICTANONYMOUS);
	if(state != NON_EXISTENT)
	{
		if(state == ACTIVE_NO_PERMISSION || state == INACTIVE_NO_PERMISSION)
		{
			m_entryItem[ST_RESTRICTANONYMOUS].restricted = true;
		}
		langStr.LoadString(IDS_RESTRICTANONYMOUS);
		m_entryItem[ST_RESTRICTANONYMOUS].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_NETWORK].hItem, 1);
		m_entryItem[ST_RESTRICTANONYMOUS].text = langStr;
		m_entryItem[ST_RESTRICTANONYMOUS].enabled=	true;
		m_entryItem[ST_RESTRICTANONYMOUS].pGroupItem =	&m_hGroupItem[GI_NETWORK];
	} else
	{
		m_entryItem[ST_RESTRICTANONYMOUS].enabled=	false;		
	}
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	//Restrict anonymous network access
	/////////////////////////////////////////////////////////////////////////////
    state = m_AntiSpySettings.TestSetting(ST_NO_WGA_CHECK);
	if(state != NON_EXISTENT)
	{
		if(state == ACTIVE_NO_PERMISSION || state == INACTIVE_NO_PERMISSION)
		{
			m_entryItem[ST_NO_WGA_CHECK].restricted = true;
		}
		langStr.LoadString(IDS_NO_WGA_CHECK);
		m_entryItem[ST_NO_WGA_CHECK].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_NETWORK].hItem, 1);
		m_entryItem[ST_NO_WGA_CHECK].text = langStr;
		m_entryItem[ST_NO_WGA_CHECK].enabled=	true;
		m_entryItem[ST_NO_WGA_CHECK].pGroupItem =	&m_hGroupItem[GI_NETWORK];
	} else
	{
		m_entryItem[ST_NO_WGA_CHECK].enabled=	false;		
	}
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	//No recent networks in network hood
	/////////////////////////////////////////////////////////////////////////////
    state = m_AntiSpySettings.TestSetting(ST_NET_NO_RECENT_DOCS);
	if(state != NON_EXISTENT)
	{
		langStr.LoadString(IDS_NET_NO_RECENT_DOCS);
		m_entryItem[ST_NET_NO_RECENT_DOCS].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_NETWORK].hItem, 1);
		m_entryItem[ST_NET_NO_RECENT_DOCS].text = langStr;
		m_entryItem[ST_NET_NO_RECENT_DOCS].enabled=	true;
		m_entryItem[ST_NET_NO_RECENT_DOCS].pGroupItem =	&m_hGroupItem[GI_NETWORK];
	} else
	{
		m_entryItem[ST_NET_NO_RECENT_DOCS].enabled=	false;		
	}
	//expand tree
	m_TreeCtrl.Expand(m_hGroupItem[GI_NETWORK].hItem, TVE_EXPAND);
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//IE6
/////////////////////////////////////////////////////////////////////////////
	langStr.LoadString(IDS_GROUP_INTERNET_EXPLORER);
	m_hGroupItem[GI_INTERNET_EXPLORER].hItem = m_TreeCtrl.InsertGroup(langStr, DISABLED, NULL);
	m_hGroupItem[GI_INTERNET_EXPLORER].text = langStr;
	if(m_AntiSpySettings.TestSetting(ST_IE_CHECK_4_UPDATES)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_IE_CHECK_4_UPDATES);
		m_entryItem[ST_IE_CHECK_4_UPDATES].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_INTERNET_EXPLORER].hItem, 1);
		m_entryItem[ST_IE_CHECK_4_UPDATES].text = langStr;
		m_entryItem[ST_IE_CHECK_4_UPDATES].enabled=	true;
		m_entryItem[ST_IE_CHECK_4_UPDATES].pGroupItem =	&m_hGroupItem[GI_INTERNET_EXPLORER];
	} else
	{
		m_entryItem[ST_IE_CHECK_4_UPDATES].enabled=	false;		
	}
	if(m_AntiSpySettings.TestSetting(ST_IE_SCHEDULED_UPDATES)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_IE_SCHEDULED_UPDATES);
		m_entryItem[ST_IE_SCHEDULED_UPDATES].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_INTERNET_EXPLORER].hItem, 1);
		m_entryItem[ST_IE_SCHEDULED_UPDATES].text = langStr;
		m_entryItem[ST_IE_SCHEDULED_UPDATES].enabled=	true;
		m_entryItem[ST_IE_SCHEDULED_UPDATES].pGroupItem =	&m_hGroupItem[GI_INTERNET_EXPLORER];
	} else
	{
		m_entryItem[ST_IE_SCHEDULED_UPDATES].enabled=	false;		
	}
	if(m_AntiSpySettings.TestSetting(ST_IE_INTEGRATED_AUTHENTICATION)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_IE_INTEGRATED_AUTHENTICATION);
		m_entryItem[ST_IE_INTEGRATED_AUTHENTICATION].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_INTERNET_EXPLORER].hItem, 1);
		m_entryItem[ST_IE_INTEGRATED_AUTHENTICATION].text = langStr;
		m_entryItem[ST_IE_INTEGRATED_AUTHENTICATION].enabled=	true;
		m_entryItem[ST_IE_INTEGRATED_AUTHENTICATION].pGroupItem =	&m_hGroupItem[GI_INTERNET_EXPLORER];
	} else
	{
		m_entryItem[ST_IE_INTEGRATED_AUTHENTICATION].enabled=	false;		
	}
	if(m_AntiSpySettings.TestSetting(ST_MAX_CONNECTIONS)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_MAX_CONNECTIONS);
		m_entryItem[ST_MAX_CONNECTIONS].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_INTERNET_EXPLORER].hItem, 1);
		m_entryItem[ST_MAX_CONNECTIONS].text = langStr;
		m_entryItem[ST_MAX_CONNECTIONS].enabled=	true;
		m_entryItem[ST_MAX_CONNECTIONS].pGroupItem =	&m_hGroupItem[GI_INTERNET_EXPLORER];
	} else
	{
		m_entryItem[ST_MAX_CONNECTIONS].enabled=	false;		
	}

	if(m_AntiSpySettings.TestSetting(ST_IE_FORM_SUGGEST)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_IE_FORM_SUGGEST);
		m_entryItem[ST_IE_FORM_SUGGEST].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_INTERNET_EXPLORER].hItem, 1);
		m_entryItem[ST_IE_FORM_SUGGEST].text = langStr;
		m_entryItem[ST_IE_FORM_SUGGEST].enabled=true;
		m_entryItem[ST_IE_FORM_SUGGEST].pGroupItem =	&m_hGroupItem[GI_INTERNET_EXPLORER];
	} else
	{
		m_entryItem[ST_IE_FORM_SUGGEST].enabled=false;		
	}

	if(m_AntiSpySettings.TestSetting(ST_IE_UPLDDRVINFO)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_IE_UPLDDRVINFO);
		m_entryItem[ST_IE_UPLDDRVINFO].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_INTERNET_EXPLORER].hItem, 1);
		m_entryItem[ST_IE_UPLDDRVINFO].text = langStr;
		m_entryItem[ST_IE_UPLDDRVINFO].enabled=true;
		m_entryItem[ST_IE_UPLDDRVINFO].pGroupItem =	&m_hGroupItem[GI_INTERNET_EXPLORER];
	} else
	{
		m_entryItem[ST_IE_UPLDDRVINFO].enabled=false;		
	}

	if(m_AntiSpySettings.TestSetting(ST_IE_JAVASCRIPT)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_IE_JAVASCRIPT);
		m_entryItem[ST_IE_JAVASCRIPT].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_INTERNET_EXPLORER].hItem, 1);
		m_entryItem[ST_IE_JAVASCRIPT].text = langStr;
		m_entryItem[ST_IE_JAVASCRIPT].enabled=true;
		m_entryItem[ST_IE_JAVASCRIPT].pGroupItem =	&m_hGroupItem[GI_INTERNET_EXPLORER];
	} else
	{
		m_entryItem[ST_IE_JAVASCRIPT].enabled=false;		
	}

	if(m_AntiSpySettings.TestSetting(ST_IE_ACTIVEX)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_IE_ACTIVEX);
		m_entryItem[ST_IE_ACTIVEX].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_INTERNET_EXPLORER].hItem, 1);
		m_entryItem[ST_IE_ACTIVEX].text = langStr;
		m_entryItem[ST_IE_ACTIVEX].enabled=true;
		m_entryItem[ST_IE_ACTIVEX].pGroupItem =	&m_hGroupItem[GI_INTERNET_EXPLORER];
	} else
	{
		m_entryItem[ST_IE_ACTIVEX].enabled=false;		
	}

	if(m_AntiSpySettings.TestSetting(ST_IE_POPUP_BLOCKER)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_IE_POPUP_BLOCKER);
		m_entryItem[ST_IE_POPUP_BLOCKER].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_INTERNET_EXPLORER].hItem, 1);
		m_entryItem[ST_IE_POPUP_BLOCKER].text = langStr;
		m_entryItem[ST_IE_POPUP_BLOCKER].enabled=true;
		m_entryItem[ST_IE_POPUP_BLOCKER].pGroupItem =	&m_hGroupItem[GI_INTERNET_EXPLORER];
	} else
	{
		m_entryItem[ST_IE_POPUP_BLOCKER].enabled=false;		
	}

	if(m_AntiSpySettings.TestSetting(ST_IE_PHISHING_FILTER)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_IE_PHISHING_FILTER);
		m_entryItem[ST_IE_PHISHING_FILTER].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_INTERNET_EXPLORER].hItem, 1);
		m_entryItem[ST_IE_PHISHING_FILTER].text = langStr;
		m_entryItem[ST_IE_PHISHING_FILTER].enabled=true;
		m_entryItem[ST_IE_PHISHING_FILTER].pGroupItem =	&m_hGroupItem[GI_INTERNET_EXPLORER];
	} else
	{
		m_entryItem[ST_IE_PHISHING_FILTER].enabled=false;		
	}

	if(m_AntiSpySettings.TestSetting(ST_IE_WATSON)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_IE_WATSON);
		m_entryItem[ST_IE_WATSON].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_INTERNET_EXPLORER].hItem, 1);
		m_entryItem[ST_IE_WATSON].text = langStr;
		m_entryItem[ST_IE_WATSON].enabled=true;
		m_entryItem[ST_IE_WATSON].pGroupItem =	&m_hGroupItem[GI_INTERNET_EXPLORER];
	} else
	{
		m_entryItem[ST_IE_WATSON].enabled=false;		
	}

	if(m_AntiSpySettings.TestSetting(ST_IE_CLEAN_CACHE)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_IE_CLEAN_CACHE);
		m_entryItem[ST_IE_CLEAN_CACHE].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_INTERNET_EXPLORER].hItem, 1);
		m_entryItem[ST_IE_CLEAN_CACHE].text = langStr;
		m_entryItem[ST_IE_CLEAN_CACHE].enabled=true;
		m_entryItem[ST_IE_CLEAN_CACHE].pGroupItem =	&m_hGroupItem[GI_INTERNET_EXPLORER];
	} else
	{
		m_entryItem[ST_IE_CLEAN_CACHE].enabled=false;		
	}
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	//Do not allow ie70 to be installed via update
	/////////////////////////////////////////////////////////////////////////////
    state = m_AntiSpySettings.TestSetting(ST_DO_NOT_ALLOW_IE70);
	if(state != NON_EXISTENT)
	{
		if(state == ACTIVE_NO_PERMISSION || state == INACTIVE_NO_PERMISSION)
		{
			m_entryItem[ST_DO_NOT_ALLOW_IE70].restricted = true;
		}
		langStr.LoadString(IDS_DO_NOT_ALLOW_IE70);
		m_entryItem[ST_DO_NOT_ALLOW_IE70].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_INTERNET_EXPLORER].hItem, 1);
		m_entryItem[ST_DO_NOT_ALLOW_IE70].text = langStr;
		m_entryItem[ST_DO_NOT_ALLOW_IE70].enabled=	true;
		m_entryItem[ST_DO_NOT_ALLOW_IE70].pGroupItem =	&m_hGroupItem[GI_INTERNET_EXPLORER];
	} else
	{
		m_entryItem[ST_DO_NOT_ALLOW_IE70].enabled=	false;		
	}

	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	//Do not take part in the customer feedback program
	/////////////////////////////////////////////////////////////////////////////
	if(m_AntiSpySettings.TestSetting(ST_IE_NO_SQM)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_IE_NO_SQM);
		m_entryItem[ST_IE_NO_SQM].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_INTERNET_EXPLORER].hItem, 1);
		m_entryItem[ST_IE_NO_SQM].text = langStr;
		m_entryItem[ST_IE_NO_SQM].enabled=	true;
		m_entryItem[ST_IE_NO_SQM].pGroupItem = &m_hGroupItem[GI_INTERNET_EXPLORER];
	} else
	{
		m_entryItem[ST_IE_NO_SQM].enabled=	false;		
	}

	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	//Do not use the 'Suggested Sites' feature
	/////////////////////////////////////////////////////////////////////////////
	if(m_AntiSpySettings.TestSetting(ST_IE_SUGGESTED_SITES)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_IE_SUGGESTED_SITES);
		m_entryItem[ST_IE_SUGGESTED_SITES].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_INTERNET_EXPLORER].hItem, 1);
		m_entryItem[ST_IE_SUGGESTED_SITES].text = langStr;
		m_entryItem[ST_IE_SUGGESTED_SITES].enabled=	true;
		m_entryItem[ST_IE_SUGGESTED_SITES].pGroupItem = &m_hGroupItem[GI_INTERNET_EXPLORER];
	} else
	{
		m_entryItem[ST_IE_SUGGESTED_SITES].enabled=	false;		
	}

	m_TreeCtrl.Expand(m_hGroupItem[GI_INTERNET_EXPLORER].hItem, TVE_EXPAND);
		
/////////////////////////////////////////////////////////////////////////////
//Services
/////////////////////////////////////////////////////////////////////////////
	langStr.LoadString(IDS_GROUP_SERVICES);
	m_hGroupItem[GI_SERVICES].hItem = m_TreeCtrl.InsertGroup(langStr, DISABLED, NULL);
	m_hGroupItem[GI_SERVICES].text = langStr;
	///////////////////////////////////////////////////////////////////////////
	if(m_AntiSpySettings.TestSetting(ST_SERVICES_ERROR_REPORTS)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_SERVICES_ERROR_REPORTS);
		m_entryItem[ST_SERVICES_ERROR_REPORTS].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_SERVICES].hItem, 1);
		m_entryItem[ST_SERVICES_ERROR_REPORTS].text = langStr;
		m_entryItem[ST_SERVICES_ERROR_REPORTS].enabled=	true;
		m_entryItem[ST_SERVICES_ERROR_REPORTS].pGroupItem =	&m_hGroupItem[GI_SERVICES];
	} else
	{
		m_entryItem[ST_SERVICES_ERROR_REPORTS].enabled=	false;		
	}
	///////////////////////////////////////////////////////////////////////////
	if(m_AntiSpySettings.TestSetting(ST_SERVICES_AUTO_UPDATES)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_SERVICES_AUTO_UPDATES);
		m_entryItem[ST_SERVICES_AUTO_UPDATES].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_SERVICES].hItem, 1);
		m_entryItem[ST_SERVICES_AUTO_UPDATES].text = langStr;
		m_entryItem[ST_SERVICES_AUTO_UPDATES].enabled=	true;
		m_entryItem[ST_SERVICES_AUTO_UPDATES].pGroupItem =	&m_hGroupItem[GI_SERVICES];
	} else
	{
		m_entryItem[ST_SERVICES_AUTO_UPDATES].enabled=	false;		
	}
	///////////////////////////////////////////////////////////////////////////
	if(m_AntiSpySettings.TestSetting(ST_SERVICES_TIMESERVER)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_SERVICES_TIMESERVER);
		m_entryItem[ST_SERVICES_TIMESERVER].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_SERVICES].hItem, 1);
		m_entryItem[ST_SERVICES_TIMESERVER].text = langStr;
		m_entryItem[ST_SERVICES_TIMESERVER].enabled=	true;
		m_entryItem[ST_SERVICES_TIMESERVER].pGroupItem =	&m_hGroupItem[GI_SERVICES];
	} else
	{
		m_entryItem[ST_SERVICES_TIMESERVER].enabled=	false;		
	}
	///////////////////////////////////////////////////////////////////////////
	if(m_AntiSpySettings.TestSetting(ST_SERVICES_TASKSCHEDULER)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_SERVICES_TASKSCHEDULER);
		m_entryItem[ST_SERVICES_TASKSCHEDULER].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_SERVICES].hItem, 1);
		m_entryItem[ST_SERVICES_TASKSCHEDULER].text = langStr;
		m_entryItem[ST_SERVICES_TASKSCHEDULER].enabled=	true;
		m_entryItem[ST_SERVICES_TASKSCHEDULER].pGroupItem =	&m_hGroupItem[GI_SERVICES];
		//m_TreeCtrl.SetEnabled(m_entryItem[12].treeItem,0);
	} else
	{
		m_entryItem[ST_SERVICES_TASKSCHEDULER].enabled=	false;		
	}
	///////////////////////////////////////////////////////////////////////////
	if(m_AntiSpySettings.TestSetting(ST_SERVICES_UPNP)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_SERVICES_UPNP);
		m_entryItem[ST_SERVICES_UPNP].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_SERVICES].hItem, 1);
		m_entryItem[ST_SERVICES_UPNP].text = langStr;
		m_entryItem[ST_SERVICES_UPNP].enabled=	true;
		m_entryItem[ST_SERVICES_UPNP].pGroupItem =	&m_hGroupItem[GI_SERVICES];
	} else
	{
		m_entryItem[ST_SERVICES_UPNP].enabled=	false;		
	}
	///////////////////////////////////////////////////////////////////////////
	if(m_AntiSpySettings.TestSetting(ST_SERVICES_MSGSRV)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_SERVICES_MSGSRV);
		m_entryItem[ST_SERVICES_MSGSRV].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_SERVICES].hItem, 1);
		m_entryItem[ST_SERVICES_MSGSRV].text = langStr;
		m_entryItem[ST_SERVICES_MSGSRV].enabled=	true;
		m_entryItem[ST_SERVICES_MSGSRV].pGroupItem =	&m_hGroupItem[GI_SERVICES];
	} else
	{
		m_entryItem[ST_SERVICES_MSGSRV].enabled=	false;		
	}
	///////////////////////////////////////////////////////////////////////////
	if(m_AntiSpySettings.TestSetting(ST_SERVICES_FIREWALL)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_SERVICES_FIREWALL);
		m_entryItem[ST_SERVICES_FIREWALL].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_SERVICES].hItem, 1);
		m_entryItem[ST_SERVICES_FIREWALL].text = langStr;
		m_entryItem[ST_SERVICES_FIREWALL].enabled=	true;
		m_entryItem[ST_SERVICES_FIREWALL].pGroupItem =	&m_hGroupItem[GI_SERVICES];
	} else
	{
		m_entryItem[ST_SERVICES_FIREWALL].enabled=	false;		
	}
	///////////////////////////////////////////////////////////////////////////
	if(m_AntiSpySettings.TestSetting(ST_SERVICES_SEC_CTR)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_SERVICES_SEC_CTR);
		m_entryItem[ST_SERVICES_SEC_CTR].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_SERVICES].hItem, 1);
		m_entryItem[ST_SERVICES_SEC_CTR].text = langStr;
		m_entryItem[ST_SERVICES_SEC_CTR].enabled=	true;
		m_entryItem[ST_SERVICES_SEC_CTR].pGroupItem =	&m_hGroupItem[GI_SERVICES];
	} else
	{
		m_entryItem[ST_SERVICES_SEC_CTR].enabled=	false;		
	}
	///////////////////////////////////////////////////////////////////////////
// 	if(m_AntiSpySettings.TestSetting(ST_SERVICES_RPCLOCATOR)!= NON_EXISTENT)
// 	{
// 		langStr.LoadString(IDS_SERVICES_RPCLOCATOR);
// 		m_entryItem[ST_SERVICES_RPCLOCATOR].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_SERVICES].hItem, 1);
// 		m_entryItem[ST_SERVICES_RPCLOCATOR].text = langStr;
// 		m_entryItem[ST_SERVICES_RPCLOCATOR].enabled=	true;
// 		m_entryItem[ST_SERVICES_RPCLOCATOR].pGroupItem =	&m_hGroupItem[GI_SERVICES];
// 	} else
// 	{
// 		m_entryItem[ST_SERVICES_RPCLOCATOR].enabled=	false;		
// 	}
	///////////////////////////////////////////////////////////////////////////
	if(m_AntiSpySettings.TestSetting(ST_SERVICES_INDEXER)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_SERVICES_INDEXER);
		m_entryItem[ST_SERVICES_INDEXER].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_SERVICES].hItem, 1);
		m_entryItem[ST_SERVICES_INDEXER].text = langStr;
		m_entryItem[ST_SERVICES_INDEXER].enabled=	true;
		m_entryItem[ST_SERVICES_INDEXER].pGroupItem =	&m_hGroupItem[GI_SERVICES];
	} else
	{
		m_entryItem[ST_SERVICES_INDEXER].enabled=	false;		
	}
	///////////////////////////////////////////////////////////////////////////
	if(m_AntiSpySettings.TestSetting(ST_SERVICES_WINDOWS_DEFENDER)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_SERVICES_WINDOWS_DEFENDER);
		m_entryItem[ST_SERVICES_WINDOWS_DEFENDER].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_SERVICES].hItem, 1);
		m_entryItem[ST_SERVICES_WINDOWS_DEFENDER].text = langStr;
		m_entryItem[ST_SERVICES_WINDOWS_DEFENDER].enabled=	true;
		m_entryItem[ST_SERVICES_WINDOWS_DEFENDER].pGroupItem =	&m_hGroupItem[GI_SERVICES];
	} else
	{
		m_entryItem[ST_SERVICES_WINDOWS_DEFENDER].enabled=	false;		
	}
///////////////////////////////////////////////////////////////////////////
	m_TreeCtrl.Expand(m_hGroupItem[GI_SERVICES].hItem, TVE_EXPAND);
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//MS Messenger
/////////////////////////////////////////////////////////////////////////////
    
	langStr.LoadString(IDS_GROUP_MESSENGER);
	m_hGroupItem[GI_MESSENGER].hItem = m_TreeCtrl.InsertGroup(langStr, DISABLED, NULL);
    m_hGroupItem[GI_MESSENGER].text = langStr;
    if(m_AntiSpySettings.TestSetting(ST_MESSENGER_AUTOSTART) != NON_EXISTENT)
	{
		langStr.LoadString(IDS_MESSENGER_AUTOSTART);
		m_entryItem[ST_MESSENGER_AUTOSTART].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_MESSENGER].hItem, 0);
		m_entryItem[ST_MESSENGER_AUTOSTART].text = langStr;
		m_entryItem[ST_MESSENGER_AUTOSTART].enabled=	true;
		m_entryItem[ST_MESSENGER_AUTOSTART].pGroupItem =	&m_hGroupItem[GI_MESSENGER];
	}
	else
	{
		m_entryItem[ST_MESSENGER_AUTOSTART].enabled=	false;
	}
    
    if(m_AntiSpySettings.TestSetting(ST_MESSENGER_6_1_AUTOSTART) != NON_EXISTENT)
	{
		langStr.LoadString(IDS_MESSENGER_AUTOSTART);
		m_entryItem[ST_MESSENGER_6_1_AUTOSTART].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_MESSENGER].hItem, 0);
		m_entryItem[ST_MESSENGER_6_1_AUTOSTART].text = langStr;
		m_entryItem[ST_MESSENGER_6_1_AUTOSTART].enabled=	true;
		m_entryItem[ST_MESSENGER_6_1_AUTOSTART].pGroupItem =	&m_hGroupItem[GI_MESSENGER];
	}
	else
	{
		m_entryItem[ST_MESSENGER_6_1_AUTOSTART].enabled=	false;
	}
    
    if(m_AntiSpySettings.TestSetting(ST_MESSENGER_DEINSTALL) != NON_EXISTENT)
	{
		langStr.LoadString(IDS_MESSENGER_DEINSTALL);
		m_entryItem[ST_MESSENGER_DEINSTALL].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_MESSENGER].hItem, 0);
		m_entryItem[ST_MESSENGER_DEINSTALL].text = langStr;
		m_entryItem[ST_MESSENGER_DEINSTALL].enabled=	true;
		m_entryItem[ST_MESSENGER_DEINSTALL].pGroupItem =	&m_hGroupItem[GI_MESSENGER];
	}
	else
	{
		m_entryItem[ST_MESSENGER_DEINSTALL].enabled=	false;
	}
	m_TreeCtrl.Expand(m_hGroupItem[GI_MESSENGER].hItem, TVE_EXPAND);

	if(!m_entryItem[ST_MESSENGER_DEINSTALL].enabled && !m_entryItem[ST_MESSENGER_6_1_AUTOSTART].enabled && !m_entryItem[ST_MESSENGER_AUTOSTART].enabled)
	{
		m_hGroupItem[GI_MESSENGER].enabled = false;
		m_TreeCtrl.DeleteItem(m_hGroupItem[GI_MESSENGER].hItem);
	}
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//Regsrv
/////////////////////////////////////////////////////////////////////////////
	if(m_AntiSpySettings.TestSetting(ST_REGSVR_REGWIZC) != NON_EXISTENT)
	{
		if(m_AntiSpySettings.IsWinXP())
		{
			langStr.LoadString(IDS_GROUP_REGSVR);
			m_hGroupItem[GI_REGSVR].hItem = m_TreeCtrl.InsertGroup(langStr, DISABLED, NULL);
			m_hGroupItem[GI_REGSVR].text = langStr;
			langStr.LoadString(IDS_REGSVR_REGWIZC);
			m_entryItem[ST_REGSVR_REGWIZC].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_REGSVR].hItem, 1);
			m_entryItem[ST_REGSVR_REGWIZC].text = langStr;
			m_entryItem[ST_REGSVR_REGWIZC].enabled=	true;
			m_entryItem[ST_REGSVR_REGWIZC].pGroupItem =	&m_hGroupItem[GI_REGSVR];
		}
		if(m_AntiSpySettings.IsWinXP())
		{
			langStr.LoadString(IDS_REGSVR_LICDLL);
			m_entryItem[ST_REGSVR_LICDLL].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_REGSVR].hItem,0);
			m_entryItem[ST_REGSVR_LICDLL].text = langStr;
			m_entryItem[ST_REGSVR_LICDLL].enabled=	true;
			m_entryItem[ST_REGSVR_LICDLL].pGroupItem =	&m_hGroupItem[GI_REGSVR];	
		}
		else
		{
			m_entryItem[ST_REGSVR_LICDLL].enabled=	false;
		}
		if(m_AntiSpySettings.IsWinXP())
		{
			m_TreeCtrl.Expand(m_hGroupItem[GI_REGSVR].hItem, TVE_EXPAND);
		}
	}
/////////////////////////////////////////////////////////////////////////////
//Tweaks
/////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	langStr.LoadString(IDS_GROUP_TWEAKS);
	m_hGroupItem[GI_TWEAKS].hItem = m_TreeCtrl.InsertGroup(langStr, DISABLED, NULL);
	m_hGroupItem[GI_TWEAKS].text = langStr;
	if(m_AntiSpySettings.TestSetting(ST_TWEAK_COMMAND_HERE)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_TWEAK_COMMAND_HERE);
		m_entryItem[ST_TWEAK_COMMAND_HERE].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_TWEAKS].hItem, 1);
		m_entryItem[ST_TWEAK_COMMAND_HERE].text = langStr;
		m_entryItem[ST_TWEAK_COMMAND_HERE].enabled=	true;
		m_entryItem[ST_TWEAK_COMMAND_HERE].pGroupItem =	&m_hGroupItem[GI_TWEAKS];
	} else
	{
		m_entryItem[ST_TWEAK_COMMAND_HERE].enabled=	false;		
	}
	///////////////////////////////////////////////////////////////////////////
	if(m_AntiSpySettings.TestSetting(ST_TWEAK_SVC_SUPPORT_CTR)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_TWEAK_SVC_SUPPORT_CTR);
		m_entryItem[ST_TWEAK_SVC_SUPPORT_CTR].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_TWEAKS].hItem, 1);
		m_entryItem[ST_TWEAK_SVC_SUPPORT_CTR].text = langStr;
		m_entryItem[ST_TWEAK_SVC_SUPPORT_CTR].enabled=	true;
		m_entryItem[ST_TWEAK_SVC_SUPPORT_CTR].pGroupItem =	&m_hGroupItem[GI_TWEAKS];
	} else
	{
		m_entryItem[ST_TWEAK_SVC_SUPPORT_CTR].enabled=	false;		
	}
	///////////////////////////////////////////////////////////////////////////
	if(m_AntiSpySettings.TestSetting(ST_TWEAK_SVC_WmdmPmSN)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_TWEAK_SVC_WmdmPmSN);
		m_entryItem[ST_TWEAK_SVC_WmdmPmSN].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_TWEAKS].hItem, 1);
		m_entryItem[ST_TWEAK_SVC_WmdmPmSN].text = langStr;
		m_entryItem[ST_TWEAK_SVC_WmdmPmSN].enabled=	true;
		m_entryItem[ST_TWEAK_SVC_WmdmPmSN].pGroupItem =	&m_hGroupItem[GI_TWEAKS];
	} else
	{
		m_entryItem[ST_TWEAK_SVC_WmdmPmSN].enabled=	false;		
	}
	if(m_AntiSpySettings.TestSetting(ST_TWEAK_DEL_CTFMON)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_TWEAK_DEL_CTFMON);
		m_entryItem[ST_TWEAK_DEL_CTFMON].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_TWEAKS].hItem, 1);
		m_entryItem[ST_TWEAK_DEL_CTFMON].text = langStr;
		m_entryItem[ST_TWEAK_DEL_CTFMON].enabled= true;
		m_entryItem[ST_TWEAK_DEL_CTFMON].pGroupItem = &m_hGroupItem[GI_TWEAKS];
	} else
	{
		m_entryItem[ST_TWEAK_DEL_CTFMON].enabled = false;		
	}
	if(m_AntiSpySettings.TestSetting(ST_TWEAK_CLEANUP_WIZ)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_TWEAK_CLEANUP_WIZ);
		m_entryItem[ST_TWEAK_CLEANUP_WIZ].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_TWEAKS].hItem, 1);
		m_entryItem[ST_TWEAK_CLEANUP_WIZ].text = langStr;
		m_entryItem[ST_TWEAK_CLEANUP_WIZ].enabled = true;
		m_entryItem[ST_TWEAK_CLEANUP_WIZ].pGroupItem = &m_hGroupItem[GI_TWEAKS];
	} else
	{
		m_entryItem[ST_TWEAK_CLEANUP_WIZ].enabled = false;		
	}
	if(m_AntiSpySettings.TestSetting(ST_TWEAK_WINUPDATE_NO_DRIVER)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_TWEAK_WINUPDATE_NO_DRIVER);
		m_entryItem[ST_TWEAK_WINUPDATE_NO_DRIVER].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_TWEAKS].hItem, 1);
		m_entryItem[ST_TWEAK_WINUPDATE_NO_DRIVER].text = langStr;
		m_entryItem[ST_TWEAK_WINUPDATE_NO_DRIVER].enabled= true;
		m_entryItem[ST_TWEAK_WINUPDATE_NO_DRIVER].pGroupItem = &m_hGroupItem[GI_TWEAKS];
	} else
	{
		m_entryItem[ST_TWEAK_WINUPDATE_NO_DRIVER].enabled= false;		
	}
	if(m_AntiSpySettings.TestSetting(ST_TWEAK_WEBSERVICE)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_TWEAK_WEBSERVICE);
		m_entryItem[ST_TWEAK_WEBSERVICE].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_TWEAKS].hItem, 1);
		m_entryItem[ST_TWEAK_WEBSERVICE].text = langStr;
		m_entryItem[ST_TWEAK_WEBSERVICE].enabled = true;
		m_entryItem[ST_TWEAK_WEBSERVICE].pGroupItem = &m_hGroupItem[GI_TWEAKS];
	} else
	{
		m_entryItem[ST_TWEAK_WEBSERVICE].enabled = false;		
	}
	if(m_AntiSpySettings.TestSetting(ST_TWEAK_BSOD_AUTOREBOOT)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_TWEAK_BSOD_AUTOREBOOT);
		m_entryItem[ST_TWEAK_BSOD_AUTOREBOOT].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_TWEAKS].hItem, 1);
		m_entryItem[ST_TWEAK_BSOD_AUTOREBOOT].text = langStr;
		m_entryItem[ST_TWEAK_BSOD_AUTOREBOOT].enabled = true;
		m_entryItem[ST_TWEAK_BSOD_AUTOREBOOT].pGroupItem = &m_hGroupItem[GI_TWEAKS];
	} else
	{
		m_entryItem[ST_TWEAK_BSOD_AUTOREBOOT].enabled = false;		
	}
	if(m_AntiSpySettings.TestSetting(ST_TWEAK_DEV_MANAGER)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_TWEAK_DEV_MANAGER);
		m_entryItem[ST_TWEAK_DEV_MANAGER].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_TWEAKS].hItem, 1);
		m_entryItem[ST_TWEAK_DEV_MANAGER].text = langStr;
		m_entryItem[ST_TWEAK_DEV_MANAGER].enabled=	true;
		m_entryItem[ST_TWEAK_DEV_MANAGER].pGroupItem = &m_hGroupItem[GI_TWEAKS];
	} else
	{
		m_entryItem[ST_TWEAK_DEV_MANAGER].enabled=	false;		
	}
	if(m_AntiSpySettings.TestSetting(ST_TWEAK_NO_WINTOUR)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_TWEAK_NO_WINTOUR);
		m_entryItem[ST_TWEAK_NO_WINTOUR].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_TWEAKS].hItem, 1);
		m_entryItem[ST_TWEAK_NO_WINTOUR].text = langStr;
		m_entryItem[ST_TWEAK_NO_WINTOUR].enabled=	true;
		m_entryItem[ST_TWEAK_NO_WINTOUR].pGroupItem = &m_hGroupItem[GI_TWEAKS];
	} else
	{
		m_entryItem[ST_TWEAK_NO_WINTOUR].enabled=	false;		
	}
	if(m_AntiSpySettings.TestSetting(ST_NO_THUMBNAIL_CACHE)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_NO_THUMBNAIL_CACHE);
		m_entryItem[ST_NO_THUMBNAIL_CACHE].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_TWEAKS].hItem, 1);
		m_entryItem[ST_NO_THUMBNAIL_CACHE].text = langStr;
		m_entryItem[ST_NO_THUMBNAIL_CACHE].enabled=	true;
		m_entryItem[ST_NO_THUMBNAIL_CACHE].pGroupItem =	&m_hGroupItem[GI_TWEAKS];
	} else
	{
		m_entryItem[ST_NO_THUMBNAIL_CACHE].enabled=	false;		
	}
	if(m_AntiSpySettings.TestSetting(ST_NO_SEARCH_ASSISTANT)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_NO_SEARCH_ASSISTANT);
		m_entryItem[ST_NO_SEARCH_ASSISTANT].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_TWEAKS].hItem, 1);
		m_entryItem[ST_NO_SEARCH_ASSISTANT].text = langStr;
		m_entryItem[ST_NO_SEARCH_ASSISTANT].enabled=	true;
		m_entryItem[ST_NO_SEARCH_ASSISTANT].pGroupItem = &m_hGroupItem[GI_TWEAKS];
	} else
	{
		m_entryItem[ST_NO_SEARCH_ASSISTANT].enabled=	false;		
	}
	if(m_AntiSpySettings.TestSetting(ST_DISABLE_UAC)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_DISABLE_UAC);
		m_entryItem[ST_DISABLE_UAC].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_TWEAKS].hItem, 1);
		m_entryItem[ST_DISABLE_UAC].text = langStr;
		m_entryItem[ST_DISABLE_UAC].enabled=	true;
		m_entryItem[ST_DISABLE_UAC].pGroupItem = &m_hGroupItem[GI_TWEAKS];
	} else
	{
		m_entryItem[ST_DISABLE_UAC].enabled=	false;		
	}
	if(m_AntiSpySettings.TestSetting(ST_NO_UNREAD_MAIL)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_NO_UNREAD_MAIL);
		m_entryItem[ST_NO_UNREAD_MAIL].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_TWEAKS].hItem, 1);
		m_entryItem[ST_NO_UNREAD_MAIL].text = langStr;
		m_entryItem[ST_NO_UNREAD_MAIL].enabled=	true;
		m_entryItem[ST_NO_UNREAD_MAIL].pGroupItem = &m_hGroupItem[GI_TWEAKS];
	} else
	{
		m_entryItem[ST_NO_UNREAD_MAIL].enabled=	false;		
	}
	if(m_AntiSpySettings.TestSetting(ST_NO_TEREDO)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_NO_TEREDO);
		m_entryItem[ST_NO_TEREDO].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_TWEAKS].hItem, 1);
		m_entryItem[ST_NO_TEREDO].text = langStr;
		m_entryItem[ST_NO_TEREDO].enabled=	true;
		m_entryItem[ST_NO_TEREDO].pGroupItem = &m_hGroupItem[GI_TWEAKS];
	} else
	{
		m_entryItem[ST_NO_TEREDO].enabled=	false;		
	}
	if(m_AntiSpySettings.TestSetting(ST_NO_ISATAP)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_NO_ISATAP);
		m_entryItem[ST_NO_ISATAP].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_TWEAKS].hItem, 1);
		m_entryItem[ST_NO_ISATAP].text = langStr;
		m_entryItem[ST_NO_ISATAP].enabled=	true;
		m_entryItem[ST_NO_ISATAP].pGroupItem = &m_hGroupItem[GI_TWEAKS];
	} else
	{
		m_entryItem[ST_NO_TEREDO].enabled=	false;		
	}
	if(m_AntiSpySettings.TestSetting(ST_NO_NCSI)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_NO_NCSI);
		m_entryItem[ST_NO_NCSI].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_TWEAKS].hItem, 1);
		m_entryItem[ST_NO_NCSI].text = langStr;
		m_entryItem[ST_NO_NCSI].enabled=	true;
		m_entryItem[ST_NO_NCSI].pGroupItem = &m_hGroupItem[GI_TWEAKS];
	} else
	{
		m_entryItem[ST_NO_TEREDO].enabled=	false;		
	}
	if(m_AntiSpySettings.TestSetting(ST_NO_ROOT_CERT)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_NO_ROOT_CERT);
		m_entryItem[ST_NO_ROOT_CERT].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_TWEAKS].hItem, 1);
		m_entryItem[ST_NO_ROOT_CERT].text = langStr;
		m_entryItem[ST_NO_ROOT_CERT].enabled=	true;
		m_entryItem[ST_NO_ROOT_CERT].pGroupItem = &m_hGroupItem[GI_TWEAKS];
	} else
	{
		m_entryItem[ST_NO_ROOT_CERT].enabled=	false;		
	}
	if(m_AntiSpySettings.TestSetting(ST_CERT_REVOCATION)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_CERT_REVOCATION);
		m_entryItem[ST_CERT_REVOCATION].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_INTERNET_EXPLORER].hItem, 1);
		m_entryItem[ST_CERT_REVOCATION].text = langStr;
		m_entryItem[ST_CERT_REVOCATION].enabled=	true;
		m_entryItem[ST_CERT_REVOCATION].pGroupItem = &m_hGroupItem[GI_INTERNET_EXPLORER];
	} else
	{
		m_entryItem[ST_CERT_REVOCATION].enabled=	false;		
	}
	if(m_AntiSpySettings.TestSetting(ST_CERT_TRUST)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_CERT_TRUST);
		m_entryItem[ST_CERT_TRUST].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_INTERNET_EXPLORER].hItem, 1);
		m_entryItem[ST_CERT_TRUST].text = langStr;
		m_entryItem[ST_CERT_TRUST].enabled=	true;
		m_entryItem[ST_CERT_TRUST].pGroupItem = &m_hGroupItem[GI_INTERNET_EXPLORER];
	} else
	{
		m_entryItem[ST_CERT_TRUST].enabled=	false;		
	}
	if(m_AntiSpySettings.TestSetting(ST_NO_HTTPS_CACHE)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_NO_HTTPS_CACHE);
		m_entryItem[ST_NO_HTTPS_CACHE].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_INTERNET_EXPLORER].hItem, 1);
		m_entryItem[ST_NO_HTTPS_CACHE].text = langStr;
		m_entryItem[ST_NO_HTTPS_CACHE].enabled=	true;
		m_entryItem[ST_NO_HTTPS_CACHE].pGroupItem = &m_hGroupItem[GI_INTERNET_EXPLORER];
	} else
	{
		m_entryItem[ST_NO_HTTPS_CACHE].enabled=	false;		
	}
	if(m_AntiSpySettings.TestSetting(ST_NO_FORCEREBOOT)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_NO_FORCEREBOOT);
		m_entryItem[ST_NO_FORCEREBOOT].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_TWEAKS].hItem, 1);
		m_entryItem[ST_NO_FORCEREBOOT].text = langStr;
		m_entryItem[ST_NO_FORCEREBOOT].enabled=	true;
		m_entryItem[ST_NO_FORCEREBOOT].pGroupItem = &m_hGroupItem[GI_TWEAKS];
	} else
	{
		m_entryItem[ST_NO_FORCEREBOOT].enabled=	false;		
	}
	if(m_AntiSpySettings.TestSetting(ST_TWEAK_NO_SHORTCUT_ARROW)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_TWEAK_NO_SHORTCUT_ARROW);
		m_entryItem[ST_TWEAK_NO_SHORTCUT_ARROW].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_TWEAKS].hItem, 1);
		m_entryItem[ST_TWEAK_NO_SHORTCUT_ARROW].text = langStr;
		m_entryItem[ST_TWEAK_NO_SHORTCUT_ARROW].enabled=	true;
		m_entryItem[ST_TWEAK_NO_SHORTCUT_ARROW].pGroupItem = &m_hGroupItem[GI_TWEAKS];
	} else
	{
		m_entryItem[ST_TWEAK_NO_SHORTCUT_ARROW].enabled=	false;		
	}
	if(m_AntiSpySettings.TestSetting(ST_TWEAK_SVC_BITS)!= NON_EXISTENT)
	{
		langStr.LoadString(IDS_TWEAK_SVC_BITS);
		m_entryItem[ST_TWEAK_SVC_BITS].treeItem = m_TreeCtrl.InsertItem(langStr, m_hGroupItem[GI_TWEAKS].hItem, 1);
		m_entryItem[ST_TWEAK_SVC_BITS].text = langStr;
		m_entryItem[ST_TWEAK_SVC_BITS].enabled=	true;
		m_entryItem[ST_TWEAK_SVC_BITS].pGroupItem = &m_hGroupItem[GI_TWEAKS];
	} else
	{
		m_entryItem[ST_TWEAK_SVC_BITS].enabled=	false;		
	}
	m_TreeCtrl.Expand(m_hGroupItem[GI_TWEAKS].hItem, TVE_EXPAND);


	

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
}

void CXPAntiSpyDlg::OnMenuAbout() 
{
	CAbout aboutDlg;
	aboutDlg.DoModal();	
}

void CXPAntiSpyDlg::OnMenuQuickReference() 
{
	if(m_pSymbolReference == NULL)
	{
		m_pSymbolReference = new LegendDlg(this);		
		m_pSymbolReference->Create(IDD_LEGEND);
	}
	m_pSymbolReference->ShowWindow(SW_SHOW);
}



void CXPAntiSpyDlg::OnSpecialRefresh() 
{
	CheckForSpyFunctions();	
	m_TreeCtrl.Invalidate();
}

void CXPAntiSpyDlg::OnSpecialSetTimeServer() 
{
	CTimeServerDlg timeServerDlg;
	timeServerDlg.DoModal();
}

void CXPAntiSpyDlg::OnRunAsAdmin() 
{
	ReRunApp(m_hWnd, NULL, true);	
}

void CXPAntiSpyDlg::ReRunApp(HWND hWnd, LPCTSTR args, bool asAdmin) 
{
	TCHAR szPath[ MAX_PATH ];
	
	::GetModuleFileName( NULL, szPath, sizeof(szPath) );
	
	SHELLEXECUTEINFO shex;
	
	memset( &shex, 0, sizeof( shex) );
	
	shex.cbSize			= sizeof( SHELLEXECUTEINFO ); 
	shex.fMask			= 0; 
	shex.hwnd			= hWnd;
	shex.lpVerb			= asAdmin==true?_T("runas"):NULL; 
	shex.lpFile			= szPath; 
	shex.lpParameters	= args; 
	shex.lpDirectory	= NULL; 
	shex.nShow			= SW_NORMAL; 
	
	::ShellExecuteEx( &shex );
	exit(0);
}


bool CXPAntiSpyDlg::GetSuiteType(OS_INFO& versionInfo)
{
//———————————————————————————————————
//
// 0 - Older than NT SP6 or Win9x
// 1 - Windows XP Home
// 2 - Windows NT/2000/XP Professional
//
    OSVERSIONINFO_CUSTOM osVerEx;

    //Try the new Ex structure. This will fail on < Windows NT 4.0 SP6
    memset (&osVerEx, 0, sizeof (OSVERSIONINFO_CUSTOM));
    osVerEx.dwOSVersionInfoSize = sizeof (OSVERSIONINFO_CUSTOM);
    if (!GetVersionEx ((OSVERSIONINFO *)&osVerEx))
	{
		memset(&versionInfo,0,sizeof(OS_INFO));
		return false;
	}
//	CString osVerInfo;
//	if(versionInfo)	versionInfo->Format("MajorVersion: %d \nMinorVersion: %d\nBuildNumber: %d\nPlatformId: %d\nCSDVersion: %s\nSuiteMask: %d",osVerEx.dwMajorVersion,osVerEx.dwMinorVersion,osVerEx.dwBuildNumber,osVerEx.dwPlatformId,osVerEx.szCSDVersion,osVerEx.wSuiteMask);
//	AfxMessageBox(osVerInfo);
    switch(osVerEx.dwPlatformId)
	{
		case VER_PLATFORM_WIN32_WINDOWS:
			if(osVerEx.dwMinorVersion==0)
			{
				versionInfo.osName="Windows 95";
			}
			if(osVerEx.dwMinorVersion==10)
			{
				versionInfo.osName="Windows 98";
			}
			if(osVerEx.dwMinorVersion==90)
			{
				versionInfo.osName="Windows ME";
			}
		break;
		case VER_PLATFORM_WIN32_NT:
			DWORD maj = osVerEx.dwMajorVersion;
			DWORD min = osVerEx.dwMinorVersion;
			if(osVerEx.dwMajorVersion==5 && osVerEx.dwMinorVersion==1)
			{
				if(osVerEx.wSuiteMask==768) 
				{
					versionInfo.osName="Windows XP Home";
					versionInfo.osType=WIN_XP_HOME;
				}
				if(osVerEx.wSuiteMask==256) 
				{
					versionInfo.osName="Windows XP Professional";
					versionInfo.osType=WIN_XP_PRO;
				}
			}
			else if(osVerEx.dwMajorVersion==5 && osVerEx.dwMinorVersion==0)
			{
				versionInfo.osName="Windows 2000";
				versionInfo.osType=WIN_XP_HOME;
			}
//#if (WINVER >= 0x600)
			else if(osVerEx.dwMajorVersion==6)
			{
				if(osVerEx.dwMinorVersion==0)
					versionInfo.osName="Windows Vista";
				else if(osVerEx.dwMinorVersion==1)
					versionInfo.osName="Windows 7";
				else
				{
					RTLGETVERSION RtlGetVersion = (RTLGETVERSION)GetProcAddress(GetModuleHandleA("ntdll.dll"), "RtlGetVersion");
					OSVERSIONINFO osv = { 0 };
					osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
					RtlGetVersion(&osv);
					maj = osv.dwMajorVersion;
					min = osv.dwMinorVersion;
					if (osv.dwMajorVersion == 6 && osv.dwMinorVersion == 2)
					{
						versionInfo.osName = "Windows 8";
					}
					else if (osv.dwMajorVersion == 6 && osv.dwMinorVersion == 3)
					{
						versionInfo.osName = "Windows 8.1";
					}
					else if (osv.dwMajorVersion == 10 && osv.dwMinorVersion == 0)
					{
						versionInfo.osName = "Windows 10";
					}
				}
				DWORD dwReturnedProductType = 0;
				pGetProductInfo = (PFUNC_GetProductInfo)::GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "GetProductInfo");

				if (pGetProductInfo) 
				{
					pGetProductInfo(maj, min, 0, 0, &dwReturnedProductType);

					DWORD type = 0;
	/*				GetProductInfo(osVerEx.dwMajorVersion,
						osVerEx.dwMinorVersion,
						osVerEx.wServicePackMajor,
						osVerEx.wServicePackMinor,
						&type);*/
					CString edName = "";
					switch(dwReturnedProductType)
					{
						case PRODUCT_HOME_PREMIUM:
							edName = "Home Premium Edition";
							break;
						case PRODUCT_BUSINESS:
							edName = "Business Edition";
							break;
						case PRODUCT_BUSINESS_N:
							edName = "Business N Edition";
						case PRODUCT_CLUSTER_SERVER:
							break;
							edName = "Cluster Server Edition";
						case PRODUCT_DATACENTER_SERVER:
							edName = "Server Datacenter Edition (full)";
							break;
						case PRODUCT_DATACENTER_SERVER_CORE:
							edName = "Server Datacenter Edition (core)";
						case PRODUCT_ENTERPRISE:
							break;
							edName = "Enterprise Edition";
						case PRODUCT_ENTERPRISE_SERVER:
							edName = "Server Enterprise Edition (full)";
							break;
						case PRODUCT_ENTERPRISE_SERVER_CORE:
							edName = "Server Enterprise Edition (core)";
						case PRODUCT_ENTERPRISE_SERVER_IA64:
							break;
							edName = "Server Enterprise Edition IA64";
						case PRODUCT_HOME_BASIC:
							edName = "Home Basic Edition";
							break;
						case PRODUCT_HOME_BASIC_N:
							edName = "Home Basic N Edition";
						case PRODUCT_HOME_SERVER:
							break;
							edName = "Home Server Edition";
							break;
						case PRODUCT_SERVER_FOR_SMALLBUSINESS:
							edName = "Server for Small Business Edition";
						case PRODUCT_SMALLBUSINESS_SERVER:
							edName = "Small Business Server";
							break;
						case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
							edName = "Small Business Server Premium";
							break;
						case PRODUCT_STANDARD_SERVER:
							edName = "Server Standard Edition (full)";
							break;
						case PRODUCT_STANDARD_SERVER_CORE:
							edName = "Server Standard Edition (full)";
							break;
						case PRODUCT_STARTER:
							edName = "Starter Edition";
							break;
						case PRODUCT_STORAGE_ENTERPRISE_SERVER:
							edName = "Storage Server Enterprise Edition";
							break;
						case PRODUCT_STORAGE_EXPRESS_SERVER:
							edName = "Storage Server Express Edition";
							break;
						case PRODUCT_STORAGE_STANDARD_SERVER:
							edName = "Storage Server Standard Edition";
							break;
						case PRODUCT_STORAGE_WORKGROUP_SERVER:
							edName = "Storage Server Workgroup Edition";
							break;
						case PRODUCT_UNDEFINED:
							edName = "Undefined";
							break;
						case PRODUCT_ULTIMATE:
							edName = "Ultimate Edition";
							break;
						case PRODUCT_WEB_SERVER:
							edName = "Web Server Edition";
							break;
						case PRODUCT_UNLICENSED:
							edName = "Unlicensed";
							break;
						case PRODUCT_PROFESSIONAL:
						case PRODUCT_PROFESSIONAL_E:
						case PRODUCT_PROFESSIONAL_N:
							edName = "Professional";
							break;
					}
					if(edName != "")
					{
						versionInfo.osName += " " + edName;
					}
				}
			}
//#endif
			versionInfo.servicePack=osVerEx.wServicePackMajor;
			versionInfo.buidNo=osVerEx.dwBuildNumber;

		break;
	}
	
	return true;
}

bool CXPAntiSpyDlg::IsSupportuser()
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

void CXPAntiSpyDlg::OnMenuVisitHP() 
{
	ShellExecute(NULL, _T("open"), _T("http://www.xp-AntiSpy.org"), NULL, NULL, SW_SHOWNORMAL);	
}

void CXPAntiSpyDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	if(IsWindow(m_split.m_hWnd))
	{
		const int margin = 2;
		CRect rect,rect2;
		GetClientRect(&rect2);
		if(m_showSearchControl)
		{
			m_searchField.GetWindowRect(&rect);
			rect.SetRect(margin + 20,margin,rect2.Width() - margin - 22,SEARCH_HEIGHT);
			m_searchField.MoveWindow(&rect);
			rect.SetRect(rect2.Width() - 22,0,rect2.Width(),SEARCH_HEIGHT + 2);
			m_btnClearSearch.MoveWindow(&rect);
		}
		
		rect=rect2;
		rect.DeflateRect(0,m_showSearchControl?SEARCH_HEIGHT+2:0,0,BTNHEIGHT);
		m_split.MoveWindow(&rect);
		rect=rect2;
		rect.DeflateRect(0,rect.Height()-BTNHEIGHT,0,0);
		m_BtnFwd.MoveWindow(&rect);	
	}
}

void CXPAntiSpyDlg::PopUpMenu(CPoint point,HTREEITEM hItem)
{
	this->ClientToScreen (&point);
	m_hSelectedItem = hItem;
	int idx(TreeItemFromHItem(m_hSelectedItem));
	if(idx >= 0)
	{
		CMenu* pSubMenu = m_hPopUp.GetSubMenu(0);
		switch(m_AntiSpySettings.TestSetting((SETTINGS)idx))
		{
			case 0:
				pSubMenu->EnableMenuItem(ID_RESET, MF_GRAYED);
				pSubMenu->EnableMenuItem(ID_SET, MF_ENABLED);
			break;
			case 1:
				pSubMenu->EnableMenuItem(ID_RESET, MF_ENABLED);
				pSubMenu->EnableMenuItem(ID_SET, MF_GRAYED);				
			break;
		}
		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,point.x,point.y,this);
	}
}

void CXPAntiSpyDlg::OnPopUpResetSetting() 
{
	SetCursor(LoadCursor(NULL, IDC_WAIT));

	//find matching item
	int i=0;
	for(i=0;i<ITEMCOUNT;++i)
	{
		if(m_hSelectedItem==m_entryItem[i].treeItem) 
			break;
	}
	CString tmp;
	tmp.Format(_T("%d"),i);
	if(i<=ITEMCOUNT&&(i!=ST_MESSENGER_DEINSTALL)&&(i!=ST_MESSENGER_AUTOSTART)&&(i!=ST_MESSENGER_6_1_AUTOSTART)&&(i!=ST_ALEXA))
	{
		m_AntiSpySettings.EnableSetting(i);
		Sleep(1000);//Boring
		CheckForSpyFunctions();
	}
	else
	{
#ifdef __ENGLISH__
		AfxMessageBox(_T("This Function can't be reset."));
#else
		AfxMessageBox(_T("Diese Funktion kann nicht zurückgesetzt werden."));
#endif
	}
	SetCursor(LoadCursor(NULL, IDC_ARROW));
	m_TreeCtrl.RedrawItem(m_hSelectedItem);
}

void CXPAntiSpyDlg::OnPopUpSetSetting() 
{
	SetCursor(LoadCursor(NULL, IDC_WAIT));

	//find matching item
	int i=0;
	for(i=0;i<ITEMCOUNT;++i)
	{
		if(m_hSelectedItem==m_entryItem[i].treeItem) 
			break;
	}
	CString tmp;
	tmp.Format(_T("%d"),i);
	m_AntiSpySettings.DisableSetting(i);
	Sleep(1000);//Boring
	CheckForSpyFunctions();
	SetCursor(LoadCursor(NULL, IDC_ARROW));
	m_TreeCtrl.RedrawItem(m_hSelectedItem);
}

void CXPAntiSpyDlg::TrackHelpText(HTREEITEM hItem)
{
	bool isGroupItem=false;
	//find matching item
	UINT i=0;
	for(i=0;i<ITEMCOUNT;++i)
	{
		if(hItem==m_entryItem[i].treeItem) 
		{
			isGroupItem=false;
			break;
		}
		if(hItem==m_hGroupItem[i].hItem) 
		{
			isGroupItem=true;
			break;
		}
	}
	m_HelpBox.SetHelpText(i,isGroupItem);
}

void CXPAntiSpyDlg::SetApplyButtonState(bool state)
{
		m_BtnFwd.EnableWindow(state);
		m_applyBtnState = state;
}

void CXPAntiSpyDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CDialog::OnLButtonUp(nFlags, point);
}

void CXPAntiSpyDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CDialog::OnLButtonDown(nFlags, point);
}

BOOL CXPAntiSpyDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if(wParam == 2 && lParam == 0)
		storeWindowSize();
	if((HWND)lParam==m_BtnFwd.m_hWnd) 
		OnBtnFwd();
	if((HWND)lParam==m_btnClearSearch.m_hWnd) 
		ResetFiltering();
	if(wParam == ID_PROFILE_NORMAL)
	{
		enableAutoStartMenu(false);
	}
	else
	{
		enableAutoStartMenu(true);
	}

	if(wParam == ID_PROFILE_NORMAL)
	{
		CheckProfile(PROFILE_NORMAL_ON, PROFILE_NORMAL_OFF, wParam);
	}
	if(wParam == ID_PROFILE_SYSTEM_DEFAULT)
	{
		CheckProfile(PROFILE_SYSTEM_DEFAULT_ON, PROFILE_SYSTEM_DEFAULT_OFF, wParam);
	}
	if(wParam == ID_PROFILE_SUGGESTED)
	{
		CheckProfile(PROFILE_SUGGESTED_ON, PROFILE_SUGGESTED_OFF, wParam);
	}
	if(wParam == ID_PROFILE_WINDOWS_UPDATE)
	{
		CheckProfile(PROFILE_WINDOWS_UPDATE_ON, PROFILE_WINDOWS_UPDATE_OFF, wParam);
	}
	if(wParam == ID_USERDEFINED_PROFILES)
	{
		showUserDefinedProfileDlg();
	}
	if(wParam == ID_PROFILE_AUTOSTART)
	{
		MENUITEMINFO mi;

		//get profiles submenu
		CMenu* pSubMenu=GetMenu()->GetSubMenu(0);
		mi.cbSize = sizeof (mi);
		mi.fMask = MIIM_STATE; 
		bool ret = pSubMenu->GetMenuItemInfo(ID_PROFILE_AUTOSTART, &mi);
		//pSubMenu->CheckMenuRadioItem(ID_PROFILE_AUTOSTART, ID_PROFILE_AUTOSTART, wParam, MF_BYCOMMAND);
		if(mi.fState == MFS_UNCHECKED)
		{
			mi.fState = MFS_CHECKED;
			ProfileDlg::SetAutoCheckProfile(true);
		}
		else if(mi.fState == MFS_CHECKED)
		{
			mi.fState = MFS_UNCHECKED;
			ProfileDlg::SetAutoCheckProfile(false);
		}	
		SetMenuItemInfo(pSubMenu->m_hMenu, ID_PROFILE_AUTOSTART, false, &mi);
	}
	if((wParam > ID_PROFILE_SYSTEM_DEFAULT)&&(wParam <= ID_PROFILE_SYSTEM_DEFAULT+m_userDefinedProfiles))
	{
		CMenu* pSubMenu=GetMenu()->GetSubMenu(0);
		pSubMenu->CheckMenuRadioItem(ID_PROFILE_NORMAL, ID_PROFILE_SYSTEM_DEFAULT + m_userDefinedProfiles, wParam, MF_BYCOMMAND);
		CString tmp;
		pSubMenu->GetMenuString(wParam, tmp, MF_BYCOMMAND);
		ResetAlerts();
		SetProfile(tmp);
		OnSpecialRefresh();
		ProfileDlg::SetStartProfile(wParam);
	}
	if((wParam == ID_HELPINDEX))
	{
		HINSTANCE h=ShellExecute(NULL, _T("open"), _T("xp-AntiSpy.chm"), NULL, NULL, SW_SHOWNORMAL);
		if ((UINT)h < 32) 
		{
			MessageBeep(0);
			CString tmp;
			tmp.LoadString(IDS_NO_HELP);
			MessageBox(tmp);
		}
	
	}
	if((wParam == ID_MODE_STANDARD))
	{
		setMode(CXPAntiSpySettingsCtrl::NORMAL);
	}
	else if((wParam == ID_MODE_ACCESSIBLE))
	{
		setMode(CXPAntiSpySettingsCtrl::ACCESSIBLE);
	}
	else if(wParam == ID_MNU_SHOW_SEARCH)
	{
		toggleSearchField();
	}
	return CDialog::OnCommand(wParam, lParam);
}
void CXPAntiSpyDlg::toggleSearchField()
{
	MENUITEMINFO mi;

	CMenu* pSubMenu=GetMenu()->GetSubMenu(1);
	mi.cbSize = sizeof (mi);
	mi.fMask = MIIM_STATE; 
	bool ret = pSubMenu->GetMenuItemInfo(ID_MNU_SHOW_SEARCH, &mi);
	if(mi.fState == MFS_UNCHECKED)
	{
		mi.fState = MFS_CHECKED;
		m_showSearchControl = true;
	}
	else if(mi.fState == MFS_CHECKED)
	{
		mi.fState = MFS_UNCHECKED;
		m_showSearchControl = false;
		
	}	
	SetMenuItemInfo(pSubMenu->m_hMenu, ID_MNU_SHOW_SEARCH, false, &mi);
	m_searchIcon.ShowWindow(m_showSearchControl);
	m_searchField.ShowWindow(m_showSearchControl);
	m_btnClearSearch.ShowWindow(m_showSearchControl);
	CRect r;
	GetWindowRect(&r);
	MoveWindow(r.left, r.top, r.Width(), r.Height() + (m_showSearchControl?SEARCH_HEIGHT:-SEARCH_HEIGHT));
	if(!m_showSearchControl)
	{
		if(m_searchField.GetWindowTextLength() > 0)
		{
			m_searchField.SetWindowText(_T(""));
			ResetFiltering();
		}

	}
	else
		m_searchField.SetFocus();

}

void CXPAntiSpyDlg::enableAutoStartMenu(bool enable)
{
	MENUITEMINFO mi;
	//get profiles submenu
	CMenu* pSubMenu=GetMenu()->GetSubMenu(0);
	mi.cbSize = sizeof (mi);
	mi.fMask = MIIM_STATE; 
	bool ret = pSubMenu->GetMenuItemInfo(ID_PROFILE_AUTOSTART, &mi);
	//pSubMenu->CheckMenuRadioItem(ID_PROFILE_AUTOSTART, ID_PROFILE_AUTOSTART, wParam, MF_BYCOMMAND);
	if(enable)
	{
		mi.fState = MFS_ENABLED;
		if(ProfileDlg::IsAutoCheckProfile())
		{
			mi.fState |= MFS_CHECKED;
		}
	}
	else
	{
		mi.fState = MFS_DISABLED | MFS_UNCHECKED;
		if(ProfileDlg::IsAutoCheckProfile())
		{
			ProfileDlg::SetAutoCheckProfile(false);
		}
	}	
	SetMenuItemInfo(pSubMenu->m_hMenu, ID_PROFILE_AUTOSTART, false, &mi);
}


void CXPAntiSpyDlg::CheckProfile(const int idOn[], const int idOff[], const int idMenu)
{
	ResetAlerts();
	SetProfile(true, idOn);
	SetProfile(false, idOff);
	//get profiles submenu
	CMenu* pSubMenu=GetMenu()->GetSubMenu(0);
	pSubMenu->CheckMenuRadioItem(ID_PROFILE_NORMAL, ID_PROFILE_SYSTEM_DEFAULT + m_userDefinedProfiles, idMenu, MF_BYCOMMAND);
	OnSpecialRefresh();
	ProfileDlg::SetStartProfile(idMenu);
}


void CXPAntiSpyDlg::OnConnectionLimit() 
{
	CConnectionLimitDlg dlg;
	dlg.DoModal();
}

int CXPAntiSpyDlg::TreeItemFromHItem(HTREEITEM item)
{
	for(int i=0; i<ITEMCOUNT; ++i)
	{
		if(m_entryItem[i].treeItem == item)
			return i;
	}
	return -1;
}

void CXPAntiSpyDlg::OnOK()
{
	
}

void CXPAntiSpyDlg::SetProfile(BOOL isON, const int id[])
{
	if(isON)
	{
		m_TreeCtrl.setMode(m_TreeCtrl.getMode());
		if(!id)
		{
			int i=0;
			while(ST_END_ENTRY!=m_profile_on[i])
			{
				if(m_profile_on[i] == -1)
				{
					++i;
					continue;
				}
				if(m_entryItem[m_profile_on[i]].enabled)
				{
					if(m_TreeCtrl.IsCheckBox(m_entryItem[m_profile_on[i]].treeItem))
					{
						const SETTING_STATE state= m_AntiSpySettings.TestSetting((SETTINGS)m_profile_on[i]);
						const BOOL checked(m_TreeCtrl.IsChecked(m_entryItem[m_profile_on[i]].treeItem));
						if(state == ACTIVE)
						{
							m_TreeCtrl.SetItemImage(m_entryItem[m_profile_on[i]].treeItem, CHECKED_RIGHT, CHECKED_RIGHT);
						}
						else if(state == ACTIVE_NO_PERMISSION)
						{
							m_TreeCtrl.SetItemImage(m_entryItem[m_profile_on[i]].treeItem, CHECKED_RIGHT_LOCKED, CHECKED_RIGHT_LOCKED);
						}
						else if(state == INACTIVE)
						{
							m_TreeCtrl.SetChecked(m_entryItem[m_profile_on[i]].treeItem, true);
							m_TreeCtrl.SetItemImage(m_entryItem[m_profile_on[i]].treeItem, UNCHECKED_WRONG, UNCHECKED_WRONG);
							AlertGroup(&m_entryItem[m_profile_on[i]]);
							SetApplyButtonState(true);
						}
						else if(state == INACTIVE_NO_PERMISSION)
						{
							m_TreeCtrl.SetChecked(m_entryItem[m_profile_on[i]].treeItem, true);
							m_TreeCtrl.SetItemImage(m_entryItem[m_profile_on[i]].treeItem, UNCHECKED_WRONG_LOCKED, UNCHECKED_WRONG_LOCKED);
							AlertGroup(&m_entryItem[m_profile_on[i]]);
						}
					}
				}
				++i;
			}
		}
		else
		{
			int i=0;
			while(ST_END_ENTRY!=id[i])
			{
				if(m_entryItem[id[i]].enabled)
				{
					if(m_TreeCtrl.IsCheckBox(m_entryItem[id[i]].treeItem))
					{
						const SETTING_STATE setting= m_AntiSpySettings.TestSetting((SETTINGS)id[i]);
						const BOOL checked(m_TreeCtrl.IsChecked(m_entryItem[id[i]].treeItem));
						if(setting == ACTIVE)
						{
							m_TreeCtrl.SetItemImage(m_entryItem[id[i]].treeItem, CHECKED_RIGHT, CHECKED_RIGHT);
						}
						else if(setting == ACTIVE_NO_PERMISSION)
						{
							m_TreeCtrl.SetItemImage(m_entryItem[id[i]].treeItem, CHECKED_RIGHT_LOCKED, CHECKED_RIGHT_LOCKED);
						}
						else if(setting == INACTIVE)
						{
							m_TreeCtrl.SetChecked(m_entryItem[id[i]].treeItem, true);
							m_TreeCtrl.SetItemImage(m_entryItem[id[i]].treeItem, UNCHECKED_WRONG, UNCHECKED_WRONG);
							AlertGroup(&m_entryItem[id[i]]);
							SetApplyButtonState(true);
						}
						else if(setting == INACTIVE_NO_PERMISSION)
						{
							m_TreeCtrl.SetChecked(m_entryItem[id[i]].treeItem, true);
							m_TreeCtrl.SetItemImage(m_entryItem[id[i]].treeItem, UNCHECKED_WRONG_LOCKED, UNCHECKED_WRONG_LOCKED);
							AlertGroup(&m_entryItem[id[i]]);
						}
					}
					m_profile_on[i] = id[i];
				}
				else
				{
					m_profile_on[i] = -1;
				}
				++i;
			}
			m_profile_on[i] = ST_END_ENTRY;
		}
	}
	else
	{
		m_TreeCtrl.setMode(m_TreeCtrl.getMode());
		if(!id)
		{
			int i=0;
			while(ST_END_ENTRY!=m_profile_off[i])
			{
				if(m_profile_off[i] == -1)
				{
					++i;
					continue;
				}
				if(m_entryItem[m_profile_off[i]].enabled)
				{
					if(m_TreeCtrl.IsCheckBox(m_entryItem[m_profile_off[i]].treeItem))
					{
						const SETTING_STATE setting= m_AntiSpySettings.TestSetting((SETTINGS)m_profile_off[i]);
						const BOOL checked(m_TreeCtrl.IsChecked(m_entryItem[m_profile_off[i]].treeItem));
						if(setting == ACTIVE)
						{
							m_TreeCtrl.SetItemImage(m_entryItem[m_profile_off[i]].treeItem, CHECKED_WRONG, CHECKED_WRONG);
							AlertGroup(&m_entryItem[m_profile_off[i]]);
						}
						else if(setting == ACTIVE_NO_PERMISSION)
						{
							m_TreeCtrl.SetItemImage(m_entryItem[m_profile_off[i]].treeItem, CHECKED_WRONG_LOCKED, CHECKED_WRONG_LOCKED);
							AlertGroup(&m_entryItem[m_profile_off[i]]);
						}
						else if(setting == INACTIVE)
						{
							m_TreeCtrl.SetChecked(m_entryItem[m_profile_off[i]].treeItem, true);
							m_TreeCtrl.SetItemImage(m_entryItem[m_profile_off[i]].treeItem, UNCHECKED_RIGHT, UNCHECKED_RIGHT);
							SetApplyButtonState(true);
						}
						else if(setting == INACTIVE_NO_PERMISSION)
						{
							m_TreeCtrl.SetChecked(m_entryItem[m_profile_off[i]].treeItem, true);
							m_TreeCtrl.SetItemImage(m_entryItem[m_profile_off[i]].treeItem, UNCHECKED_RIGHT_LOCKED, UNCHECKED_RIGHT_LOCKED);
						}
					}
				}
				++i;
			}
		}
		else
		{
			int i=0;
			while(ST_END_ENTRY!=id[i])
			{
				if(m_entryItem[id[i]].enabled)
				{
					if(m_TreeCtrl.IsCheckBox(m_entryItem[id[i]].treeItem))
					{
						const SETTING_STATE setting= m_AntiSpySettings.TestSetting((SETTINGS)id[i]);
						const BOOL checked(m_TreeCtrl.IsChecked(m_entryItem[id[i]].treeItem));
						if(setting == ACTIVE)
						{
							m_TreeCtrl.SetItemImage(m_entryItem[id[i]].treeItem, UNCHECKED_WRONG, UNCHECKED_WRONG);
						}
						else if(setting == ACTIVE_NO_PERMISSION)
						{
							m_TreeCtrl.SetItemImage(m_entryItem[id[i]].treeItem, UNCHECKED_WRONG_LOCKED, UNCHECKED_WRONG_LOCKED);
						}
						else if(setting == INACTIVE)
						{
							m_TreeCtrl.SetChecked(m_entryItem[id[i]].treeItem, true);
							m_TreeCtrl.SetItemImage(m_entryItem[id[i]].treeItem, UNCHECKED_RIGHT, UNCHECKED_RIGHT);
							SetApplyButtonState(true);
						}
						else if(setting == INACTIVE_NO_PERMISSION)
						{
							m_TreeCtrl.SetChecked(m_entryItem[id[i]].treeItem, true);
							m_TreeCtrl.SetItemImage(m_entryItem[id[i]].treeItem, UNCHECKED_RIGHT_LOCKED, UNCHECKED_RIGHT_LOCKED);
						}
					}
					m_profile_off[i] = id[i];
				}
				else
				{
					m_profile_off[i] = -1;
				}
				++i;
			}
			m_profile_off[i] = ST_END_ENTRY;
		}
	}

}

bool CXPAntiSpyDlg::toggleItem(const HTREEITEM& hItem)
{
	for(int i = 0; i<ITEMCOUNT; ++i)
	{
		if(m_entryItem[i].treeItem == hItem)
		{
			const SETTING_STATE setting= m_AntiSpySettings.TestSetting((SETTINGS)i);
			const BOOL checked(m_TreeCtrl.IsChecked(hItem));
			const unsigned int profileSuggestion = GetProfileSuggestion((SETTINGS)i);
			if(profileSuggestion == PROFILE_SUGGESTS_OFF && !setting)
				return false;
			if(profileSuggestion == PROFILE_SUGGESTS_ON && setting)
				return false;
			if((setting == ACTIVE) && (checked))
			{
				m_TreeCtrl.SetItemImage(m_entryItem[i].treeItem, UNCHECKED_CHANGE, UNCHECKED_CHANGE);
				SetApplyButtonState(true);
			}
			else if((setting == INACTIVE) && (!checked))
			{
				m_TreeCtrl.SetItemImage(m_entryItem[i].treeItem, CHECKED_CHANGE, CHECKED_CHANGE);
				SetApplyButtonState(true);
			}
			else if((setting == ACTIVE) && (!checked))
			{
				if(profileSuggestion == PROFILE_SUGGESTS_OFF)
					m_TreeCtrl.SetItemImage(m_entryItem[i].treeItem, CHECKED_WRONG, CHECKED_WRONG);
				else
				{
					m_TreeCtrl.SetItemImage(m_entryItem[i].treeItem, CHECKED, CHECKED);
					m_TreeCtrl.SetCheck(m_entryItem[i].treeItem, true);
				}
			}
			else if((setting == INACTIVE) && checked)
			{
				if(profileSuggestion == PROFILE_SUGGESTS_ON)
					m_TreeCtrl.SetItemImage(m_entryItem[i].treeItem, UNCHECKED_WRONG, UNCHECKED_WRONG);
				else
				{
					m_TreeCtrl.SetItemImage(m_entryItem[i].treeItem, UNCHECKED, UNCHECKED);
					m_TreeCtrl.SetChecked(m_entryItem[i].treeItem, false);
				}
			}
	
			return true;
		}
	}
}

SettingVec CXPAntiSpyDlg::GetSettings()
{
	SettingVec vec;
	for(int i=0; i<ITEMCOUNT; ++i)
	{
		if(m_entryItem[i].enabled)
		{
			SETTING set;
			set.checked = m_TreeCtrl.IsChecked(m_entryItem[i].treeItem);
			set.settingID = i;
			vec.push_back(set);
		}
	}
	return vec;
}

void CXPAntiSpyDlg::UpdateProfiles()
{
	CMenu* pMenu = GetMenu()->GetSubMenu(0);
	//clean out old userdefined entries
	int start = 0;
	int stop = 0;
	int i=0;
	for(i = 0; i < pMenu->GetMenuItemCount(); ++i)
	{
		int id = pMenu->GetMenuItemID(i);
		if(id == (ID_PROFILE_SYSTEM_DEFAULT))
			start = i + 1;
		//look for the divider
		if(id == 0)
		{
			stop = i;
			break;
		}
	}
	//if userdefined entry exist
	if(start != 0)
	{
		for(i = start; i < stop; ++i)
		{
			pMenu->RemoveMenu(start, MF_BYPOSITION);
		}
	}
	ProfileDlg::StringVec vec = ProfileDlg::GetExistingProfiles();
	m_userDefinedProfiles = vec.size();
	for(i=0; i<vec.size(); ++i)
	{
		pMenu->InsertMenu(4, MF_BYPOSITION, ID_PROFILE_SYSTEM_DEFAULT+i+1, vec[i]);
	}

}

void CXPAntiSpyDlg::SetProfile(CString name)
{
	SettingVec vec = ProfileDlg::GetProfile(name);
	//set the 'on' values
	int idx = 0;
	int i = 0;
	for(i = 0; i < vec.size(); ++i)
	{
		SETTING set = vec[i];
		if(set.checked)
		{
			m_profile_on[idx] = set.settingID;
			++idx;
		}
	}
	m_profile_on[idx] = ST_END_ENTRY;
	//set the 'off' values
	idx = 0;
	for(i = 0; i < vec.size(); ++i)
	{
		SETTING set = vec[i];
		if(!set.checked)
		{
			m_profile_off[idx] = set.settingID;
			++idx;
		}
	}
	m_profile_off[idx] = ST_END_ENTRY;
	
	SetProfile(true);
	SetProfile(false);
//	setMode(m_TreeCtrl.getMode());
}

void CXPAntiSpyDlg::ResetFiltering()
{
	if(!m_isTreeFiltered)
	{
		toggleSearchField();
		return;
	}
	HTREEITEM root = m_TreeCtrl.GetRootItem();	
	m_TreeCtrl.DeleteAllItems();
	for(int j=0; j<ITEMCOUNT; ++j)
	{
		//reset the handle to the tree item, since it's deleted
		m_entryItem[j].treeItem = NULL;
	}
	for(int i=0; i<GROUPCOUNT; ++i)
	{
		if(i == GI_REGSVR)
		{
			int t = 0;
			++t;
		}
		if(m_hGroupItem[i].enabled)
		{
			CString tx = m_hGroupItem[i].text.c_str();
			m_hGroupItem[i].hItem = m_TreeCtrl.InsertGroup(tx, DISABLED, NULL);
			bool foundChild = false;
			for(int j=0; j<ITEMCOUNT; ++j)
			{
				if(m_entryItem[j].pGroupItem == &m_hGroupItem[i])
				{
					foundChild = true;
					m_entryItem[j].treeItem = m_TreeCtrl.InsertItem(m_entryItem[j].text.c_str(), m_hGroupItem[i].hItem, 1);
				}

			}
			if(foundChild)
				m_TreeCtrl.Expand(m_hGroupItem[i].hItem, TVE_EXPAND);
			else
			{
				m_TreeCtrl.DeleteItem(m_hGroupItem[i].hItem);
				m_hGroupItem[i].hItem = NULL;
			}
			m_TreeCtrl.Expand(m_hGroupItem[i].hItem, TVE_EXPAND);
		}
	}
	CheckForSpyFunctions();
	m_isTreeFiltered = false;
	m_searchField.SetWindowText(_T(""));
}

void CXPAntiSpyDlg::FilterTreeControl(CString searchText)
{
	searchText.MakeUpper();
	searchText.TrimLeft();
	searchText.TrimRight();
	if(searchText.GetLength() == 0)
		return;
	HTREEITEM root = m_TreeCtrl.GetRootItem();	
	m_TreeCtrl.DeleteAllItems();
	for(int j=0; j<ITEMCOUNT; ++j)
	{
		//reset the handle to the tree item, since it's deleted
		m_entryItem[j].treeItem = NULL;
	}
	for(int i=0; i<GROUPCOUNT; ++i)
	{
		if(m_hGroupItem[i].enabled)
		{
			CString tx = m_hGroupItem[i].text.c_str();
			m_hGroupItem[i].hItem = m_TreeCtrl.InsertGroup(tx, DISABLED, NULL);
			bool foundChild = false;
			for(int j=0; j<ITEMCOUNT; ++j)
			{
				if(m_entryItem[j].pGroupItem == &m_hGroupItem[i])
				{
					//search setting title
					CString txt = m_entryItem[j].text.c_str();
					txt.MakeUpper();
					int find = txt.Find(searchText);
					if(find > -1)
					{
						foundChild = true;
						m_entryItem[j].treeItem = m_TreeCtrl.InsertItem(m_entryItem[j].text.c_str(), m_hGroupItem[i].hItem, 1);
					}
					else
					{
						//search setting description
						txt = CResource::getHelpText(j);
						txt.MakeUpper();
						int find = txt.Find(searchText);
						if(find > -1)
						{
							foundChild = true;
							m_entryItem[j].treeItem = m_TreeCtrl.InsertItem(m_entryItem[j].text.c_str(), m_hGroupItem[i].hItem, 1);
						}
					}

				}

			}
			if(foundChild)
				m_TreeCtrl.Expand(m_hGroupItem[i].hItem, TVE_EXPAND);
			else
			{
				m_TreeCtrl.DeleteItem(m_hGroupItem[i].hItem);
				m_hGroupItem[i].hItem = NULL;
			}
				
		}
	}
	m_isTreeFiltered = true;
	CheckForSpyFunctions();
}

BOOL CXPAntiSpyDlg::PreTranslateMessage(MSG* pMsg) 
{
    if(pMsg->message==WM_KEYDOWN)
    {
		if(pMsg->hwnd == m_searchField.m_hWnd)
		{
			if(pMsg->wParam == VK_RETURN)
			{
				CString search;
				m_searchField.GetWindowText(search);
				if(search.GetLength() > 0)
					FilterTreeControl(search);
			}

		}
		TRACE1("Key: %d", pMsg->wParam);
        if(pMsg->wParam==VK_F5)
		{
			OnSpecialRefresh();
			return 1;
		}
		else if(pMsg->wParam==VK_F2)
		{	
			setMode(CXPAntiSpySettingsCtrl::NORMAL);
			return 1;
		}
		else if(pMsg->wParam==VK_F3)
		{
			setMode(CXPAntiSpySettingsCtrl::ACCESSIBLE);
			return 1;
		}
		else if(pMsg->wParam==VK_SPACE && GetKeyState(VK_CONTROL)<0)
		{
			OnBtnFwd();
			return 1;
		}
		else if(GetKeyState(VK_SHIFT)<0 && pMsg->wParam==VK_TAB)
		{
			SwitchFocus(false);
			return 1;
		}
		else if(pMsg->wParam==VK_TAB)
		{
			TRACE0("Tabbed\n");
			SwitchFocus(true);
			return 1;
		}
		else if(pMsg->wParam==VK_ESCAPE && pMsg->hwnd == m_searchField.m_hWnd)
		{
			ResetFiltering();
			return true;
		}
		else if(GetKeyState(VK_CONTROL)<0)
		{
			if(pMsg->wParam==49)
			{
				CheckProfile(PROFILE_NORMAL_ON, PROFILE_NORMAL_OFF, ID_PROFILE_NORMAL);
				enableAutoStartMenu(false);
				return 1;
			}
			else if(pMsg->wParam==50)
			{
				CheckProfile(PROFILE_SUGGESTED_ON, PROFILE_SUGGESTED_OFF, ID_PROFILE_SUGGESTED);
				enableAutoStartMenu(true);
				return 1;
			}
			else if(pMsg->wParam==51)
			{
				CheckProfile(PROFILE_WINDOWS_UPDATE_ON, PROFILE_WINDOWS_UPDATE_OFF, ID_PROFILE_WINDOWS_UPDATE);
				enableAutoStartMenu(true);
				return 1;
			}
			else if(pMsg->wParam==52)
			{
				CheckProfile(PROFILE_SYSTEM_DEFAULT_ON, PROFILE_SYSTEM_DEFAULT_OFF, ID_PROFILE_SYSTEM_DEFAULT);
				enableAutoStartMenu(true);
				return 1;
			}
			else if(pMsg->wParam==13)
			{
				showUserDefinedProfileDlg();
				return 1;
			}
			else if(pMsg->wParam==70)
			{
				toggleSearchField();
				return 1;
			}
		}

    }	

	return CDialog::PreTranslateMessage(pMsg);
}

void CXPAntiSpyDlg::setMode(CXPAntiSpySettingsCtrl::MODES mode)
{
	CMenu* pSubMenu=GetMenu()->GetSubMenu(2);
	if(mode == CXPAntiSpySettingsCtrl::NORMAL)
	{
		pSubMenu->CheckMenuRadioItem(ID_MODE_STANDARD, ID_MODE_ACCESSIBLE, ID_MODE_STANDARD, MF_BYCOMMAND);
	}
	else if(mode == CXPAntiSpySettingsCtrl::ACCESSIBLE)
	{
		pSubMenu->CheckMenuRadioItem(ID_MODE_STANDARD, ID_MODE_ACCESSIBLE, ID_MODE_ACCESSIBLE, MF_BYCOMMAND);
		::SetFocus(m_TreeCtrl.m_hWnd);
	}

	m_TreeCtrl.setMode(mode);
	setStartMode(mode);
	m_TreeCtrl.Invalidate(TRUE);
}

void CXPAntiSpyDlg::showUserDefinedProfileDlg()
{
	ProfileDlg dlg(GetSettings());
	dlg.DoModal();
	UpdateProfiles();
}

void CXPAntiSpyDlg::SwitchFocus(bool forward)
{
	TRACE1("forward %d", forward);
	HWND hWnd = ::GetFocus();
	if(m_TreeCtrl.m_hWnd == hWnd)
	{
		if(forward)
		{
			ChangeFocusTo(m_HelpBox.m_hWnd);
			TRACE0("setfocus helpbox\n");
		}
		else
		{
			if(::IsWindowEnabled(m_BtnFwd.m_hWnd))
			{
				ChangeFocusTo(m_BtnFwd.m_hWnd);
			TRACE0("setfocus btn\n");
			}
			else
			{
				ChangeFocusTo(m_HelpBox.m_hWnd);
			TRACE0("setfocus helpbox\n");
			}
		}
	}
	else if(m_HelpBox.m_hWnd == hWnd)
	{
		if(forward)
		{
			if(::IsWindowEnabled(m_BtnFwd.m_hWnd))
			{
				ChangeFocusTo(m_BtnFwd.m_hWnd);
			TRACE0("setfocus btn\n");
			}
			else
			{
			ChangeFocusTo(m_TreeCtrl.m_hWnd);
			TRACE0("setfocus tree\n");
			}

		}
		else
		{
			TRACE0("setfocus tree\n");
			ChangeFocusTo(m_TreeCtrl.m_hWnd);
		}
	}
	else if(m_BtnFwd.m_hWnd == hWnd)
	{
		if(forward)
		{
			ChangeFocusTo(m_TreeCtrl.m_hWnd);
			TRACE0("setfocus tree\n");
		}
		else
		{
			//ChangeFocusTo(m_TreeCtrl.m_hWnd);
			ChangeFocusTo(m_HelpBox.m_hWnd);
			TRACE0("setfocus helpbox\n");
		}
	}
	else
	{
		ChangeFocusTo(m_TreeCtrl.m_hWnd);
		TRACE0("setfocus tree\n");
	}
}

TreeItem* CXPAntiSpyDlg::GetItemArray()
{
	return &m_entryItem[0];
}

void CXPAntiSpyDlg::ClearHelpText()
{
	m_HelpBox.ClearHelpText();
}

CXPAntiSpySettingsCtrl::MODES CXPAntiSpyDlg::getStartMode()
{
	CRegKey regKey;
	regKey.Open(HKEY_CURRENT_USER, ProfileDlg::REGISTRY_LOC, KEY_READ);
	DWORD cnt = MAX_PATH;
	TCHAR tmp[MAX_PATH];
	regKey.QueryValue(tmp, _T("startup_mode"), &cnt);
	CString retVal(tmp);
	int mode = _wtoi(retVal);
	if(mode == 0)
	{
		return CXPAntiSpySettingsCtrl::NORMAL;
	}
	else if(mode == 1)
	{
		return CXPAntiSpySettingsCtrl::ACCESSIBLE;
	}
	else return CXPAntiSpySettingsCtrl::NORMAL;
}

void CXPAntiSpyDlg::setStartMode(CXPAntiSpySettingsCtrl::MODES mode)
{
	//if the regkey doesn't exist, create it
	CRegKey tmp;
	if(tmp.Open(HKEY_CURRENT_USER, ProfileDlg::REGISTRY_LOC, KEY_READ) != ERROR_SUCCESS)
	{
		tmp.Create(HKEY_CURRENT_USER, ProfileDlg::REGISTRY_LOC, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_WRITE);
	}
	tmp.Close();
	CRegKey regKey;
	if(regKey.Open(HKEY_CURRENT_USER, ProfileDlg::REGISTRY_LOC, KEY_WRITE) == ERROR_SUCCESS)
	{
		if(mode == CXPAntiSpySettingsCtrl::ACCESSIBLE)
		{
			regKey.SetValue(_T("1"), _T("startup_mode"));
		}
		else if(mode == CXPAntiSpySettingsCtrl::NORMAL)
		{
			regKey.SetValue(_T("0"), _T("startup_mode"));
		}
		regKey.Close();
	}
}


void CXPAntiSpyDlg::ChangeFocusTo(HWND hWnd)
{
	::SendMessage(this->m_hWnd, WM_NEXTDLGCTL, (WPARAM)hWnd, TRUE);
}

void CXPAntiSpyDlg::AlertGroup(TreeItem *treeItem)
{
	GroupItem* pItem = treeItem->pGroupItem;
	pItem->alerted = true;
	return;
}

bool CXPAntiSpyDlg::IsGroupAlerted(HTREEITEM hItem)
{
	for(int i = 0; i < GROUPCOUNT; ++i)
	{
		if(m_hGroupItem[i].hItem == hItem)
		{
			if(m_hGroupItem[i].alerted)
				return true;
			else
				return false;
		}
	}
	return false;
}

void CXPAntiSpyDlg::ResetAlerts()
{
	for(int i=0; i<GROUPCOUNT; ++i)
	{
		m_hGroupItem[i].alerted = false;
	}
}
