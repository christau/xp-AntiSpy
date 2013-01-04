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
#include "resource.h"
#include "XPAntiSpySettingsCtrl.h"
#include "XPAntiSpyDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT TREE_OPTIONS_COMBOBOX_ID = 100;
const UINT TREE_OPTIONS_EDITBOX_ID = 100;



//////////////// Implementation //////////////////////////////////////
IMPLEMENT_DYNAMIC(CXPAntiSpySettingsCtrl, CTreeCtrl)

BEGIN_MESSAGE_MAP(CXPAntiSpySettingsCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CXPAntiSpySettingsCtrl)
	ON_NOTIFY_REFLECT ( NM_CUSTOMDRAW, OnCustomdraw )
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CXPAntiSpySettingsCtrl::OnCustomdraw ( NMHDR* pNMHDR, LRESULT* pResult )
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );
	*pResult = CDRF_DODEFAULT;
    if ( CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage )
    {
		*pResult = CDRF_NOTIFYITEMDRAW;
    }
    else if ( CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage )
    {
		int offset = GetScrollPos(SB_HORZ);
		HTREEITEM hti=(HTREEITEM)pLVCD->nmcd.dwItemSpec;
		TVITEM tvi;
		TCHAR szText[1024];
		tvi.hItem=hti;
		tvi.mask		= TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_HANDLE;
		tvi.pszText		= szText;
		tvi.cchTextMax	= 1024;
		GetItem(&tvi);

		//get a different background color for group items
		CDC* pDC=CDC::FromHandle(pLVCD->nmcd.hdc);
		pDC->SetBkMode(TRANSPARENT);
		CRect rect;
		GetItemRect((HTREEITEM)pLVCD->nmcd.dwItemSpec,&rect,LVIR_BOUNDS);
		pDC->FillSolidRect(&rect,(COLORREF)0x00FFFFFF);
		rect.left -= offset;
		if(pLVCD->iSubItem == 0)
		{
			CDC			memDC;
			memDC.CreateCompatibleDC(pDC);
			CBitmap* pOld=memDC.SelectObject(&m_bmpDivL);
			pDC->StretchBlt(rect.left, rect.top, 6, rect.Height() ,&memDC, 0, 0, 6, 18, SRCCOPY);
			memDC.SelectObject(&m_bmpDivM);
			pDC->StretchBlt(rect.left+6,rect.top,rect.Width()-12,rect.Height(), &memDC, 0, 0, 6, 18, SRCCOPY );
			memDC.SelectObject(&m_bmpDivR);
			pDC->StretchBlt(rect.Width()-6, rect.top, 6, rect.Height(), &memDC, 0, 0, 6, 18, SRCCOPY);
			if(m_pDlg->IsGroupAlerted(tvi.hItem))
			{
				ImageList_Draw(m_imgList, ALERT, pDC->m_hDC, rect.right-16, rect.top, ILD_TRANSPARENT);
			}
			memDC.SelectObject(pOld);	

			
			UINT state=GetItemState(hti,TVIS_EXPANDED);
			if((state & TVIS_EXPANDED))
			{
				CPoint pt[3];
				pt[0].x=rect.left+4;
				pt[0].y=rect.top+(rect.Height()/2)-2;
				pt[1].x=rect.left+12;
				pt[1].y=rect.top+(rect.Height()/2)-2;
				pt[2].x=rect.left+8;
				pt[2].y=rect.top+(rect.Height()/2)+2;
				pDC->Polygon(pt,3);
			}
			else
			{
				CPoint pt[3];
				pt[0].x=rect.left+4;
				pt[0].y=rect.top+(rect.Height()/2)-4;
				pt[1].x=rect.left+8;
				pt[1].y=rect.top+(rect.Height()/2);
				pt[2].x=rect.left+4;
				pt[2].y=rect.top+(rect.Height()/2)+4;
				pDC->Polygon(pt,3);
			}
		}
		else if(m_hPinnedItem == hti)
		{
			CPen border;
			CPen* pOldPen;
			CBrush fill;
			CBrush* pOldBrush;
			border.CreatePen(PS_SOLID,1,(COLORREF)0x0096E4FE);
			pOldPen=pDC->SelectObject(&border);
			fill.CreateSolidBrush(0x00CEFFFF);
			pOldBrush = pDC->SelectObject(&fill);
			pDC->Rectangle(&rect);
			pDC->SelectObject(pOldBrush);
			pDC->SelectObject(*pOldPen);
			ImageList_Draw(m_imgList, PINNED, pDC->m_hDC, rect.right - 16, rect.top, ILD_TRANSPARENT);
		}

		//draw item image
		GetItemRect(hti,rect,false);
		rect.left -= offset;
		if(pLVCD->iSubItem != 0)
		{
			ImageList_Draw(m_imgList, tvi.iImage, pDC->m_hDC, rect.left, rect.top, ILD_TRANSPARENT);
		}
		//draw text
		CString text;
//		if(m_mode == ACCESSIBLE)
//		{
//			text = getModeString(tvi.iImage);
//			text = text + tvi.pszText;
//		}
//		else
		{
			text = tvi.pszText;
		}
		if(pLVCD->iSubItem == 0)
		{
			CFont *oldFont=pDC->SelectObject(&m_boldFont);
			pDC->SetTextColor(0x00000000);
			pDC->DrawText(text, &rect, DT_CENTER);
			pDC->SelectObject(oldFont);
		}
		else
		{
			pDC->SetTextColor(0x00000000);
			rect.left+=20;
			CFont *oldFont=pDC->SelectObject(&m_normFont);
			pDC->TextOut(rect.left, rect.top, text);
			pDC->SelectObject(oldFont);
		}


	*pResult = CDRF_SKIPDEFAULT;
	}
}

