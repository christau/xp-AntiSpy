//Based on
//http://www.codeproject.com/KB/edit/EditStyle.aspx

#if !defined(AFX_EXEDIT_H__8F7B8C2F_8DB0_481F_9777_C3098B0E05BD__INCLUDED_)
#define AFX_EXEDIT_H__8F7B8C2F_8DB0_481F_9777_C3098B0E05BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CExEdit window
class CExEdit : public CEdit
{
// Construction
public:
	CExEdit();

// Attributes
public:

// Operations
public:
	void setTextColor( COLORREF clrText );
	void setFontSize( int nSize );
	void setFontStyle( BOOL bItalic, BOOL bBold, BOOL bUnderLine );
	void setBkColor( COLORREF clrBk );
	void setFontName( CString strFontName);
	void drawEditFrame();
	void setFontHelper();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExEdit)
	public:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CExEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CExEdit)
	afx_msg void OnKillfocus();
	afx_msg void OnSetfocus();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseHover( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnMouseLeave( WPARAM wParam, LPARAM lParam );
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	int      m_nFontSize;
	COLORREF m_clrText;
	COLORREF m_clrBk;
	BOOL     m_bItalic;
	BOOL     m_bBold;
	BOOL     m_bUnderLine;
	BOOL     m_bFocus;
	BOOL     m_bOver;
	CString  m_strFontName;
	CBrush   m_bkBrush;
	CFont    *m_pFont;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXEDIT_H__8F7B8C2F_8DB0_481F_9777_C3098B0E05BD__INCLUDED_)
