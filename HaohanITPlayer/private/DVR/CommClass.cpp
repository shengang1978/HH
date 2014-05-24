// CommClass.cpp: implementation of the CCommClass class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CommClass.h" 

#include "commdef.h"
#include "./NetDef.h"
#include "ComSocket.h"
#include "DvrSDKErr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL CCommClass::ExtractFilePath( TCHAR* chPath )
{
	TCHAR path[_MAX_PATH] = {0X00};
	HMODULE hCurModule = NULL;
	::GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCWSTR)ExtractFilePath, &hCurModule);
	::GetModuleFileName(hCurModule,(TCHAR*)path,sizeof(path));
	CString strPath = path;
	INT nPos = strPath.ReverseFind( '\\' );
	_tcsncpy( chPath, strPath, nPos );
	return TRUE;
}


BOOL CCommClass::GetLocalIP( TCHAR* chIP, TCHAR* chHostName )
{
	BOOL bRet = FALSE;
	WORD wVersionRequested; 
	WSADATA wsaData; 
	char chName[255]; 
	PHOSTENT hostinfo; 
	wVersionRequested =MAKEWORD(2,0); 
	if( WSAStartup( wVersionRequested, &wsaData ) == 0 ) 
	{ 
		if( gethostname(chName,sizeof(chName)) == 0 ) 
		{
			if((hostinfo = gethostbyname(chName)) != NULL ) 
			{
				LPCSTR ip=inet_ntoa(*(struct in_addr*)*hostinfo->h_addr_list); 
				_tcscpy( chIP, CA2T(ip) );
				if( chHostName != NULL )
					_tcscpy( chHostName, CA2T(hostinfo->h_name));
				bRet = TRUE;
			} 
		} 
		WSACleanup(); 
	}
	return bRet;
}

BOOL CCommClass::SetLocalComputerTime(LPSYSTEMTIME	lpSystemTime)
{	
	HANDLE 	hdlProcessHandle = INVALID_HANDLE_VALUE;
    HANDLE	hdlTokenHandle = INVALID_HANDLE_VALUE;
    LUID	tmpLuid;
    
    TOKEN_PRIVILEGES tkp;
	memset( &tkp, 0x00, sizeof(tkp) );
    TOKEN_PRIVILEGES tkpNewButIgnored;
	memset( &tkpNewButIgnored, 0x00, sizeof(tkp) ); 
    
    DWORD	lBufferNeeded = 0;
    
    hdlProcessHandle = GetCurrentProcess();
    if( !OpenProcessToken(hdlProcessHandle, (TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY), &hdlTokenHandle) )
	{
		return FALSE;
	}
	
    if( !LookupPrivilegeValue(_T(""), _T("SeSystemtimePrivilege"), &tmpLuid) )
	{
		return FALSE;
	}
    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Luid = tmpLuid;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	
    if( !AdjustTokenPrivileges(hdlTokenHandle, 0, &tkp,sizeof(tkpNewButIgnored), &tkpNewButIgnored,&lBufferNeeded) )
	{
		CloseHandle( hdlTokenHandle );

		return FALSE;
	}
    
    if( !SetLocalTime(lpSystemTime) )
	{
		CloseHandle( hdlTokenHandle );

		return FALSE;
	}	

	CloseHandle( hdlTokenHandle );
	return TRUE;
}

/*
 *	070213 modify
 */
void CCommClass:: LongToTime( ULONG dat , DATE_INFO * date , TIME_INFO *time )
{
	int  days[13] = { 0, 31 , 29 , 31 , 30 , 31 , 30 , 31 , 31 , 30 , 31 , 30 , 31 } ;
    int  i ;
	
    time->second = (unsigned char)(dat % 60) ;
    dat          = dat / 60 ;        //dat is minute
    
    time->minute = (unsigned char)(dat % 60) ;
    dat          = dat / 60 ;        //dat is hour  
    
    time->hour   = (unsigned char)(dat % 24) ;
    dat          = dat / 24 ;        //dat is day   
	
    i            = 0    ;	
	
	bool bLeapYear = true; //从2000.1.1日开始
	while( dat >= (unsigned int)( bLeapYear ? 366 :365 ))// 存在闰年问题
    {
        if( bLeapYear )
            dat -= 366 ;
        else
            dat -= 365 ;
        i++ ;
		bLeapYear = (2000+i)%400==0 || ( (2000+i)%4==0 && (2000+i)%100!=0 ) ;
    }
    
    date->year  = 2000 + i ;
    if( !bLeapYear ) 
        days[2] = 28 ;
    
    for( i = 1 ; i < 13 ; i++ )
    {
        if( dat < (unsigned int)days[i] )	
            break;
        dat -= days[i] ;    
    }
    date->month  =  i  ;
    date->date    = (unsigned char)(dat + 1) ;
}

