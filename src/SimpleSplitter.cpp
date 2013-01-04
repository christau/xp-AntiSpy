#include "stdafx.h"
#include "SimpleSplitter.h"


BEGIN_MESSAGE_MAP(CSimpleSplitter, CWnd)
	//{{AFX_MSG_MAP(CSimpleSplitter)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_NCCREATE()
	ON_WM_WINDOWPOSCHANGING()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CSimpleSplitter::CSimpleSplitter(int nPanes, UINT nOrientation, int nMinSize, int nBarThickness):
	m_nPanes(nPanes),
	m_nOrientation(nOrientation),
	m_nMinSize(nMinSize), 
	m_nBarThickness(nBarThickness)
{
	ASSERT(nPanes > 0);
	ASSERT(nOrientation == SSP_HORZ || nOrientation == SSP_VERT);
	ASSERT(nMinSize >= 0);
	ASSERT(nBarThickness >= 0);

	m_pane = new CWnd*[m_nPanes];
	m_orig = new int[m_nPanes + 1];
	::ZeroMemory(m_pane, m_nPanes * sizeof(CWnd*));
}

CSimpleSplitter::~CSimpleSplitter()
{
	delete[] m_pane;
	delete[] m_orig;
}


BOOL CSimpleSplitter::Create(CWnd* pParent, UINT nID)
{
	HCURSOR crsResize;
	int i, initial_size;
	CRect rcOuter;

	ASSERT(pParent);

	crsResize = ::LoadCursor(0, MAKEINTRESOURCE(m_nOrientation == SSP_HORZ ? AFX_IDC_HSPLITBAR : AFX_IDC_VSPLITBAR));
	if (crsResize == NULL)
		crsResize = ::LoadCursor(0, m_nOrientation == SSP_HORZ ? IDC_SIZEWE : IDC_SIZENS);
	pParent->GetClientRect(&rcOuter);
	initial_size = (rcOuter.Width() - m_nBarThickness * (m_nPanes - 1)) / m_nPanes;
	m_orig[0] = 0;
	for (i = 1; i <= m_nPanes; i++)					// default, set equal size to all panes
		m_orig[i] = m_orig[i - 1] + initial_size + m_nBarThickness;
	return CreateEx(0, AfxRegisterWndClass(CS_DBLCLKS, crsResize, NULL, NULL), NULL, WS_CHILD | WS_VISIBLE, rcOuter, pParent, nID, NULL);
}

BOOL CSimpleSplitter::CreatePane(int nIndex, CWnd* pPaneWnd, DWORD dwStyle, DWORD dwExStyle, LPCTSTR lpszClassName)
{
	CRect rcPane;

	ASSERT((nIndex >= 0) && (nIndex < m_nPanes));
	m_pane[nIndex] = pPaneWnd;
	dwStyle |= WS_CHILD | WS_VISIBLE;
	GetPaneRect(nIndex, rcPane);
	return pPaneWnd->CreateEx(dwExStyle, lpszClassName, NULL, dwStyle, rcPane, this, AFX_IDW_PANE_FIRST + nIndex);
}

void CSimpleSplitter::SetPane(int nIndex, CWnd* pPaneWnd)
{
	CRect rcPane;

	ASSERT((nIndex >= 0) && (nIndex < m_nPanes));
	ASSERT(pPaneWnd);
	ASSERT(pPaneWnd->m_hWnd);

	m_pane[nIndex] = pPaneWnd;
	GetPaneRect(nIndex, rcPane);
	pPaneWnd->MoveWindow(rcPane, false);
}

CWnd* CSimpleSplitter::GetPane(int nIndex) const
{
	ASSERT((nIndex >= 0) && (nIndex < m_nPanes));
	return m_pane[nIndex];
}

void CSimpleSplitter::SetActivePane(int nIndex)
{
	ASSERT((nIndex >= 0) && (nIndex < m_nPanes));
	m_pane[nIndex]->SetFocus();
}

CWnd* CSimpleSplitter::GetActivePane(int* pIndex) const
{
	for (int i = 0; i < m_nPanes; i++)
		if (m_pane[i]->GetFocus())
		{
			*pIndex = i;
			return m_pane[i];
		}
	return NULL;
}

