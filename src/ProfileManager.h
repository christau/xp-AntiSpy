#ifndef PROFILEMANAGER_H
#define PROFILEMANAGER_H

#include "GlobalConstants.h"
#include "AntiSpySettings.h"

class ProfileManager
{
public:
	typedef std::vector<CString> StringVec;
    enum PROFILE_STATE
    {
        PROFILE_OK, //settings current state is ok to the current selected profile
        PROFILE_NOK,//settings current state is not ok to the current selected profile
        PROFILE_NEUTRAL //settings current state doesn't matter to the current selected profile
    };

    enum PRESET_PROFILE
    {
        PROFILE_NORMAL, PROFILE_SUGGESTED, PROFILE_WINDOWS_UPDATE, PROFILE_SYSTEM_DEFAULT
    };

    void ActivateProfile(CString profile);
    void ActivatePresetProfile(PRESET_PROFILE preset);
    StringVec GetProfiles();
    PROFILE_STATE GetProfileState(SETTINGS settingID);
    void DeleteProfile(CString profileName);

    void setAntiSpySettings(CAntiSpySettings* settings);

    void ExportProfile(CString profileName, CString fileName);

private:
    CAntiSpySettings* m_antiSpySettings;
};
#endif //PROFILEMANAGER_H
