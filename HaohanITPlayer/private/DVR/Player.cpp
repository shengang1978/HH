// Player.cpp: implementation of the CPlayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "atlimage.h"
#include "Player.h"
#include <process.h>
#include "./DvrSDKErr.h"
#include "./CommClass.h"
//#include "./struct_TCPServ.h"
#include "./NetDef.h"
#include "./PlayMp4H_fFunDef.h"
#include "Metadata_Types.h"
#include "DVRPlayer.h"
#include <stdio.h>
#include "TraceLog.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
BOOL CPlayer::m_disconnection;
//WATERMARK_VER1_INFO CPlayer::m_strWaterMark;
CPlayer::CPlayer()
{
	m_hRenderWnd = NULL;
	m_PlayHandle = -1;
	m_hExitEvent	=  CreateEvent( NULL, TRUE, FALSE, _T("") );
	memset( m_meta, 0x00, sizeof(m_meta) );

	m_playbacklen = 0;
	m_pos = 0;

	m_playType = 0;
	m_spScaleMetaData.reset(new ScaleFrameMetaDataList);
}

CPlayer::~CPlayer()
{
	CloseHandle(m_hExitEvent);
    m_hExitEvent = NULL;
}

void CPlayer::Init(HWND hNotifyWnd)
{
	m_hRenderWnd = NULL;
    m_exit       = TRUE;
    memset(m_buffer, 0x00, sizeof(m_buffer));
    ResetEvent(m_hExitEvent);
    m_hNotifyWnd  = hNotifyWnd;
	NAME(PlayM4_GetPort)(&m_index);
    //m_index = index_MTManager;
	m_hEventKill = NULL; 
	m_hEventInput = NULL;
	m_hThread = NULL;
	m_bStreamType = TRUE;
    m_streamParser.ClearBuf();
	memset(&m_rcRenderRect, 0, sizeof(RECT));

	NET_DVR_Init();//初始化DVR
	m_hkDVRUserID = -1;
	m_lPlayHandle = -1;
	m_hkLogout = FALSE;
	m_hkStopMonitor = FALSE;
}
void CPlayer::Clearup()
{
    m_PlayHandle = -1;
    m_index = -1;
    m_streamParser.ClearBuf();//清除缓冲
	memset(&m_rcRenderRect, 0, sizeof(RECT));
}

void CPlayer::ResetBuf()
{
	NAME(PlayM4_ResetBuffer)(m_index, BUF_VIDEO_RENDER);
	NAME(PlayM4_ResetBuffer)(m_index, BUF_AUDIO_RENDER);
	NAME(PlayM4_ResetBuffer)(m_index, BUF_VIDEO_SRC);
	NAME(PlayM4_ResetBuffer)(m_index, BUF_AUDIO_SRC);
	NAME(PlayM4_ResetSourceBuffer)(m_index);
}

int CPlayer::ReStartMonitor()
{
	m_comSocket.Close();
	m_streamParser.ClearBuf();//清除缓冲
	memset(&m_rcRenderRect, 0, sizeof(RECT));
	if( m_comSocket.Connect(m_clientInfo.connInfo.ip, m_clientInfo.connInfo.port) < 0 )
    {
        m_comSocket.Close();
        return HHV_ERROR_CONNECT;
    }
	int	streamHeaderSize = 4*1024;
	memset( m_streamHeader, 0x00, sizeof(m_streamHeader) );

    int ret = MonitorStartCmdMT(m_comSocket.m_hSocket, &m_clientInfo, m_streamHeader, streamHeaderSize);
    if( ret < 0 )
    {
        m_comSocket.Close();
        return ret;
    }

	PlayM4_Stop(m_index);
	PlayM4_CloseStream( m_index);
	ResetBuf();

	//mode- 流模式（1-实时流/2-文件流）
	int nErr = 0;
	m_playType = STREAME_REALTIME;
	BOOL  bret =  PlayM4_SetStreamOpenMode(m_index, m_playType);
    bret =  PlayM4_OpenStream( m_index, (BYTE*)m_streamHeader, streamHeaderSize, 600*1024);
	if (!bret)
	{
		m_comSocket.Close();
		nErr =  PlayM4_GetLastError(m_index);

		return -nErr;
	}

	m_disconnection = false;
	if( ! PlayM4_RigisterDrawFun( m_index, MP4SDKDrawFun, (DWORD)this) )
	{
		m_comSocket.Close();
		nErr =  PlayM4_GetLastError(m_index);
		//TRACE_SDK_ERROR("执行 CHKSeries::RegisterDrawFun 中  PlayM4_RigisterDrawFun 函数 Fail nErr = %d strErr = %s\r\n\r\n", nErr, strErr.c_str() );
		return -nErr;
	}

	bret =  PlayM4_Play(m_index, m_hWnd);
	if (!bret)
	{
		nErr =  PlayM4_GetLastError(m_index);
		//string strErr = GetPlayMp4LastErrorStr( nErr );
		//TRACE_SDK_ERROR("执行 CHKSeries::OpenStream 中  PlayM4_Play 函数 Fail nErr = %d strErr = %s\r\n\r\n", nErr, strErr.c_str() );
		return -nErr;
	}

	m_hRenderWnd = m_hWnd;
	ResizeMonitorWindow();

    //memcpy(&m_clientInfo, clientInfo, sizeof(UNISDK_CLIENT_INFO));
    //开启线程, 接收视频流
    m_exit = false;
    UINT threadid(0);
    m_Thread = (HANDLE)_beginthreadex( NULL, 4*1024*1024, DecoderRoutine, this, 0, &threadid ); 
    if(m_Thread <= 0)
    {
        TRACE(_T("(MTVideo)!!!!!!开辟接受数据线程失败！m_Thread:%d errno:%d _doserrno:%d \r\n"),
            m_Thread, errno, _doserrno);
        SetEvent( m_hExitEvent );
    }
	
    CloseHandle( m_Thread );
  //
  //  TRACE(_T("(MTVideo)监视成功结束 m_PlayHandle = %d dvrIP = %s channel = %d\r\n"),
		//m_PlayHandle, m_clientInfo.connInfo.ip, clientInfo->channel);
    return m_index;
}