void CSimpleSplitter::SetPaneSizes(const int* sizes)
{
	CRect rcOuter;
	int* s = new int[m_nPanes];
	int i, free_panes = 0;
	int remain, demand = 0;
	
	GetClientRect(&rcOuter);
	remain = m_nOrientation == SSP_HORZ ? rcOuter.Width() : rcOuter.Height();
	remain -= m_nBarThickness * (m_nPanes - 1);
	for (i = 0; i < m_nPanes; i++)
		if (sizes[i] == -1)
			free_panes++;
		else
			demand += sizes[i];

	if ((free_panes != 0) && (demand + free_panes * m_nMinSize <= remain))
	{
		for (i = 0; i < m_nPanes; i++)				
			if (sizes[i] != -1)					// set the specified size to non-free panes
			{
				s[i] = sizes[i];
				if (s[i] < m_nMinSize)
					s[i] = m_nMinSize;
				remain -= s[i];
			}
		for (i = 0; i < m_nPanes; i++)
			if (sizes[i] == -1)					// divide the remainder to free panes
				s[i] = remain / free_panes;
	}
	else										// divide the splitter proportionally to sizes
	{
		for (i = 0; i < m_nPanes; i++)			// ensure that smallest panes will be at least
		{										// m_nMinSize pixels wide
			if(demand != 0)
				s[i] = sizes[i] * remain / demand;
			if (s[i] < m_nMinSize)
			{
				s[i] = m_nMinSize;
				remain -= s[i];
				demand -= sizes[i];
			}
			else s[i] = 0;
		}
		for (i = 0; i < m_nPanes; i++)			// divide the remainder proportionally to 			
		{										// larger panes
			if (s[i] == 0)
				if(demand != 0)
					s[i] = sizes[i] * remain / demand;
			if (s[i] < m_nMinSize)
				s[i] = m_nMinSize;
			remain -= s[i];
			demand -= sizes[i];
		}
	}

	for (i = 1; i <= m_nPanes; i++)				// set new m_orig values
	{
		TRACE2("before orig %d %d\n", i, m_orig[i]);
		m_orig[i] = m_orig[i - 1] + s[i - 1] + m_nBarThickness;
		TRACE2("after orig %d %d\n", i, m_orig[i]);
	}
	if (m_nOrientation == SSP_HORZ)				// resize panes
		for (i = 0; i < m_nPanes; i++)
		{
			if (m_pane[i])
				m_pane[i]->MoveWindow(m_orig[i], 0, s[i], rcOuter.Height());
		}
	else
		for (i = 0; i < m_nPanes; i++)
		{
			if (m_pane[i])
				m_pane[i]->MoveWindow(0, m_orig[i], rcOuter.Width(), s[i]);
		}

	delete[] s;
}

void CSimpleSplitter::GetPaneRect(int nIndex, CRect& rcPane) const
{
	ASSERT(nIndex >= 0 && nIndex < m_nPanes);
	GetClientRect(&rcPane);
	if (m_nOrientation == SSP_HORZ)
	{
		rcPane.left = m_orig[nIndex];
		rcPane.right = m_orig[nIndex + 1] - m_nBarThickness;
	}
	else
	{
		rcPane.top = m_orig[nIndex];
		rcPane.bottom = m_orig[nIndex + 1] - m_nBarThickness;
	}
}

void CSimpleSplitter::GetBarRect(int nIndex, CRect& rcBar) const
{
	ASSERT(nIndex > 0 && nIndex < m_nPanes);
	GetClientRect(&rcBar);
	if (m_nOrientation == SSP_HORZ)
	{
		rcBar.left = m_orig[nIndex] - m_nBarThickness;
		rcBar.right = m_orig[nIndex];
	}
	else
	{
		rcBar.top = m_orig[nIndex] - m_nBarThickness;
		rcBar.bottom = m_orig[nIndex];
	}
}

// CSimpleSplitter protected

void CSimpleSplitter::RecalcSizes()
{
	int* rs = new int[m_nPanes];

	for (int i = 0; i < m_nPanes; i++)
		rs[i] = m_orig[i + 1] - m_orig[i] - m_nBarThickness;
	SetPaneSizes(rs);
	delete[] rs;
}

