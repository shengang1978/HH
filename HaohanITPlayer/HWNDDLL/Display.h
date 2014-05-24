#if !defined(AFX_DISPLAY_H__7771DE09_3FA1_4116_BDBC_DA54CB2C6967__INCLUDED_)
#define AFX_DISPLAY_H__7771DE09_3FA1_4116_BDBC_DA54CB2C6967__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Display.h : header file
//
#define		MAX_CLIENT_WINDOW	64
/////////////////////////////////////////////////////////////////////////////
// CDisplay window

#define MAX_OSD_STRING	100
struct OSD_DISPLAY
{   
    OSD_DISPLAY()
	{	
		memset( this, 0x00, sizeof(OSD_DISPLAY) );
	}    
    //窗口OSD字体叠加 
	int			m_config;
    char		m_Name1  [MAX_OSD_STRING];
    char		m_Name2  [MAX_OSD_STRING];
	char		m_Name3  [MAX_OSD_STRING];
	char		m_Name4  [MAX_OSD_STRING];

	COLORREF	m_crColor1;
	COLORREF	m_crColor2;
	COLORREF	m_crColor3;
	COLORREF	m_crColor4;
} ;

class CDisplay : public CWnd
{
// Construction
public:
	CDisplay();

// Attributes
public:

// Operations
public:
	BOOL CreateWnd ( HWND hWnd, CRect& rect );
	VOID UpdateRect(CRect& rect);
	VOID SetVisible( BOOL bVisible );
	VOID SetFocusWnd( BOOL bFocus );
	//指示是否需要刷新背景
	VOID SetPaintBG( BOOL bPaitBG );

	void SetOsdText(int line, char* text,COLORREF crColor);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDisplay)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDisplay();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDisplay)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	HWND	m_hParentWnd;
	BOOL	m_bFocus;
	BOOL	m_bPaintBG;//默认是刷新背景
	HBITMAP   m_Logo             ;                   //LOGO

	OSD_DISPLAY	m_osdDisplay;

public:
	BOOL m_bVisible;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DISPLAY_H__7771DE09_3FA1_4116_BDBC_DA54CB2C6967__INCLUDED_)
