// xpAntiSpy.h : Haupt-Header-Datei für die Anwendung xpAntiSpy
//

#if !defined(AFX_xpAntiSpy_H__6138FF4B_3A86_4733_9A55_AAA6EA7FF7BC__INCLUDED_)
#define AFX_xpAntiSpy_H__6138FF4B_3A86_4733_9A55_AAA6EA7FF7BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// Hauptsymbole

/////////////////////////////////////////////////////////////////////////////
// CXPAntiSpyApp:
// Siehe xpAntiSpy.cpp für die Implementierung dieser Klasse
//
/////////////////////////////////////////////////////////////////////////////
class CXPAntiSpyApp : public CWinApp
{
public:
	CXPAntiSpyApp();

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CXPAntiSpyApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementierung

	//{{AFX_MSG(CXPAntiSpyApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_xpAntiSpy_H__6138FF4B_3A86_4733_9A55_AAA6EA7FF7BC__INCLUDED_)
