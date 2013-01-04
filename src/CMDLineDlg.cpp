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

#include "stdafx.h"
#include <iostream>
#include <atlconv.h>
#include "resource.h"
#include "CMDLineDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CCMDLineDlg::CCMDLineDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCMDLineDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCMDLineDlg)
	//}}AFX_DATA_INIT
}


void CCMDLineDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCMDLineDlg)
	DDX_Control(pDX, IDC_LIST1, m_lstCommands);
	//}}AFX_DATA_MAP
}

BOOL CCMDLineDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if((HWND)lParam == m_btnCopy.m_hWnd)
	{
		CString text;
		for(int i=0; i < m_lstCommands.GetCount(); ++i)
		{
			CString tmp;
			m_lstCommands.GetText(i,tmp);
			text += "\n" + tmp;
		}
		USES_CONVERSION;
		CopyToClipboard(W2A( text));

	}

	return CDialog::OnCommand(wParam, lParam);
}



BEGIN_MESSAGE_MAP(CCMDLineDlg, CDialog)
	//{{AFX_MSG_MAP(CCMDLineDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CCMDLineDlg::initListCtrl()
{
	CString cmdStr;
	cmdStr.LoadString(IDS_CMD_LINE_HELP);
	m_lstCommands.AddString(cmdStr);
	m_lstCommands.AddString(_T("---------------------------------"));
	cmdStr.LoadString(IDS_CMD_LINE_SYNTAX);
	m_lstCommands.AddString(cmdStr);
	cmdStr.LoadString(IDS_CMD_LINE_ENABLE);
	m_lstCommands.AddString(cmdStr);
	cmdStr.LoadString(IDS_CMD_LINE_DISABLE);
	m_lstCommands.AddString(cmdStr);
	cmdStr.LoadString(IDS_CMD_LINE_CONNECTION_LIMIT);
	m_lstCommands.AddString(cmdStr);
	cmdStr.LoadString(IDS_CMD_LINE_CONNECTION_LIMIT2);
	m_lstCommands.AddString(cmdStr);
	m_lstCommands.AddString(_T("---------------------------------"));
	cmdStr.LoadString(IDS_CMD_LINE_N);
	m_lstCommands.AddString(cmdStr);

	m_lstCommands.AddString(getString(ST_ALEXA, IDS_ALEXA));
	m_lstCommands.AddString(getString(ST_MEDIAPLAYER_LICENSES, IDS_MEDIAPLAYER_LICENSES));
	m_lstCommands.AddString(getString(ST_MEDIAPLAYER_SEND_GUID, IDS_MEDIAPLAYER_SEND_GUID));
	m_lstCommands.AddString(getString(ST_MEDIAPLAYER_CODECS, IDS_MEDIAPLAYER_CODECS));
	m_lstCommands.AddString(getString(ST_ERROR_REPORTS_SEND, IDS_ERROR_REPORTS_SEND));
	m_lstCommands.AddString(getString(ST_IE_SCHEDULED_UPDATES, IDS_IE_SCHEDULED_UPDATES));
	m_lstCommands.AddString(getString(ST_IE_CHECK_4_UPDATES, IDS_IE_CHECK_4_UPDATES));
	m_lstCommands.AddString(getString(ST_IE_INTEGRATED_AUTHENTICATION, IDS_IE_INTEGRATED_AUTHENTICATION));
	m_lstCommands.AddString(getString(ST_SERVICES_ERROR_REPORTS, IDS_SERVICES_ERROR_REPORTS));
	m_lstCommands.AddString(getString(ST_SERVICES_AUTO_UPDATES, IDS_SERVICES_AUTO_UPDATES));
	m_lstCommands.AddString(getString(ST_SERVICES_TIMESERVER, IDS_SERVICES_TIMESERVER));
	m_lstCommands.AddString(getString(ST_SERVICES_TASKSCHEDULER, IDS_SERVICES_TASKSCHEDULER));
	m_lstCommands.AddString(getString(ST_MESSENGER_AUTOSTART, IDS_MESSENGER_AUTOSTART));
	m_lstCommands.AddString(getString(ST_MESSENGER_DEINSTALL, IDS_MESSENGER_DEINSTALL));
	m_lstCommands.AddString(getString(ST_SUPPORTUSER, IDS_SUPPORTUSER));
	m_lstCommands.AddString(getString(ST_REGSVR_REGWIZC, IDS_REGSVR_REGWIZC));
	m_lstCommands.AddString(getString(ST_REGSVR_LICDLL, IDS_REGSVR_LICDLL));
	m_lstCommands.AddString(getString(ST_REMOTEDESKTOP, IDS_REMOTEDESKTOP));
	m_lstCommands.AddString(getString(ST_REGDONE, IDS_REGDONE));
	m_lstCommands.AddString(getString(ST_TIME_SYNC, IDS_TIME_SYNC));
	m_lstCommands.AddString(getString(ST_BALLOON_TIPS, IDS_BALLOON_TIPS));
	m_lstCommands.AddString(getString(ST_CLEAR_PAGEFILE, IDS_CLEAR_PAGEFILE));
	m_lstCommands.AddString(getString(ST_OE_START_MESSENGER, IDS_OE_START_MESSENGER));
	m_lstCommands.AddString(getString(ST_SERVICES_UPNP, IDS_SERVICES_UPNP));
	m_lstCommands.AddString(getString(ST_OXP_ERRORREPORT, IDS_OXP_ERRORREPORT));
	m_lstCommands.AddString(getString(ST_FAST_SHUTDOWN, IDS_FAST_SHUTDOWN));
	m_lstCommands.AddString(getString(ST_MAX_CONNECTIONS, IDS_MAX_CONNECTIONS));
	m_lstCommands.AddString(getString(ST_REGSVR_ZIPFLDR, IDS_REGSVR_ZIPFLDR));
	m_lstCommands.AddString(getString(ST_SERVICES_MSGSRV, IDS_SERVICES_MSGSRV));
	m_lstCommands.AddString(getString(ST_IE_FORM_SUGGEST, IDS_IE_FORM_SUGGEST));
	m_lstCommands.AddString(getString(ST_IE_UPLDDRVINFO, IDS_IE_UPLDDRVINFO));
	m_lstCommands.AddString(getString(ST_MP9_ADDMEDIATOLIB, IDS_MP9_ADDMEDIATOLIB));
	m_lstCommands.AddString(getString(ST_MP9_RETRIEVEMETADATA, IDS_MP9_RETRIEVEMETADATA));
	m_lstCommands.AddString(getString(ST_MP9_USAGETRACKING, IDS_MP9_USAGETRACKING));
	m_lstCommands.AddString(getString(ST_MP9_SAVEMRU, IDS_MP9_SAVEMRU));
	m_lstCommands.AddString(getString(ST_IE_JAVASCRIPT, IDS_IE_JAVASCRIPT));
	m_lstCommands.AddString(getString(ST_IE_ACTIVEX, IDS_IE_ACTIVEX));
	m_lstCommands.AddString(getString(ST_IE_POPUP_BLOCKER, IDS_IE_POPUP_BLOCKER));
	m_lstCommands.AddString(getString(ST_FIREWALL, IDS_FIREWALL));
	m_lstCommands.AddString(getString(ST_SERVICES_FIREWALL, IDS_SERVICES_FIREWALL));
	m_lstCommands.AddString(getString(ST_SERVICES_SEC_CTR, IDS_SERVICES_SEC_CTR));
	m_lstCommands.AddString(getString(ST_SHOW_FILE_EXT, IDS_SHOW_FILE_EXT));
	m_lstCommands.AddString(getString(ST_O2k3_ERRORREPORT, IDS_O2K3_ERRORREPORT));
	m_lstCommands.AddString(getString(ST_HIDE_IN_NETWORK, IDS_HIDE_IN_NETWORK));
	m_lstCommands.AddString(getString(ST_DENY_REGEDIT, IDS_DENY_REGEDIT));
	m_lstCommands.AddString(getString(ST_SCRIPTING_HOST, IDS_SCRIPTING_HOST));
	m_lstCommands.AddString(getString(ST_SHOW_LAST_USERNAME, IDS_SHOW_LAST_USERNAME));
	m_lstCommands.AddString(getString(ST_IE_WATSON, IDS_IE_WATSON));
	m_lstCommands.AddString(getString(ST_IE_CLEAN_CACHE, IDS_IE_CLEAN_CACHE));
	m_lstCommands.AddString(getString(ST_MEDIAPLAYER_UPDATE, IDS_MEDIAPLAYER_UPDATE));
	m_lstCommands.AddString(getString(ST_LNK_SUFFIX, IDS_LNK_SUFFIX));
	m_lstCommands.AddString(getString(ST_PIF_SUFFIX, IDS_PIF_SUFFIX));
	m_lstCommands.AddString(getString(ST_SHC_SUFFIX, IDS_SHC_SUFFIX));
	m_lstCommands.AddString(getString(ST_URL_SUFFIX, IDS_URL_SUFFIX));
	m_lstCommands.AddString(getString(ST_SHB_SUFFIX, IDS_SHB_SUFFIX));
	m_lstCommands.AddString(getString(ST_CD_AUTORUN, IDS_CD_AUTORUN));
	m_lstCommands.AddString(getString(ST_CLEAN_RECENT_DOCS, IDS_CLEAN_RECENT_DOCS));
	m_lstCommands.AddString(getString(ST_MP_START_IN_MEDIA_GUIDE, IDS_MP_START_IN_MEDIA_GUIDE));
	m_lstCommands.AddString(getString(ST_DONT_REPORT_INFECTION, IDS_DONT_REPORT_INFECTION));
	m_lstCommands.AddString(getString(ST_RESTRICTANONYMOUS, IDS_RESTRICTANONYMOUS));
	m_lstCommands.AddString(getString(ST_NONETCRAWL, IDS_NONETCRAWL));
	m_lstCommands.AddString(getString(ST_MEDIAPLAYER_BLOCKUNINSTALL, IDS_MEDIAPLAYER_BLOCKUNINSTALL));
	m_lstCommands.AddString(getString(ST_SERVICES_INDEXER, IDS_SERVICES_INDEXER));
	m_lstCommands.AddString(getString(ST_NOLMHASH, IDS_NOLMHASH));
	m_lstCommands.AddString(getString(ST_NO_WGA_CHECK, IDS_NO_WGA_CHECK));
	m_lstCommands.AddString(getString(ST_DO_NOT_ALLOW_IE70, IDS_DO_NOT_ALLOW_IE70));
	m_lstCommands.AddString(getString(ST_IE_PHISHING_FILTER, IDS_IE_PHISHING_FILTER));
	m_lstCommands.AddString(getString(ST_SERVICES_WINDOWS_DEFENDER, IDS_SERVICES_WINDOWS_DEFENDER));
	m_lstCommands.AddString(getString(ST_WMP_PREVENT_LIBRARY_SHARING, IDS_WMP_PREVENT_LIBRARY_SHARING));
	m_lstCommands.AddString(getString(ST_TWEAK_COMMAND_HERE, IDS_TWEAK_COMMAND_HERE));
	m_lstCommands.AddString(getString(ST_TWEAK_SVC_WmdmPmSN, IDS_TWEAK_SVC_WmdmPmSN));
	m_lstCommands.AddString(getString(ST_TWEAK_SVC_SUPPORT_CTR, IDS_TWEAK_SVC_SUPPORT_CTR));
	m_lstCommands.AddString(getString(ST_TWEAK_DEL_CTFMON, IDS_TWEAK_DEL_CTFMON));
	m_lstCommands.AddString(getString(ST_TWEAK_CLEANUP_WIZ, IDS_TWEAK_CLEANUP_WIZ));
	m_lstCommands.AddString(getString(ST_TWEAK_WINUPDATE_NO_DRIVER, IDS_TWEAK_WINUPDATE_NO_DRIVER));
	m_lstCommands.AddString(getString(ST_TWEAK_WEBSERVICE, IDS_TWEAK_WEBSERVICE));
	m_lstCommands.AddString(getString(ST_TWEAK_BSOD_AUTOREBOOT, IDS_TWEAK_BSOD_AUTOREBOOT));
	m_lstCommands.AddString(getString(ST_TWEAK_DEV_MANAGER, IDS_TWEAK_DEV_MANAGER));
	m_lstCommands.AddString(getString(ST_TWEAK_NO_WINTOUR, IDS_TWEAK_NO_WINTOUR));
	m_lstCommands.AddString(getString(ST_NET_NO_RECENT_DOCS, IDS_NET_NO_RECENT_DOCS));
	m_lstCommands.AddString(getString(ST_NO_THUMBNAIL_CACHE, IDS_NO_THUMBNAIL_CACHE));
	m_lstCommands.AddString(getString(ST_NO_SEARCH_ASSISTANT, IDS_NO_SEARCH_ASSISTANT));
	m_lstCommands.AddString(getString(ST_DISABLE_UAC, IDS_DISABLE_UAC));
	m_lstCommands.AddString(getString(ST_IE_NO_SQM, IDS_IE_NO_SQM));
	m_lstCommands.AddString(getString(ST_IE_SUGGESTED_SITES, IDS_IE_SUGGESTED_SITES));
	m_lstCommands.AddString(getString(ST_NO_UNREAD_MAIL, IDS_NO_UNREAD_MAIL));
	m_lstCommands.AddString(getString(ST_NO_VDM, IDS_NO_VDM));
	m_lstCommands.AddString(getString(ST_NO_TEREDO, IDS_NO_TEREDO));
	m_lstCommands.AddString(getString(ST_NO_ISATAP, IDS_NO_ISATAP));
	m_lstCommands.AddString(getString(ST_NO_NCSI, IDS_NO_NCSI));
	m_lstCommands.AddString(getString(ST_NO_ROOT_CERT, IDS_NO_ROOT_CERT));
	m_lstCommands.AddString(getString(ST_CERT_REVOCATION, IDS_CERT_REVOCATION));
	m_lstCommands.AddString(getString(ST_CERT_TRUST, IDS_CERT_TRUST));
	m_lstCommands.AddString(getString(ST_NO_HTTPS_CACHE, IDS_NO_HTTPS_CACHE));
	m_lstCommands.AddString(getString(ST_NO_FORCEREBOOT, IDS_NO_FORCEREBOOT));
	m_lstCommands.AddString(getString(ST_TWEAK_NO_SHORTCUT_ARROW, IDS_NO_SHORTCUT_ARROW));
	m_lstCommands.AddString(getString(ST_NO_RECENT_DOCS, IDS_NO_RECENT_DOCS));
	m_lstCommands.AddString(getString(ST_TWEAK_SVC_BITS, IDS_TWEAK_SVC_BITS));
	m_lstCommands.AddString(_T("---------------------------------"));
	cmdStr.LoadString(IDS_CMD_LINE_EXAMPLE);
	m_lstCommands.AddString(cmdStr);
}