UINT CCommClass::DateToLong(  DATE_INFO * date )
{
	int  days[13] = { 0, 31 , 29 , 31 , 30 , 31 , 30 , 31 , 31 , 30 , 31 , 30 , 31 } ;
    int  day  , i ;
    unsigned long  seconds ;
    
    seconds =    0 ;
    day     =    0 ;
    
	bool bLeapYear = date->year%400 == 0 || ( date->year%4 == 0 && date->year%100 != 0 ) ;
    if( !bLeapYear )
        days[2] = 28 ;
    
    for( i = 0 ; i < ( date->year - 2000 ) ; i++ )
    {
        bLeapYear = (2000+i)%400 == 0 || ( (2000+i)%4 == 0 && (2000+i)%100 != 0 ) ;
		if( bLeapYear )
            day += 366 ;
        else
            day += 365 ;
    }    
    for( i = 1 ; i < date->month ; i++ )  
    {
        day += days[i] ;
    }       
    day += ( date->date - 1 );     
    
    seconds = day * 24 * 3600 ;
    
    return seconds ; 
}

//   Function : LongToTime
// Description: convert date & time information to long value  seconds relative 2000.1.1
UINT CCommClass::TimeToLong(  DATE_INFO * date , TIME_INFO *time )
{
	int  days[13] = { 0, 31 , 29 , 31 , 30 , 31 , 30 , 31 , 31 , 30 , 31 , 30 , 31 } ;
    int  day  , i ;
    unsigned long  seconds ;
    
    seconds =    0 ;
    day     =    0 ;
    
	bool bLeapYear = date->year%400 == 0 || ( date->year%4 == 0 && date->year%100 != 0 ) ;
    if( !bLeapYear )
        days[2] = 28 ;
    
    for( i = 0 ; i < ( date->year - 2000 ) ; i++ )
    {
        bLeapYear = (2000+i)%400 == 0 || ( (2000+i)%4 == 0 && (2000+i)%100 != 0 ) ;
		if( bLeapYear )
            day += 366 ;
        else
            day += 365 ;
    }    
    for( i = 1 ; i < date->month ; i++ )  
    {
        day += days[i] ;
    }       
    day += ( date->date - 1 );     
    
    seconds = ( day * 24 + time->hour )*3600 + time->minute * 60 + time->second ;
    
    return seconds ; 
}

unsigned long  CCommClass::CTimeToULong(CTime time)
{  
	
    int  days[13] = { 0, 31 , 29 , 31 , 30 , 31 , 30 , 31 , 31 , 30 , 31 , 30 , 31 } ;
    int  day  , i ;
    unsigned long  seconds ;
    
    seconds =    0 ;
    day     =    0 ;
    
	bool bLeapYear = time.GetYear()%400 == 0 || ( time.GetYear()%4 == 0 && time.GetYear()%100 != 0 ) ;
    if( !bLeapYear )
        days[2] = 28 ;
    
    for( i = 0 ; i < ( time.GetYear()-2000 ) ; i++ )    
    {
		bLeapYear = (2000+i)%400 == 0 || ( (2000+i)%4 == 0 && (2000+i)%100 != 0 ) ;
        if( bLeapYear )
            day += 366 ;
        else
            day += 365 ;
    }    
    for( i = 1 ; i < time.GetMonth() ; i++ ) 
    {
        day += days[i] ;
    }       
    day += ( time.GetDay() - 1 );     
    
    seconds = ( day * 24 + time.GetHour() )*3600 + time.GetMinute() * 60 + time.GetSecond() ;
    return seconds ;    
}

//////////////////////////////////////////////////////////////////////
// CTimeSynch Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL CTimeSynch::DoTimeSynch(DATE_INFO& di, TIME_INFO& ti)
{
	SYSTEMTIME  cstime;
	memset(&cstime, 0x00, sizeof(cstime));
	cstime.wYear = di.year;
	cstime.wMonth = di.month;
	cstime.wDay = di.date;
	cstime.wHour = ti.hour;
	cstime.wMinute = ti.minute;
	cstime.wSecond = ti.second;
	if( CCommClass::SetLocalComputerTime(&cstime) )		
		return TRUE;
	return FALSE;
}

