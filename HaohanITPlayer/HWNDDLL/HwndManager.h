// HwndManager.h: interface for the CHwndManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HWNDMANAGER_H__EA4D0DF9_BE46_4089_8D9C_FA6125B506F2__INCLUDED_)
#define AFX_HWNDMANAGER_H__EA4D0DF9_BE46_4089_8D9C_FA6125B506F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Display.h"
#include "HWndDllExport.h"
#include "CWndContainer.h"
#include "afxtempl.h"

#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }								
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }								
#define SAFE_CLOSE(p)   { if(p) { CloseHandle((p));(p)=NULL;} }

#define MAX_CONTAINER 36
class CHwndManager  
{
public:
	CHwndManager();
	virtual ~CHwndManager();

public:	
    int     Init( HWND hParentWnd );
	void	Clear( int spliteHandle );
    HWND    GetHWnd( int spliteHandle, int index );
	int		GetIndex( int spliteHandle, HWND hWnd );
    int     GetFocus( int spliteHandle );
    bool    SetSplitMode( int spliteHandle, int nPlayer,int num, int order = 0 );
    void    OnUpdateFocus( int spliteHandle, POINT pt ) ;
    bool	SetFocus( int spliteHandle, int index );
	void	ResizeWindow();
	void	SetPaintBG( int spliteHandle, int index, BOOL bPaitBG );
	BOOL	GetVisible( int spliteHandle, int index);	//2008-08-11
	bool    CheckHandle(int spliteHandle);
	void	SetOsdText(int spliteHandle, int index, int line, char* text, COLORREF crColor);
private:
    //void    UpdateSplitRect( int num /* 0,1,2,3,4*/ , HWND hWnd, int order = 0);
	//void	CreateWndByRect();
    //void    UpdatePanelByRect( int order = 0);
    //bool    DrawWindow( int index );
    //void    VisibleAll(bool bVisible);
private:
	//CDisplay*		 m_pDisplays[MAX_CLIENT_WINDOW];
    //RECT             m_ClientRect[MAX_CLIENT_WINDOW];
    //int              m_DisplayMode;
    //HWND			 m_hParentWnd;
    //int              m_CurrentClient;
    //int              m_ClientNum;
	CWndContainer*	 m_wndContainer[MAX_CONTAINER];
	//CMap<HWND, HWND, int, int> m_mHWnd;
};

#endif // !defined(AFX_HWNDMANAGER_H__EA4D0DF9_BE46_4089_8D9C_FA6125B506F2__INCLUDED_)
