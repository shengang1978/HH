// UserLogin.cpp: implementation of the CLoginDvrMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LoginDvrMgr.h"
#include "LoginDvr.h"
#include "Process.h"
#include "Player.h"
#include "DvrSDKErr.h"
#include "assert.h"
#include "CommDef.h"

#define SUYUAN_NO_CHECK_DVR_ONLINE	1	//苏源不检测dvr是否掉线

CLoginDvrMgr		g_LoginDvrMgr;

CLoginDvrMgr::CLoginDvrMgr()
	: m_loginInfo(MAX_LOGIN_DVR)
{
	m_status = STOP_STATUS;

	m_hHeattbeat = NULL;
}

CLoginDvrMgr::~CLoginDvrMgr()
{
	Clearup();
}

void CLoginDvrMgr::Startup()
{
	if( m_status == RUN_STATUS )
		return;
	for( int i = 0; i < MAX_LOGIN_DVR; ++i )
		m_loginInfo[i] = NULL;
	m_bExit = false;
	m_hEventThreadExit	= CreateEvent( NULL, TRUE, FALSE, NULL );
	m_hCreateWindowEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	UINT threadid(0)   ; 
	m_hCheckOnline = (HANDLE)_beginthreadex(  NULL , 0 , CheckOnlineThread  ,  this , 0 , &threadid ) ;
	
	m_status = RUN_STATUS;

//	for( i = 0; i < MAX_TRANSSERIAL_NUM; ++i )
//		m_transSerial[i] = NULL;
}
void CLoginDvrMgr::Clearup()
{
	if( m_status == STOP_STATUS )
	{
		//TRACE(_T("CLoginDvrMgr::Clearup()中m_status == STOP_STATUS， 返回\r\n\r\n");
		return;
	}
	m_bExit = true;

	//设置退出事件
	SetEvent( m_hEventThreadExit );
	
	//等待线程退出
	WaitForSingleObject( m_hCheckOnline , INFINITE );

	//关闭句柄
//	TRACE(_T("CLoginDvrMgr::Clearup()中关闭句柄 \r\n\r\n"));
	SAFE_CLOSE( m_hEventThreadExit );
	SAFE_CLOSE( m_hCheckOnline );
	
	
	for( int i = 0; i < MAX_LOGIN_DVR; ++i )
		SAFE_DELETE(m_loginInfo[i]);	

	m_status = STOP_STATUS;
	ATLTRACE(_T("CLoginDvrMgr::Clearup 成功返回\r\n\r\n"));
}

//return the index handle
int	CLoginDvrMgr::Login(LPCTSTR userName, LPCTSTR pwd, LPCTSTR addr, int port)
{
	if( m_status == STOP_STATUS )
	{
		ATLTRACE(_T("处于STOP_STATUS状态 CLoginDvrMgr::Login IP = %s port = %d\r\n\r\n"), addr, port);
		return HHV_ERROR_STOP_STATUS;
	}
	
	CLoginDvr* login = NULL;
	int index = 0;

	m_csLock.Lock();//加锁
	login = GetDvrLoginInfo(addr, port);
	if( login == NULL )//不存在，先创建一个，然后占位
	{
		index = GetFreeIndex();
		if( index < 0 )
		{
			//g_Config.lastError = HHV_ERROR_LOCALLOGINMAX;
			ATLTRACE(_T("获得占位序号失败 CLoginDvrMgr::Login GetFreeIndex IP = %s port = %d\r\n\r\n"), addr, port);
			m_csLock.Unlock();//解锁
			return HHV_ERROR_LOCALLOGINMAX;
		}

		login = new CLoginDvr( addr, port, userName, pwd, index);
		m_loginInfo[index]	= login;//完成占位
	}
	m_csLock.Unlock();//解锁
	
	int ret = login->Login( userName, pwd, addr, port );
	if( ret < 0 )
		return ret;

	return ret;//实际返回index
}

int	CLoginDvrMgr::Logout( int index )
{
	if( m_status == STOP_STATUS )
	{
		ATLTRACE(_T("处于STOP_STATUS状态 CLoginDvrMgr::Logout index = %d\r\n\r\n"), index);
		return HHV_ERROR_STOP_STATUS;
	}	
	
	if( CheckUserID( index) < 0 )
		return HHV_ERROR_INVALIDPARA;

	m_csLock.Lock();	
	CLoginDvr* login = m_loginInfo[index];
	m_csLock.Unlock();

	if( login == NULL )
	{
		return HHV_ERROR_NOLOGIN;
	}
	
	login->Logout();

	return 0;
}

int CLoginDvrMgr::GetFreeIndex()
{
	for( int i = 0; i < MAX_LOGIN_DVR; ++i )
	{
		if( m_loginInfo[i] == NULL )
			return i;
	}
	return -1;
}

CLoginDvr* CLoginDvrMgr::GetDvrLoginInfo(LPCTSTR addr, int port)
{
	for( int i = 0; i < MAX_LOGIN_DVR; ++i )
	{
		if( m_loginInfo[i] == NULL )
			continue;
		CLoginDvr* pLoginDvr = m_loginInfo[i];
		if( pLoginDvr->IsSame( addr, port ) )
		{
			return pLoginDvr;
		}
	}
	return NULL;
}
int CLoginDvrMgr::CheckUserID(int index)
{
	if( index < 0 || index > MAX_LOGIN_DVR  )
		return -1;
	return 0;
}