int	Send( SOCKET sk, char* buf, int size, int flag)
{
	timeval		m_tv;
	memset( &m_tv, 0x00, sizeof(m_tv) );
	m_tv.tv_sec = 2;
	fd_set		fdwrite;
	int ret = 0;
	
	FD_ZERO( &fdwrite );
	FD_SET( sk, &fdwrite );
	
	ret = select( 0, NULL,&fdwrite, NULL, &m_tv );
	if(ret == SOCKET_ERROR)//出错
	{
		return SOCKET_ERROR;	
	}	
	if(ret == 0)//超时
		return 0;
	
	if(FD_ISSET(sk, &fdwrite))		
		return send( sk, buf, size, flag );
	return -2;
}
int	Recv( SOCKET sk, char* buf, int nLength, int flag)
{
	DWORD DataRead = 0        ;
	INT DataRequest , ret ; 
	INT received    = 0;
	CHAR *pDst = NULL;	
	
	DataRead = 0    ;   
	pDst     = (CHAR*)buf  ;

	if( nLength > 0 )
	{ 	
		DataRequest = nLength ;		
		received     =  0 ;
		
		while( received <  DataRequest )   
		{
			ret = recv( sk , pDst , DataRequest - received , flag) ;   
			if( ret <= 0 ) 
			{
				DWORD error = WSAGetLastError();
				return 0 ;  
			}
			
			pDst      += ret ;       
			nLength    -= ret ;//not used
			received  += ret ;
		}	                                
		DataRead  += DataRequest ;          	            	            	            
	} 
	return DataRead ;
}

SOCKET CreateSocket( int type, INT nTimeOut )
{
	SOCKET s;
	if( type == 0 )
		s=  socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	else
		s= socket(AF_INET, SOCK_DGRAM, 0);	
	
	setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, (char*)&nTimeOut, sizeof(nTimeOut));
	setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char*)&nTimeOut, sizeof(nTimeOut));
	
	DWORD size = MAX_RECV_BUFFER ;	//设置接收缓存大小
	setsockopt( s , SOL_SOCKET , SO_RCVBUF , ( CHAR*)&size , sizeof(size) );
	
	return s;
}

//nTimeOut 单位S
int Connect( SOCKET sk, char* remote, int port, int nTimeOut )
{	
	//设置非阻塞方式连接
	unsigned long ul = 1;  //“无符号长整形“非零值， 启用非锁定模式
	
	//ioctlsocket 控制套接口的模式。  FIONBIO：允许或禁止套接口的非阻塞模式。
	if ( ioctlsocket(sk, FIONBIO, (unsigned long*)&ul) == SOCKET_ERROR )
		return SOCKET_ERROR;
	
	//SOCKADDR_IN dvr;
	//dvr.sin_family		= AF_INET;
	//dvr.sin_port		= htons((unsigned short)port);
	
	//dvr.sin_addr.s_addr	= inet_addr(addr);
	
	SOCKADDR_IN saServer;
	memset( &saServer, 0x00, sizeof(saServer) );
	saServer.sin_family		= AF_INET;
	LONG lIPAddress = 0;
	if( isalpha(remote[0]) )
	{
		hostent *hostEnt = gethostbyname( remote );	
		if( hostEnt != NULL )
		{
			lIPAddress = ((in_addr*)hostEnt->h_addr)->s_addr;
			saServer.sin_addr.s_addr = lIPAddress;
		}
	}	
    else
    {
		saServer.sin_addr.s_addr = inet_addr( remote );
    }
	saServer.sin_port		= htons((unsigned short)port);
	
	if ( connect( sk, (SOCKADDR*)&saServer, sizeof( saServer ) ) == SOCKET_ERROR )
	{
		DWORD dwError = WSAGetLastError();
		
		if ( dwError != WSAEWOULDBLOCK )
			return SOCKET_ERROR;
	}
	
	//检索套接字是否接受到数据.若无,则ret<=0,即自动在fdconn中删除了该套接字
	
	struct timeval tv ;	
	fd_set fdconn;	
	FD_ZERO(&fdconn);  //初始化为空集合	
	FD_SET(sk, &fdconn);  //将套接字句柄传入fdconn	
	tv.tv_sec = nTimeOut;   //等待时间， 秒	
	tv.tv_usec = 0;     //毫秒
	
	int ret = select(0, 0, &fdconn, 0, &tv);	
	if ( ret <= 0 )
	{
		//closesocket(sk);
		return SOCKET_ERROR;
	}
	
	//设回阻塞模式
	ul = 0 ; //0为阻塞模式
	
	ret = ioctlsocket( sk, FIONBIO, (unsigned long*) & ul );
	
	if ( ret == SOCKET_ERROR )
	{
		closesocket(sk);
		return SOCKET_ERROR;
	}
	return 0;
}