CXPAntiSpySettingsCtrl::CXPAntiSpySettingsCtrl():CTreeCtrl()
{
	m_mode = NORMAL;
	m_hControlItem = NULL;
	m_oldhItem = 0;
	m_hPinnedItem = NULL;

#ifdef __RUSSIAN__
	BYTE nCharSet = RUSSIAN_CHARSET;
#else
	BYTE nCharSet = ANSI_CHARSET;
#endif
	LOGFONT lFont;
	lFont.lfWeight = FW_BOLD;
	lFont.lfItalic = (BYTE)FALSE;
	lFont.lfUnderline = (BYTE)FALSE;
	lFont.lfStrikeOut = (BYTE)FALSE;
	lFont.lfHeight = 15;
	lFont.lfCharSet = nCharSet;
	lFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lFont.lfQuality = DEFAULT_QUALITY;
	lFont.lfPitchAndFamily = DEFAULT_PITCH|FF_SWISS;
	lFont.lfEscapement = 0;
	
	_tcsncpy(lFont.lfFaceName, _T("MS Shell Dlg 2"), 32);
//	m_boldFont.CreateFontIndirect(&lFont);
	
	lFont.lfWeight = FW_NORMAL;
//	m_normFont.CreateFontIndirect(&lFont);

	m_boldFont.CreateFont(15,0,0,0,FW_BOLD,false,false,0,nCharSet,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_SWISS,_T("Microsoft Sans Serif"));
	m_normFont.CreateFont(15,0,0,0,FW_NORMAL,false,false,0,nCharSet,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_SWISS,_T("Microsoft Sans Serif"));
	m_bmpDivL.LoadBitmap(IDB_DIV_LEFT);
	m_bmpDivM.LoadBitmap(IDB_DIV_MID);
	m_bmpDivR.LoadBitmap(IDB_DIV_RIGHT);
//	m_alert.LoadBitmap(IDB_ALERT);

	m_imgList=ImageList_Create(16,16,ILC_COLOR24|ILC_MASK,0,6);
	ImageList_AddMasked(m_imgList,LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_UNCHECKED)),RGB(255,0,255));
	ImageList_AddMasked(m_imgList,LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_CHECKED)),RGB(255,0,255));
	ImageList_AddMasked(m_imgList,LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_UNCHECKED_RIGHT)),RGB(255,0,255));
	ImageList_AddMasked(m_imgList,LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_CHECKED_RIGHT)),RGB(255,0,255));
	ImageList_AddMasked(m_imgList,LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_UNCHECKED_WRONG)),RGB(255,0,255));
	ImageList_AddMasked(m_imgList,LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_CHECKED_WRONG)),RGB(255,0,255));
	ImageList_AddMasked(m_imgList,LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_UNCHECKED_CHANGE)),RGB(255,0,255));
	ImageList_AddMasked(m_imgList,LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_CHECKED_CHANGE)),RGB(255,0,255));
	ImageList_AddMasked(m_imgList,LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_UNCHECKED_LOCKED)),RGB(255,0,255));
	ImageList_AddMasked(m_imgList,LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_CHECKED_LOCKED)),RGB(255,0,255));
	ImageList_AddMasked(m_imgList,LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_UNCHECKED_RIGHT_LOCKED)),RGB(255,0,255));
	ImageList_AddMasked(m_imgList,LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_CHECKED_RIGHT_LOCKED)),RGB(255,0,255));
	ImageList_AddMasked(m_imgList,LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_UNCHECKED_WRONG_LOCKED)),RGB(255,0,255));
	ImageList_AddMasked(m_imgList,LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_CHECKED_WRONG_LOCKED)),RGB(255,0,255));
	ImageList_AddMasked(m_imgList,LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_DISABLED)),RGB(255,0,255));
	ImageList_AddMasked(m_imgList,LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_UNPINNED)),RGB(255,0,255));
	ImageList_AddMasked(m_imgList,LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_PINNED)),RGB(255,0,255));
	ImageList_AddMasked(m_imgList,LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_ALERT)),RGB(255,0,255));
	TreeView_SetImageList(m_hWnd,m_imgList,LVSIL_SMALL);

}

void CXPAntiSpySettingsCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	UINT uFlags=0;
	HTREEITEM hItem = HitTest(point, &uFlags);
  //If the mouse was over the label, icon or to the left or right of the item ?
	if ((uFlags & TVHT_ONITEM) || (uFlags & TVHT_ONITEMINDENT) || (uFlags & TVHT_ONITEMRIGHT))
	{
		CRect rect;
		GetItemRect(hItem, &rect, false);
		//if the user clicked the pin
		if(point.x > (rect.right - 16) && !IsGroup(hItem))
		{
			//see if it'S already pinned and toggle state
			if(hItem == m_hPinnedItem)
				m_hPinnedItem = NULL;
			else
				m_hPinnedItem = hItem;
			m_pDlg->TrackHelpText(hItem);
			this->SetFocus();
			Invalidate();
			return;
		}
		else if (IsCheckBox(hItem) && (point.x < (rect.left + 16)))
		{
			  BOOL bEnable;
			  VERIFY(bEnable = IsEnabled(hItem));
			  if (bEnable)
			  {
					//Toggle the state of check items
					BOOL bCheck;
					bCheck=IsChecked(hItem);
					//reset if the gui doesnt want if to be changed
					if(m_pDlg->toggleItem(hItem))
						;//SetChecked(hItem, !bCheck);
					if(m_mode == ACCESSIBLE)
						setMode(ACCESSIBLE);

			  }
		}
		else if(IsGroup(hItem))
		{
		  Expand(hItem, TVE_TOGGLE);
		}
		this->SelectItem(hItem);
	}
	else
	{
	}
	this->SetFocus();
}

void CXPAntiSpySettingsCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	
	
	UINT uFlags=0;
	HTREEITEM hItem = HitTest(point, &uFlags);
	if (IsCheckBox(hItem)) 
	{
		if(IsLocked(hItem))
		{
			return;
		}
		BOOL bEnable;
		VERIFY(bEnable = IsEnabled(hItem));
		this->SelectItem(hItem);
		m_pDlg->PopUpMenu(point,hItem);
	}
}

void CXPAntiSpySettingsCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
 	if (nChar == VK_SPACE)
	{
		HTREEITEM hItem = GetSelectedItem();

		//If the space key is hit, and the item is a combo item, then toggle the check value
		if (IsCheckBox(hItem))
		{
			if(IsLocked(hItem))
			{
				return;
			}
			BOOL bEnable;
			VERIFY(bEnable = IsEnabled(hItem));
			if (bEnable)
			{
				//Toggle the state of check items
				BOOL bCheck;
				bCheck=IsChecked(hItem);
				//reset if the gui doesnt want it to be changed
				if(m_pDlg->toggleItem(hItem))
					;//SetChecked(hItem, !bCheck);
				if(m_mode == ACCESSIBLE)
					setMode(ACCESSIBLE);
			}
		}
	}
	CTreeCtrl::OnChar(nChar, nRepCnt, nFlags);
}


