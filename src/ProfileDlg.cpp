// ProfileDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "resource.h"
#include "ProfileDlg.h"

#include <atlbase.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

LPCTSTR ProfileDlg::PROFILES_LOC = _T("Software\\xp-AntiSpy\\Profiles");
LPCTSTR ProfileDlg::REGISTRY_LOC = _T("Software\\xp-AntiSpy");

ProfileDlg::ProfileDlg(SettingVec settings, CWnd* pParent /*=NULL*/)
	: CDialog(ProfileDlg::IDD, pParent), m_settings(settings)
{
	//{{AFX_DATA_INIT(ProfileDlg)
	//}}AFX_DATA_INIT
}


void ProfileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ProfileDlg)
	DDX_Control(pDX, IDC_IMPORT_PROFILE, m_btnImport);
	DDX_Control(pDX, IDC_EXPORT_PROFILE, m_btnExport);
	DDX_Control(pDX, IDC_CREATE_PROFILE, m_btnCreateProfile);
	DDX_Control(pDX, IDC_DEL_ALL_PROFILES, m_btnDelAllProfiles);
	DDX_Control(pDX, IDC_DEL_PROFILE, m_btnDelProfile);
	DDX_Control(pDX, IDC_COMBO_PROFILES, m_profiles);
	DDX_Control(pDX, IDC_EDIT_NAME, m_txtName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ProfileDlg, CDialog)
	//{{AFX_MSG_MAP(ProfileDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void ProfileDlg::OnCreateProfile() 
{
	CString name;
	m_txtName.GetWindowText(name);
	name.TrimLeft(_T(" "));
	name.TrimRight(_T(" "));
	if(name == _T("")) 
		return;
	CreateProfile(name, m_settings);
	UpdateProfilesCombo();
}

BOOL ProfileDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_btnDelProfile.setHasBorder(true);
	m_btnExport.setHasBorder(true);
	m_btnDelAllProfiles.setHasBorder(true);
	m_btnCreateProfile.setHasBorder(true);
	m_btnImport.setHasBorder(true);
	UpdateProfilesCombo();	
	return TRUE;
}

