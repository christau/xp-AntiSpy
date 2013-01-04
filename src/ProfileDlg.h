#if !defined(AFX_PROFILEDLG_H__EA07CC6D_6B18_4C61_8BF9_1A3176B19D70__INCLUDED_)
#define AFX_PROFILEDLG_H__EA07CC6D_6B18_4C61_8BF9_1A3176B19D70__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "GlobalConstants.h"
#include "HoverButton.h"
/////////////////////////////////////////////////////////////////////////////
class ProfileDlg : public CDialog
{
public:
	static void SetStartProfile(const int menuID);
	static bool ProfileDlg::IsAutoCheckProfile();

	static int GetStartProfile();
	static bool SetAutoCheckProfile(bool autoStart);
	virtual void OnOK();
	static SettingVec GetProfile(CString name);
	/**
	* Gets one of the internal profiles
	*/
	static SettingVec GetProfile(const int off[], const int on[]);
	typedef std::vector<CString> StringVec;
	ProfileDlg(SettingVec settings, CWnd* pParent = NULL);   // Standardkonstruktor
	static StringVec GetExistingProfiles();
	static LPCTSTR REGISTRY_LOC;
	static LPCTSTR PROFILES_LOC;

private:
	void OnCreateProfile();
	void OnDelProfile();
	void OnDelAllProfiles();
	void OnExportProfile();
	void OnImportProfile();
	void CreateProfile(CString name, SettingVec settings);
	void UpdateProfilesCombo();
	void DeleteProfile(CString profileName);
private:
	SettingVec m_settings;
	//{{AFX_DATA(ProfileDlg)
	enum { IDD = IDD_USERDEFINED_PROFILES };
	CHoverButton	m_btnImport;
	CHoverButton	m_btnExport;
	CHoverButton	m_btnCreateProfile;
	CHoverButton	m_btnDelAllProfiles;
	CHoverButton	m_btnDelProfile;
	CComboBox	m_profiles;
	CEdit	m_txtName;
	//}}AFX_DATA

protected:
	//{{AFX_VIRTUAL(ProfileDlg)
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(ProfileDlg)
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}

#endif // AFX_PROFILEDLG_H__EA07CC6D_6B18_4C61_8BF9_1A3176B19D70__INCLUDED_
