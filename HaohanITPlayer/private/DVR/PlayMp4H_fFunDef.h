#ifndef __PLAYMP4H_FFUNDEF_H
#define __PLAYMP4H_FFUNDEF_H
#include "playm4.h"

typedef  BOOL    (__stdcall *fPlayM4_Play)(LONG nPort, HWND hWnd);
typedef  BOOL    (__stdcall *fPlayM4_OpenStream)(LONG nPort,PBYTE pFileHeadBuf,DWORD nSize,DWORD nBufPoolSize);
typedef  DWORD   (__stdcall *fPlayM4_GetLastError)(LONG nPort);
typedef  BOOL    (__stdcall *fPlayM4_CloseStream)(LONG nPort);
typedef  BOOL    (__stdcall *fPlayM4_Stop)(LONG nPort);
typedef  BOOL    (__stdcall *fPlayM4_InputData)(LONG nPort,PBYTE pBuf,DWORD nSize);
typedef  BOOL    (__stdcall *fPlayM4_OpenFile)(LONG nPort,LPSTR sFileName);
typedef  BOOL    (__stdcall *fPlayM4_CloseFile)(LONG nPort);
typedef  BOOL    (__stdcall *fPlayM4_Pause)(LONG nPort,DWORD nPause);
typedef  BOOL    (__stdcall *fPlayM4_OneByOne)(LONG nPort);
typedef  BOOL    (__stdcall *fPlayM4_SetPlayPos)(LONG nPort,float fRelativePos);
typedef  BOOL    (__stdcall *fPlayM4_PlaySound)(LONG nPort);
typedef  BOOL    (__stdcall *fPlayM4_StopSound)();
typedef  BOOL    (__stdcall *fPlayM4_Fast)(LONG nPort);
typedef  BOOL    (__stdcall *fPlayM4_Slow)(LONG nPort);
typedef  DWORD   (__stdcall *fPlayM4_GetPlayedTime)(LONG nPort);
typedef  float   (__stdcall *fPlayM4_GetPlayPos)(LONG nPort);
typedef  BOOL    (__stdcall *fPlayM4_RigisterDrawFun)(LONG nPort,void (CALLBACK* DrawFun)(long nPort,HDC hDc,LONG nUser),LONG nUser);
typedef  BOOL	 (__stdcall *fPlayM4_GetBMP)(LONG nPort,PBYTE pBitmap,DWORD nBufSize,DWORD* pBmpSize);
typedef  BOOL	 (__stdcall	*fPlayM4_SetStreamOpenMode)(LONG nPort,DWORD nMode);

typedef  BOOL	 (__stdcall *fPlayM4_SetDecCBStream)(LONG nPort,DWORD nStream);

typedef  BOOL	 (__stdcall *fPlayM4_SetDecCallBackMend)(LONG nPort,
														 void (CALLBACK* DecCBFun)(long nPort,char * pBuf,long nSize,FRAME_INFO * pFrameInfo, long nUser,long nReserved2), 
														 long nUser);

typedef BOOL	 (__stdcall	*fPlayM4_SetDisplayCallBack)(LONG nPort,
														 void (CALLBACK* DisplayCBFun)(long nPort,char * pBuf,long nSize,long nWidth,long nHeight,long nStamp,long nType,long nReserved));
typedef BOOL	 (__stdcall	*fPLayM4_ConvertToBmpFile)(char * pBuf,long nSize,long nWidth,long nHeight,long nType,char *sFileName);

typedef DWORD	 (__stdcall	*fPlayM4_GetFileHeadLength)();
typedef DWORD	 (__stdcall *fPlayM4_GetSourceBufferRemain)(LONG nPort);

extern fPlayM4_Play                 HK_PlayM4_Play;
extern fPlayM4_OpenStream           HK_PlayM4_OpenStream;
extern fPlayM4_GetLastError         HK_PlayM4_GetLastError;
extern fPlayM4_CloseStream          HK_PlayM4_CloseStream;
extern fPlayM4_Stop                 HK_PlayM4_Stop;
extern fPlayM4_InputData            HK_PlayM4_InputData;
extern fPlayM4_OpenFile             HK_PlayM4_OpenFile;
extern fPlayM4_CloseFile            HK_PlayM4_CloseFile;
extern fPlayM4_Pause                HK_PlayM4_Pause;
extern fPlayM4_OneByOne             HK_PlayM4_OneByOne;
extern fPlayM4_SetPlayPos           HK_PlayM4_SetPlayPos;
extern fPlayM4_PlaySound            HK_PlayM4_PlaySound;
extern fPlayM4_StopSound            HK_PlayM4_StopSound;
extern fPlayM4_Fast                 HK_PlayM4_Fast;
extern fPlayM4_Slow                 HK_PlayM4_Slow;
extern fPlayM4_GetPlayedTime        HK_PlayM4_GetPlayedTime;
extern fPlayM4_GetPlayPos           HK_PlayM4_GetPlayPos;
extern fPlayM4_RigisterDrawFun      HK_PlayM4_RigisterDrawFun;
extern fPlayM4_GetBMP				HK_PlayM4_GetBMP;
extern fPlayM4_SetStreamOpenMode	HK_PlayM4_SetStreamOpenMode;

extern fPlayM4_SetDecCBStream		HK_PlayM4_SetDecCBStream;
extern fPlayM4_SetDecCallBackMend	HK_PlayM4_SetDecCallBackMend;

extern fPlayM4_SetDisplayCallBack	HK_PlayM4_SetDisplayCallBack;
extern fPLayM4_ConvertToBmpFile		HK_PLayM4_ConvertToBmpFile;

extern fPlayM4_GetFileHeadLength		HK_PlayM4_GetFileHeadLength;
extern fPlayM4_GetSourceBufferRemain	HK_PlayM4_GetSourceBufferRemain;


extern      HMODULE ghMP4SDK;
int         LoadPlayMp4Dll();
int         FreePlayMp4Dll();

#endif 
