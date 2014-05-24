#include "stdafx.h"
#include "HWndManager.h"
#include "./CommClass.h"
//---------------------------------------------------------------------------
CHWndManager::CHWndManager()
{
	m_hParentWnd = NULL;
	m_SpliteHandle = -1;
    m_videoWindowNum = MAX_CLIENT_WINDOW;
}

//----------------------------------------------------------------------------
CHWndManager::~CHWndManager()
{
	if(m_SpliteHandle >= 0) //先启动监视，然后服务器回放两次后m_SpliteHandle = -1，IE崩溃
	{
		if(HWnd_Clear)	
			HWnd_Clear(m_SpliteHandle);
	}
	FreeHWndDll();
}

//-----------------------------------------------------------------------------
int CHWndManager::InitSplit(HWND hParentWnd)
{
	if ( LoadHWndDll() < 0 )
		return -1;

	m_hParentWnd = hParentWnd;  	
	m_SpliteHandle = HWnd_Init(m_hParentWnd);
	if(m_SpliteHandle < 0)
		return -1;

    //窗口都初始化为空闲状态
    for (int i = 0; i < MAX_CLIENT_WINDOW; i++)
	{
		HWND hWnd = HWnd_GetHWnd(m_SpliteHandle, i);
        m_mWndFree[i].index = i;
        m_mWndFree[i].hWnd = hWnd;
        m_mWndFree[i].bFree = true;
    }
    return 0;
}

int CHWndManager::GetFocus()
{
	return HWnd_GetFocus(m_SpliteHandle);
}
//-----------------------------------------------------------------------------
void CHWndManager::SetFocus( int index )
{
	HWnd_SetFocus(m_SpliteHandle, index);
}

HWND CHWndManager::GetHWnd( int index )
{
	if (index < 0)
		return NULL;
		
	return HWnd_GetHWnd(m_SpliteHandle, index);
}

HWND CHWndManager::GetFocusHWnd(  )
{
	return GetHWnd( GetFocus() );
}

void   CHWndManager::SelectChannel(POINT pt)
{
	if (m_SpliteHandle != -1)//如果m_SpliteHandle = -1 服务器回放的时候点击播放窗口IE崩溃
	{
		HWnd_OnUpdateFocus(m_SpliteHandle, pt);
	}
}

//------------------------------------------------------------------------------
void CHWndManager::SetSplitMode ( int currIndex, int num, int clickNum )
{
	if (num == 0 && currIndex != 0)
	{
		currIndex = 0;
	}

    HWnd_SetSplitMode(m_SpliteHandle, currIndex, num);
    //HWnd_SetSplitMode_WithOrder(m_SpliteHandle, currIndex, num, clickNum);
    switch(num)
    {
        case SPLIT_1:
            m_videoWindowNum = 1;
            break;
        case SPLIT_4:
            m_videoWindowNum = 4;
            break;
        case SPLIT_6:
            m_videoWindowNum = 6;
            break;
        case SPLIT_9:
            m_videoWindowNum = 9;
            break;
        case SPLIT_16:
            m_videoWindowNum = 16;
            break;
        case SPLIT_25:
            m_videoWindowNum = 25;
            break;
        case SPLIT_36:
            m_videoWindowNum = 36;
            break;
        case SPLIT_3:
            m_videoWindowNum = 3;
            break;
        case SPLIT_6_2:
            m_videoWindowNum = 7; // 实际分割数是6，为了与SPLIT_6相区别，将6改为7 
            break;
        case SPLIT_49:
            m_videoWindowNum = 49;
            break;
        case SPLIT_64:
            m_videoWindowNum = 64;
            break;
        default:
            m_videoWindowNum = 16;
            break;
    }
}

