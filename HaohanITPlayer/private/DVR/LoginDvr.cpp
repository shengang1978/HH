// UserLogin.cpp: implementation of the CLoginDvr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LoginDvr.h"
#include "Player.h"
#include "DvrSDKErr.h"
//#include "struct_TCPServ.h"
#include "./NetDef.h"
#include "CommClass.h"
#include "UploadFileMgr.h"

//#define SUYUAN_NO_CHECK_DVR_ONLINE	1

int g_connectTimeOut = 5;//10*1000;

extern HWND			g_hWnd;
VOID __stdcall TimerProc(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime);

CLoginDvr::CLoginDvr(LPCTSTR pdvrIP, int pdvrPort, LPCTSTR puserName, LPCTSTR puserPwd, int nIndex)
{
	strcpy( m_loginInfo.dvrIP, CT2A(pdvrIP) );
	m_loginInfo.dvrPort = pdvrPort;

	TRACE(_T("CLoginDvr::CLoginDvr中pdvrIP = %s, pdvrPort = %d, puserName =%s, puserPwd = %s \r\n"), pdvrIP, pdvrPort, puserName, puserPwd);
	strcpy( m_loginInfo.userName, CT2A(puserName) );
	strcpy( m_loginInfo.userPwd, CT2A(puserPwd) );
	m_index = nIndex;
	m_loginInfo.index = nIndex;
	m_loginInfo.logined = STATUS_UNLOGINED;
}

CLoginDvr::~CLoginDvr()
{
	Clearup();
}

void CLoginDvr::Startup()
{

}
void CLoginDvr::Clearup()
{

}

//return the index handle
int	CLoginDvr::Login(LPCTSTR userName, LPCTSTR pwd, LPCTSTR addr, int port)
{
	LOGIN_STRUCT* login = &m_loginInfo;
	int index = m_index;

	//2010-03-19 added
	m_csLock.Lock();
	if( login->hSocket != INVALID_SOCKET )//先关闭socket
	{
		TRACE(_T("登录时先关闭原socket index = %d IP = %s SOCKET = %d\r\n\r\n"),login->index, login->dvrIP, login->hSocket );
		closesocket( login->hSocket );
		login->hSocket = INVALID_SOCKET;
	}
	login->logined = STATUS_UNLOGINED;
	
	int ret = LoginDvr_T(userName, pwd, addr, port, login);//返回登录成功SOCKET 
	
	if( ret < 0 )
	{
		m_csLock.Unlock();
		return ret;
	}

	login->hSocket = (SOCKET)ret;
	
	//登录成功
	login->logined = STATUS_LOGINED;//记录登录状态
	
	m_csLock.Unlock();

	return login->index;
}

/* 登出时不删除占位，只关闭socket
 *	
 */
int	CLoginDvr::Logout( )
{	
	LOGIN_STRUCT* login = &m_loginInfo;

	m_csLock.Lock();
	
	closesocket( login->hSocket );//只关闭socket
	login->hSocket = INVALID_SOCKET;
	login->logined = STATUS_UNLOGINED;
	login->loginID = -1;
	m_csLock.Unlock();
	return 0;
}

//检测是否在线,兼发送心跳
int CLoginDvr::CheckOnlineStatus( int index)
{
	LOGIN_STRUCT* login = &m_loginInfo;
	
	CAutoLock_CS		lc(&m_csLock);
	if( login->logined == STATUS_UNLOGINED )
	{
//		TRACE(_T("CLoginDvr::CheckOnlineStatus()中login->logined == STATUS_UNLOGINED， 退出--------------checkonline------2 \r\n");
		return 0;
	}
	int ret = -1;
	//ret = CInterfaceCmd::CheckSocketStatus( login->hSocket );
	if( ret < 0 )
	{
		Logout( );
		NotifyLogoutMessage(m_index, login->dvrIP );
	}
	
	return ret;
}

