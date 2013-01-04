#if !defined(AFX_CHECKPROFILEDLG_H__90AB5696_7B96_42FC_8AE2_6394F7A39FD3__INCLUDED_)
#define AFX_CHECKPROFILEDLG_H__90AB5696_7B96_42FC_8AE2_6394F7A39FD3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// checkprofiledlg.h : Header-Datei
//
#include "HoverButton.h"
#include "GlobalConstants.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CCheckProfileDlg 

class CCheckProfileDlg : public CDialog
{
// Konstruktion
public:
	CCheckProfileDlg(CString profileName, SettingVec settings, bool needsAdmin, CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CCheckProfileDlg)
	enum { IDD = IDD_DLG_CHECK_PROFILE };
	CHoverButton	m_btnReApply;
	CHoverButton	m_btnIgnore;
	CStatic	m_txtInfo;
	CListBox	m_lstSettings;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CCheckProfileDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CCheckProfileDlg)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CString m_profileName;
	SettingVec m_settings;
	bool m_needsAdmin;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_CHECKPROFILEDLG_H__90AB5696_7B96_42FC_8AE2_6394F7A39FD3__INCLUDED_
