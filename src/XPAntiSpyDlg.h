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


#if !defined(AFX_XPANTISPYDLG_H__AFC348BF_1F11_448B_A5EE_116CEDC7ACDF__INCLUDED_)
#define AFX_XPANTISPYDLG_H__AFC348BF_1F11_448B_A5EE_116CEDC7ACDF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HelpBox.h"
#include "AntiSpySettings.h"
#include "ProfileManager.h"
#include "GlobalConstants.h"
#include "About.h"
#include "SimpleSplitter.h"
#include "XPAntiSpySettingsCtrl.h"
#include "legenddlg.h"
#include "ExEdit.h"
#include "HoverButton.h"



class GroupItem
{
public:
		HTREEITEM hItem;//handle to the group
		bool		alerted;
		std::wstring text; //the displayed string;
		bool enabled;
		GroupItem(): hItem(0), alerted(false), text(_T("")), enabled(true)
		{}
};

class TreeItem
{
public:
		HTREEITEM treeItem; //handle to the entry
		GroupItem *pGroupItem;
		bool		enabled;	//is item for the actual session enabled?
		bool	restricted; //is this item not changeable due to restriced user rights
		std::wstring text; //the displayed string;
		TreeItem(): treeItem(0), pGroupItem(0), restricted(false), text(_T(""))
		{}
};

/////////////////////////////////////////////////////////////////////////////
// CXPAntiSpyDlg Dialogfeld

class CXPAntiSpyDlg : public CDialog
{
// Konstruktion
public:

	SettingVec CXPAntiSpyDlg::GetSettings();
	/**
	 * Toggles the check state of the setting
	 *
	 * Returns true if the state could be toggled, false if not
	 * The state can't be toggled if it's already suggested by the profile
	 */
	bool toggleItem(const HTREEITEM& hItem);
	virtual void OnOK();
	/**
	 * Sets the state of the apply button
	 *
	 * @param state TRUE enables the button
	 * @return void 
	 */
	void SetApplyButtonState(bool state);
	/**
	 * Shows the help text in the help box to the specified treeitem
	 *
	 * @param hItem 
	 * @return void 
	 */
	void TrackHelpText(HTREEITEM hItem);
	/**
	 * Pops up the context menu to the treeitem at the given point
	 *
	 * @param point the point where to pop up the menu
	 * @param hItem the treeitem to display the context menu about
	 * @return void 
	 */
	void PopUpMenu(CPoint point, HTREEITEM hItem);

	/**
	 * Constructs an instance of this Dialog
	 *
	 * @param pParent parent window if exist
	 * @return  
	 */
	CXPAntiSpyDlg(CWnd* pParent = NULL);
	~CXPAntiSpyDlg();

	TreeItem* GetItemArray();

public:	
	static void ReRunApp(HWND hwnd, LPCTSTR args, bool asAdmin);
	void ResetAlerts();
	bool IsGroupAlerted(HTREEITEM hItem);
	void ClearHelpText();
	CMenu menu;
	CMenu m_hPopUp;
	CString m_CommandLine; //the commandline passed to the app
	//{{AFX_DATA(CXPAntiSpyDlg)
	enum { IDD = IDD_xpAntiSpy_DIALOG };
	CComboBox	m_combo;
	//}}AFX_DATA

private:
	/**
	 * Returns what the selected profile suggests for this setting
	 *  PROFILE_SUGGESTS_ON - setting should be enabled
	 *  PROFILE_SUGGESTS_NOTHING - setting should not be altered
	 *  PROFILE_SUGGESTS_OFF - settings should be disabled
	 **/
	int GetProfileSuggestion(SETTINGS item);
	void CheckProfile(const int idOn[], const int idOff[], const int idMenu);
	/**
	 * Sets a user defined profile as the actual profile
	 *
	 * @param name the name of the user defined profile
	 * @return void 
	 */
	void SetProfile(CString name);
	void UpdateProfiles();
	/**
	 * Sets a predefined profile as the the actual profile
	 *
	 * @param isOn determines if the activated or deactivated settings are to be set
	 * @param id the array of the settings.
	 * @return void 
	 */
	void SetProfile(BOOL isOn,const int id[] = 0);

	int TreeItemFromHItem(HTREEITEM item);
	
	void CheckForSpyFunctions();
	/**
	 * Checks if the support user in the system exists
	 *
	 * @return bool True if it exists
	 */
	bool IsSupportuser();
	bool GetSuiteType(OS_INFO&);
	void InitTreeView();
	/**
	 * Returns the version of the installed mediaplayer as int (7,8,9,10,...)
	 *
	 * @return unsigned int the version number
	 */
	unsigned int GetMediaPlayerVersion();

	void checkState(TreeItem &item, SETTING_STATE state);
	
	/**
	 * Check if this app is run for the first time. 
	 * In this case open up a dialog asking the user to create a
	 * profile from the current settings
	 */
	void checkForInitialProfile();

	void FilterTreeControl(CString searchText);
	/**
	*Resets the filtering of the tree and clears the search text field
	*/
	void ResetFiltering();
	void enableAutoStartMenu(bool enable);
private:
	void toggleSearchField();
	void restoreWindowSize();
	void storeWindowSize();
	void AlertGroup(TreeItem *treeItem);
	void ChangeFocusTo(HWND hWnd);
	CXPAntiSpySettingsCtrl::MODES getStartMode();
	void setStartMode(CXPAntiSpySettingsCtrl::MODES mode);
	void SwitchFocus(bool forward);
	void showUserDefinedProfileDlg();
	void setMode(CXPAntiSpySettingsCtrl::MODES mode);
	int m_profile_on[ITEMCOUNT];
	int m_profile_off[ITEMCOUNT];
	CXPAntiSpySettingsCtrl	m_TreeCtrl;
	CHoverButton	m_BtnFwd;
	CString infoBoxTxt;
	UINT actLang;
	bool m_mouseSplit;
	CPoint m_mousePoint;
	int m_helpBoxHeight;
	bool m_applyBtnState;
	CSimpleSplitter m_split;
	CToolTipCtrl* tmp;
	TreeItem* m_entryItem;
	GroupItem* m_hGroupItem;
	HTREEITEM m_hSelectedItem; //handle of the actual selcted Item
	CHelpBox m_HelpBox;
    CAntiSpySettings m_AntiSpySettings;
    //ProfileManager m_profileManager;
	int m_userDefinedProfiles;
	LegendDlg* m_pSymbolReference;

	CFont m_txtFont;

	CExEdit m_searchField;
	CStatic m_searchIcon;
	CHoverButton m_btnClearSearch;
	bool m_showSearchControl;
	bool m_isTreeFiltered;

protected:
	//{{AFX_VIRTUAL(CXPAntiSpyDlg)
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL
	
	// Generierte Message-Map-Funktionen
	//{{AFX_MSG(CXPAntiSpyDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnFwd();
	afx_msg void OnMenuAbout();
	afx_msg void OnMenuQuickReference();
	afx_msg void OnSpecialRefresh();
	afx_msg void OnSpecialSetTimeServer();
	afx_msg void OnRunAsAdmin();
	afx_msg void OnMenuVisitHP();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPopUpResetSetting();
	afx_msg void OnPopUpSetSetting();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnConnectionLimit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	HICON m_hIcon;

};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_XPANTISPYDLG_H__AFC348BF_1F11_448B_A5EE_116CEDC7ACDF__INCLUDED_)
