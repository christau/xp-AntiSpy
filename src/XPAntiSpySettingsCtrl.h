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

#ifndef __XPAntiSpySettingsCtrl_H__
#define __XPAntiSpySettingsCtrl_H__


//forward declaration
class CXPAntiSpySettingsCtrl;



class CXPAntiSpyDlg; //forward declaration

//The actual tree options control class
class CXPAntiSpySettingsCtrl : public CTreeCtrl
{
public:
	enum MODES
	{
		NORMAL,
		ACCESSIBLE
	};
	MODES getMode();
	void setMode( MODES mode);
	int ChangeItem(HTREEITEM hItem, int nImage, int nSelectedImage);
	void SetAppDlgPointer(CXPAntiSpyDlg *pDlg);
	CXPAntiSpyDlg* m_pDlg;
  CXPAntiSpySettingsCtrl();


//Inserting items into the control
  HTREEITEM InsertGroup(LPCTSTR lpszItem, int nImage, HTREEITEM hParent = TVI_ROOT);
  HTREEITEM InsertItem(LPCTSTR lpszItem, HTREEITEM hParent, BOOL bCheck = TRUE);
  void RemoveGroup(HTREEITEM hItem);
//Validation methods
  BOOL IsGroup(HTREEITEM hItem);
  BOOL IsCheckBox(HTREEITEM hItem);

  BOOL SetChecked(HTREEITEM hItem, BOOL bCheck);
  BOOL IsChecked(HTREEITEM hItem);//, BOOL& bCheck);
  
  BOOL SetLocked(HTREEITEM hItem);
  BOOL IsLocked(HTREEITEM hItem);

//Enable / Disbale items
  BOOL SetEnabled(HTREEITEM hItem, BOOL bEnable);
  BOOL IsEnabled(HTREEITEM hItem);
  void RedrawItem(HTREEITEM hItem);

private:
	CString getModeString(int index);
	HTREEITEM m_oldhItem;
	CImageList m_ilTree;
	HIMAGELIST m_imgList;
	UINT m_nilID;
	HTREEITEM m_hControlItem;
	CFont m_boldFont;
	CFont m_normFont;
	CBitmap m_bmpDiv, *m_pOldBmp;
	CBitmap m_bmpDivL, m_bmpDivM, m_bmpDivR;
	CBitmap m_alert;
	HTREEITEM m_hPinnedItem; 
	MODES m_mode;

	
	//{{AFX_VIRTUAL(CXPAntiSpySettingsCtrl)
	protected:
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXPAntiSpySettingsCtrl)
	afx_msg void OnCustomdraw ( NMHDR* pNMHDR, LRESULT* pResult );
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

  DECLARE_DYNAMIC(CXPAntiSpySettingsCtrl)

	DECLARE_MESSAGE_MAP()
};


#endif //__XPAntiSpySettingsCtrl_H__



