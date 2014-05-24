// Display.cpp : implementation file
//

#include "stdafx.h"
#include "HWndDll.h"
#include "Display.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//视频窗口之间的间隔
#define WINDOW_SPACE  1

#define BACK_COLOR_BLACK 1


#if _MSC_VER >= 1300    // for VC 7.0
// from ATL 7.0 sources
#ifndef _delayimp_h
extern "C" IMAGE_DOS_HEADER __ImageBase;
#endif
#endif

HMODULE GetCurrentModule()
{
#if _MSC_VER < 1300    // earlier than .NET compiler (VC 6.0)
	
	// Here's a trick that will get you the handle of the module
	// you're running in without any a-priori knowledge:
	// http://www.dotnet247.com/247reference/msgs/13/65259.aspx
	
	MEMORY_BASIC_INFORMATION mbi;
	static int dummy;
	VirtualQuery( &dummy, &mbi, sizeof(mbi) );
	
	return reinterpret_cast<HMODULE>(mbi.AllocationBase);
	
#else    // VC 7.0
	
	// from ATL 7.0 sources
	
	return reinterpret_cast<HMODULE>(&__ImageBase);
#endif
}

BOOL ExtractFilePath( HMODULE hModule, CHAR* chPath )
{
	char path[256] = {0X00};
	GetModuleFileName(hModule,(char*)path,sizeof(path));
	//CString strPath = path;
	char* pos = strrchr( path, '\\' );
	if( pos == NULL)
		return FALSE;
	*(pos + 1) = 0;
	strncpy( chPath, path, (pos + 1) - path );
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CDisplay

CDisplay::CDisplay()
{
	m_bFocus = FALSE;
	m_bPaintBG = TRUE;
	m_Logo               = NULL;
	m_bVisible = TRUE;	//2008-08-11
}
 
CDisplay::~CDisplay()
{
	if( m_Logo )
    {
        DeleteObject( m_Logo );
        m_Logo = NULL ;
    }
	DestroyWindow();
}


BEGIN_MESSAGE_MAP(CDisplay, CWnd)
	//{{AFX_MSG_MAP(CDisplay)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDisplay message handlers

BOOL CDisplay::CreateWnd ( HWND hWnd, CRect& rect )
{	
    rect.DeflateRect(WINDOW_SPACE,WINDOW_SPACE,WINDOW_SPACE,WINDOW_SPACE);
	char	path[_MAX_PATH] = {0x00};
	ExtractFilePath( GetCurrentModule(), path );
	strcat( path, "Logo.bmp" );
	m_Logo  = (HBITMAP)LoadImage(NULL, path , IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);
	m_hParentWnd = hWnd;
	//::GetClientRect ( hWnd ,  &rect );
    if(  CreateEx( WS_EX_TOPMOST /*|  WS_EX_CLIENTEDGE*/, NULL, NULL , WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 
		rect.left  , rect.top , rect.Width() , rect.Height() , hWnd, NULL ) )
	{ 
		ShowWindow( SW_SHOW ) ; 	           //显示背景窗口    	    	    
		return TRUE ;
    }	
    return FALSE ;
}

void CDisplay::OnPaint() 
{
	CPaintDC dc(this);
	CRect rect;
	GetClientRect(&rect);
	if( m_bPaintBG )
	{
		if( m_Logo )
		{
			BITMAP     bm ;
			CDC  hdcImage ;
			
			hdcImage.CreateCompatibleDC( NULL ) ;
			hdcImage.SelectObject( m_Logo ) ;
			GetObject( m_Logo , sizeof(bm), &bm );
			
			dc.StretchBlt(  rect.left , rect.top , rect.right - rect.left , rect.bottom - rect.top , 
				&hdcImage  , 0 , 0 , bm.bmWidth , bm.bmHeight , SRCCOPY);
			
			hdcImage.DeleteDC  ();
		}
		else
		{
#ifdef BACK_COLOR_BLACK
			dc.FillSolidRect( &rect, RGB( 30 , 30, 30 ) );
#else
			dc.FillSolidRect( &rect, RGB( 32 , 32, 96 ) );
#endif
		}
	}
	int x = 0;
	int y = 0;
	if( m_osdDisplay.m_config & 1 << 0 )
	{
		x = 1;
		y = 0;
		dc.SetTextColor(m_osdDisplay.m_crColor1);
		dc.TextOut(x, y, m_osdDisplay.m_Name1 );
	}
	if( m_osdDisplay.m_config & 1 << 1 )
	{
		x = 1;
		y += 20;
		dc.SetTextColor(m_osdDisplay.m_crColor2);
		dc.TextOut(x, y, m_osdDisplay.m_Name2 );
	}
	if( m_osdDisplay.m_config & 1 << 2 )
	{
		x = 1;
		y += 20;
		dc.SetTextColor(m_osdDisplay.m_crColor3);
		dc.TextOut(x, y, m_osdDisplay.m_Name3 );
	}
	if( m_osdDisplay.m_config & 1 << 3 )
	{
		x = 1;
		y += 20;
		dc.SetTextColor(m_osdDisplay.m_crColor4);
		dc.TextOut(x, y, m_osdDisplay.m_Name4 );
	}

	//if( m_bFocus )
	//{
		//dc.Draw3dRect( &rect , RGB( 255 , 255 , 255 ) , RGB( 255 , 255 , 255 ) );        
	//	dc.Draw3dRect( &rect , RGB( 0 , 255 , 0 ) , RGB( 0 , 255 , 0 ) );         //focus rect 
	//}
	//else
	//	dc.Draw3dRect( &rect , RGB( 255 , 255 , 255 ) , RGB( 255 , 255 , 255 ) ); //none focus rect 
}

BOOL CDisplay::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	return CWnd::OnEraseBkgnd(pDC);
	//return TRUE;
}

