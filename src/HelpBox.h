#if !defined(AFX_HELPBOX_H__5CBA66BD_6D6A_4DB1_AACB_08A1CFCF74BA__INCLUDED_)
#define AFX_HELPBOX_H__5CBA66BD_6D6A_4DB1_AACB_08A1CFCF74BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HelpBox.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Fenster CHelpBox 

class CHelpBox : public CRichEditCtrl
{
// Konstruktion
public:
	CHelpBox();

// Attribute
public:

// Operationen

// Implementierung
public:
	void ClearHelpText();
	void SetHelpText(UINT ,bool);
	virtual ~CHelpBox();

	// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CHelpBox)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL


	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	//{{AFX_MSG(CHelpBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	CButton m_btn;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_HELPBOX_H__5CBA66BD_6D6A_4DB1_AACB_08A1CFCF74BA__INCLUDED_