INT CPlayer::StartMonitor(HWND hWnd, HHV_CLIENT_INFO* clientInfo)
{	
	m_hWnd = hWnd;
	memcpy(&m_clientInfo, clientInfo, sizeof(HHV_CLIENT_INFO));
	m_disconnection = false;
	if( clientInfo == NULL )
	{
		return HHV_ERROR_INVALID_PARA;
	}
    if( !CCommClass::CheckIP(std::string(clientInfo->connInfo.ip)) )
    {   
        return HHV_ERROR_INVALID_IP;
    }
	
    if( m_comSocket.Connect(clientInfo->connInfo.ip, clientInfo->connInfo.port) < 0 )
    {
        m_comSocket.Close();
        return HHV_ERROR_CONNECT;
    }
	
	int   streamHeaderSize = 4*1024;
	memset( m_streamHeader, 0x00, sizeof(m_streamHeader) );

    int ret = MonitorStartCmdMT(m_comSocket.m_hSocket, clientInfo, m_streamHeader, streamHeaderSize);
    if( ret < 0 )
    {
        //TRACE(_T("(MTVideo)监视失败 发送cmd失败或被服务器拒绝 vsIP = %s vsPort = %d dvrIP = %s channel = %d\r\n",
		//	clientInfo->RFIP, clientInfo->RFTCPPort, clientInfo->connInfo.ip, clientInfo->channel);
        m_comSocket.Close();
        return ret;
    }
    //TRACE(_T("(MTVideo)监视 发送cmd成功 vsIP = %s vsPort = %d dvrIP = %s channel = %d\r\n",
	//	clientInfo->RFIP, clientInfo->RFTCPPort, clientInfo->connInfo.ip, clientInfo->channel);

    //mode- 流模式（1-实时流/2-文件流）
	int nErr = 0;
	m_playType = STREAME_REALTIME;
	//NAME(PlayM4_GetPort)(&m_index);
	BOOL  bret =  PlayM4_SetStreamOpenMode(m_index, m_playType);
	nErr =  PlayM4_GetLastError(m_index);
    bret =  PlayM4_OpenStream( m_index, (BYTE*)m_streamHeader, streamHeaderSize, 600*1024);
	nErr =  PlayM4_GetLastError(m_index);
	if (!bret)
	{
		m_comSocket.Close();
		nErr =  PlayM4_GetLastError(m_index);
		//string strErr = GetPlayMp4LastErrorStr( nErr );
		//TRACE_SDK_ERROR("执行 CHKSeries::OpenStream 中  PlayM4_OpenStream 函数 Fail nErr = %d strErr = %s\r\n\r\n", nErr, strErr.c_str() );
		//RACE_LOG("(MTVideo)监视失败 <Error pDvrSDK->OpenPlayStream Fail> m_PlayHandle = %d dvrIP = %s channel = %d\r\n",
		//	m_PlayHandle, clientInfo->connInfo.ip, clientInfo->channel);

		return -nErr;
	}

	if( ! PlayM4_RigisterDrawFun( m_index, MP4SDKDrawFun, (DWORD)this) )
	{
		m_comSocket.Close();
		nErr =  PlayM4_GetLastError(m_index);
		//TRACE_SDK_ERROR("执行 CHKSeries::RegisterDrawFun 中  PlayM4_RigisterDrawFun 函数 Fail nErr = %d strErr = %s\r\n\r\n", nErr, strErr.c_str() );
		return -nErr;
	}

	bret =  PlayM4_Play(m_index, hWnd);
	if (!bret)
	{
		nErr =  PlayM4_GetLastError(m_index);
		//string strErr = GetPlayMp4LastErrorStr( nErr );
		//TRACE_SDK_ERROR("执行 CHKSeries::OpenStream 中  PlayM4_Play 函数 Fail nErr = %d strErr = %s\r\n\r\n", nErr, strErr.c_str() );
		return -nErr;
	}

	m_hRenderWnd = hWnd;
	ResizeMonitorWindow();

    TRACE(_T("(MTVideo)监视 OpenPlayStream成功 m_index = %d dvrIP = %s channel = %d\r\n"),
		m_index, clientInfo->connInfo.ip, clientInfo->channel);
    //memcpy(&m_clientInfo, clientInfo, sizeof(UNISDK_CLIENT_INFO));
    //开启线程, 接收视频流
    m_exit = false;
    UINT threadid(0);
    m_Thread = (HANDLE)_beginthreadex( NULL, 4*1024*1024, DecoderRoutine, this, 0, &threadid ); 
    if(m_Thread <= 0)
    {
        TRACE(_T("(MTVideo)!!!!!!开辟接受数据线程失败！m_Thread:%d errno:%d _doserrno:%d \r\n"),
            m_Thread, errno, _doserrno);
        SetEvent( m_hExitEvent );
    }
	
    CloseHandle( m_Thread );
  
    TRACE(_T("(MTVideo)监视成功结束 m_PlayHandle = %d dvrIP = %s channel = %d\r\n"),
		m_PlayHandle, clientInfo->connInfo.ip, clientInfo->channel);
    return m_index;
}

void CPlayer::Play(BOOL bPause)
{
	if(bPause)
	{
		NAME(PlayM4_Pause)(m_index, FALSE);
	}
	else
	{
		m_nSpeed = 0;
		if(m_bStreamType)
		{
			SetEvent(m_hEventInput);
		}
		NAME(PlayM4_Play)(m_index, m_hRenderWnd);
		NAME(PlayM4_RefreshPlay)(m_index);	
	}
	

	//if(m_enumState == CPlayer::eState_Play)
	//{
	//	return;
	//}
	//
	//if(m_enumState == CPlayer::eState_Pause)
	//{

	//	NAME(PlayM4_Pause)(m_index, FALSE);

	//	m_enumState = CPlayer::eState_Play;

	//}
	//else if (m_enumState == CPlayer::eState_Stop && m_bStreamType)
	//{
	//	OpenVsFile(m_hRenderWnd, m_strPlayFileName);
	//}
	//else if(m_enumState == CPlayer::eState_Step)
	//{
	//	NAME(PlayM4_Play)(m_index, m_hRenderWnd);
	//	m_nSpeed = 0;

	//	m_enumState = CPlayer::eState_Play;
	//}
	//else
	//{
	//	//ZeroMemory(&m_strWaterMark, sizeof(WATERMARK_VER1_INFO));
	//	
	//	m_nSpeed = 0;

	//	if(m_bStreamType)
	//	{
	//		SetEvent(m_hEventInput);
	//	}

	//	if(NAME(PlayM4_Play)(m_index, m_hRenderWnd))
	//	{	
	//		m_enumState = CPlayer::eState_Play; 

	//		NAME(PlayM4_RefreshPlay)(m_index);
	//	}
	//}
}

void CPlayer::Pause()
{
	NAME(PlayM4_Pause)(m_index, TRUE);
	/*if(m_enumState == CPlayer::eState_Play)
	{
		NAME(PlayM4_Pause)(m_index, TRUE);
		m_enumState = CPlayer::eState_Pause;
	}*/
}

void CPlayer::Stop()
{
	NAME(PlayM4_Stop)(m_index);
	if(m_bStreamType)
	{
		ResetEvent(m_hEventInput);
	}

	/*if(m_enumState == CPlayer::eState_Stop)
	{
		return;
	}

	if(NAME(PlayM4_Stop)(m_index))	
	{
	
		if(m_bStreamType)
		{
			ResetEvent(m_hEventInput);
		}
	}	*/

	//ZeroMemory(&m_strWaterMark, sizeof(WATERMARK_VER1_INFO));
	//m_enumState = CPlayer::eState_Stop;
}

void CPlayer::Fast()
{
	if(NAME(PlayM4_Fast)(m_index))
	{
		m_nSpeed ++;
		if(m_nSpeed > 0)
		{
			NAME(PlayM4_ThrowBFrameNum)(m_index, 2);
		}
	}
}

void CPlayer::Slow()
{
	if(NAME(PlayM4_Slow)(m_index))
	{
		m_nSpeed --;
		if(m_nSpeed <= 0)
		{
			NAME(PlayM4_ThrowBFrameNum)(m_index, 0);
		}
	}
}