VOID CDisplay::UpdateRect(CRect& rect)
{	
	rect.DeflateRect(WINDOW_SPACE,WINDOW_SPACE,WINDOW_SPACE,WINDOW_SPACE);
	::MoveWindow(m_hWnd, rect.left, rect.top, rect.Width(), rect.Height(), TRUE);
}

VOID CDisplay::SetVisible( BOOL bVisible )
{
	ShowWindow( bVisible );
	m_bVisible = bVisible;	//2008-08-11
}

VOID CDisplay::SetFocusWnd( BOOL bFocus )
{
	m_bFocus = bFocus;
	Invalidate(TRUE);
}

void CDisplay::OnLButtonDown(UINT nFlags, CPoint point) 
{
	//m_bPaintBG = TRUE;
	POINT	ptScreen;
	ptScreen.x = point.x;
	ptScreen.y = point.y;
	ClientToScreen(&ptScreen);
	::ScreenToClient(m_hParentWnd, &ptScreen);//转化为m_hParentWnd的坐标

	//发送到父窗口中去
	::PostMessage( GetParent()->GetSafeHwnd() , WM_LBUTTONDOWN , MK_RBUTTON , MAKELPARAM( ptScreen.x , ptScreen.y ) );  
	CWnd::OnLButtonDown(nFlags, point);
}

void CDisplay::OnRButtonDown(UINT nFlags, CPoint point) 
{
	//m_bPaintBG = TRUE;
	POINT	ptScreen;
	ptScreen.x = point.x;
	ptScreen.y = point.y;
	ClientToScreen(&ptScreen);
	if( !::ScreenToClient(m_hParentWnd, &ptScreen) )//转化为m_hParentWnd的坐标
	{
		DWORD dwError = ::GetLastError();
	}
	//发送到父窗口中去
	::PostMessage( GetParent()->GetSafeHwnd() , WM_RBUTTONDOWN , MK_RBUTTON , MAKELPARAM( ptScreen.x , ptScreen.y ) );
	CWnd::OnRButtonDown(nFlags, point);
}

void CDisplay::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	POINT	ptScreen;
	ptScreen.x = point.x;
	ptScreen.y = point.y;
	ClientToScreen(&ptScreen);
	::ScreenToClient(m_hParentWnd, &ptScreen);//转化为m_hParentWnd的坐标
	
	::PostMessage( GetParent()->GetSafeHwnd() , WM_LBUTTONDBLCLK , MK_RBUTTON , MAKELPARAM( ptScreen.x , ptScreen.y ) );	
	CWnd::OnLButtonDblClk(nFlags, point); 
}

VOID CDisplay::SetPaintBG(BOOL bPaitBG )
{
	m_bPaintBG = bPaitBG;
}

BOOL CDisplay::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

void CDisplay::SetOsdText(int line, char* text,COLORREF crColor)
{
	if( line == 0 )
	{
		m_osdDisplay.m_config |= 1 << 0;
		strcpy( m_osdDisplay.m_Name1, text );
		m_osdDisplay.m_crColor1 = crColor;
	}
	if( line == 1 )
	{
		m_osdDisplay.m_config |= 1 << 1;
		strcpy( m_osdDisplay.m_Name2, text );
		m_osdDisplay.m_crColor2 = crColor;
	}
	if( line == 2 )
	{
		m_osdDisplay.m_config |= 1 << 2;
		strcpy( m_osdDisplay.m_Name3, text );
		m_osdDisplay.m_crColor3 = crColor;
	}
	if( line == 3 )
	{
		m_osdDisplay.m_config |= 1 << 3;
		strcpy( m_osdDisplay.m_Name4, text );
		m_osdDisplay.m_crColor4 = crColor;
	}
	Invalidate(TRUE);
}