BOOL CCMDLineDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CString tmp;
	tmp.LoadString(IDS_CLIPBOARD_COPY);
	CRect rect;
	this->GetClientRect(rect);
	rect.top = rect.bottom - 23;
	m_btnCopy.Create(tmp,WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_BITMAP,rect,this,4);
	m_btnCopy.setHasBorder(true);
	initListCtrl();
	return TRUE;
}

void CCMDLineDlg::CopyToClipboard(const char* stringbuffer)
{
   //try to open clipboard first
   if (!OpenClipboard())
   {
      return;
   }
  
   //clear clipboard
   EmptyClipboard();
 
   HGLOBAL clipbuffer;
   char * buffer;
   
   //alloc enough mem for the string;
   //must be GMEM_DDESHARE to work with the clipboard
   clipbuffer = GlobalAlloc(GMEM_DDESHARE, strlen(stringbuffer)+1);
   buffer = (char*)GlobalLock(clipbuffer);
   strcpy(buffer, LPCSTR(stringbuffer));
   GlobalUnlock(clipbuffer);
   
   //fill the clipboard with data
   //CF_TEXT indicates that the buffer is a text-string
   ::SetClipboardData(CF_TEXT,clipbuffer);
   //close clipboard as we don't need it anymore
   CloseClipboard();
}

CString CCMDLineDlg::getString(SETTINGS set, UINT nID)
{
	CString tmp, cmdStr;
	tmp.LoadString(nID);
	cmdStr.Format(_T("%d ==> %s"),set,tmp);

	return cmdStr;
}