BOOL CPlayer::SetVSFilePointer(float pointer)
{
	return m_spStreamParser->SetFilePointer(pointer);
}

void CPlayer::OpenStream()
{
	m_dwDisplayHour		= 0;
	m_dwDisplayMinute   = 0;
	m_dwDisplaySecond   = 0;
	m_dwTotalFrames     = 0;
	m_bFileEnd          = FALSE;
	HIK_MEDIAINFO		stInfo;
	ZeroMemory(&stInfo,sizeof(HIK_MEDIAINFO));

	m_spStreamParser.reset(CStreamParseFactory::OpenFile(m_strPlayFileName));

	if(m_spStreamParser.get() == NULL)
	{
		MessageBox(NULL, _T("Open file failed"), _T("Error"), MB_OK);
		throw 0;
	}
	m_dwMaxFileSize = m_spStreamParser->GetFileSize();
	if (m_dwMaxFileSize == 0)
	{
		MessageBox(NULL, _T("Open file failed"), _T("Error"), MB_OK);
		throw 0;
	}
	NAME(PlayM4_SetStreamOpenMode)(m_index, STREAME_FILE);
	m_dwHeadSize = NAME(PlayM4_GetFileHeadLength)();
	PBYTE pBuf = NULL;
	TCHAR szError[50];

	pBuf = new BYTE[m_dwHeadSize];
	if(!pBuf)
	{
		MessageBox(NULL, _T("Alloc memory failed"), _T("Error"), MB_OK);
		throw 0;
	}
	DWORD nRealRead = m_spStreamParser->ReadCacheData(pBuf, m_dwHeadSize);
	if(nRealRead != m_dwHeadSize)
	{
		MessageBox(NULL, _T("File is too small"), _T("Error"), MB_OK);
		delete []pBuf;
		pBuf = NULL;
		throw 0;
	}

	unsigned int g_strFileHdr[10] = { 0x484B4834, 0xd6d0b3fe, 0x20040308, 0x0, 0x10011001, 0x00101001, 0x3e80, 0x01200160, 0x1011, 0x4 };

#ifdef SPLIT_INPUT
	if(!NAME(PlayM4_OpenStreamEx)(m_lPort, (BYTE*)&stInfo, m_dwHeadSize, 6*1000*1024))
	{
		_stprintf(szError, _T("Open stream failed(%s)"), MyErrorToString(NAME(PlayM4_GetLastError)(m_lPort)));
		MessageBox(csError);
		delete []pBuf;
		pBuf = NULL;
		throw 0;
	}
	PlayM4_SyncToAudio(m_lPort, FALSE);
#else defined MIX_INPUT	

	if(!NAME(PlayM4_OpenStream)(m_index, pBuf, sizeof(stInfo),  6*1000*1024))
	{
		MessageBox(NULL, _T("Open file failed"), _T("Error"), MB_OK);
		delete []pBuf;
		pBuf = NULL;
		throw 0;
	}
#endif

	delete []pBuf;
	pBuf = NULL;

	DWORD dw;
	m_hThread	  = NULL;
	m_hEventInput = NULL;
	m_hEventKill  = NULL;

	ResizeMonitorWindow();

	m_hThread	  =	CreateThread(NULL,0, LPTHREAD_START_ROUTINE (InputStreamThread),this,0,&dw);
	m_hEventInput =	CreateEvent(NULL,TRUE,FALSE,NULL);
	m_hEventKill  =	CreateEvent(NULL,FALSE,FALSE,NULL);
	if( !(m_hThread&&m_hEventInput&&m_hEventKill) )
	{
		MessageBox(NULL, _T("Create thread failed"), _T("Error"), MB_OK);
		throw 0;
	}
	m_dwMaxFileTime = m_spStreamParser->GetLastFrameTime()-m_spStreamParser->GetFirstFrameTime();
	m_dwStreamCurrentTime = 0;
	if(!m_dwMaxFileTime)
	{
		m_dwMaxFileTime = 1;
	}

	m_dwDisplayHour		= m_dwMaxFileTime/3600;
	m_dwDisplayMinute   = (m_dwMaxFileTime % 3600) / 60;
	m_dwDisplaySecond   = m_dwMaxFileTime % 60;
	m_dwTotalFrames		= m_dwMaxFileTime;

	Play();
}

void CPlayer::CloseStream()
{
	Stop();
	NAME(PlayM4_CloseStream)(m_index);

	if(m_hThread)
	{
		SetEvent(m_hEventKill);
		DWORD dwStatus;
		for(int i = 0; i < 5; i++)
		{
			if(!::GetExitCodeThread(m_hThread, &dwStatus)|| (i == 4) )
			{
				TerminateThread(m_hThread, 0);
				TRACE(_T("GetExitCode option error!-decodethread\n"));
			}
			else
			{
				if(dwStatus == STILL_ACTIVE)
				{
					SetEvent(m_hEventKill);
					Sleep(2);
				}
				else
				{
					break;
				}
			}
		}
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}

	if(m_hEventInput)
	{
		CloseHandle(m_hEventInput);
		m_hEventInput = NULL;
	}

	if(m_hEventKill)
	{
		CloseHandle(m_hEventKill);
		m_hEventKill = NULL;
	}

	m_spStreamParser.reset();//->->CloseFile();
	NAME(PlayM4_FreePort)(m_index);

	m_index = -1;
	//m_enumState = CPlayer::eState_Close;
	m_bFileRefCreated =	FALSE;
}

void CPlayer::HKOpenFile()
{
	CString strError;

	NAME(PlayM4_SetFileEndMsg)(m_index, m_hRenderWnd, WM_FILE_END);

	//NAME(PlayM4_SetFileRefCallBack)(m_index, FileRefDone, (DWORD)this);

	//NAME(PlayM4_SetDecCallBackEx)(m_index, DecCBFun, NULL, 0);	

	char szFilename[MAX_PATH];
	strcpy(szFilename, CT2A((LPCTSTR)m_strPlayFileName));

	if(!NAME(PlayM4_OpenFile)(m_index, szFilename))
	{
		//m_strPlayFileName = _T("");
		//strError.Format(_T("Open file failed(%s)"), MyErrorToString(NAME(PlayM4_GetLastError)(m_index)));
		//MessageBox(m_hRenderWnd, strError, _T("Error"), MB_OK);
		throw 0;
	}

	m_dwMaxFileTime = NAME(PlayM4_GetFileTime)(m_index);
	if(!m_dwMaxFileTime)
	{
		m_dwMaxFileTime = 1;
	}

	m_dwDisplayHour		= m_dwMaxFileTime/3600;
	m_dwDisplayMinute   = (m_dwMaxFileTime % 3600) / 60;
	m_dwDisplaySecond   = m_dwMaxFileTime % 60;
	m_dwTotalFrames		= NAME(PlayM4_GetFileTotalFrames)(m_index);

	Play();
}

void CPlayer::HKCloseFile()
{
	Stop();
	NAME(PlayM4_CloseFile)(m_index);
}

LONG CPlayer::HKDVRLogin(char *sDVRIp, WORD wDVRPort, char *sUserName, char *sPassword)
{
	NET_DVR_DEVICEINFO_V30 struDeviceInfo;

	memset(&struDeviceInfo, 0, sizeof(NET_DVR_DEVICEINFO_V30));

	m_hkDVRUserID = NET_DVR_Login_V30(sDVRIp, wDVRPort, sUserName, sPassword, &struDeviceInfo);

	return m_hkDVRUserID;
}

