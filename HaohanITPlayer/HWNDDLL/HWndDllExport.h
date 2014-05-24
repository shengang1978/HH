// HWndDllExport.h: interface for the CHWndDllExport class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _HWNDDLLEXPORT_H
#define _HWNDDLLEXPORT_H

#ifdef  HWNDDLL_EXPORT
#define HWNDDLL_API __declspec(dllexport) __stdcall
#else
#define HWNDDLL_API __declspec(dllimport) __stdcall
#endif

//画面分割模式
#define SPLIT_1				0
#define SPLIT_4				1
#define SPLIT_6				2
#define SPLIT_9				3
#define SPLIT_16			4
#define SPLIT_25			5
#define SPLIT_36			6

#define SPLIT_3				7
#define SPLIT_6_2			8
#define SPLIT_49			9
#define SPLIT_64			10


//六分割
//#define SPLIT_6_1           0
//#define SPLIT_6_2           1
//初始化返回hParentWnd分配到的spliteHandle，以后各种操作需要使用 spliteHandle
//spliteHandle从0开始
int     HWNDDLL_API HWnd_Init(HWND hParentWnd);

void	HWNDDLL_API HWnd_Clear(int spliteHandle);
HWND    HWNDDLL_API HWnd_GetHWnd(int spliteHandle, int index);
int     HWNDDLL_API HWnd_GetFocus(int spliteHandle);
void    HWNDDLL_API HWnd_SetFocus(int spliteHandle, int index);
bool    HWNDDLL_API HWnd_SetSplitMode(int spliteHandle, int nPlayer, int num);
void    HWNDDLL_API HWnd_OnUpdateFocus(int spliteHandle, POINT pt) ;
void	HWNDDLL_API HWnd_SetPaintBG(int spliteHandle, int index, BOOL bPaitBG);
int		HWNDDLL_API HWnd_GetIndex(int spliteHandle, HWND hWnd);
bool	HWNDDLL_API HWnd_SetSplitMode_WithOrder(int spliteHandle, int nPlayer, int num, int order);
bool	HWNDDLL_API HWnd_GetVisible(int spliteHandle, int index);
void	HWNDDLL_API HWnd_SetOsdText(int spliteHandle, int index, int line, char* text);
void	HWNDDLL_API HWnd_SetOsdTextEx(int spliteHandle, int index, int line, char* text, COLORREF crColor);
#endif
