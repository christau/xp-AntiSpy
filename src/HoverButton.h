#if !defined(AFX_HOVERBUTTON_H__D020C260_B9EA_4419_850E_4F9E140B4601__INCLUDED_)
#define AFX_HOVERBUTTON_H__D020C260_B9EA_4419_850E_4F9E140B4601__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HoverButton.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Fenster CHoverButton 

class CHoverButton : public CButton
{
// Konstruktion
public:
	CHoverButton();

public:
	enum BUTTON_STATE{ BUTTON_OFF, BUTTON_ON, BUTTON_OVER, BUTTON_GREYED};
private:
	BOOL            m_bMouseTracking;
	BUTTON_STATE    m_ButtonState;
	bool			m_ButtonPressed;
	bool			m_hasBorder;
	bool			m_useBitmap;
	HBITMAP			m_bmpHover;
public:
    BOOL PreTranslateMessage(MSG* pMsg);
	CString m_sTipText;
    void SetTipText( LPCTSTR sz ) 
	{ 
		m_sTipText= sz;      
		if (m_sTipText > "" ) {
			  EnableToolTips( TRUE );
			  m_tooltip.Create(this, TTS_NOPREFIX| TTS_ALWAYSTIP );
			  m_tooltip.AddTool(this, (LPCTSTR)m_sTipText );
			  m_tooltip.Activate(TRUE);
		}
	};
    CToolTipCtrl m_tooltip;
	
// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CHoverButton)
	public:
		virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
		virtual void PreSubclassWindow();	
	//}}AFX_VIRTUAL

// Implementierung
public:
	void setHasBorder(bool flag);
	void setUseBitmap(bool flag)
	{
		m_useBitmap = flag;
	}
	void setHoverBitmap(HBITMAP bm)
	{
		m_bmpHover = bm;
	}
	virtual ~CHoverButton();

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
    //{{AFX_MSG(CHoverButton)
        afx_msg void OnMouseMove(UINT nFlags, CPoint point);
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
		afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnMouseEnter(WPARAM wparam, LPARAM lparam) ;
	//}}AFX_MSG
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_HOVERBUTTON_H__D020C260_B9EA_4419_850E_4F9E140B4601__INCLUDED_
