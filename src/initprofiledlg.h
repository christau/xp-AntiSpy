#if !defined(AFX_INITPROFILEDLG_H__3AAC392C_D2DB_4F8E_B2B2_C42015CE37C1__INCLUDED_)
#define AFX_INITPROFILEDLG_H__3AAC392C_D2DB_4F8E_B2B2_C42015CE37C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// initprofiledlg.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CinitProfileDlg 

class CinitProfileDlg : public CDialog
{
// Konstruktion
public:
	CinitProfileDlg(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CinitProfileDlg)
	enum { IDD = IDD_INIT_PROFILE };
	CButton	m_btnNever;
	CButton	m_btnLater;
	CButton	m_btnOk;
	CEdit	m_dlgText;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CinitProfileDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CinitProfileDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnLater();
	afx_msg void OnBtnNever();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_INITPROFILEDLG_H__3AAC392C_D2DB_4F8E_B2B2_C42015CE37C1__INCLUDED_
