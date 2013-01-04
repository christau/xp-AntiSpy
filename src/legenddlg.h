#if !defined(AFX_LEGENDDLG_H__B5652F75_3E1B_4EF5_B62C_F7F9342AB3A5__INCLUDED_)
#define AFX_LEGENDDLG_H__B5652F75_3E1B_4EF5_B62C_F7F9342AB3A5__INCLUDED_

#include "resource.h"
#include "coloredit.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// legenddlg.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld LegendDlg 

class LegendDlg : public CDialog
{
// Konstruktion
public:
	LegendDlg(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(LegendDlg)
	enum { IDD = IDD_LEGEND };
	ColorEdit	m_unckeckedWrongInfo;
	ColorEdit	m_unckeckedRightInfo;
	ColorEdit	m_unckeckedChgInfo;
	ColorEdit	m_unckeckedInfo;
	ColorEdit	m_lockedInfo;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(LegendDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
private:
	void CalcRect(CRect &rect, ColorEdit &edit, int xOffs, int right);

protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(LegendDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_LEGENDDLG_H__B5652F75_3E1B_4EF5_B62C_F7F9342AB3A5__INCLUDED_