int	CLoginDvr::LoginDvr_T(LPCTSTR userName,  LPCTSTR pwd, LPCTSTR addr, int port,LOGIN_STRUCT* login )
{
	SOCKET sk = CreateSocket( 0, 30 );

	if( sk == INVALID_SOCKET )
	{
		TRACE(_T("创建Socket失败 CLoginDvr::Login IP = %s port = %d\r\n\r\n"), addr, port);
		return HHV_ERROR_CREATESOCKET;
	}
	if( Connect(sk, CT2A(addr), port,g_connectTimeOut ) == SOCKET_ERROR )
	{
		int err = GetLastError();
		TRACE(_T("连接DVR失败 CLoginDvr::Login IP = %s port = %d SOCKET = %d, err=%d\r\n\r\n"), addr, port, sk, err);
		closesocket(sk);
		return HHV_ERROR_CONNECT;
	}
	
	int ret = LoginDvrCmd( sk, userName, pwd, login );
	if( ret < 0 )
	{
		TRACE(_T("登录命令交互失败 CLoginDvr::Login IP = %s port = %d userName = %s pwd = %s\r\n\r\n"), addr, port, userName, pwd);
		closesocket( sk );
		return ret;
	}
	return (int)sk;
}

int CLoginDvr::MakeKeyFrame(LONG index, LONG channel)
{
	TRACE(_T("进入CLoginDvr::MakeKeyFrame，\r\n") );
	LOGIN_STRUCT* login = &m_loginInfo;
	if( login == NULL )
		return HHV_ERROR_NOLOGIN;

	CAutoLock_CS lc(&m_csLock);
	
	REMOTE_MAKE_IFRAME_REQUEST		msg;
	REMOTE_MAKE_IFRAME_RESPONSE		rsp;
	msg.header.command	= REMOTE_SYSTEM_MAKE_IFRAME;
	msg.loginID	= login->logined;
	msg.Channel	= channel;
	msg.h2n();
	
	int ret = SendAndRecv(login->hSocket, (char*)&msg, sizeof(msg), (char*)&rsp, sizeof(rsp) );
	if( ret < 0 )
	{
		int err = GetLastError();
		TRACE(_T("MakeKeyFrame 返回小于0， ret = %d,  err = %d\r\n"), ret, err );
		return ret;
	}
	rsp.n2h();
	if( rsp.header.command != SYSTEM_REQUEST_ACCEPT )
	{
		return HHV_ERROR_GENEIFRAME;
	}
	return 0;
}

void CLoginDvr::NotifyLogoutMessage( int index, char* dvrIP)
{
	TRACE(_T("CLoginDvr::NotifyLogoutMessage  \r\n"));
	
//	UINT uMsg = WM_DLDVR_LOGOUT_NOTIFY;
//	UINT wParam = index; 
//	UINT lParam = 0;//保留
//	
//	if( g_Config.hNotifyWnd != NULL)
//	{
//		::PostMessage( g_Config.hNotifyWnd, uMsg, wParam, lParam );	
//	}
//
//	NET_DLDVR_EXCEPTION	ext;
//	ext.Msg = uMsg;
//	ext.wParam = wParam;
//	ext.lPara = lParam;
//	if( g_Config.funCallBack != NULL )
//	{
//		TRACE(_T("NotifyLogoutMessage 中调用g_Config.funCallBack， index = %d IP = %s \r\n\r\n", index, dvrIP);
//
//		g_Config.funCallBack( EXCEPTION_NOTIFY, dvrIP, (char*)&ext, sizeof(NET_DLDVR_EXCEPTION) );			
//	}
//
//	SEGMENT_LOGIN_STRUCT	login;	
//	long userID = -1;
//	for( int i =0; i< MAX_LOGIN_DVR; i++)
//	{
//		int ret = g_LoginDvrMgr.GetSegmentLoginInfoByUserID( i, login );
//		if( ret < 0 )
//			continue;
//		if( strcmp( dvrIP, login.dvrIP ) == 0)
//		{
//			userID = i;
//			break;
//		}
//	}
//	if( g_Config.funCallBackEx != NULL )
//	{
//		g_Config.funCallBackEx( EXCEPTION_NOTIFY, userID, (char*)&ext, sizeof(NET_DLDVR_EXCEPTION) );			
//	}
//
//	for( i = 0; i < MAX_PLAYER; i++)
//	{
//		if( g_Config.funCallBack_Index[i] != NULL)
//		{
//			g_Config.funCallBack_Index[i](  EXCEPTION_NOTIFY, dvrIP, (char*)&ext, sizeof(NET_DLDVR_EXCEPTION) );		
//		}
//		
//		if( g_Config.funCallBackEx_Index[i] != NULL )
//		{
//			g_Config.funCallBackEx_Index[i]( EXCEPTION_NOTIFY, userID, (char*)&ext, sizeof(NET_DLDVR_EXCEPTION)) ;			
//		}
//	}

}

