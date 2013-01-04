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

#if !defined(AFX_CMDLINEDLG_H__061A787D_5BE3_4E56_9E27_7E858FFF1AD5__INCLUDED_)
#define AFX_CMDLINEDLG_H__061A787D_5BE3_4E56_9E27_7E858FFF1AD5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HoverButton.h"
#include "GlobalConstants.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CCMDLineDlg 

class CCMDLineDlg : public CDialog
{
// Konstruktion
public:
	CCMDLineDlg(CWnd* pParent = NULL);

	//{{AFX_DATA(CCMDLineDlg)
	enum { IDD = IDD_CMDLINE_DLG };
	CListBox	m_lstCommands;
	//}}AFX_DATA


	//{{AFX_VIRTUAL(CCMDLineDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(CCMDLineDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CHoverButton	m_btnCopy;
	CString getString(SETTINGS set, UINT nID);
	void initListCtrl();
	void CopyToClipboard(const char* stringbuffer);
};

//{{AFX_INSERT_LOCATION}}

#endif // AFX_CMDLINEDLG_H__061A787D_5BE3_4E56_9E27_7E858FFF1AD5__INCLUDED_
