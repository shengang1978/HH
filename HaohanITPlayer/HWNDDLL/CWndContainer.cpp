// HwndManager.cpp: implementation of the CWndContainer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HWndDll.h"
#include "CWndContainer.h"
//#include "CacheCfg.h"

//分割映射表
typedef struct
{
	int  count ;
}SPLIT_MAP ;

#define INTERVER_VALUE	2

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
SPLIT_MAP    m_SplitMap[] = {
    {   1   } ,
    {   4	} ,
    {   6	} ,
    {   9	} ,
    {   16  } ,
	{   25  } ,
	{   36  } ,
	{   3   } ,
	{   6   } ,
	{   49  } ,
	{   64  } ,
	
};

int m_SplitFactor[11] = { 1 , 2 , 3, 3, 4,5,6, 2,3,7,8} ; //分割画面的放大因子

CWndContainer::CWndContainer()
{
    memset( m_pDisplays, 0x00, sizeof(m_pDisplays) );
	m_CurrentClient = 0;
    m_ClientNum = 0;
    for( int i = 0; i < MAX_CLIENT_WINDOW; ++i )
    {
        CDisplay* pDisplay = new CDisplay();
        m_pDisplays[i] = pDisplay;
    }
    m_DisplayMode = SPLIT_64;//SPLIT_16;
	m_SPLIT_4_order = -1;
//	ReadCacheConfig(g_CacheIni);
}
CWndContainer::~CWndContainer()
{
    Clear();
	DestroyWindow();
}

BEGIN_MESSAGE_MAP(CWndContainer, CWnd)
	//{{AFX_MSG_MAP(CDisplay)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

bool CWndContainer::Init(HWND hParent)
{
    m_hParentWnd = hParent;
	CreateWnd( );//创建容器窗体
	
	UpdateSplitRect(SPLIT_64, m_hWnd);//SPLIT_16
    CreateWndByRect();//创建子窗体

    return true;
}

void CWndContainer::Clear()
{
	for( int i = 0; i < MAX_CLIENT_WINDOW; ++i )
	{
		delete m_pDisplays[i];
		m_pDisplays[i] = NULL;
	}
}