void CXPAntiSpySettingsCtrl::OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // Up and down are in the OnKeyDown so that the user can hold down the arrow
    // keys to scroll through the entries.
    BOOL Control = GetKeyState (VK_CONTROL) < 0;
    switch (nChar)
    {
		case VK_UP :
		{
			HTREEITEM hItem = GetSelectedItem();
			HTREEITEM hNext = GetPrevVisibleItem(hItem);
			if(hItem == NULL || hNext == NULL)
			if(hItem == NULL || hNext == NULL)
			{
				m_pDlg->ClearHelpText();
				return;
			}

			TVITEM tvi;
			TCHAR szText[1024];
			tvi.hItem=hNext;
			tvi.mask		= TVIF_TEXT;
			tvi.pszText		= szText;
			tvi.cchTextMax	= 1024;
			GetItem(&tvi);
			int i = GetCount();
			m_hPinnedItem = hNext;
			m_oldhItem = hItem;
			SelectItem(hNext);
			RedrawItem(hNext);
			m_pDlg->TrackHelpText(hNext);
			
			return;
		}
		case VK_DOWN :
		{
			HTREEITEM hItem = GetSelectedItem();
			HTREEITEM hNext = GetNextVisibleItem(hItem);
			if(hItem == NULL || hNext == NULL)
			{
				m_pDlg->ClearHelpText();
				return;
			}

			TVITEM tvi;
			TCHAR szText[1024];
			tvi.hItem=hNext;
			tvi.mask		= TVIF_TEXT;
			tvi.pszText		= szText;
			tvi.cchTextMax	= 1024;
			GetItem(&tvi);
			int i = GetCount();
			m_hPinnedItem = hNext;
			m_oldhItem = hItem;
			SelectItem(hNext);
			RedrawItem(hNext);
			m_pDlg->TrackHelpText(hNext);			
		}
		case VK_HOME :
		{
			return;
		}
		case VK_END :
		{
			return;
		}
    }

    CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

HTREEITEM CXPAntiSpySettingsCtrl::InsertGroup(LPCTSTR lpszItem, int nImage, HTREEITEM hParent)
{
  HTREEITEM hItem = CTreeCtrl::InsertItem(lpszItem, nImage, nImage, hParent);
  SetItemData(hItem, 0); //Item data must be 0
  return hItem;
}

void CXPAntiSpySettingsCtrl::RemoveGroup(HTREEITEM hItem)
{
  CTreeCtrl::DeleteItem(hItem);
}

HTREEITEM CXPAntiSpySettingsCtrl::InsertItem(LPCTSTR lpszItem, HTREEITEM hParent, BOOL bCheck)
{
  ASSERT(IsGroup(hParent));

  HTREEITEM hItem = CTreeCtrl::InsertItem(lpszItem, 0, 0, hParent);
  SetItemData(hItem, 0);    //Item data must be 0
  BOOL bSuccess = SetChecked(hItem, bCheck);
  ASSERT(bSuccess);

  return hItem;
}

BOOL CXPAntiSpySettingsCtrl::IsGroup(HTREEITEM hItem)
{
	if(hItem==0) return false;
	return (GetParentItem(hItem)==NULL);
}

BOOL CXPAntiSpySettingsCtrl::IsCheckBox(HTREEITEM hItem)
{
  int nImage;
  int nSelectedImage;
  BOOL bSuccess = GetItemImage(hItem, nImage, nSelectedImage);

  return (nImage >= UNCHECKED && nImage < DISABLED);
}

BOOL CXPAntiSpySettingsCtrl::SetChecked(HTREEITEM hItem, BOOL bCheck)
{
	ASSERT(IsCheckBox(hItem));
	if(!IsEnabled(hItem)) 
		return false;
	BOOL bSuccess;
	if (bCheck)
		bSuccess = SetItemImage(hItem, CHECKED, CHECKED);
	else
		bSuccess = SetItemImage(hItem, UNCHECKED, UNCHECKED);   
	return bSuccess;
}

BOOL CXPAntiSpySettingsCtrl::SetLocked(HTREEITEM hItem)
{
	ASSERT(IsCheckBox(hItem));
	if(!IsEnabled(hItem)) 
		return false;
	BOOL bSuccess;
	if (IsChecked(hItem))
		bSuccess = SetItemImage(hItem, CHECKED_LOCKED, CHECKED_LOCKED);
	else
		bSuccess = SetItemImage(hItem, UNCHECKED_LOCKED, UNCHECKED_LOCKED);   
	return bSuccess;
}