int CLoginDvrMgr::CheckOnlineStatusLoop( )
{
	while (!m_bExit)
	{
		DWORD ret = WaitForSingleObject( m_hEventThreadExit, 1000 );
		if( ret == WAIT_OBJECT_0 )//退出
		{
			break;
		}
		
		CLoginDvr*	login = NULL;		
		for( int i = 0; i < MAX_LOGIN_DVR; ++i )
		{
			if( m_bExit )
				break;
			m_csLock.Lock();
			login = m_loginInfo[i];
			m_csLock.Unlock();
		
//			TRACE(_T("DVR类型 = %d\r\n", login->m_loginInfo.deviceInfo.byDVRType);
			if( login != NULL && (login->m_loginInfo.loginID >= 0) )
			{		
				login->CheckOnlineStatus(0);
			}
		}
	}
	return 0;
}

int CLoginDvrMgr::MakeKeyFrame(LONG index, LONG lChannel)
{
	if( m_status == STOP_STATUS )
		return HHV_ERROR_STOP_STATUS;
	
	if( CheckUserID( index ) < 0 )
		return HHV_ERROR_INVALIDPARA;

	m_csLock.Lock();
	CLoginDvr* login = m_loginInfo[index];
	m_csLock.Unlock();

	if( login == NULL )
		return HHV_ERROR_NOLOGIN;

	int ret = login->MakeKeyFrame(index, lChannel);
	return 0;
}

UINT WINAPI CLoginDvrMgr::CheckOnlineThread( void * dat)
{
	CLoginDvrMgr* pOwner = (CLoginDvrMgr*)dat;
	assert( pOwner );
	
	pOwner->CheckOnlineStatusLoop();
	return 1;
}

void CLoginDvrMgr::NotifyLogoutMessage( int index, char* dvrIP)
{
	ATLTRACE(_T("CLoginDvrMgr::NotifyLogoutMessage  \r\n"));
}

int CLoginDvrMgr::ReStartDVR( int userID, int restartType, int channel )
{
	if( CheckUserID( userID ) < 0 )
	{
		return HHV_ERROR_INVALIDPARA;
	}
	
	m_csLock.Lock();
	CLoginDvr* login = m_loginInfo[userID];
	m_csLock.Unlock();
	
	if( login == NULL )
	{
		return HHV_ERROR_NOLOGIN;
	}
	
	return login->ReStartDVR( userID, restartType, channel );
	return 0;	
}

int CLoginDvrMgr::StartDVRRecord( int userID, int channel )
{
	if( CheckUserID( userID ) < 0 )
	{
		return HHV_ERROR_INVALIDPARA;
	}
	
	m_csLock.Lock();
	CLoginDvr* login = m_loginInfo[userID];
	m_csLock.Unlock();
	
	if( login == NULL )
	{
		return HHV_ERROR_NOLOGIN;
	}
	
	return login->StartDVRRecord( userID, channel );
}
int CLoginDvrMgr::StopDVRRecord( int userID, int channel )
{
	if( CheckUserID( userID ) < 0 )
	{
		return HHV_ERROR_INVALIDPARA;
	}
	
	m_csLock.Lock();
	CLoginDvr* login = m_loginInfo[userID];
	m_csLock.Unlock();
	
	if( login == NULL )
	{
		return HHV_ERROR_NOLOGIN;
	}
	
	return login->StopDVRRecord( userID, channel );
}

int CLoginDvrMgr::SetVideoEffect(int lUserID, int channel, int bright,int contrast, int saturation, int hueValue)
{
	//if( CheckUserID( lUserID ) < 0 )
	//	{
	//		return HHV_ERROR_INVALIDPARA;
	//	}
	//	
	//	m_csLock.Lock();
	//	CLoginDvr* login = m_loginInfo[lUserID];
	//	m_csLock.Unlock();
	//	
	//	if( login == NULL )
	//	{
	//		return HHV_ERROR_NOLOGIN;
	//	}
	//	
	//	return login->SetVideoEffect( lUserID, channel, bright, contrast, saturation, hueValue);
	return 0;
}


int CLoginDvrMgr::GetVideoEffect(int lUserID, int channel,int &bright, int &contrast, 
							int &saturation, int &hueValue)
{
	//if( CheckUserID( lUserID ) < 0 )
	//	{
	//		return HHV_ERROR_INVALIDPARA;
	//	}
	//	
	//	m_csLock.Lock();
	//	CLoginDvr* login = m_loginInfo[lUserID];
	//	m_csLock.Unlock();
	//	
	//	if( login == NULL )
	//	{
	//		return HHV_ERROR_NOLOGIN;
	//	}
	//	
	//	return login->GetVideoEffect( lUserID, channel, bright, contrast, saturation, hueValue);
	return 0;
}


INT CLoginDvrMgr::UploadCfgFile(int userID, int cfgType, int channel, LPCTSTR imageName )
{
	if( m_status == STOP_STATUS )
		return HHV_ERROR_STOP_STATUS;
	
	if( CheckUserID( userID ) < 0 )
		return HHV_ERROR_INVALIDPARA;

	m_csLock.Lock();
	CLoginDvr* login = m_loginInfo[userID];
	m_csLock.Unlock();

	if( login == NULL )
		return HHV_ERROR_NOLOGIN;

	return login->UploadCfgFile(cfgType, channel, imageName );
}