ProfileDlg::StringVec ProfileDlg::GetExistingProfiles()
{
	StringVec profiles;
	HKEY hKey;
	if (::RegOpenKeyEx(HKEY_CURRENT_USER, PROFILES_LOC, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
		return profiles;
	
	DWORD dwIndex = 0;
	LONG lRet;
	DWORD len = 50;
	TCHAR profileName[50];

	while ((lRet = ::RegEnumKeyEx(hKey, dwIndex, profileName, &len, NULL,
	NULL, NULL, NULL)) != ERROR_NO_MORE_ITEMS)
	{
		// Do we have a key to open?
		if (lRet == ERROR_SUCCESS)
		{
			// Open the key and get the value
			HKEY hItem;
			if (::RegOpenKeyEx(hKey, profileName, 0, KEY_READ, &hItem) != ERROR_SUCCESS)
				continue;
			profiles.push_back(profileName);
			::RegCloseKey(hItem);
		}
		dwIndex++;
		len = 50;
	}
	::RegCloseKey(hKey);
	return profiles;
}

void ProfileDlg::DeleteProfile(CString profileName)
{
	CRegKey regKey;
	regKey.Open(HKEY_CURRENT_USER,PROFILES_LOC,KEY_READ);
	regKey.RecurseDeleteKey(profileName);
	regKey.Close();
}

void ProfileDlg::OnDelProfile() 
{
	CString selection;
	int iSel = m_profiles.GetCurSel();
	if(iSel==-1) return;
	m_profiles.GetLBText(iSel,selection);
	if(!selection.IsEmpty())
	{
		CString tmp;
		tmp.LoadString(IDS_PROFILE_DELETE);
		if(MessageBox(tmp, NULL, MB_YESNO) == IDNO)
			return;
		DeleteProfile(selection);
		UpdateProfilesCombo();
	}
}

void ProfileDlg::UpdateProfilesCombo()
{
	m_profiles.ResetContent();
	StringVec vec = GetExistingProfiles();
	for(int i=0; i<vec.size(); ++i)
	{
		m_profiles.InsertString(0,vec[i]);
	}

}

void ProfileDlg::OnDelAllProfiles() 
{
	CString tmp;
	tmp.LoadString(IDS_PROFILE_DELETE_ALL);
	if(MessageBox(tmp, NULL, MB_YESNO) == IDNO)
		return;
	for(int i=0; i<m_profiles.GetCount(); ++i)
	{
		CString selection;
		m_profiles.GetLBText(i,selection);
		if(!selection.IsEmpty())
		{
			DeleteProfile(selection);
		}
	}
	UpdateProfilesCombo();
}

void ProfileDlg::CreateProfile(CString name, SettingVec settings)
{
	CRegKey tmp;
	if(tmp.Open(HKEY_CURRENT_USER, REGISTRY_LOC, KEY_READ) != ERROR_SUCCESS)
	{
		tmp.Create(HKEY_CURRENT_USER, REGISTRY_LOC, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_WRITE);
	}
	tmp.Close();
	if(tmp.Open(HKEY_CURRENT_USER, PROFILES_LOC, KEY_READ) != ERROR_SUCCESS)
	{
		tmp.Create(HKEY_CURRENT_USER, PROFILES_LOC, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_WRITE);
	}
	tmp.Close();
	
	CString profileName = PROFILES_LOC + CString(_T("\\")) + name;
	CRegKey regKey;
	if(regKey.Create(HKEY_CURRENT_USER,profileName, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_WRITE)== ERROR_SUCCESS)
	{
		for(int i=0; i<settings.size(); ++i)
		{
			CString name;
			name.Format(_T("%d"), settings[i].settingID);
			regKey.SetValue((settings[i].checked)?1:0, name);
		}
		regKey.Close();
	}

	
	UpdateProfilesCombo();
}

bool ProfileDlg::SetAutoCheckProfile(bool autoStart)
{
	CString str = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run");
	CRegKey regKey;
	if(regKey.Open(HKEY_CURRENT_USER, str, KEY_SET_VALUE) == ERROR_SUCCESS)
	{
		if(autoStart)
		{
			//get the path of our program
			CString name = AfxGetApp()->m_pszExeName;
     
			name +=_T(".exe");
			HMODULE hmod = GetModuleHandle(name);
     
			CString path;
			DWORD pathLen = ::GetModuleFileName( hmod, path.GetBufferSetLength(MAX_PATH+1), MAX_PATH); 
			path.ReleaseBuffer( pathLen );

			regKey.SetValue(path + _T(" /check"), _T("xp-AntiSpy Profile Check"));
		}
		else
			regKey.DeleteValue(_T("xp-AntiSpy Profile Check"));
		regKey.Close();
		return true;
	}
	return false;
}

bool ProfileDlg::IsAutoCheckProfile()
{
	CString str = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run");
	CRegKey regKey;
	if(regKey.Open(HKEY_CURRENT_USER, str, KEY_READ) == ERROR_SUCCESS)
	{
		DWORD cnt = MAX_PATH;
		TCHAR tmp[MAX_PATH];
		long ret = regKey.QueryValue(tmp, _T("xp-AntiSpy Profile Check"), &cnt);
		if(ret == ERROR_SUCCESS && cnt > 0)
		{
			return true;
		}
		
		regKey.Close();
	}
	return false;
}

SettingVec ProfileDlg::GetProfile(const int off[], const int on[])
{
	SettingVec vec;
	int i=0;
	while(ST_END_ENTRY!=on[i])
	{
		SETTING set;
		set.settingID = on[i];
		set.checked = true;
		vec.push_back(set);
		++i;
	}

	i=0;
	while(ST_END_ENTRY!=off[i])
	{
		SETTING set;
		set.settingID = off[i];
		set.checked = false;
		vec.push_back(set);
		++i;
	}

	return vec;
}

SettingVec ProfileDlg::GetProfile(CString name)
{
	SettingVec vec;
	CRegKey regKey;
	CString loc;
	HKEY hKey;
	loc.Format(_T("%s\\%s"), PROFILES_LOC, name);
	if(RegOpenKeyEx(HKEY_CURRENT_USER, loc, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
	{
		DWORD dwIndex = 0;
		DWORD len = MAX_PATH;
		TCHAR valName[MAX_PATH];
		LPTSTR value[50];
		DWORD size = sizeof(value);
		DWORD type = REG_DWORD;
		//strcpy(profileName,loc);
		while(::RegEnumValue(hKey, dwIndex, valName, &len, 0, &type,(LPBYTE)value, &size) != ERROR_NO_MORE_ITEMS)
		{
			SETTING set;
			if(value[0] == 0)
				set.checked = false;
			else
				set.checked = true;
			set.settingID = _wtoi(valName);
			vec.push_back(set);

			dwIndex++;
			len = MAX_PATH;
		}
				::RegCloseKey(hKey);
	}

	return vec;
}

void ProfileDlg::OnExportProfile() 
{
	CString name;
	int sel = m_profiles.GetCurSel();
	if(sel<0) return;
	
	CString tmp;
	tmp.LoadString(IDS_PROFILE_FILEDIALOG);
	CFileDialog fd( FALSE, _T("xpas"), name, OFN_ENABLESIZING, tmp + " (*.xpas)|*.xpas||", this);
	tmp.LoadString(IDS_PROFILE_SAVE);
	fd.m_ofn.lpstrTitle = tmp;
	if(fd.DoModal() == IDOK)
	{
		CFile file(fd.GetPathName(), CFile::modeCreate|CFile::modeWrite);
		m_profiles.GetLBText(sel, name);
		SettingVec vec = GetProfile(name);
		
		CArchive* ar=new CArchive(&file,CArchive::store);
		for(int i=0; i<vec.size(); ++i)
		{
			
			*ar<<vec[i].settingID<<vec[i].checked;
		}
		ar->Close();
		if(ar) delete ar;
		file.Close();
	}
	
}

void ProfileDlg::OnImportProfile() 
{
	CString name;
	CString tmp;
	tmp.LoadString(IDS_PROFILE_FILEDIALOG);
	CFileDialog fd( TRUE, _T("prf"), NULL, OFN_ENABLESIZING|OFN_FILEMUSTEXIST, tmp + _T(" (*.xpas)|*.xpas||"), this);
	tmp.LoadString(IDS_PROFILE_LOAD);
	fd.m_ofn.lpstrTitle = tmp;
	if(fd.DoModal() == IDOK)
	{
		StringVec profiles = GetExistingProfiles();
		for(int i = 0; i < profiles.size(); ++i)
		{
			profiles[i].MakeLower();
			fd.GetFileName().MakeLower();
			if(profiles[i] == fd.GetFileTitle())
			{
				CString tmp;
				tmp.LoadString(IDS_PROFILE_EXISTS);
				if(MessageBox(tmp, NULL, MB_YESNO) == IDNO)
				{
					return;
				}
				else
				{
					DeleteProfile(fd.GetFileTitle());
				}
			}
		}

		CFile file(fd.GetPathName(), CFile::modeRead);
		SettingVec vec;
		
		CArchive* ar=new CArchive(&file,CArchive::load);
		try
		{
			while(1)
			{
				SETTING set;
				*ar>>set.settingID;
				*ar>>set.checked;
				if((set.settingID <= ITEMCOUNT && set.settingID >= 0)
					|| (set.checked >= 0 && set.checked <= 1))
				{
					vec.push_back(set);
				}
			}
		}
		catch(CArchiveException* e)
		{
			e->Delete();
			if(vec.size() > 0)
				CreateProfile(fd.GetFileTitle(),vec);
			else
			{
				CString tmp;
				tmp.LoadString(IDS_PROFILE_ERROR);
				AfxMessageBox(tmp);
			}
		}
		ar->Close();
		if(ar) delete ar;
		file.Close();
	}
	UpdateProfilesCombo();
}

BOOL ProfileDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if((HWND)lParam == m_btnCreateProfile.m_hWnd)
	{
		OnCreateProfile();
	}
	if((HWND)lParam == m_btnDelProfile.m_hWnd)
	{
		OnDelProfile();
	}
	if((HWND)lParam == m_btnDelAllProfiles.m_hWnd)
	{
		OnDelAllProfiles();
	}
	if((HWND)lParam == m_btnExport.m_hWnd)
	{
		OnExportProfile();
	}
	if((HWND)lParam == m_btnImport.m_hWnd)
	{
		OnImportProfile();
	}

	return CDialog::OnCommand(wParam, lParam);
}

void ProfileDlg::OnOK()
{

}

void ProfileDlg::SetStartProfile(const int menuID)
{
	//if the regkey doesn't exist, create it
	CRegKey tmp;
	if(tmp.Open(HKEY_CURRENT_USER, REGISTRY_LOC, KEY_READ) != ERROR_SUCCESS)
	{
		tmp.Create(HKEY_CURRENT_USER, REGISTRY_LOC, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_WRITE);
	}
	tmp.Close();
	//CString profileName = REGISTRY_LOC + CString("\\") + "startup_profile";
	CRegKey regKey;
	if(regKey.Open(HKEY_CURRENT_USER, REGISTRY_LOC, KEY_WRITE) == ERROR_SUCCESS)
	{
		CString tmp;
		tmp.Format(_T("%d"),menuID);
		regKey.SetValue(tmp, _T("startup_profile"));
		regKey.Close();
	}
}
int ProfileDlg::GetStartProfile()
{
	CRegKey regKey;
	regKey.Open(HKEY_CURRENT_USER, REGISTRY_LOC, KEY_READ);
	DWORD cnt = MAX_PATH;
	TCHAR tmp[MAX_PATH];
	regKey.QueryValue(tmp, _T("startup_profile"), &cnt);
	CString retVal(tmp);
	
	return _wtoi(retVal);
}