void CHWndManager::SetOsdTextEx(int index, int line, char* text, COLORREF osdcolor)
{
    HWnd_SetOsdTextEx( m_SpliteHandle, index, line, text, osdcolor);
}
fHWnd_Init              HWnd_Init               = NULL;
fHWnd_Clear             HWnd_Clear              = NULL;
fHWnd_GetHWnd           HWnd_GetHWnd            = NULL;
fHWnd_GetFocus          HWnd_GetFocus           = NULL;
fHWnd_SetFocus          HWnd_SetFocus           = NULL;
fHWnd_SetSplitMode      HWnd_SetSplitMode       = NULL;
fHWnd_OnUpdateFocus     HWnd_OnUpdateFocus      = NULL;
fHWnd_SetPaintBG        HWnd_SetPaintBG         = NULL;
fHWnd_GetIndex          HWnd_GetIndex           = NULL;
fHWnd_SetSplitMode_WithOrder HWnd_SetSplitMode_WithOrder =NULL;
fHWnd_GetVisible        HWnd_GetVisible = NULL;
fHWnd_SetOsdTextEx      HWnd_SetOsdTextEx = NULL;
HMODULE ghWndDLL = NULL;

int LoadHWndDll()
{
	TCHAR path[_MAX_PATH] = {0X00};
	CCommClass::ExtractFilePath( path );
	_tcscat( path, _T("\\HWndDll.dll"));
    ghWndDLL = LoadLibrary(path);

	if (NULL == ghWndDLL)
	{
		TCHAR chError[256] = {0x00};
		_stprintf( chError, _T("!!!加载 %s 的动态库失败!!!"), path);
		//AfxMessageBox( chError );
		::MessageBox(NULL, chError, _T("错误"), MB_OK);
		return -1;
	}

	HWnd_Init = (fHWnd_Init)
				GetProcAddress(ghWndDLL, "HWnd_Init");
	            
	HWnd_Clear = (fHWnd_Clear)
				 GetProcAddress(ghWndDLL, "HWnd_Clear");
	             
	HWnd_GetHWnd = (fHWnd_GetHWnd)
				   GetProcAddress(ghWndDLL, "HWnd_GetHWnd");
	               
	HWnd_GetFocus = (fHWnd_GetFocus)
					GetProcAddress(ghWndDLL, "HWnd_GetFocus");

	HWnd_SetFocus = (fHWnd_SetFocus)
					GetProcAddress(ghWndDLL, "HWnd_SetFocus");
	                
	HWnd_SetSplitMode = (fHWnd_SetSplitMode)
						GetProcAddress(ghWndDLL, "HWnd_SetSplitMode");
	                    
	HWnd_OnUpdateFocus = (fHWnd_OnUpdateFocus)
						 GetProcAddress(ghWndDLL, "HWnd_OnUpdateFocus");

    HWnd_SetPaintBG = (fHWnd_SetPaintBG)
						 GetProcAddress(ghWndDLL, "HWnd_SetPaintBG");
    HWnd_GetIndex = (fHWnd_GetIndex)
	                     GetProcAddress(ghWndDLL, "HWnd_GetIndex");

    HWnd_SetSplitMode_WithOrder = (fHWnd_SetSplitMode_WithOrder)GetProcAddress(ghWndDLL, "HWnd_SetSplitMode_WithOrder");

    HWnd_GetVisible = (fHWnd_GetVisible)GetProcAddress(ghWndDLL, "HWnd_GetVisible");  //2008-08-25

    HWnd_SetOsdTextEx = (fHWnd_SetOsdTextEx)GetProcAddress(ghWndDLL, "HWnd_SetOsdTextEx");

	if (   HWnd_Init == NULL || HWnd_GetHWnd == NULL || HWnd_GetFocus == NULL ||
		   HWnd_SetSplitMode == NULL || HWnd_OnUpdateFocus == NULL ||
           HWnd_SetPaintBG == NULL || HWnd_GetIndex == NULL ||
           HWnd_SetSplitMode_WithOrder == NULL || HWnd_GetVisible == NULL || HWnd_SetOsdTextEx == NULL)
		return -2;
		
	return 0;
}

int FreeHWndDll()
{
	if ( ghWndDLL != NULL )
	{
		FreeLibrary(ghWndDLL);
		ghWndDLL = NULL;
	}
	
	return 0;
}
