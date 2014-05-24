// Player.h : main header file for the PLAYER application
//

#if !defined(AFX_PLAYER_H__B22F676B_7805_4359_8D74_B5A63BA0B5F9__INCLUDED_)
#define AFX_PLAYER_H__B22F676B_7805_4359_8D74_B5A63BA0B5F9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#ifndef __AFXWIN_H__
//	#error include 'stdafx.h' before including this file for PCH
//#endif

#include "../resource.h"		// main symbols

#ifdef _FOR_HIKPLAYM4_DLL_
	//#define NAME(x) Hik_##x
	//#include "hikplaympeg4.h"
#define NAME(x) x
	#include "PlayM4.h"
#else
	#define NAME(x) x
	//#include "plaympeg4.h"
	#include "PlayM4.h"
#endif

#include "./ComSocket.h"
#include "./StreamParser.h"
#include "./RealStreamParser.h"
#include "./PlayStreamParser.h"
#include "./NetDef.h"
#include "DVRSettings.h"
#include "Sync_Locks.h"
#include "FileStreamParser.h"

class ScaleFrameMetaDataList;

#define SYN_READ_STREAM
#define WAIT_THREAD_EXIT_MAX_TIME    (5*1000)

//错误类型定义
#define SDK_ERROR       1
#define CLIENT_ERROR    2
#define SERVER_ERROR    3

class CPlayer  
{
public:
	CPlayer();
	virtual ~CPlayer();
	
	//VOID Init(HWND hNotifyWnd, int index_MTManager);
	VOID Init(HWND hNotifyWnd);
	VOID Clearup();

	void ResetBuf();
	
	INT StartMonitor(HWND hWnd, HHV_CLIENT_INFO* clientInfo);
	VOID StopMonitor( );
	int ReStartMonitor();//网络断开后，自动重新连接

	int  StartPlayBackByTime(HWND hWnd, SYSTEM_VIDEO_FILE* recdFile,char* ssIP, int ssPort);
	void StopPlayBackByTime(int realHandle);

	static BOOL m_disconnection;//接受不到视频源

	//static WATERMARK_VER1_INFO m_strWaterMark;

	void ResizeMonitorWindow();

	INT OpenVsFile(HWND hWnd, LPCTSTR szFile);//播放VS文件
	void CloseVsFile();

	void OpenStream();
	void CloseStream();

	void Play(BOOL bPause = FALSE);
	void Pause();
	void Stop();

	void Fast();
	void Slow();

	BOOL SetVSFilePointer(float pointer);

	LONG GetMaxVSFileTime(){return m_dwMaxFileTime;}

	LONG GetMaxVSFileCurrentTime(){return m_dwStreamCurrentTime;}

	LONG GetHKDVRUserID(){return m_hkDVRUserID;}

	//海康DVR

	LONG HKDVRLogin(char *sDVRIp, WORD wDVRPort, char *sUserName, char *sPassword);
	
	BOOL HKDVRLogout();

	LONG HKStartMonitor(HWND hWnd, LONG lChannel, char *sMultiCastIP);

	BOOL HKStopMonitor();

	void HKOpenFile(); //播放海康格式文件

	void HKCloseFile();

	/*enum ePlayState 
	{ 
		eState_Close  = 0,
		eState_Play   = 1,
		eState_Pause  = 2,
		eState_Stop   = 3,
		eState_Step   = 4
	};*/

	/*ePlayState GetPlayState()
	{
		return m_enumState;
	}*/

private:
	INT MonitorStartCmdMT(SOCKET sk, HHV_CLIENT_INFO* clientInfo, 
		char* streamHeader, int& size);

	int PlayStartCmdByTime( SOCKET sk, SYSTEM_VIDEO_FILE* recdFile,
							char* streamHeader, int& size,
							SPlayBackByTimeResponseInfo&  rspBody );
private:
	static UINT __stdcall DecoderRoutine(void * dat);
	INT InputData();

	static DWORD WINAPI InputStreamThread( LPVOID lpParameter);

	static void CALLBACK MP4SDKDrawFun(long nPort,HDC hDc,LONG nUser);

	static void CALLBACK OnDrawFun(long nPort, HDC hDC, LONG nUser);

private:
	CComSocket m_comSocket;
	
	BOOL m_exit;
	
	CHAR m_buffer[MAX_FRAME_LENGTH];
	CHAR m_meta[10*1024];

	INT  m_PlayHandle;

	//ePlayState	m_enumState;

	BOOL		m_bStreamType;

	LONG		m_nSpeed;                       // video play speed

	CString		m_strPlayFileName;

	//INT  m_index; 

	LONG  m_index;//后来dxl加的，作为海康的播放通道号

	HHV_CLIENT_INFO m_clientInfo;

	HANDLE m_Thread ;

	HANDLE	m_hThread;

	HANDLE m_hExitEvent;

	HANDLE m_hEventKill;                   // kill inputstream thread event
	HANDLE m_hEventInput;                  // inputstream thread event

	HWND m_hNotifyWnd ;
	HWND m_hRenderWnd;

	HWND m_hWnd;

	CHAR m_streamHeader[4*1024];

	int            m_playbacklen;  //回放的文件长度
	int            m_pos;        //现在位置 ==   已经接受到的文件大小
	
	CRealStreamParser m_streamParser;
	CPlayStreamParser m_streamParser_PlayBack;

	FRAME_HEADER m_frameHeader;

	INT InputData_Frame();//按帧读取填充
	INT InputData_Frame_PlayBack();//按帧读取填充

	int m_playType;

	CCritSec	m_MetaDataLock;

	RECT m_rcRenderRect;

	std::auto_ptr<ScaleFrameMetaDataList>	m_spScaleMetaData;

	std::auto_ptr<CFileStreamParser>	m_spStreamParser;

	DWORD		m_dwDisplayHour;
	DWORD		m_dwDisplayMinute;
	DWORD		m_dwDisplaySecond;
	DWORD		m_dwTotalFrames;
	BOOL		m_bFileEnd;
	DWORD		m_dwMaxFileSize;
	DWORD		m_dwHeadSize;

	DWORD		m_dwMaxFileTime;				// Media File duration.
	DWORD		m_dwStreamCurrentTime;			// vs file current time;

	bool		m_bFileRefCreated;				// Back seek Index is created.

	LONG GetPort(){return m_index;}

	LONG		m_hkDVRUserID;					// 登录海康DVR的返回值

	LONG		m_lPlayHandle;

	BOOL		m_hkLogout;

	BOOL		m_hkStopMonitor;

};
#endif // !defined(AFX_PLAYER_H__B22F676B_7805_4359_8D74_B5A63BA0B5F9__INCLUDED_)