BOOL CPlayer::HKDVRLogout()
{
	if(m_hkDVRUserID >= 0)
	{
		m_hkLogout = NET_DVR_Logout(m_hkDVRUserID);
	}
	 
	return m_hkLogout;
}

LONG CPlayer::HKStartMonitor(HWND hWnd, LONG lChannel, char *sMultiCastIP)
{
	NET_DVR_CLIENTINFO struPlayInfo;

	struPlayInfo.hPlayWnd = hWnd;	
	struPlayInfo.lChannel = lChannel;
	struPlayInfo.lLinkMode = 0;
	struPlayInfo.sMultiCastIP = sMultiCastIP;

	m_lPlayHandle = NET_DVR_RealPlay_V30(m_hkDVRUserID, &struPlayInfo, NULL, 0, 0);

	return m_lPlayHandle;
}

BOOL CPlayer::HKStopMonitor()
{
	if(m_lPlayHandle >= 0)
	{
		m_hkStopMonitor = NET_DVR_StopRealPlay(m_lPlayHandle);
		m_lPlayHandle = -1;
	}
	return m_hkStopMonitor;
}

INT CPlayer::OpenVsFile(HWND hWnd, LPCTSTR szFile)
{
	m_hRenderWnd = hWnd;

	if (szFile != NULL)
	{
		m_strPlayFileName = szFile;
	}
	else
		return -1;

	try
	{
		NAME(PlayM4_RegisterDrawFun)(m_index, OnDrawFun, (LONG)this);

		NAME(PlayM4_SetEncChangeMsg)(m_index, m_hRenderWnd, WM_ENC_CHANGE);

		//NAME(PlayM4_SetFileEndCallback)(m_index, FileEndCallBack, this);

		m_bStreamType = m_strPlayFileName.Right(3).CompareNoCase(_T(".vs")) == 0;

		if(m_bStreamType)
		{
			OpenStream();
		}
		else
		{
			HKOpenFile();
		}

		//NAME(PlayM4_SetPicQuality)(m_index, m_bPicQuality);
		//NAME(PlayM4_SetDeflash)(m_index, m_bDeflash);

		//NAME(PlayM4_GetPictureSize)(m_index, &m_nWidth, &m_nHeight);
		/*if( (m_nWidth == WIDTH*2) && (m_nHeight <= HEIGHT_PAL) )
		{
			m_nHeight *= 2;
		}*/

	}
	catch(...)
	{
		CloseVsFile();
	}

	return m_index;
}

void CPlayer::CloseVsFile()
{
	if(m_bStreamType)
	{
		CloseStream();
	}
	else
	{
		HKCloseFile();
	}
}

void CPlayer::ResizeMonitorWindow()
{
	if (m_hRenderWnd != NULL)
	{
		::GetWindowRect(m_hRenderWnd, &m_rcRenderRect);
	}
}
VOID CPlayer::StopMonitor( )
{
	TRACE(_T("(MTVideo)停止监视 开始\r\n"));
    m_exit = true;
    m_comSocket.Close();
    //TRACE(_T("(MTVideo)停止监视 将执行WaitForSingleObject dvrIP = %s channel = %d m_PlayHandle = %d m_index_MTManager = %d\r\n",
	//	m_UniSDKCltInfo.connInfo.ip, m_UniSDKCltInfo.channel, m_PlayHandle, m_index_MTManager);
    DWORD dwret = WaitForSingleObject( m_hExitEvent, WAIT_THREAD_EXIT_MAX_TIME );
    if(dwret == WAIT_TIMEOUT)
    {
        TRACE(_T("(MTVideo)!!!!!停止监视时 WaitFor退出信号超时 \r\n"));
    }
    
	BOOL bret =  PlayM4_Stop(m_index);
	
	int nErr = 0;
	if (!bret)
	{
		nErr =  PlayM4_GetLastError(m_index);
		//string strErr = GetPlayMp4LastErrorStr( nErr );
		//TRACE_SDK_ERROR("执行 CHKSeries::CloseStream 中  PlayM4_Stop 函数 Fail nErr = %d strErr = %s\r\n\r\n", nErr, strErr.c_str() );
		//return -nErr;
	}
	
	
	bret =  PlayM4_CloseStream( m_index);
	if (!bret)
	{
		nErr =  PlayM4_GetLastError(m_index);
		//string strErr = GetPlayMp4LastErrorStr( nErr );
		//TRACE_SDK_ERROR("执行 CHKSeries::CloseStream 中  PlayM4_CloseStream 函数 Fail nErr = %d strErr = %s\r\n\r\n", nErr, strErr.c_str() );
		//return -nErr;
	}
	
	
    //TRACE(_T("(MTVideo)停止监视成功 DvrIP = %s channnel = %d m_PlayHandle = %d m_index_MTManager = %d\r\n",
	//	m_UniSDKCltInfo.connInfo.ip, m_UniSDKCltInfo.channel, m_PlayHandle, m_index_MTManager);
	NAME(PlayM4_FreePort)(m_index);
	m_index = -1;
    m_PlayHandle = -1;
	memset(&m_rcRenderRect, 0, sizeof(RECT));
	m_hRenderWnd = NULL;
}

UINT __stdcall CPlayer::DecoderRoutine( void * dat)
{
    CPlayer* pPlayer = (CPlayer*)dat;

	int playType = pPlayer->m_playType;
	int ret = 0;
	
	pPlayer->m_streamParser.SetSocketPara(&pPlayer->m_comSocket);
    while( !pPlayer->m_exit )
    {
		if( playType == STREAME_REALTIME )
			ret = pPlayer->InputData_Frame();
		else
			ret = pPlayer->InputData_Frame_PlayBack();

		if( ret < 0 )
			break;
    }
		/*pPlayer->m_comSocket.Close();
		SetEvent( pPlayer->m_hExitEvent );*/
	if(pPlayer->m_exit)
	{
		pPlayer->m_comSocket.Close();
		SetEvent( pPlayer->m_hExitEvent );
	}
    TRACE(_T("线程 '监视' (0x%x)%d 已退出\r\n"), GetCurrentThreadId(), GetCurrentThreadId());
    return 0;
}

