// HWndDllExport.cpp: implementation of the CHWndDllExport class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HWndDll.h"
#include "HWndDllExport.h"
#include "HwndManager.h"
CHwndManager	g_hWndManager;

int HWNDDLL_API HWnd_Init(HWND hParentWnd)
{
	return g_hWndManager.Init(hParentWnd);
}
void HWNDDLL_API HWnd_Clear( int spliteHandle )
{
	g_hWndManager.Clear(spliteHandle);
}
HWND HWNDDLL_API HWnd_GetHWnd( int spliteHandle, int index )
{
	return g_hWndManager.GetHWnd( spliteHandle, index );
}
int HWNDDLL_API HWnd_GetFocus( int spliteHandle )
{
	return g_hWndManager.GetFocus(spliteHandle);
}
void HWNDDLL_API HWnd_SetFocus( int spliteHandle, int index )
{
	g_hWndManager.SetFocus( spliteHandle, index);
}
bool HWNDDLL_API HWnd_SetSplitMode( int spliteHandle, int nPlayer,int num)
{
	return g_hWndManager.SetSplitMode( spliteHandle, nPlayer, num );
}
void HWNDDLL_API HWnd_OnUpdateFocus( int spliteHandle, POINT pt )
{
	g_hWndManager.OnUpdateFocus( spliteHandle, pt );
}

void HWNDDLL_API HWnd_SetPaintBG( int spliteHandle, int index, BOOL bPaitBG)
{
	g_hWndManager.SetPaintBG( spliteHandle, index, bPaitBG );
}
int HWNDDLL_API HWnd_GetIndex( int spliteHandle, HWND hWnd )
{
	return g_hWndManager.GetIndex( spliteHandle, hWnd );	
}
 
//2008-07-29
bool HWNDDLL_API HWnd_SetSplitMode_WithOrder( int spliteHandle, int nPlayer,int num, int order)
{
	return g_hWndManager.SetSplitMode( spliteHandle, nPlayer, num, order );
}

bool HWNDDLL_API HWnd_GetVisible( int spliteHandle, int index)
{
	return !!g_hWndManager.GetVisible( spliteHandle, index );
}

void HWNDDLL_API HWnd_SetOsdText(int spliteHandle, int index, int line, char* text)
{
	COLORREF crColor = RGB(0,170,255);
	g_hWndManager.SetOsdText( spliteHandle, index, line, text, crColor );
}

void HWNDDLL_API HWnd_SetOsdTextEx(int spliteHandle, int index, int line, char* text, COLORREF crColor)
{
	g_hWndManager.SetOsdText( spliteHandle, index, line, text, crColor );
}