void CWndContainer::UpdateSplitRect( int num /* 0,1,2,3,4*/, HWND hWnd, int order)
{
    int i , w , h  , factor , index ;
    RECT  rect ;

    ::GetClientRect( hWnd,&rect ) ;
    m_DisplayMode = num ;
    factor  =  m_SplitFactor[ m_DisplayMode ]  ;
	w = (rect.right - rect.left)/factor ;
	h = (rect.bottom - rect.top)/factor;
	if(m_DisplayMode == SPLIT_3) //三分割
	{
		w = (rect.right - rect.left)/3;
		h = (rect.bottom - rect.top)/2;
		for(int j = 0 ; j < 3; ++j)
		{
			index = j;
			switch(j)
			{
			case 0:
				{
					m_ClientRect[index].top = 0*h;
					m_ClientRect[index].left = 0*w;
					m_ClientRect[index].bottom = m_ClientRect[index].top + h*2;
					m_ClientRect[index].right = m_ClientRect[index].left + w*2;
					break;
				}
			case 1:
				{
					m_ClientRect[index].top = 0;
					m_ClientRect[index].left = 2*w;
					m_ClientRect[index].bottom = m_ClientRect[index].top + h;
					m_ClientRect[index].right = m_ClientRect[index].left + w;	
					break;
				}
			case 2:
				{
					m_ClientRect[index].top = h;
					m_ClientRect[index].left = 2*w;
					m_ClientRect[index].bottom = m_ClientRect[index].top + h;
					m_ClientRect[index].right = m_ClientRect[index].left + w;
					break;
				}
			default:
				break;
			}		
		}	
	}
    else if( m_DisplayMode == SPLIT_6 )//原有的6分割
    {
		for( int i = 0; i < 6; ++i )
		{
			index  =  i ;
			
			switch( i )
			{
			case 0 :
				{
					m_ClientRect[index].top  = 0*h  ;
					m_ClientRect[index].left = 0*w;
					
					//6分割模式下第一个窗口比较大
					m_ClientRect[index].bottom  =  m_ClientRect[index].top  + h ;
					m_ClientRect[index].right   =  m_ClientRect[index].left + w ;
					m_ClientRect[index].bottom *= 2 ;
					m_ClientRect[index].right  *= 2 ;
				}
				break;
			case 1 :
				{
					m_ClientRect[index].top  = 0  ;
					m_ClientRect[index].left = w*2;
					
					//6分割模式下第一个窗口比较大
					m_ClientRect[index].bottom  =  m_ClientRect[index].top  + h ;
					m_ClientRect[index].right   =  m_ClientRect[index].left + w ;
				}
				break;
			case 2 :
				{
					m_ClientRect[index].top  = h ;
					m_ClientRect[index].left = w*2;
					
					//6分割模式下第一个窗口比较大
					m_ClientRect[index].bottom  =  m_ClientRect[index].top  + h ;
					m_ClientRect[index].right   =  m_ClientRect[index].left + w ;
				}
				break;
			case 3 :
				{
					m_ClientRect[index].top  = h * 2;
					m_ClientRect[index].left = 0;
					
					//6分割模式下第一个窗口比较大
					m_ClientRect[index].bottom  =  m_ClientRect[index].top  + h ;
					m_ClientRect[index].right   =  m_ClientRect[index].left + w ;
				}
				break;
			case 4 :
				{
					m_ClientRect[index].top  = h *2 ;
					m_ClientRect[index].left =w;
					
					//6分割模式下第一个窗口比较大
					m_ClientRect[index].bottom  =  m_ClientRect[index].top  + h ;
					m_ClientRect[index].right   =  m_ClientRect[index].left + w ;
				}
				break;
			case 5 :
				{
					m_ClientRect[index].top  = h*2  ;
					m_ClientRect[index].left = w*2;
					
					//6分割模式下第一个窗口比较大
					m_ClientRect[index].bottom  =  m_ClientRect[index].top  + h ;
					m_ClientRect[index].right   =  m_ClientRect[index].left + w ;
				}
				break;
			default :
				break;
			}
		}
    }
	else if( m_DisplayMode == SPLIT_6_2 )//特殊6分割 20100613
	{		
		w = (rect.right - rect.left)/3 ;
		h = (rect.bottom - rect.top)/2;//特殊的六分割
		
		for( int i = 0; i < 6; ++i )
		{
			index  =  i ;
			w = (rect.right - rect.left)/3 ;
			h = (rect.bottom - rect.top)/2;
			
			switch( i )
			{
            case 0 :
				{
					m_ClientRect[index].top  = 0*h  ;
					m_ClientRect[index].left = 0*w;
					
					//6分割模式下第一个窗口比较大
					m_ClientRect[index].bottom  =  m_ClientRect[index].top  + h ;
					m_ClientRect[index].right   =  m_ClientRect[index].left + w ;
					//m_ClientRect[index].bottom *= 2 ;
					//m_ClientRect[index].right  *= 2 ;
				}
				break;
            case 1 :
				{
					m_ClientRect[index].top  = 0  ;
					m_ClientRect[index].left = w*1;
					
					//6分割模式下第一个窗口比较大
					m_ClientRect[index].bottom  =  m_ClientRect[index].top  + h ;
					m_ClientRect[index].right   =  m_ClientRect[index].left + w ;
				}
				break;
            case 2 :
				{
					m_ClientRect[index].top  = 0 ;
					m_ClientRect[index].left = w*2;
					
					//6分割模式下第一个窗口比较大
					m_ClientRect[index].bottom  =  m_ClientRect[index].top  + h ;
					m_ClientRect[index].right   =  m_ClientRect[index].left + w ;
				}
				break;
            case 3 :
				{
					m_ClientRect[index].top  = h * 1;
					m_ClientRect[index].left = 0;
					
					//6分割模式下第一个窗口比较大
					m_ClientRect[index].bottom  =  m_ClientRect[index].top  + h ;
					m_ClientRect[index].right   =  m_ClientRect[index].left + w ;
				}
				break;
            case 4 :
				{
					m_ClientRect[index].top  = h *1 ;
					m_ClientRect[index].left =w;
					
					//6分割模式下第一个窗口比较大
					m_ClientRect[index].bottom  =  m_ClientRect[index].top  + h ;
					m_ClientRect[index].right   =  m_ClientRect[index].left + w ;
				}
				break;
            case 5 :
				{
					m_ClientRect[index].top  = h*1  ;
					m_ClientRect[index].left = w*2;
					
					//6分割模式下第一个窗口比较大
					m_ClientRect[index].bottom  =  m_ClientRect[index].top  + h ;
					m_ClientRect[index].right   =  m_ClientRect[index].left + w ;
				}
				break;
            default :
				break;
			}
		}
	}
    else
    {
		
		int count = m_SplitMap[ m_DisplayMode ].count;
        for( i = 0; i < count; ++i )
        {
			if( m_DisplayMode == SPLIT_4 )
			{//2008-07-29
				index = i + (4 * order);
			}
			else
				index = i;

			if( m_DisplayMode == SPLIT_1 )
				index = m_CurrentClient;

			if( index < MAX_CLIENT_WINDOW)
			{
				m_ClientRect[index].top  = i/factor*h  ;
				m_ClientRect[index].left = (i%factor)*w;

				m_ClientRect[index].bottom  =  m_ClientRect[index].top  + h ;
				m_ClientRect[index].right   =  m_ClientRect[index].left + w ;
			}
        }
    }
}