int SendAndRecv(SOCKET sk, char *msg, int msgSize, char *rsp, int rspSize)
{
	int ret = 0;
	if( (ret = send(sk, (char*)msg, msgSize, 0)) <= 0 )
	{	
		int err = WSAGetLastError();
		if( err == WSAETIMEDOUT )
			return HHV_ERROR_TIMEOUT;
		return HHV_ERROR_SEND;
	}
	if( (ret = recv(sk, (char*)rsp, rspSize, 0 )) <= 0 )
	{
		int err = WSAGetLastError();
		if( err == WSAETIMEDOUT )
			return HHV_ERROR_TIMEOUT;
		return HHV_ERROR_RECV;
	}
	return 0;
}

BOOL CCommClass::CreateFullDirectory(TCHAR* path)
{
	if( NULL == path )
	{
		return FALSE;
	}

	CString cstrPath = path;
	cstrPath.TrimLeft();
	cstrPath.TrimRight();

	std::vector<CString> vecCStr;
	vecCStr.push_back(cstrPath);

	int nFindPos = cstrPath.ReverseFind('\\');
	while( 0 < nFindPos )
	{
		cstrPath = cstrPath.Mid(0, nFindPos);
		if( ':' != cstrPath.GetAt(nFindPos - 1) )
		{		
			vecCStr.push_back(cstrPath);
		}
		
		nFindPos = cstrPath.ReverseFind('\\');
	}

	LPTSTR p = NULL;
	int nSize = vecCStr.size();
	int i = 0;
	while( i < nSize )
	{
		cstrPath = vecCStr.back();
		vecCStr.pop_back();

		p = cstrPath.GetBuffer(cstrPath.GetLength());

		CCommClass::CreateDirectory( p );

		cstrPath.ReleaseBuffer();

		i++;
	}

	return TRUE;
}

BOOL CCommClass::CreateDirectory(TCHAR* path)
{
	SECURITY_ATTRIBUTES sa;
	sa.nLength              = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle       = 1;	
	
	return ::CreateDirectory( path, &sa );
}

UINT CCommClass::GetCurrentTime()
{
	SYSTEMTIME	st;
	GetLocalTime(&st);
	DATE_INFO di;
	TIME_INFO ti;
	di.year		= st.wYear;
	di.month	= st.wMonth;
	di.date		= st.wDay;
	ti.hour		= st.wHour;
	ti.minute	= st.wMinute;
	ti.second	= st.wSecond;
	return TimeToLong(&di, &ti);
}

BOOL CCommClass::CheckIP( std::string ip )
{
	int i, n = 0, m;
	char buf[64];
	
	//如果输入的是域名
	sscanf(ip.c_str(), "%s" , buf);
	if ( isalpha(buf[0]) )
		return true;
	
	//如果输入的是IP
	for ( i = 0 ; i < 4 ; i++)
	{
		n = ip.find(".");      // n=0 .12     wrong   // n=4 1233.23   wrong
		if ( i == 3 )          // n=1 1.2     right   //find 0-based
            n = 3; //第4节     // n=2 12.23   right
		if ( n < 1 || n > 3 )  // n=3 123.23  right
			return false;
		else
		{
			m = atoi( ip.substr( 0, n ).c_str() );
			if ( m > 255 || ( m == 0 && i == 0 ) ) // IP范围:001.000.000.000~255.255.255.255
				return false;
			
			if ( i == 3 && ip.length() > 3 )
				return false;      //2005-2-22 第四段不能超过3个字符
			
            if( (UINT)(n + 1) > ip.length() ) n = ip.length() -1;
			ip = ip.substr( n + 1 , ip.length() - n );
			if ( ip == "" && i != 3 ) //第4节
				return false;
		}
	}
	return true;
}

DWORD CCommClass::PowerGetFileSize(const TCHAR *file)
{
	struct _stat buf;
	int result = 0;
	result = _tstat( file, &buf );
	if( result != 0 )
		return 0;
	else
		return buf.st_size;	
}
