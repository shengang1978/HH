// HwndManager.h: interface for the CWndContainer class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __CWNDCONTAINER_H
#define __CWNDCONTAINER_H

#include "Display.h"
#include "HWndDllExport.h"


class CWndContainer : public CWnd 
{
public:
	CWndContainer();
	virtual ~CWndContainer();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDisplay)
public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	//}}AFX_VIRTUAL
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CWndContainer)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:	
	BOOL	CreateWnd ( );
    bool    Init(HWND hParent);
	void	Clear();
    HWND    GetHWnd(int index );
	int		GetIndex(HWND hWnd );
    int     GetFocus();
    bool    SetSplitMode(int nPlayer,int num, int order = 0);
    void    OnUpdateFocus( POINT pt ) ;
    bool	SetFocus( int index );
	void	ResizeWindow();
	void	SetPaintBG(int index, BOOL bPaitBG );
	BOOL	GetVisible(int index);	//2008-08-11

	void	SetOsdText(int index, int line, char* text,COLORREF crColor);

private:
    void    UpdateSplitRect( int num /* 0,1,2,3,4*/ , HWND hWnd, int order = 0);
	void	CreateWndByRect();
    void    UpdatePanelByRect( int order = 0);
    bool    DrawWindow( int index );
    void    VisibleAll(bool bVisible);
	void	DrawFocus( CDisplay* pDisplay, BOOL bFocus );
private:
	CDisplay*		 m_pDisplays[MAX_CLIENT_WINDOW];
    RECT             m_ClientRect[MAX_CLIENT_WINDOW];
    int              m_DisplayMode;
    HWND			 m_hParentWnd;
    int              m_CurrentClient;
    int              m_ClientNum;
	int				 m_SPLIT_4_order; //第几个4分割 0,1,2,3
};

#endif //