int CPlayer::InputData()
{
    int ret = m_comSocket.Receive((char*)m_buffer, 8*1024);  //每次收1K
    if( ret <= 0 )
    {
        if(m_exit)
        {
            //TRACE(_T("CMTVideo::InputData 退出1 dvrIP = %s Channel = %d m_PlayHandle = %d m_index_MTManager = %d ret = %d nReceiveOneTime = %dkbps\r\n",
			//	m_UniSDKCltInfo.connInfo.ip, m_UniSDKCltInfo.channel, m_PlayHandle, m_index_MTManager, ret, nReceiveOneTime);
            return -1;
        }
        //TRACE(_T("CMTVideo::InputData <m_comSocket.Recv Fail> dvrIP = %s Channel = %d m_PlayHandle = %d  m_index_MTManager = %d ret = %d nReceiveOneTime = %dkbps\r\n\r\n",
		//	m_UniSDKCltInfo.connInfo.ip, m_UniSDKCltInfo.channel, m_PlayHandle, m_index_MTManager, ret, nReceiveOneTime);
        //TRACE(_T("！！！！检测到网络出错，准备发送消息 majortype = %d, subtype = %d, m_index_MTManager = %d\r\n\r\n",
		//	m_UniSDKCltInfo.connInfo.dvrType, m_UniSDKCltInfo.connInfo.subType, m_index_MTManager);                     
        //::PostMessage(m_hNotifyWnd, WM_NET_ERROR, m_index_MTManager, m_UniSDKCltInfo.connInfo.dvrType << 16 | m_UniSDKCltInfo.connInfo.subType );
        m_exit = true;
        return -1;
    }
    if(m_exit)
    {
        //TRACE(_T("CMTVideo::InputData 退出2 dvrIP = %s Channel = %d m_PlayHandle = %d m_index_MTManager = %d nReceiveOneTime = %dkbps\r\n",
			//m_UniSDKCltInfo.connInfo.ip, m_UniSDKCltInfo.channel, m_PlayHandle, m_index_MTManager, nReceiveOneTime);
        return -1;
    }

	BOOL bret =  PlayM4_InputData(m_index, (BYTE*)m_buffer, ret);
	if (!bret)
	{
		int nErr =  PlayM4_GetLastError(m_index);
		if( nErr == PLAYM4_BUF_OVER )
			return 0;
		
		//string strErr = GetPlayMp4LastErrorStr( nErr );
		//TRACE_SDK_ERROR("执行 CHKSeries::SetStreamBuf 中  PlayM4_InputData 函数 Fail nErr = %d strErr = %s\r\n\r\n", nErr, strErr.c_str() );
		
		//TRACE(_T("CMTVideo::InputData 退出3 dvrIP = %s Channel = %d m_PlayHandle = %d m_index_MTManager = %d nReceiveOneTime = %dkbps\r\n\r\n",
		//	m_UniSDKCltInfo.connInfo.ip, m_UniSDKCltInfo.channel, m_PlayHandle, m_index_MTManager, nReceiveOneTime);
        //TRACE(_T("！！！！inputdata<0检测到网络出错，准备发送消息 majortype = %d, subtype = %d, m_index_MTManager = %d\r\n\r\n",
		//	m_UniSDKCltInfo.connInfo.dvrType, m_UniSDKCltInfo.connInfo.subType, m_index_MTManager);
        //::PostMessage(m_hNotifyWnd, WM_NET_ERROR, m_index_MTManager, m_UniSDKCltInfo.connInfo.dvrType << 16 | m_UniSDKCltInfo.connInfo.subType );
		
		
		return -nErr;
	}

    return 0;
}

int CPlayer::InputData_Frame()
{
    int ret = m_streamParser.GetOneFrame((BYTE*)m_buffer, &m_frameHeader);
    if( ret <= 0 )
    {
        if(m_exit)
        {
            //TRACE(_T("CMTVideo::InputData_Frame 退出1 dvrIP = %s Channel = %d m_PlayHandle = %d m_index_MTManager = %d\r\n",
			//	m_UniSDKCltInfo.connInfo.ip, m_UniSDKCltInfo.channel, m_PlayHandle, m_index_MTManager);
            return -1;
        }
        //TRACE(_T("CMTVideo::InputData_Frame <m_streamParser.GetOneFrame Fail> dvrIP = %s Channel = %d m_PlayHandle = %d  m_index_MTManager = %d\r\n\r\n",
		//	m_UniSDKCltInfo.connInfo.ip, m_UniSDKCltInfo.channel, m_PlayHandle, m_index_MTManager);
        //TRACE(_T("！！！！检测到网络出错，准备发送消息 majortype = %d, subtype = %d, m_index_MTManager = %d\r\n\r\n",
		//	m_UniSDKCltInfo.connInfo.dvrType, m_UniSDKCltInfo.connInfo.subType, m_index_MTManager);
        //::PostMessage(m_hNotifyWnd, WM_NET_ERROR, m_index_MTManager, m_UniSDKCltInfo.connInfo.dvrType << 16 | m_UniSDKCltInfo.connInfo.subType );
		//网络异常，接受不到数据给出警告！
		m_disconnection = true;

		while(ReStartMonitor()<0)//重新尝试连接并启动监视
		{
			if(m_exit)
			{
				return -1;
			}
			Sleep(1000);
		}
		return -1;
			//m_exit = true;
			//return -1;
    }

    if(m_exit)
    {
        //TRACE(_T("CMTVideo::InputData_Frame 退出2 dvrIP = %s Channel = %d m_PlayHandle = %d m_index_MTManager = %d\r\n",
		//	m_UniSDKCltInfo.connInfo.ip, m_UniSDKCltInfo.channel, m_PlayHandle, m_index_MTManager);
        return -1;
    }
	int encFrameLength = ret - m_frameHeader.MetaLength;
    BOOL bret =  PlayM4_InputData(m_index, (BYTE*)m_buffer, encFrameLength);
	if (!bret)
	{
		int nErr =  PlayM4_GetLastError(m_index);
		if( nErr == PLAYM4_BUF_OVER )
			return 0;
		
		//string strErr = GetPlayMp4LastErrorStr( nErr );
		//TRACE_SDK_ERROR("执行 CHKSeries::SetStreamBuf 中  PlayM4_InputData 函数 Fail nErr = %d strErr = %s\r\n\r\n", nErr, strErr.c_str() );
		
		//TRACE(_T("CMTVideo::InputData 退出3 dvrIP = %s Channel = %d m_PlayHandle = %d m_index_MTManager = %d nReceiveOneTime = %dkbps\r\n\r\n",
		//	m_UniSDKCltInfo.connInfo.ip, m_UniSDKCltInfo.channel, m_PlayHandle, m_index_MTManager, nReceiveOneTime);
        //TRACE(_T("！！！！inputdata<0检测到网络出错，准备发送消息 majortype = %d, subtype = %d, m_index_MTManager = %d\r\n\r\n",
		//	m_UniSDKCltInfo.connInfo.dvrType, m_UniSDKCltInfo.connInfo.subType, m_index_MTManager);
        //::PostMessage(m_hNotifyWnd, WM_NET_ERROR, m_index_MTManager, m_UniSDKCltInfo.connInfo.dvrType << 16 | m_UniSDKCltInfo.connInfo.subType );
		
		
		return -nErr;
	}
	//取出meta数据，叠加到视频
	if (m_frameHeader.FrameType <= 3)//去掉音频帧
	{
		m_MetaDataLock.Lock();
		memcpy( m_meta, &m_frameHeader.MetaLength, sizeof(U32));
		memcpy( m_meta+sizeof(U32), m_buffer + encFrameLength, m_frameHeader.MetaLength );
		m_MetaDataLock.Unlock();
	}
    return 0;
}

