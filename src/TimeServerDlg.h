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
    along with xp-AntiSpy. If not, see <http://www.gnu.org/licenses/
*/
#if !defined(AFX_TIMESERVERDLG_H__BBF253F6_F6E9_4F57_8C35_1142BDB1103F__INCLUDED_)
#define AFX_TIMESERVERDLG_H__BBF253F6_F6E9_4F57_8C35_1142BDB1103F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TimeServerDlg.h : Header-Datei
//

#include "HoverButton.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CTimeServerDlg 

class CTimeServerDlg : public CDialog
{
// Konstruktion
public:
	CTimeServerDlg(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CTimeServerDlg)
	enum { IDD = IDD_TIMESERVERDLG_DIALOG };
	CHoverButton	m_btnOk;
	CHoverButton	m_NewTimeServerApply;
	CEdit	m_TimeServerEdit;
	CHoverButton	m_TimeServerApply;
	CComboBox	m_TimeServerCombo;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CTimeServerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CTimeServerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeTimeServerCombo();
	afx_msg void OnChangeTimeServerEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void TimeServerApply();
	void NewTimeServerApply();
	void EnumTimeServers();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_TIMESERVERDLG_H__BBF253F6_F6E9_4F57_8C35_1142BDB1103F__INCLUDED_