BOOL CXPAntiSpySettingsCtrl::IsChecked(HTREEITEM hItem)//, BOOL& bCheck)
{
  bool bCheck;
  ASSERT(IsCheckBox(hItem)); 

  int nImage;
  int nSelectedImage;
  BOOL bSuccess = GetItemImage(hItem, nImage, nSelectedImage);
  ASSERT(bSuccess);

  bCheck = (nImage == CHECKED || nImage == CHECKED_RIGHT || nImage == CHECKED_WRONG || 
			nImage == CHECKED_CHANGE || nImage == CHECKED_LOCKED || nImage == CHECKED_RIGHT_LOCKED ||
			nImage == CHECKED_WRONG_LOCKED);

  return bCheck;
}

BOOL CXPAntiSpySettingsCtrl::IsLocked(HTREEITEM hItem)//, BOOL& bCheck)
{
  bool bCheck;
  ASSERT(IsCheckBox(hItem)); 

  int nImage;
  int nSelectedImage;
  BOOL bSuccess = GetItemImage(hItem, nImage, nSelectedImage);
  ASSERT(bSuccess);

  bCheck = (nImage == CHECKED_LOCKED || nImage == UNCHECKED_LOCKED || nImage == CHECKED_RIGHT_LOCKED ||
			nImage == UNCHECKED_RIGHT_LOCKED || nImage == CHECKED_WRONG_LOCKED || nImage == UNCHECKED_WRONG_LOCKED);

  return bCheck;
}

BOOL CXPAntiSpySettingsCtrl::SetEnabled(HTREEITEM hItem, BOOL bEnable)
{
  ASSERT(IsCheckBox(hItem)); //Must be a check box
  BOOL bSuccess = FALSE;

  if (bEnable)
  {
    BOOL bCheck;
    bCheck=IsChecked(hItem);
    if (bCheck)
      bSuccess = SetItemImage(hItem, CHECKED, CHECKED);
    else
      bSuccess = SetItemImage(hItem, UNCHECKED, UNCHECKED);
  }
  else
  {
      bSuccess = SetItemImage(hItem, DISABLED, DISABLED);
  }

  return bSuccess;
}

BOOL CXPAntiSpySettingsCtrl::IsEnabled(HTREEITEM hItem)
{
  ASSERT(IsCheckBox(hItem)); //Must be a check box

  int nImage;
  int nSelectedImage;
  BOOL bSuccess = GetItemImage(hItem, nImage, nSelectedImage);
  ASSERT(bSuccess);


  return (nImage >= UNCHECKED && nImage <= CHECKED_WRONG_LOCKED);  
}

void CXPAntiSpySettingsCtrl::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;
}

int CXPAntiSpySettingsCtrl::ChangeItem(HTREEITEM hItem, int nImage, int nSelectedImage)
{
	if(SetItemImage(hItem,nImage,nSelectedImage)!=0) return 1;
	return 0;
}


void CXPAntiSpySettingsCtrl::SetAppDlgPointer(CXPAntiSpyDlg *pDlg)
{
	m_pDlg = pDlg;
}


void CXPAntiSpySettingsCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	UINT uFlags=0;
	HTREEITEM hItem = HitTest(point, &uFlags);
	if(hItem == 0) return;
	if(hItem==m_oldhItem) return;//only set the text once, not on every mousemove
	HTREEITEM tmpItem=m_oldhItem;
	m_oldhItem=hItem;
	RedrawItem(tmpItem);
	RedrawItem(hItem);
	if(m_hPinnedItem == NULL)
		m_pDlg->TrackHelpText(hItem);
	CTreeCtrl::OnMouseMove(nFlags, point);
}


void CXPAntiSpySettingsCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	//avoid double clicking behavior of treectrls
}

