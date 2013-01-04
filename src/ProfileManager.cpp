#include "ProfileManager.h"
#include "AntiSpySettings.h"
#include <atlbase.h>

LPCTSTR PROFILES_LOCATION = "Software\\xp-AntiSpy\\Profiles";
LPCTSTR REGISTRY_LOCATION = "Software\\xp-AntiSpy";

void ProfileManager::ActivateProfile(CString profile)
{

}

ProfileManager::StringVec ProfileManager::GetProfiles()
{
	StringVec profiles;
	HKEY hKey;
	if (::RegOpenKeyEx(HKEY_CURRENT_USER, PROFILES_LOCATION, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
		return profiles;
	
	DWORD dwIndex = 0;
	LONG lRet;
	DWORD len = 50;
	char profileName[50];

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

void ProfileManager::DeleteProfile(CString profileName)
{
	CRegKey regKey;
	regKey.Open(HKEY_CURRENT_USER,PROFILES_LOCATION,KEY_READ);
	regKey.RecurseDeleteKey(profileName);
	regKey.Close();
}
void ProfileManager::setAntiSpySettings(CAntiSpySettings* settings)
{
	m_antiSpySettings = settings;
}