BOOL CWndContainer::CreateWnd ( )
{	
    char	path[_MAX_PATH] = {0x00};
	//ExtractFilePath( GetCurrentModule(), path );
	//strcat( path, "Logo.bmp" );
	//m_Logo  = (HBITMAP)LoadImage(NULL, path , IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);
	//m_hParentWnd = hWnd;
	CRect	rect;
	::GetClientRect(m_hParentWnd, &rect);
	rect.left = rect.left + INTERVER_VALUE;//留出边框
	rect.top = rect.top + INTERVER_VALUE;
	rect.bottom = rect.bottom - INTERVER_VALUE;
	rect.right = rect.right - INTERVER_VALUE;
	//::GetClientRect ( hWnd ,  &rect );
    if(  CreateEx( WS_EX_TOPMOST /*|  WS_EX_CLIENTEDGE*/, NULL, NULL , WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 
		rect.left  , rect.top , rect.Width() , rect.Height() , m_hParentWnd, NULL ) )
	{ 
		ShowWindow( SW_SHOW ) ; 	           //显示背景窗口    	    	    
		return TRUE ;
    }	
    return FALSE ;
}

HWND CWndContainer::GetHWnd(int index )
{
    if(index < 0 || index >=  MAX_CLIENT_WINDOW )
        return NULL;

    //int mapindex =  m_SplitMap[ m_DisplayMode ].map[index];
	int mapindex = index;
    return m_pDisplays[mapindex]->m_hWnd;
}

int CWndContainer::GetIndex(HWND hWnd )
{	
	for( int i = 0; i < MAX_CLIENT_WINDOW; ++i )
	{
		if( !m_pDisplays[i] )
			continue;
		if( m_pDisplays[i]->m_hWnd == hWnd )
			return i;
	}
	return -1;
}

bool CWndContainer::SetSplitMode(int player, int mode, int order)
{
    CRect	rect;
	::GetClientRect( m_hParentWnd, &rect );
	rect.left = rect.left + INTERVER_VALUE;//留出边框
	rect.top = rect.top + INTERVER_VALUE;
	rect.bottom = rect.bottom - INTERVER_VALUE;
	rect.right = rect.right - INTERVER_VALUE;
	::MoveWindow(m_hWnd, rect.left, rect.top, rect.Width(), rect.Height(), TRUE);

	if( mode == SPLIT_1 )
		m_CurrentClient = player;

    //m_preDisplayMode = m_DisplayMode;//记录原来模式，为了在显示一分割时现在当前活动窗体
    m_DisplayMode = mode;
	m_SPLIT_4_order = order;
    UpdateSplitRect(m_DisplayMode,m_hWnd, order);
    UpdatePanelByRect(order);
	DrawWindow( m_CurrentClient );
    return true;
}

