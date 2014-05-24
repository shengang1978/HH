//---------------------------------------------------------------------------
#ifndef CMonitorAreaManagerH
#define CMonitorAreaManagerH

#include<map>
using namespace std;


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

#define	MAX_CLIENT_WINDOW	128
#define HWNDISNULL          -100


struct S_HWND_INDEX
{
    S_HWND_INDEX(){
        memset(this, 0x00, sizeof(S_HWND_INDEX) );
        bFree = true;
    }
    int     index;
    HWND    hWnd;
    bool    bFree;   //窗口是否空闲 true表示空闲, false表示占用
};
//---------------------------------------------------------------------------
class CHWndManager
{
private:   
    HWND            m_hParentWnd;	
	int				m_SpliteHandle;//分割handle,由分割库InitSplit返回
    S_HWND_INDEX    m_mWndFree[MAX_CLIENT_WINDOW];     
public:
    CHWndManager();
    ~CHWndManager();
    int         InitSplit(HWND hParentWnd);
    HWND        GetHWnd( int channel );
    HWND        GetFocusHWnd( );
    
    void        SelectChannel(POINT pt);
    void        SetSplitMode ( int currIndex,int num, int clickNum = 0 );
    int         GetFocus();
    void		SetFocus( int index );
    void		SetFocus( HWND hWnd );
   
    //debug end
    int         m_videoWindowNum;   //当前画面分割数
    void        SetOsdTextEx(int index, int line, char* text, COLORREF osdcolor);
private:
     BOOL       CheckHwndFree_NoMutex(HWND hWnd, int& index);//检查一个窗口是否空闲. 空闲则返回为true; 占用则返回
    
};

typedef int  (__stdcall *fHWnd_Init)(HWND hParentWnd);
typedef void (__stdcall *fHWnd_Clear)(int spliteHandle);
typedef HWND (__stdcall *fHWnd_GetHWnd)(int spliteHandle, int index );
typedef int  (__stdcall *fHWnd_GetFocus)(int spliteHandle);
typedef void (__stdcall *fHWnd_SetFocus)(int spliteHandle, int index);
typedef bool (__stdcall *fHWnd_SetSplitMode)(int spliteHandle, int nPlayer, int num);
typedef void (__stdcall *fHWnd_OnUpdateFocus)(int spliteHandle, POINT pt) ;
typedef void (__stdcall *fHWnd_SetPaintBG)(int spliteHandle, int index, BOOL bPaitBG);
typedef int  (__stdcall *fHWnd_GetIndex)(int spliteHandle, HWND hWnd);
typedef bool (__stdcall *fHWnd_SetSplitMode_WithOrder)(int spliteHandle, int nPlayer, int num, int order);
typedef bool (__stdcall *fHWnd_GetVisible)(int spliteHandle, int index);
typedef void (__stdcall *fHWnd_SetOsdTextEx)(int spliteHandle, int index, int line, char* text, COLORREF crColor);

extern fHWnd_Init               HWnd_Init              ;
extern fHWnd_Clear              HWnd_Clear             ;
extern fHWnd_GetHWnd            HWnd_GetHWnd           ;
extern fHWnd_GetFocus           HWnd_GetFocus          ;
extern fHWnd_SetFocus           HWnd_SetFocus          ;
extern fHWnd_SetSplitMode       HWnd_SetSplitMode      ;
extern fHWnd_OnUpdateFocus      HWnd_OnUpdateFocus     ;
extern fHWnd_SetPaintBG         HWnd_SetPaintBG        ;
extern fHWnd_GetIndex           HWnd_GetIndex          ;
extern fHWnd_SetSplitMode_WithOrder HWnd_SetSplitMode_WithOrder;
extern fHWnd_SetOsdTextEx       HWnd_SetOsdTextEx;
extern fHWnd_GetVisible         HWnd_GetVisible; 

extern HMODULE ghWndDLL ;

int LoadHWndDll();
int FreeHWndDll();
#endif