void CSimpleSplitter::InvertTracker()
{
	CDC* pDC = GetDC();
	CBrush* pBrush = CDC::GetHalftoneBrush();
	HBRUSH hOldBrush;
	
	hOldBrush = (HBRUSH)SelectObject(pDC->m_hDC, pBrush->m_hObject);
	if (m_nOrientation == SSP_HORZ)
		pDC->PatBlt(m_nTracker - m_nBarThickness, 0, m_nBarThickness, m_nTrackerLength, PATINVERT);
	else
		pDC->PatBlt(0, m_nTracker - m_nBarThickness, m_nTrackerLength, m_nBarThickness, PATINVERT);
	if (hOldBrush != NULL)
		SelectObject(pDC->m_hDC, hOldBrush);
	ReleaseDC(pDC);
}

// CSimpleSplitter messages

void CSimpleSplitter::OnPaint() 
{
	CPaintDC dc(this);
	CRect rcClip, rcClipPane;
	COLORREF clrBar = ::GetSysColor(COLOR_BTNFACE);
	int i, x = 0;

	dc.GetClipBox(&rcClip);
	if (m_nOrientation == SSP_HORZ)
		for (i = 1; i < m_nPanes; i++)
			dc.FillSolidRect(m_orig[i] - m_nBarThickness, rcClip.top, m_nBarThickness, rcClip.Height(), clrBar);
	else
		for (i = 1; i < m_nPanes; i++)
			dc.FillSolidRect(rcClip.left, m_orig[i] - m_nBarThickness, rcClip.Width(), m_nBarThickness, clrBar);
}

void CSimpleSplitter::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	 WINDOWPLACEMENT wp;
    GetWindowPlacement(&wp);
	if(!(wp.rcNormalPosition.left == 0 &&
		wp.rcNormalPosition.top == 0 &&
		wp.rcNormalPosition.right == 0 &&
		wp.rcNormalPosition.bottom == 0))
	RecalcSizes();
}

void CSimpleSplitter::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CRect rcClient;
	int mouse_pos = m_nOrientation == SSP_HORZ ? point.x : point.y;

	SetCapture();
	for (m_nTrackIndex = 1; (m_nTrackIndex < m_nPanes && m_orig[m_nTrackIndex] < mouse_pos); m_nTrackIndex++);
	m_nTracker = m_orig[m_nTrackIndex];
	m_nTrackerMouseOffset = mouse_pos - m_nTracker;
	GetClientRect(&rcClient);
	m_nTrackerLength = m_nOrientation == SSP_HORZ ? rcClient.Height() : rcClient.Width();
	InvertTracker();
}

void CSimpleSplitter::OnLButtonUp(UINT nFlags, CPoint point) 
{
	InvertTracker();
	ReleaseCapture();
	m_orig[m_nTrackIndex] = m_nTracker;
	RecalcSizes();
}

void CSimpleSplitter::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (GetCapture() != this) 
		return;
	InvertTracker();
	m_nTracker = (m_nOrientation == SSP_HORZ ? point.x : point.y) - m_nTrackerMouseOffset;
	if (m_nTracker > m_orig[m_nTrackIndex + 1] - m_nBarThickness - m_nMinSize)
		m_nTracker = m_orig[m_nTrackIndex + 1] - m_nBarThickness - m_nMinSize;
	else if (m_nTracker < m_orig[m_nTrackIndex - 1] + m_nBarThickness + m_nMinSize)
		m_nTracker = m_orig[m_nTrackIndex - 1] + m_nBarThickness + m_nMinSize;
	InvertTracker();
}

BOOL CSimpleSplitter::OnNcCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (!CWnd::OnNcCreate(lpCreateStruct))
		return FALSE;

	CWnd* pParent = GetParent();
	ASSERT_VALID(pParent);
	pParent->ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_DRAWFRAME);
	return TRUE;
}

void CSimpleSplitter::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
{
	lpwndpos->flags |= SWP_NOCOPYBITS;
	CWnd::OnWindowPosChanging(lpwndpos);
}