void CXPAntiSpySettingsCtrl::RedrawItem(HTREEITEM hItem)
{
	if(IsGroup(hItem))
	{
		return;
	}
	CDC* pDC=GetDC();
	TVITEM tvi;
	TCHAR szText[1024];
	tvi.hItem=hItem;
	tvi.mask		= TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_HANDLE;
	tvi.pszText		= szText;
	tvi.cchTextMax	= 1024;
	GetItem(&tvi);
	CRect rect;
	int offset = GetScrollPos(SB_HORZ);
	GetItemRect(hItem,rect,false);
	rect.left -= offset;
	if(hItem==m_oldhItem && hItem != m_hPinnedItem)			
	{
		CPen border;
		CPen* pOldPen;
		CBrush fill;
		CBrush* pOldBrush;
		border.CreatePen(PS_SOLID,1,(COLORREF)0x00BBBBBB);
		pOldPen=pDC->SelectObject(&border);
		fill.CreateSolidBrush(0x00EEEEEE);
		pOldBrush = pDC->SelectObject(&fill);
		pDC->Rectangle(&rect);
		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(*pOldPen);
		ImageList_Draw(m_imgList, UNPINNED, pDC->m_hDC, rect.right - 16, rect.top, ILD_TRANSPARENT);
	}
	else if(hItem == m_hPinnedItem && (hItem != 0))
	{
		CPen border;
		CPen* pOldPen;
		CBrush fill;
		CBrush* pOldBrush;
		border.CreatePen(PS_SOLID,1,(COLORREF)0x0096E4FE);
		pOldPen=pDC->SelectObject(&border);
		fill.CreateSolidBrush(0x00CEFFFF);
		pOldBrush = pDC->SelectObject(&fill);
		pDC->Rectangle(&rect);
		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(*pOldPen);
		ImageList_Draw(m_imgList, PINNED, pDC->m_hDC, rect.right - 16, rect.top, ILD_TRANSPARENT);
	}
	else
	{
		pDC->FillSolidRect(&rect,(COLORREF)0x00FFFFFF);
	}

	ImageList_Draw(m_imgList, tvi.iImage, pDC->m_hDC, rect.left, rect.top, ILD_TRANSPARENT);
	//draw text
	pDC->SetBkMode(TRANSPARENT);
	CFont *oldFont = pDC->SelectObject(&m_normFont);

	rect.left+=20;
	CString text;
	text = tvi.pszText;
	pDC->DrawText(text, &rect, DT_LEFT);
	pDC->SelectObject(oldFont);

	ReleaseDC(pDC);
}

BOOL CXPAntiSpySettingsCtrl::OnEraseBkgnd(CDC* pDC) 
{
	CRect rect;
	GetClientRect(&rect);
	pDC->FillSolidRect(rect,(COLORREF)0xFFFFFF);
	return true;
}

void CXPAntiSpySettingsCtrl::setMode(MODES mode)
{
	m_mode = mode;
	if(m_mode == ACCESSIBLE)
	{
		TreeItem* pItem = m_pDlg->GetItemArray();
		for(int i=0; i<ITEMCOUNT; ++i)
		{
			TVITEM item;
			TCHAR txt[1024];
			item.hItem = pItem->treeItem;
			item.mask		= TVIF_IMAGE | TVIF_TEXT;
			item.pszText = txt;
			item.cchTextMax = 1024;
			GetItem(&item);
			CString text = getModeString(item.iImage);
			text = text + pItem->text.c_str();
			wcscpy(txt,text.operator LPCTSTR());
			item.pszText = txt;
			item.mask		= TVIF_TEXT;
			SetItem(&item);
			++pItem;
		}
	}
	else
	{
		TreeItem* pItem = m_pDlg->GetItemArray();
		for(int i=0; i<ITEMCOUNT; ++i)
		{
			TVITEM item;
			TCHAR txt[1024];
			item.hItem = pItem->treeItem;
			item.mask		= TVIF_IMAGE | TVIF_TEXT;
			item.pszText = txt;
			item.cchTextMax = 1024;
			GetItem(&item);
			wcscpy(txt,pItem->text.c_str());
			item.pszText = txt;
			item.mask		= TVIF_TEXT;
			SetItem(&item);
			++pItem;
		}
	}
	Invalidate(TRUE);
}

CString CXPAntiSpySettingsCtrl::getModeString(int index)
{
	CString text;
	switch (index)
	{
	case 0:
		text.LoadString(IDS_UNCHECKED);
		break;
	case 1:
		text.LoadString(IDS_CHECKED);
		break;
	case 2:
		text.LoadString(IDS_UNCHECKED_RIGHT);
		break;
	case 3:
		text.LoadString(IDS_CHECKED_RIGHT);
		break;
	case 4:
		text.LoadString(IDS_UNCHECKED_WRONG);
		break;
	case 5:
		text.LoadString(IDS_CHECKED_WRONG);
		break;
	case 6:
		text.LoadString(IDS_UNCHECKED_CHANGE);
		break;
	case 7:
		text.LoadString(IDS_CHECKED_CHANGE);
	}
	return text;
}

CXPAntiSpySettingsCtrl::MODES CXPAntiSpySettingsCtrl::getMode()
{
	return m_mode;
}