void  CWndContainer::OnUpdateFocus( POINT pt )
{  
	int i , index , preCurrentClient ;
    RECT   rect    ;
    
    int count = m_SplitMap[ m_DisplayMode ].count;
	if( m_DisplayMode == SPLIT_1 )
		count = MAX_CLIENT_WINDOW;


	for( i = 0 ; i < count ; i++ )
    {
		int nNo = i;
		if (m_DisplayMode == SPLIT_4)	//2008-11-25 ma add
		{
			if(m_SPLIT_4_order < 0 || m_SPLIT_4_order > 3)
				m_SPLIT_4_order = 0;
			nNo += m_SPLIT_4_order * 4; 
		}		
		index = nNo;
		//if( m_DisplayMode == SPLIT_1 )
		//	index = m_CurrentClient;
        rect  = m_ClientRect[index] ;             
        
        if( PtInRect( &rect , pt ) )
        {
			if( nNo != m_CurrentClient )
			{    
			    if( m_DisplayMode == SPLIT_1 )
				{
			        DrawWindow( m_CurrentClient );
					break ;	
				}
			        				              
                //焦点切换
				preCurrentClient	=  m_CurrentClient;
				m_CurrentClient		= nNo;

				DrawWindow( m_CurrentClient );
				DrawWindow( preCurrentClient );
			}
			else
				DrawWindow( m_CurrentClient );
            break ;
        }
    }
}

int CWndContainer::GetFocus()
{
    return m_CurrentClient;
}

bool CWndContainer::DrawWindow( int index )
{                  
    int   limit = m_SplitMap[m_DisplayMode].count ;
    if( m_DisplayMode == SPLIT_1 || m_DisplayMode == SPLIT_4)
		limit = MAX_CLIENT_WINDOW;
    
    if( index < limit )
    {
		CDisplay* pDisplay = m_pDisplays[index];
		if( index == m_CurrentClient || m_DisplayMode == SPLIT_1 ) 
		{
			DrawFocus( pDisplay, TRUE );
        }
		else
        {
			DrawFocus( pDisplay, FALSE );
        }
    }
           
	return true ;
}
void CWndContainer::UpdatePanelByRect(int order)
{
    VisibleAll(false);
	
    CDisplay* pDisplay = NULL;
    RECT rect;
    int index = 0;
	/*
	for( index = m_SplitMap[ m_DisplayMode ].count; index < MAX_CLIENT_WINDOW; ++index )
	{
        //if( index == m_CurrentClient )
		//	continue;
		pDisplay = m_pDisplays[index];
        if(pDisplay != NULL)
        {
			CRect	crect;
			memset( &crect, 0x00, sizeof(crect) );
			crect.left		= 0;
			crect.top		= 0;
			crect.right		= 2;
			crect.bottom	= 2;
			pDisplay->UpdateRect(crect);
        }
	}
	*/
    for( int i = 0; i < m_SplitMap[ m_DisplayMode ].count; ++i )
    {
	//2008-07-30	index = i;
		if( m_DisplayMode == SPLIT_4 )
		{//2008-07-30
			index = i + (4 * order);
		}
		else
			index = i;

		BOOL	bPaintBG = TRUE;
        if(m_DisplayMode == 0)//用有焦点的画面填充单画面
		{
			index = m_CurrentClient;
			//bPaintBG = FALSE;
		}

        pDisplay = m_pDisplays[index];
        if(pDisplay != NULL)
        {
            pDisplay->SetVisible( true );
            rect = m_ClientRect[index];
			CRect	crect(rect);
			pDisplay->UpdateRect(crect);
        }
    }
	
}

void CWndContainer::CreateWndByRect()
{
    CDisplay* pDisplay = NULL;
    RECT rect;
    int index = 0;
    for( int i = 0; i < m_SplitMap[ m_DisplayMode ].count; ++i )
    {
		index = i;
        if(m_DisplayMode == 0)
            pDisplay = m_pDisplays[m_CurrentClient];
        else
            pDisplay = m_pDisplays[index];
        if(pDisplay != NULL)
        {
            rect = m_ClientRect[index];
			CRect	crect(rect);
			pDisplay->CreateWnd( m_hWnd, crect );
        }
    }
	DrawFocus( m_pDisplays[0], TRUE );
}

void CWndContainer::VisibleAll(bool bVisible)
{
	for(int i = 0 ; i < MAX_CLIENT_WINDOW; ++i)
    {
        m_pDisplays[i]->SetVisible( bVisible );
    }
}