#define MPEG_TEMP_SIZE  (  6 * 1024 )
#define MPEG_SMALLTEMP_SIZE  (  0.05 * 1024 )
int CPlayer::InputData_Frame_PlayBack()
{    
    int DataLeft = 0;
    int DataRequest = 0;
    int DataRequestOneTime = 0;
    int nret = 0;
	
    //CAutoLock2  lock(&m_cMutex);
    if(m_playbacklen > 3 * 1024 *1024)
        DataRequestOneTime = MPEG_TEMP_SIZE;
    else
        DataRequestOneTime = MPEG_SMALLTEMP_SIZE;
	
    DataLeft = m_playbacklen - m_pos;
    if( DataLeft >= DataRequestOneTime )   //每次请求的数据不能超过 m_buffer的大小
        DataRequest = DataRequestOneTime;
    else
        DataRequest = DataLeft;
	
    nret = m_streamParser_PlayBack.GetOneFrame((BYTE*)m_buffer, &m_frameHeader);
    if(nret <= 0)
    {
        m_exit = true;
        TRACE(_T("(存储回放 解包1)error: GetOneFrame<0 ret=%d\r\n"), nret);
        if( nret == READ_DATA_OVER )//读数据结束
        {
            //等待播放结束
			int playRemain =  PlayM4_GetSourceBufferRemain(m_index);
			while (playRemain > 0)
			{
				Sleep(8000);
				playRemain =  PlayM4_GetSourceBufferRemain(m_index);
			}
			return 0;
        }
        return -1;
    }
    m_pos += nret;
    char* pDst = m_buffer;
	int encFrameLength = nret - m_frameHeader.MetaLength;
	int nLeft = encFrameLength;

	while ( nLeft > 0 )
	{
        if( m_exit )
            return 0;
		
		BOOL bret =  PlayM4_InputData(m_index, (BYTE*)pDst, nLeft);
        if (!bret)
		{
			int nErr =  PlayM4_GetLastError(m_index);
			while( nErr == PLAYM4_BUF_OVER )
			{
				Sleep(100);
				bret =  PlayM4_InputData(m_index, (BYTE*)pDst, nLeft);
				if (!bret)
				{
					nErr =  PlayM4_GetLastError(m_index);
				}
				else
				{
					nErr = 0;
				}
			}
		}
        pDst += nret;
		nLeft -= nret;
        if(nLeft > 0)
            Sleep(1);
    }
	
	if (m_frameHeader.FrameType <= 3)//去掉音频帧
	{
		//取出meta数据，叠加到视频
		m_MetaDataLock.Lock();
		memcpy( m_meta, &m_frameHeader.MetaLength, sizeof(m_frameHeader.MetaLength));
		memcpy( m_meta+sizeof(m_frameHeader.MetaLength), m_buffer + encFrameLength, m_frameHeader.MetaLength ); 
		m_MetaDataLock.Unlock();
		while(PlayM4_GetSourceBufferRemain(m_index) > 0)//服务器回放和本地播放一样，数据读取比较快，metaData全读取完了
		{
			::Sleep(10);
		}
	}
    return 0;
}



INT CPlayer::MonitorStartCmdMT(SOCKET sk, HHV_CLIENT_INFO* clientInfo, char* streamHeader,
                                int& size)
{   
	Client_Start_Monitor_Direct request ;
    CLIENT_START_MONITOR_RESPONSE response;
    ZeroMemory(&response, sizeof(response));

    request.header.command = SYSTEM_MONITOR_START_DIRECT;
    request.header.length = sizeof(request) - sizeof(COMMAND_HEADER);
    strncpy((char*)request.dvrIP, clientInfo->connInfo.ip, sizeof(request.dvrIP)-1 );
	request.dvrPort	= clientInfo->connInfo.port;
    strcpy(request.userName, clientInfo->connInfo.userName);
    strcpy( (char*)request.pwd, clientInfo->connInfo.pwd );
    request.channel = clientInfo->channel;
    request.childstream = clientInfo->childstream;
    //request.dvrtype = clientInfo->connInfo;
    //request.subtype = clientInfo->connInfo.subType;
    //request.modelID = clientInfo->connInfo.modelID; 
    //request.transType = 0;//tcp方式
    
    request.u32DVRMaxLinkNum = 60;
    request.u32VSMaxLinkNum =  500;
    //strcpy(request.DeviceSerialNum, info.IPAddr_L);
    //strcpy(request.ProxyIP, info.ProxyServerIP);
    //request.ProxyPort = info.ProxyServerPort;
    //发送接收数据
    request.h2n();
	
    if( send(sk, (char*)&request,sizeof(request), 0) <= 0 )
	{
        TRACE(_T("(MonitorStartCmdMT) 发送头失败\r\n"));
        //CCommClass::ErrorDeal(pErrorMsg, CLIENT_ERROR, VCT_NETERROR_SEND, false);
    	return HHV_ERROR_SEND;
    }

    int nRouteInfo = 0;
    if( send(sk, (char*)&nRouteInfo,sizeof(int), 0) <= 0 )
	{
        //TRACE(_T("(MonitorStartCmdMT) 发送nRouteInfo失败\r\n");
        //CCommClass::ErrorDeal(pErrorMsg, CLIENT_ERROR, VCT_NETERROR_SEND, false);
    	return HHV_ERROR_SEND;
    }
	
    if(nRouteInfo > 0)
    {
		char pRouteInfo[128] = {0x00};
		if( send(sk, (char*)pRouteInfo,nRouteInfo, 0) <= 0 )
		{
            //TRACE(_T("(MonitorStartCmdMT) 发送pRouteInfo失败\r\n");
        	//CCommClass::ErrorDeal(pErrorMsg, CLIENT_ERROR, VCT_NETERROR_SEND, false);
    		return HHV_ERROR_SEND;
    	}
    }

    if( recv(sk, (char*)&response,sizeof(response), 0 ) <= 0 )
	{
        //CCommClass::ErrorDeal(pErrorMsg, CLIENT_ERROR, VCT_NETERROR_RECV, false);
    	return HHV_ERROR_RECV;
    }

    response.n2h();
	if( response.header.command == SYSTEM_REQUEST_ACCEPT )
	{
        TRACE(_T("(MonitorStartCmdMT) Accept:%d n:%d\r\n"),
            response.header.command, htonl(response.header.command));
        TRACE(_T("(MonitorStartCmdMT)回应命令ACCEPT streamheadsize = %d dvrIP = %s channel=%d\r\n"),
                    response.streamheadsize, clientInfo->connInfo.ip, clientInfo->channel);
        if( recv(sk, (char*)streamHeader, response.streamheadsize, 0 ) <= 0 )
		{
            TRACE(_T("(MonitorStartCmdMT) 收streamHeader失败！streamheadsize = %d dvrIP = %s\r\n"),
                    response.streamheadsize, clientInfo->connInfo.ip);
            //CCommClass::ErrorDeal(pErrorMsg, CLIENT_ERROR, VCT_NETERROR_RECV, false);
            return HHV_ERROR_RECV;
        }
        size = response.streamheadsize;
        TRACE(_T("(MonitorStartCmdMT)成功结束 dvrIP = %s channel=%d\r\n"),
            clientInfo->connInfo.ip, clientInfo->channel);
		return 0;
	}
    else
    {
        size = response.reason;
        int errtype = response.streamheadsize ? SERVER_ERROR : SDK_ERROR;  //0:sdk错误， 1为Server错误
        //CCommClass::ErrorDeal(pErrorMsg, errtype, size, false);
        TRACE(_T("(MonitorStartCmdMT)服务器拒绝 cmd=%d(%d) reason=%d errtype(sdk(0) server(1))=%d dvrIP=%s channel=%d\r\n"),
            response.header.command, htonl(response.header.command), size, response.streamheadsize,
            clientInfo->connInfo.ip, clientInfo->channel);
    }
	return HHV_ERROR_CONNECT;
}