int CLoginDvr::ReStartDVR( int loginID, int restartType, int channel )
{
	int ret = 0;
	LOGIN_STRUCT* login = &m_loginInfo;	
	CAutoLock_CS lc(&m_csLock);

	REMOTE_REBOOT_DVR_REQUEST				msg;
	REMOTE_REBOOT_DVR_RESPONSE				rsp;
	msg.header.command					= REMOTE_SYSTEM_REBOOT_DVR;
	msg.loginID							= loginID;
	msg.h2n();
	
	ret = SendAndRecv(login->hSocket, (char*)&msg, sizeof(msg), (char*)&rsp, sizeof(rsp) );
	if( ret < 0 )
		return ret;	
	rsp.n2h();
	if( rsp.header.command != SYSTEM_REQUEST_ACCEPT )
		return HHV_ERROR_REBOOTDVR;
	return 0;
}

int CLoginDvr::StartDVRRecord( int userID, int channel )
{
	int ret = 0;
	LOGIN_STRUCT* login = &m_loginInfo;	
	CAutoLock_CS lc(&m_csLock);

	SRecordStart_Msg_Direct msg;
	memset( &msg, 0x00, sizeof(SRecordStart_Msg_Direct) );
	msg.header.command = SYSTEM_START_RECORD_Direct;
	msg.Channel = channel;
	
	msg.h2n();

	if(send(login->hSocket, (char*)&msg, sizeof(msg), 0 ) <= 0 )
	{
		return HHV_ERROR_SEND;
    }


	return 0;
}

int CLoginDvr::StopDVRRecord( int userID, int channel )
{
	int ret = 0;
	LOGIN_STRUCT* login = &m_loginInfo;	
	CAutoLock_CS lc(&m_csLock);
	
	SRecordStop_Msg_Direct oSRecStop;
	memset( &oSRecStop, 0x00, sizeof(SRecordStop_Msg_Direct) );

	oSRecStop.header.command = SYSTEM_STOP_RECORD_Direct;
	
	oSRecStop.Channel = channel;
	
	oSRecStop.h2n();

	if(send(login->hSocket, (char*)&oSRecStop, sizeof(oSRecStop), 0 ) <= 0 )
	{
		return HHV_ERROR_SEND;
    }

	return 0;
}

//int CLoginDvr::GetDvrWorkState( int userID, NET_DLDVR_WORKSTATE* workState )
//{
	//int ret = 0;
	//	LOGIN_STRUCT* login = &m_loginInfo;	
	//	CAutoLock_CS lc(&m_csLock);
	//	
	//	ret = CInterfaceCmd::GetDvrState( login->hSocket, login->loginID, workState, NULL);
	//
	//	OnNetDisconnected_T( ret );
	////	TRACE(_T("正确退出CDVRCfg::GetDvrWorkState, ret=%d\r\n",ret);
	//	return ret;
//	return 0;
//}

int CLoginDvr::GetDvrVersion(long userID, char *buf)
{
	//int ret = 0;
	//	LOGIN_STRUCT* login = &m_loginInfo;	
	//	CAutoLock_CS lc(&m_csLock);
	//	
	//	ret = CInterfaceCmd::GetDvrVersion(login->hSocket, login->loginID, buf, NULL );
	//	OnNetDisconnected_T( ret );
	//	
	//	return ret;
	return 0;
}

//int CLoginDvr::GetDvrDiskState( int userID, NET_DLDVR_DVRDISKSTATE* diskState )
//{
	//int ret = 0;
	//	LOGIN_STRUCT* login = &m_loginInfo;	
	//	CAutoLock_CS lc(&m_csLock);
	//	
	//	ret = CInterfaceCmd::GetDvrDiskState( login->hSocket, login->loginID, diskState, NULL);
	//
	//	OnNetDisconnected_T( ret );
	//	return ret;
//	return 0;
//}

