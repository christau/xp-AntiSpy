//////////////////////////////////////////////////////////////////////
// class for the enabling and disabling of the xp-antispy functions
//////////////////////////////////////////////////////////////////////
// class:  CAntiSpySettings
// date:   9.11.2002
// author: etri
//////////////////////////////////////////////////////////////////////
// When using this class, don't forget to include GlobalConstants.h
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_ANTISPYSETTINGS_H__04847B73_279C_46AA_B688_DDDB4989F74C__INCLUDED_)
#define AFX_ANTISPYSETTINGS_H__04847B73_279C_46AA_B688_DDDB4989F74C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// forward declaration
class CServicesControl;
#include <vector>
#include "GlobalConstants.h"

class CAntiSpySettings
{
public:
	enum SUITE_TYPE
	{
		DEPRECATED_WINDOWS,
		WINDOWS_XP_HOME,
		WINDOWS_NT_2000,
		WINDOWS_XP_PROFESSIONAL,
		WINDOWS_VISTA,
		WINDOWS_7,
		UNKNOWN
	};
	enum REGKEY_TEST_RESULT
	{
		MATCH, //means the requested key was found and the value matched
		NO_MATCH, //means the requested key was found but did not match
		NOT_FOUND //means the requested key was not found
	};
	static CString GetRegKeyAsString(const unsigned int id);
	/////////////////////////////////////////////////////////
	// disable setting, so it's not active anymore
	/////////////////////////////////////////////////////////
	const bool DisableSetting(const UINT setting)const;
	/////////////////////////////////////////////////////////
	// reset setting to it's initial state
	/////////////////////////////////////////////////////////
	const bool EnableSetting(const UINT setting)const;
	/////////////////////////////////////////////////////////
	// 0 means, this setting is deactivated
	// 1 means it's active (original system state)
	// 2 means doesn't exist in this system
	/////////////////////////////////////////////////////////
	const SETTING_STATE TestSetting(const UINT setting)const;
	CAntiSpySettings();
	virtual ~CAntiSpySettings();
	
	const bool	IsSupportuser() const;
	const bool	IsWinXP() const;
	const unsigned int CAntiSpySettings::GetIEVersion() const;
	const bool	IsMS_Messenger() const; // test if ms messenger (no matter which version) exists
	const bool	IsMediaPlayer() const; // test if mp exists in the right version
	const bool	IsMediaPlayer9() const; // test if mp exists in the 9.x version
	const bool	IsMediaPlayer11() const; // test if mp exists in the 11.x version
	const bool	IsMediaPlayer12() const; // test if mp exists in the 12.x version
	const int	GetMediaPlayerVersion() const;
	const bool	IsOfficeXpInstalled() const;
	const bool	IsOffice2k3Installed() const;
	const bool	IsOffice2k7Installed() const;
	bool		IsQOSEnabled() const;
	UINT		GetSP() const;
	int			GetSuiteType(CString* versionInfo);
	static SUITE_TYPE	GetSuiteType();
	const unsigned int ScanUserAssist(std::vector<CString>&);
	const bool ServiceExist(LPCTSTR svc) const;
	
	static CString GetRegKeyValue(const HKEY parentKey,LPCTSTR lpszKeyName, LPCTSTR lpszValueName,const DWORD type);
	static CString GetRegKeyValue(const HKEY parentKey,LPCTSTR lpszKeyName, LPCTSTR lpszValueName,const DWORD type, const bool isWow64);
	static DWORD GetDWORDRegKeyValue(const HKEY parentKey,LPCTSTR lpszKeyName, LPCTSTR lpszValueName);
	static DWORD GetDWORDRegKeyValue(const HKEY parentKey,LPCTSTR lpszKeyName, LPCTSTR lpszValueName, const bool isWow64);
	static bool	TestRegKey(const HKEY, LPCTSTR);
	static bool	TestRegKey(const HKEY, LPCTSTR, const bool isWow64);
	void SetRegPermission(CString keyStr);
private:
	static bool	TestKeyWritePermission(const HKEY, LPCTSTR);
	static bool	TestKeyWritePermission(const HKEY, LPCTSTR, const bool isWow64);
	const bool	SetRegKeyValue(const HKEY, LPCTSTR, LPCTSTR, LPCTSTR, const DWORD, const UINT, const DWORD)const;
	const bool	SetRegKeyValue(const HKEY, LPCTSTR, LPCTSTR, LPCTSTR, const DWORD, const UINT, const DWORD, const bool isWow64)const;
	const bool SetDWORDRegKeyValue(const HKEY, LPCTSTR, LPCTSTR, DWORD) const;
	const bool SetDWORDRegKeyValue(const HKEY, LPCTSTR, LPCTSTR, DWORD, const bool isWow64) const;
	const bool SetBINARYRegKeyValue(const HKEY, LPCTSTR, LPCTSTR, TCHAR) const;
	const bool SetBINARYRegKeyValue(const HKEY, LPCTSTR, LPCTSTR, TCHAR, const bool isWow64) const;
	const REGKEY_TEST_RESULT	TestRegKeyValue(const HKEY, LPCTSTR, LPCTSTR, const DWORD, const DWORD, LPCTSTR)const;
	const REGKEY_TEST_RESULT	TestRegKeyValue(const HKEY, LPCTSTR, LPCTSTR, const DWORD, const DWORD, LPCTSTR, const bool isWow64)const;
	const bool	DeleteRegKey(const HKEY, LPCTSTR, LPCTSTR)const;
	const bool	DeleteRegKey(const HKEY, LPCTSTR, LPCTSTR, const bool isWow64)const;
	const bool	RecursiveDeleteRegKey(const HKEY, const CString&)const;
	const bool	RecursiveDeleteRegKey(const HKEY, const CString&, const bool isWow64)const;
	const bool	DeleteRegKeyValue(const HKEY, LPCTSTR, LPCTSTR)const;
	const bool	DeleteRegKeyValue(const HKEY, LPCTSTR, LPCTSTR, const bool isWow64)const;
	const bool	ExistFile(CString file)const;
	void		RegisterDlls(LPCTSTR)const;
	void		UnregisterDlls(LPCTSTR dll)const;
	void		DelMsMsg()const;
	void		DelSupportUser() const;
	const bool	SetBandwidthLimit(bool) const;
	bool		SetUpldDrvInfo(const bool) const;
	const int	GetAdobeReaderVersion() const;
	CServicesControl* m_svcControl;
	CString GetUninstallString(CString startString);
	static bool CreateCTFMonDummy(CString file);
	static bool CreateEmptyIcon(CString file);

	///
	//returns the one of the given key by determining which is writeable.
	///
	const HKEY chooseHKey(const HKEY choice1, HKEY choice2, LPCTSTR key) const;
	const CString getCurrentUserSID() const;
};

#endif // !defined(AFX_ANTISPYSETTINGS_H__04847B73_279C_46AA_B688_DDDB4989F74C__INCLUDED_)