void CALLBACK CPlayer::MP4SDKDrawFun(long nPort,HDC hDc,LONG nUser)
{
	if (!CDVRSettings::GetInstance()->m_bDrawMetaData) // show/hide metadata
	{
		return;
	}

	Gdiplus::Graphics graphics(hDc);
	if(m_disconnection)
	{
		CDVRPlayer::FillRectAndDrawTextMeta(hDc);
		return;
	}

	CPlayer* pPlayer = (CPlayer*)nUser;
	if( pPlayer != NULL )
	{
		int renderWndWidth =  pPlayer->m_rcRenderRect.right-pPlayer->m_rcRenderRect.left;
		int renderWndHeight =  pPlayer->m_rcRenderRect.bottom-pPlayer->m_rcRenderRect.top;
		if (renderWndWidth <= 0 && renderWndHeight <= 0)
		{
			return;
		}

		BYTE byMetaData[sizeof(pPlayer->m_meta)];
		U32 metaLen = 0;
		pPlayer->m_MetaDataLock.Lock();
		memcpy(&metaLen, pPlayer->m_meta, sizeof(U32));
		memcpy(byMetaData, pPlayer->m_meta+sizeof(U32), metaLen);
		pPlayer->m_MetaDataLock.Unlock();

		HHV::FrameMetaDataList metaList;
		U32 readSize = 0;
		BYTE* pFMDStartPos = byMetaData;
		while(readSize < metaLen)
		{
			HHV::FrameMetaData fmd;
			pFMDStartPos = fmd.FromMemory(pFMDStartPos);
			metaList.push_back(fmd);
			readSize += fmd.memsize();
		}
		
		HHV::FrameMetaDataList scaledMetaData;

		pPlayer->m_spScaleMetaData->GetScaledFrameMetaDataList(scaledMetaData, metaList, renderWndWidth, renderWndHeight);
		
		for (HHV::FrameMetaDataList::const_iterator it = scaledMetaData.begin(); it != scaledMetaData.end(); ++it)
		{
			// Now, let's draw the meta data.
			CDVRPlayer::DrawFrameMetaData(graphics, *it, renderWndWidth, renderWndHeight);
		}
	}
}

void CALLBACK CPlayer::OnDrawFun(long nPort, HDC hDc, LONG nUser)
{
	if (!CDVRSettings::GetInstance()->m_bDrawMetaData) // show/hide metadata
	{
		return;
	}

	CPlayer* pPlayer = (CPlayer*)nUser;
	if( pPlayer != NULL )
	{
		int renderWndWidth =  pPlayer->m_rcRenderRect.right-pPlayer->m_rcRenderRect.left;
		int renderWndHeight =  pPlayer->m_rcRenderRect.bottom-pPlayer->m_rcRenderRect.top;
		if (renderWndWidth <= 0 && renderWndHeight <= 0)
		{
			return;
		}

		BYTE byMetaData[sizeof(pPlayer->m_meta)];
		U32 metaLen = 0;
		pPlayer->m_MetaDataLock.Lock();
		memcpy(&metaLen, pPlayer->m_meta, sizeof(U32));
		memcpy(byMetaData, pPlayer->m_meta+sizeof(U32), metaLen);
		pPlayer->m_MetaDataLock.Unlock();

		HHV::FrameMetaDataList metaList;
		U32 readSize = 0;
		BYTE* pFMDStartPos = byMetaData;
		while(readSize < metaLen)
		{
			HHV::FrameMetaData fmd;
			pFMDStartPos = fmd.FromMemory(pFMDStartPos);
			metaList.push_back(fmd);
			readSize += fmd.memsize();
		}
		
		HHV::FrameMetaDataList scaledMetaData;

		pPlayer->m_spScaleMetaData->GetScaledFrameMetaDataList(scaledMetaData, metaList, renderWndWidth, renderWndHeight);
		Gdiplus::Graphics graphics(hDc);

		for (HHV::FrameMetaDataList::const_iterator it = scaledMetaData.begin(); it != scaledMetaData.end(); ++it)
		{
			CDVRPlayer::DrawFrameMetaData(graphics, *it, renderWndWidth, renderWndHeight);
		}
	}

}

int CPlayer::StartPlayBackByTime(HWND hWnd, SYSTEM_VIDEO_FILE* recdFile,
								 char* ssIP, int ssPort)
{
	if( m_comSocket.Connect(ssIP, ssPort) < 0 )
    {
        m_comSocket.Close();
        return -1;
    }
	
    int   streamHeaderSize = 4*1024;  
    memset( m_streamHeader, 0x00, sizeof(m_streamHeader) ); 

	SPlayBackByTimeResponseInfo  rspBody;
    
    if( PlayStartCmdByTime(m_comSocket.m_hSocket, recdFile, m_streamHeader, streamHeaderSize, rspBody) < 0 )
    {
        m_comSocket.Close();
        return -1;
    }

	m_playbacklen = rspBody.nPlayBackLength;
    m_streamParser_PlayBack.SetSocketPara(&m_comSocket);
    m_streamParser_PlayBack.SetFileLenPara(m_playbacklen);

    //mode- 流模式（1-实时流/2-文件流）
	int nErr = 0;
	m_playType = STREAME_FILE;
	BOOL  bret =  PlayM4_SetStreamOpenMode(m_index, m_playType);
    bret =  PlayM4_OpenStream( m_index, (BYTE*)m_streamHeader, streamHeaderSize, 600*1024);
	if (!bret)
	{
		m_comSocket.Close();
		nErr =  PlayM4_GetLastError(m_index);
		//string strErr = GetPlayMp4LastErrorStr( nErr );
		//TRACE_SDK_ERROR("执行 CHKSeries::OpenStream 中  PlayM4_OpenStream 函数 Fail nErr = %d strErr = %s\r\n\r\n", nErr, strErr.c_str() );
		//RACE_LOG("(MTVideo)监视失败 <Error pDvrSDK->OpenPlayStream Fail> m_PlayHandle = %d dvrIP = %s channel = %d\r\n",
		//	m_PlayHandle, clientInfo->connInfo.ip, clientInfo->channel);
		
		return -nErr;
	}
	
	if( ! PlayM4_RigisterDrawFun( m_index, MP4SDKDrawFun, (DWORD)this) )
	{
		m_comSocket.Close();
		nErr =  PlayM4_GetLastError(m_index);
		//TRACE_SDK_ERROR("执行 CHKSeries::RegisterDrawFun 中  PlayM4_RigisterDrawFun 函数 Fail nErr = %d strErr = %s\r\n\r\n", nErr, strErr.c_str() );
		return -nErr;
	}
	
	bret =  PlayM4_Play(m_index, hWnd);
	if (!bret)
	{
		nErr =  PlayM4_GetLastError(m_index);
		//string strErr = GetPlayMp4LastErrorStr( nErr );
		//TRACE_SDK_ERROR("执行 CHKSeries::OpenStream 中  PlayM4_Play 函数 Fail nErr = %d strErr = %s\r\n\r\n", nErr, strErr.c_str() );
		return -nErr;
	}

	m_hRenderWnd = hWnd;
	ResizeMonitorWindow();
    //开启线程, 接收视频流
    m_exit = false;
    UINT threadid(0);
    m_Thread = (HANDLE)_beginthreadex( NULL, 4*1024*1024, DecoderRoutine, this, 0, &threadid ); 
    if(m_Thread <= 0)
    {
        TRACE(_T("(MTVideo)!!!!!!开辟接受数据线程失败！m_Thread:%d errno:%d _doserrno:%d \r\n"),
            m_Thread, errno, _doserrno);
        SetEvent( m_hExitEvent );
    }
	
    CloseHandle( m_Thread );
	
    return m_index;
	return 0;
}
void CPlayer::StopPlayBackByTime(int realHandle)
{
	//memset(&m_rcRenderRect, 0, sizeof(RECT));
	StopMonitor();
	m_hRenderWnd = NULL;
}