int CLoginDvr::FindFileByTime(int userID, int channel, UINT startTime, UINT stopTime,  WORD MaxCount, char *buf)
{
	//LOGIN_STRUCT* login = &m_loginInfo;	
	//	CAutoLock_CS lc(&m_csLock);
	//	
	//	int ret = 0;
	//	
	//	ret = CInterfaceCmd::FindFileByTime( login->hSocket, login->loginID, channel, startTime, stopTime, MaxCount, buf, NULL );
	//	
	//
	//	OnNetDisconnected_T( ret );
	//	TRACE(_T("CDVRCfg::FindFileOpen中调用CInterfaceCmd::FindFileOpen ret= %d，退出FindFileOpen\r\n", ret);
	//	return ret;

	return 0;
	
}

int CLoginDvr::LoginDvrCmd( SOCKET sk, LPCTSTR strUser, LPCTSTR strPass, LOGIN_STRUCT* login )
{
    NET_LOGIN_MESSAGE			msg ;
	NET_LOGIN_RESPONSE			res;
	memset(&msg, 0x00, sizeof(msg));
	memset(&res, 0x00, sizeof(res));
	
	msg.header.command  = SYSTEM_LOGIN_REQUEST;
	msg.header.length = sizeof(msg) - sizeof(msg.header);
	strcpy( msg.UserName, CT2A(strUser));
	strcpy( msg.PassWord, CT2A(strPass));
	
	msg.h2n();
	
	if(send(sk, (char*)&msg, sizeof(msg), 0 ) <= 0 )
	{
        //Application->MessageBoxA( "登陆错误1, 发送登陆命令失败!",
		//	GetString("ID_MESSAGE_ERROR").c_str(), MB_ICONWARNING );
		return HHV_ERROR_SEND;
    }
	
	if(recv(sk, (char*)&res, sizeof(res), 0 ) <= 0 )
    {
        //Application->MessageBoxA( "登陆错误2, 收不到服务器回应,请检查服务器是否打开!",
		//	GetString("ID_MESSAGE_ERROR").c_str(), MB_ICONWARNING );
		return HHV_ERROR_RECV ;
    }
	
	res.n2h();
	
	if ( res.header.command != SYSTEM_REQUEST_ACCEPT )
	{
		switch ( res.reason )
		{
			
		case USER_INVALID_USERNAME:
		case USER_INVALID_PASSWORD:
		case USER_GROUP_NOTALLOCATE:
			//Application->MessageBoxA( GetString("ID_COMMCLASS_TEXT_C9").c_str(),
			//	GetString("ID_MESSAGE_ERROR").c_str(), MB_ICONWARNING );
			
			return HHV_ERROR_INVALID_USERNAMEORPWD;
			
		case USER_ADMINI_ONLINE:
			//Application->MessageBoxA( GetString("ID_COMMCLASS_TEXT_C10").c_str(),
			//	GetString("ID_MESSAGE_ERROR").c_str(), MB_ICONWARNING );
			return HHV_ERROR_ADMINI_ONLINE;
			
			break;
        case DataBase_ERROR:
            //Application->MessageBoxA( GetString("ID_COMMCLASS_TEXT_C16").c_str(),
			//	GetString("ID_MESSAGE_ERROR").c_str(), MB_ICONWARNING );
			return HHV_ERROR_DataBase_ERROR;
			break;
        default:
            //Application->MessageBoxA( GetString("ID_COMMCLASS_TEXT_C17").c_str(),
			//	GetString("ID_MESSAGE_ERROR").c_str(), MB_ICONWARNING );
			return HHV_ERROR_USER_NORESON;
			break;
		}
	}

    //nCSVersion = res.CSVersion; //CS版本信息               
	//csSameNameID = res.csSameNameID;//用户ID

	return 0;
}  

bool CLoginDvr::IsSame( LPCTSTR dvrIP, int dvrPort )
{
	if( strcmp(m_loginInfo.dvrIP, CT2A(dvrIP)) == 0 && 
		m_loginInfo.dvrPort == dvrPort )
		return true;
	return false;
}


INT CLoginDvr::UploadCfgFile(int fileType, int channel, LPCTSTR imageName )
{
	int ret = 0;
	LOGIN_STRUCT* login = &m_loginInfo;	
	CAutoLock_CS lc(&m_csLock);

	return g_UploadFileMgr.UploadCfgFile(login->loginID, CA2T(login->dvrIP), login->dvrPort,
										fileType, channel, imageName);
}

