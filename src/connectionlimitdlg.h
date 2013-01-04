#if !defined(AFX_CONNECTIONLIMITDLG_H__BF97925E_B639_4138_8E35_825E6DD33E5A__INCLUDED_)
#define AFX_CONNECTIONLIMITDLG_H__BF97925E_B639_4138_8E35_825E6DD33E5A__INCLUDED_

#include "HoverButton.h"
#include "ConnectionLimitNG.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CConnectionLimitDlg : public CDialog
{
// Konstruktion
public:
	CConnectionLimitDlg(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CConnectionLimitDlg)
	enum { IDD = IDD_TCPIP_SYS_DLG };
	CHoverButton	m_btnBackupRestore;
	CHoverButton	m_btnBackupClear;
	CHoverButton	m_btnCreateBackup;
	CListBox	m_lstBackup;
	CStatic	m_txtMaxConns;
	CStatic	m_txtOn;
	CStatic	m_grpInfo;
	CStatic	m_txtConn;
	CHoverButton	m_btnCancel;
	CStatic	m_txtPatched;
	CStatic	m_txtBuild;
	CHoverButton	m_btnReset;
	CHoverButton	m_btnOk;
	CComboBox	m_cboConnections;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CConnectionLimitDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CConnectionLimitDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeBackupList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void OnBtnReset();
	void OnBtnPatch();
	void checkFile();
	void initBackupList();
	void OnBtnCreateBackup();
	void OnBackupClear();
	void OnBackupRestore();
	CRichEditCtrl m_warning;
	CHoverButton m_warningOk;
	ConnectionLimit m_conns;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_CONNECTIONLIMITDLG_H__BF97925E_B639_4138_8E35_825E6DD33E5A__INCLUDED_