bool CWndContainer::SetFocus( int index )
{
	int i ,  j ;
    
    for( i = 0 ; i < m_SplitMap[ m_DisplayMode ].count ; i++ )
    {
        if(  i == index )
        {
			if( i != m_CurrentClient )
			{    
				if( m_DisplayMode == SPLIT_1 )
					break ;			    			    			        				
				
                //焦点切换
				j =  m_CurrentClient;
				m_CurrentClient = i;
				
				DrawWindow( i );
				DrawWindow( j );
			}
            break ;
        }
    }
	return TRUE;
}

void CWndContainer::ResizeWindow()
{

}

void CWndContainer::SetPaintBG(int index, BOOL bPaitBG )
{
	if(index < 0 || index >=  MAX_CLIENT_WINDOW )
        return;	
	int mapindex = index;
    m_pDisplays[mapindex]->SetPaintBG(bPaitBG);
}

BOOL CWndContainer::GetVisible(int index)
{
	if(index < 0 || index >=  MAX_CLIENT_WINDOW )
        return false;	
	
    return m_pDisplays[index]->m_bVisible;
}



void CWndContainer::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	int count = m_SplitMap[ m_DisplayMode ].count;//MAX_CLIENT_WINDOW
	if (SPLIT_4 == m_DisplayMode) 
		count = MAX_CLIENT_WINDOW;
	for( int i = 0; i < MAX_CLIENT_WINDOW; ++i )
	{
		//if( i == m_CurrentClient )
		//	DrawFocus( m_pDisplays[i], TRUE );
		//else
			DrawFocus( m_pDisplays[i], FALSE );
	}
	if (m_CurrentClient <= count - 1) 
	{
		DrawFocus( m_pDisplays[m_CurrentClient], TRUE );
	}	
}

BOOL CWndContainer::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	return CWnd::OnEraseBkgnd(pDC);
	//return TRUE;
}

void CWndContainer::OnLButtonDown(UINT nFlags, CPoint point) 
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

void CWndContainer::OnRButtonDown(UINT nFlags, CPoint point) 
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

void CWndContainer::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	POINT	ptScreen;
	ptScreen.x = point.x;
	ptScreen.y = point.y;
	ClientToScreen(&ptScreen);
	::ScreenToClient(m_hParentWnd, &ptScreen);//转化为m_hParentWnd的坐标
	
	::PostMessage( GetParent()->GetSafeHwnd() , WM_LBUTTONDBLCLK , MK_RBUTTON , MAKELPARAM( ptScreen.x , ptScreen.y ) );	
	CWnd::OnLButtonDblClk(nFlags, point); 
}

BOOL CWndContainer::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

void CWndContainer::DrawFocus( CDisplay* pDisplay, BOOL bFocus )
{
	CRect rt;
	pDisplay->GetWindowRect(&rt);
	ScreenToClient(&rt);
	rt.InflateRect(1,1);

	CDC *pDC=GetDC();
	if(!pDC) 
		return;

	if(bFocus)
	{
#ifdef HARD_CARD_DECODER//为解码卡程序定义
		pDC->Draw3dRect(&rt, RGB( 0 , 0 , 255 ) , RGB( 0 , 0 , 255 ) );
#else 
		pDC->Draw3dRect(&rt, RGB( 0 , 255 , 0 ) , RGB( 0 , 255 , 0 ) );
#endif
		
	}
	else
	{
#ifdef  BACK_COLOR_BLACK
		{
#define NONE_FOCUS_COLOR	120
			pDC->Draw3dRect( &rt , RGB( NONE_FOCUS_COLOR , NONE_FOCUS_COLOR , NONE_FOCUS_COLOR ) ,
								RGB( NONE_FOCUS_COLOR , NONE_FOCUS_COLOR , NONE_FOCUS_COLOR ) );
		}
#else
		{
#define NONE_FOCUS_COLOR	180
			pDC->Draw3dRect( &rt , RGB( NONE_FOCUS_COLOR , NONE_FOCUS_COLOR , NONE_FOCUS_COLOR ) ,
				RGB( NONE_FOCUS_COLOR , NONE_FOCUS_COLOR , NONE_FOCUS_COLOR ) ); //none focus rect 
		}
#endif
	}
	ReleaseDC(pDC);
}

void CWndContainer::SetOsdText(int index, int line, char* text,COLORREF crColor)
{
	if(index < 0 || index >=  MAX_CLIENT_WINDOW )
        return;	
	int mapindex = index;
    m_pDisplays[mapindex]->SetOsdText(line, text, crColor);
}