int CPlayer::PlayStartCmdByTime( SOCKET sk, SYSTEM_VIDEO_FILE* recdFile, char* streamHeader, 
								int& size, SPlayBackByTimeResponseInfo&  rspBody )//回放请求
{
    //headersize OK
    Playback_ByTime_REQUEST_MSG           msg;
    memset( &msg, 0x00, sizeof(msg));
    msg.header.command = SYSTEM_VIDEO_Playback_ByTime_REQUEST;
    msg.header.length = sizeof(msg) - sizeof(COMMAND_HEADER);
    msg.channel = recdFile->channel;
    msg.StartDate = recdFile->start_date;
    msg.StartTime = recdFile->start_time;
    msg.EndDate = recdFile->end_date;
    msg.EndTime = recdFile->end_time;
    msg.h2n();
    
    if (send(sk, (char*)&msg,sizeof(msg), 0) <= 0 )
	{
        return HHV_ERROR_SEND;
    }

	FILE_OPEN_RESPONSE_MSG	msgresp;
	memset(&msgresp, 0, sizeof(msgresp));
    if( recv(sk, (char*)&msgresp, sizeof(msgresp), 0 ) <= 0 )
    {
        return HHV_ERROR_RECV;
    }
	
    if( htonl(msgresp.header.command) != SYSTEM_REQUEST_ACCEPT )
    {
       return -1;
    }

	//SPlayBackByTimeResponseInfo  rspBody;
    memset(&rspBody, 0, sizeof(rspBody));
    if( recv(sk, (char*)&rspBody, sizeof(rspBody), 0 ) <= 0 )
    {
        return HHV_ERROR_RECV;
    }
	rspBody.h2n();

	m_playbacklen = rspBody.nPlayBackLength;
    int streamHeaderSize = rspBody.nFileHeadSize;
    //if( recv(sk, (char*)&streamHeaderSize, sizeof(int), 0 ) <= 0 )
    //    {
    //        return HHV_ERROR_RECV;
    //    }
    //    streamHeaderSize = ntohl(streamHeaderSize);
    
    if(streamHeaderSize > 0)
    {
        if( recv(sk, (char*)streamHeader, streamHeaderSize, 0 ) <= 0 )
        {
            return HHV_ERROR_RECV;
        }
    }
    size = streamHeaderSize;
    return 0;
}


#define BUF_SIZE 3008
DWORD WINAPI CPlayer::InputStreamThread( LPVOID lpParameter)
{
	CPlayer* pThis = (CPlayer*)lpParameter;
	HANDLE hMulEvents[2];
	hMulEvents[0] = pThis->m_hEventKill;
	hMulEvents[1] = pThis->m_hEventInput;
	BYTE pBuf[BUF_SIZE];
	DWORD nRealRead;
	BOOL bBufFull = FALSE;
	DWORD dwSize = BUF_SIZE;
	unsigned char chType;
	DWORD	dwDataLen = 0;

	const int nMaxNumNoMetaDataFrame = 8; //There are about 10 MetaData per second. 
	int nCountNoMetaData = 0;
	while (WAIT_OBJECT_0 != WaitForMultipleObjects(2, hMulEvents, FALSE, INFINITE))
	{	
		if(!bBufFull)
		{
			int nRet = 0;
			if(TRUE)
			{
				memset(pThis->m_buffer, 0, sizeof(CHAR)*MAX_FRAME_LENGTH);
				nRet = pThis->m_spStreamParser->GetOneFrame((BYTE*)pThis->m_buffer, &pThis->m_frameHeader);
				//if(!(ReadFile(pThis->m_hStreamFile, pBuf, dwSize, &nRealRead, NULL) && (nRealRead == dwSize)))
				if (nRet <= 0)
				{
					bBufFull = FALSE;
					//FileEndCallBack(pThis->GetPort(), pThis);
					ResetEvent(pThis->m_hEventInput);
					break;//文件结束，退出循环
				}
				dwDataLen = nRet;
			}

			if (pThis->m_frameHeader.FrameType <= 3)//去掉音频帧
			{
				int encFrameLength = nRet - pThis->m_frameHeader.MetaLength;
			
				if (pThis->m_frameHeader.MetaLength == 0)
				{
					++nCountNoMetaData;
				}
				else
				{
					nCountNoMetaData = 0;
				}
				if (pThis->m_frameHeader.FrameRate == 25)
				{
					pThis->m_dwStreamCurrentTime = htonl(pThis->m_frameHeader.FrameTime) - pThis->m_spStreamParser->GetFirstFrameTime();
				}
				if (pThis->m_frameHeader.MetaLength > 0 || 
					nCountNoMetaData >= nMaxNumNoMetaDataFrame)	//To make sure, it's true there is no metadata. 
				{
					pThis->m_MetaDataLock.Lock();
					memcpy( pThis->m_meta, &pThis->m_frameHeader.MetaLength, sizeof(U32));
					if (pThis->m_frameHeader.MetaLength != 0) 
					{
						memcpy( pThis->m_meta+sizeof(U32), pThis->m_buffer + encFrameLength, pThis->m_frameHeader.MetaLength );
					}
				
					pThis->m_MetaDataLock.Unlock();
					nCountNoMetaData = 0;
				}

				if ( !NAME(PlayM4_InputData)(pThis->m_index, (BYTE*)pThis->m_buffer, encFrameLength) )
				{
					int nErr = PlayM4_GetLastError(pThis->m_index);
					if (nErr  == PLAYM4_PARA_OVER)
					{
						Sleep(10);
						continue;
					}
					if (nErr  == PLAYM4_BUF_OVER )
					{
						Sleep(10);
						continue;
					}

					bBufFull = TRUE;
					ResetEvent(pThis->m_hEventInput);
					continue;
				}

				int playRemain =  PlayM4_GetSourceBufferRemain(pThis->m_index);

				while(playRemain > 0)
				{
					::Sleep(10);
					playRemain =  PlayM4_GetSourceBufferRemain(pThis->m_index);
				}
			}
		}
	}

	return 1;
}