#include "stdafx.h"
#include "ComSocket.h"
//#include "Global.h"
#include "./DvrSDKErr.h"


CComSocket::CComSocket(INT nTimeOut, INT skRecvBufSize ):
	m_bConnected(FALSE), 
	m_nTimeOut( nTimeOut ), 
	m_skRecvBufSize(skRecvBufSize)
{
	memset( &m_sockaddr, 0, sizeof( m_sockaddr ) );
	memset( &m_rsockaddr, 0, sizeof( m_rsockaddr ) );
	memset( &m_skDescription, 0, sizeof( m_skDescription ));
	m_hSocket = INVALID_SOCKET;  
}

CComSocket::~CComSocket()
{
	//WSACleanup();
}
VOID  CComSocket::SetSkDescription(CHAR* pDescription)
{
	if(!pDescription)
		return;
	if(strlen(pDescription) > SOCKET_DESCRIBE_LENGTH)
	{
		strncpy(m_skDescription, pDescription, SOCKET_DESCRIBE_LENGTH-1);
		return;
	}
	strcpy(m_skDescription, pDescription);
}

INT CComSocket::Create( )
{
	m_hSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    if(m_hSocket == INVALID_SOCKET)
    {
        DWORD dwError = WSAGetLastError();
        //TRACE(_T("!%s! CComSocket::Create socket返回INVALID_SOCKET, lasterror: %d \r\n", m_skDescription, dwError);
        return HHV_ERROR_CREATESOCKET;
    }
	
    //设置接收发送超时
	//等待时间由原来的10秒改成2秒
    int timeout = m_nTimeOut * 1000;
	setsockopt (m_hSocket, SOL_SOCKET, SO_RCVTIMEO, (CHAR*)&timeout, sizeof(timeout));
	setsockopt (m_hSocket, SOL_SOCKET, SO_SNDTIMEO, (CHAR*)&timeout, sizeof(timeout));

    //SO_RCVBUF  为接收确定缓冲区大小
    int bufSize = 1024 * m_skRecvBufSize;
	setsockopt ( m_hSocket , SOL_SOCKET , SO_RCVBUF , ( CHAR*)&bufSize , sizeof(bufSize) ) ;
    
    //SO_LINGER选项，如关闭时有未发送数据，则逗留。
	linger  lg;
	lg.l_onoff = 1;
	lg.l_linger = 4;
	setsockopt( m_hSocket, SOL_SOCKET, SO_LINGER, (char*)&lg, sizeof(lg) );

	return 0;
}

BOOL CComSocket::Close( )
{
	if ( m_hSocket != INVALID_SOCKET )
	{
		shutdown( m_hSocket, SD_BOTH );
		closesocket( m_hSocket ) ; //关闭套接字
		m_hSocket = INVALID_SOCKET;
	}
	return TRUE;
}

sockaddr_in CComSocket::GetSockAddr()
{
	return m_sockaddr;
}

INT CComSocket::Connect( CHAR* strRemote, UINT nPort )
{
	Close();

	int ret = 0;
	
	if ( strlen( strRemote ) == 0 || nPort == 0 )
	{
    	return HHV_ERROR_INVALID_PARA;
    }
		
	ret = Create();
	if ( ret < 0 )  //套接字创建不成功
	{
    	return ret;
    }
	
	//设置非阻塞方式连接
	ULONG ul = 1;
	
	//ioctlsocket 控制套接口的模式。  FIONBIO：允许或禁止套接口的非阻塞模式。
	ioctlsocket(m_hSocket, FIONBIO, (unsigned long*)&ul);
		
	hostent *hostEnt = NULL;
	LONG lIPAddress = 0;  	
	//主机名字解析成ip地址，返回给hostent类型指针变量
	if ( isalpha(strRemote[0]) )
	{
		hostEnt = gethostbyname( strRemote );
		
		if ( hostEnt != NULL )
		{
			lIPAddress = ((in_addr*)hostEnt->h_addr)->s_addr;
			m_sockaddr.sin_addr.s_addr = lIPAddress;
		}
		else
		{
			m_sockaddr.sin_addr.s_addr = inet_addr( strRemote );
		}
	}
	else
		m_sockaddr.sin_addr.s_addr = inet_addr( strRemote );
		
	m_sockaddr.sin_family = AF_INET;   //ip地址家族
	m_sockaddr.sin_port = htons( nPort ); //端口

	//TRACE(_T("!%s! CComSocket::Connect(): before API connect \r\n", m_skDescription);
	if ( connect( m_hSocket, (SOCKADDR*)&m_sockaddr, sizeof( m_sockaddr ) ) == SOCKET_ERROR )
	{
		DWORD dwError = WSAGetLastError();
		if ( dwError != WSAEWOULDBLOCK )
		{
            //TRACE(_T("!%s! CComSocket::Connect(): connect failed LastError: %d\r\n", m_skDescription, dwError);
        	return HHV_ERROR_CONNECT;
        } 
	}
	
	struct timeval tv ;
	fd_set fdconn;
	FD_ZERO(&fdconn);  
	FD_SET(m_hSocket, &fdconn);  
	tv.tv_sec = m_nTimeOut;   // 秒
	tv.tv_usec = 0;     //毫秒
    //TRACE(_T("!%s! CComSocket::Connect(): before API select %ds \r\n", m_skDescription, nTimeOut);
	ret = select(0, 0, &fdconn, 0, &tv);
	if ( ret <= 0 )
	{
        DWORD dwError = WSAGetLastError();
        //TRACE(_T("!%s! CComSocket::Connect(): select failed LastError: %d, ret_select = %d\r\n",
         //           m_skDescription, dwError, ret);
		closesocket(m_hSocket);
		return HHV_ERROR_SOCKET_SELECT;
	}
	
	//设回阻塞模式
	ul = 0 ; //0为阻塞模式
	ioctlsocket( m_hSocket, FIONBIO, (unsigned long*) & ul );

	m_bConnected = TRUE;

	return 0;
}

INT CComSocket::Send( char* strData, int iLen )
{
	if ( strData == NULL || iLen == 0 )
		return HHV_ERROR_INVALID_PARA;

	int   nret = 0;
	char* pDst = strData;
	int   nLeft = iLen;
	while ( nLeft > 0 )
	{
		nret =  send( m_hSocket, pDst, nLeft, 0 );
		if ( nret <= 0 )
		{
			return nret;
		}
		pDst += nret;
		nLeft -= nret;
	}          	
	return iLen;
}
//参数说明:
// strData : 接受数据的BUF (OUT)
// iLen    : 接受iLen个字节
BOOL CComSocket::Receive( char* strData, int iLen )
{
	if ( strData == NULL || !m_bConnected )
		return FALSE;
	
	int   nret = 0;
	char* pDst = strData;
	int nLeft = iLen;
	while ( nLeft > 0 )                           
	{ 
		nret = recv ( m_hSocket , pDst , nLeft, 0  ) ;
		if ( nret <= 0)
		{
			DWORD dwError = WSAGetLastError();
            //TRACE(_T("!%s! CComSocket::Receive(): recv LastError: %d, nret: %d\r\n",
            //        m_skDescription, dwError, nret);
            Close();
            m_bConnected  = false;
            return FALSE ;
		}
		pDst      += nret ;
		nLeft     -= nret ;
	}
	return TRUE;
}

INT CComSocket::Recv( char* buf, int len )
{    
    return recv( m_hSocket, buf , len , 0 );    
}
