// HwndManager.cpp: implementation of the CHwndManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HWndDll.h"
#include "HwndManager.h"
#include "LogClass.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//分割映射表
typedef struct{
	int  count ;
}SPLIT_MAP ;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
SPLIT_MAP    m_SplitMap_T[] = {
    {   1   } ,
	{   3 } ,
    {   4	} ,
    {   6	} ,
    {   9	} ,
    {   16  } ,
	{   25  } ,
	{   36  } ,
	
};

int m_SplitFactor_Temp[8] = { 1 , 2, 2 , 3, 3, 4, 5, 6} ; //分割画面的放大因子

CHwndManager::CHwndManager()
{
	memset( m_wndContainer, 0, sizeof ( m_wndContainer ) );
}
CHwndManager::~CHwndManager()
{
	for (int i = 0; i < MAX_CONTAINER; i++)
	{
		if (m_wndContainer[i] != NULL)
			Clear(i);
	}
}
//2009-09-14 增加检查handle函数，判断handle是否越界
bool CHwndManager::CheckHandle(int spliteHandle)
{
	if (spliteHandle < 0 || spliteHandle > MAX_CONTAINER-1)
		return false;
	if (!m_wndContainer[spliteHandle])
	{
		TRACE_LOG("CHwndManager::Init spliteHandle对应的容器指针为NULL！, handle=%d", spliteHandle);
		return false;
	}
	return true;
}
int CHwndManager::Init(HWND hParent)
{
	CWndContainer* pContainer = NULL;
	for (int i = 0; i < MAX_CONTAINER; i++)
	{
		if( m_wndContainer[i] == NULL )
		{
			m_wndContainer[i] = new CWndContainer();
			if(!m_wndContainer[i])
				TRACE_LOG("CHwndManager::Init new CWndContainer 失败，出现异常！, index=%d", i);
			m_wndContainer[i]->Init(hParent);
			return i;
		}
	}
	return -1;
}

void CHwndManager::Clear(int spliteHandle)
{
	if(!CheckHandle(spliteHandle))
	{
		TRACE_LOG("CHwndManager::Clear spliteHandle(%d)无效 ", spliteHandle);
		return;
	}
	m_wndContainer[spliteHandle]->Clear();
	SAFE_DELETE(m_wndContainer[spliteHandle]);
}

//void CHwndManager::UpdateSplitRect( int num /* 0,1,2,3,4*/, HWND hWnd, int order)
//{
//    m_wndContainer->UpdateSplitRect( num, hWnd, order );
//}

HWND CHwndManager::GetHWnd(int spliteHandle, int index)
{
	if(!CheckHandle(spliteHandle))
	{
		TRACE_LOG("CHwndManager::GetHWnd spliteHandle(%d)无效 ", spliteHandle);
		return NULL;
	}
    return m_wndContainer[spliteHandle]->GetHWnd( index );
}

int CHwndManager::GetIndex(int spliteHandle, HWND hWnd)
{
	if(!CheckHandle(spliteHandle))
	{
		TRACE_LOG("CHwndManager::GetIndex spliteHandle(%d)无效 ", spliteHandle);
		return -1;
	}
	return m_wndContainer[spliteHandle]->GetIndex(hWnd);
}

bool CHwndManager::SetSplitMode(int spliteHandle, int player, int mode, int order)
{
	if(!CheckHandle(spliteHandle))
	{
		TRACE_LOG("CHwndManager::SetSplitMode spliteHandle(%d)无效 ", spliteHandle);
		return false;
	}
	m_wndContainer[spliteHandle]->SetSplitMode( player, mode, order );
    return true;
}

void  CHwndManager::OnUpdateFocus(int spliteHandle, POINT pt)
{
	if(!CheckHandle(spliteHandle))
	{
		TRACE_LOG("CHwndManager::OnUpdateFocus spliteHandle(%d)无效 ", spliteHandle);
		return;
	}
	m_wndContainer[spliteHandle]->OnUpdateFocus( pt );
}

int CHwndManager::GetFocus( int spliteHandle )
{
	if(!CheckHandle(spliteHandle))
	{
		TRACE_LOG("CHwndManager::GetFocus spliteHandle(%d)无效 ", spliteHandle);
		return -1;
	}
    return m_wndContainer[spliteHandle]->GetFocus();
}

//bool CHwndManager::DrawWindow( int index )
///{                  
//    m_wndContainer->DrawWindow( index );           
//	return true ;
//}
//void CHwndManager::UpdatePanelByRect(int order)
//{
//    m_wndContainer->UpdatePanelByRect( order );
//}

//void CHwndManager::CreateWndByRect()
//{
//    m_wndContainer->CreateWndByRect();
//}

//void CHwndManager::VisibleAll(bool bVisible)
//{
//	m_wndContainer->VisibleAll( bVisible );
//}

bool CHwndManager::SetFocus( int spliteHandle, int index )
{
	if(!CheckHandle(spliteHandle))
	{
		TRACE_LOG("CHwndManager::SetFocus spliteHandle(%d)无效 ", spliteHandle);
		return false;
	}
	m_wndContainer[spliteHandle]->SetFocus( index );
	return true;
}

void CHwndManager::ResizeWindow()
{

}

void CHwndManager::SetPaintBG( int spliteHandle, int index, BOOL bPaitBG )
{
	if(!CheckHandle(spliteHandle))
	{
		TRACE_LOG("CHwndManager::SetPaintBG spliteHandle(%d)无效 ", spliteHandle);
		return ;
	}
	m_wndContainer[spliteHandle]->SetPaintBG( index, bPaitBG );
}

BOOL CHwndManager::GetVisible( int spliteHandle, int index)
{
	if(!CheckHandle(spliteHandle))
	{
		TRACE_LOG("CHwndManager::GetVisible spliteHandle(%d)无效 ", spliteHandle);
		return FALSE;
	}
	return m_wndContainer[spliteHandle]->GetVisible( index );
}

void CHwndManager::SetOsdText(int spliteHandle, int index, int line, char* text, COLORREF crColor)
{
	if(!CheckHandle(spliteHandle))
	{
		TRACE_LOG("CHwndManager::GetVisible spliteHandle(%d)无效 ", spliteHandle);
		return;
	}
	m_wndContainer[spliteHandle]->SetOsdText( index, line, text, crColor );
}