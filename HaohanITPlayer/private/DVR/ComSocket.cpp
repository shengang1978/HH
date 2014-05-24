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
        //TRACE(_T("!%s! CComSocket::Create socket����INVALID_SOCKET, lasterror: %d \r\n", m_skDescription, dwError);
        return HHV_ERROR_CREATESOCKET;
    }
	
    //���ý��շ��ͳ�ʱ
	//�ȴ�ʱ����ԭ����10��ĳ�2��
    int timeout = m_nTimeOut * 1000;
	setsockopt (m_hSocket, SOL_SOCKET, SO_RCVTIMEO, (CHAR*)&timeout, sizeof(timeout));
	setsockopt (m_hSocket, SOL_SOCKET, SO_SNDTIMEO, (CHAR*)&timeout, sizeof(timeout));

    //SO_RCVBUF  Ϊ����ȷ����������С
    int bufSize = 1024 * m_skRecvBufSize;
	setsockopt ( m_hSocket , SOL_SOCKET , SO_RCVBUF , ( CHAR*)&bufSize , sizeof(bufSize) ) ;
    
    //SO_LINGERѡ���ر�ʱ��δ�������ݣ�������
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
		closesocket( m_hSocket ) ; //�ر��׽���
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
	if ( ret < 0 )  //�׽��ִ������ɹ�
	{
    	return ret;
    }
	
	//���÷�������ʽ����
	ULONG ul = 1;
	
	//ioctlsocket �����׽ӿڵ�ģʽ��  FIONBIO��������ֹ�׽ӿڵķ�����ģʽ��
	ioctlsocket(m_hSocket, FIONBIO, (unsigned long*)&ul);
		
	hostent *hostEnt = NULL;
	LONG lIPAddress = 0;  	
	//�������ֽ�����ip��ַ�����ظ�hostent����ָ�����
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
		
	m_sockaddr.sin_family = AF_INET;   //ip��ַ����
	m_sockaddr.sin_port = htons( nPort ); //�˿�

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
	tv.tv_sec = m_nTimeOut;   // ��
	tv.tv_usec = 0;     //����
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
	
	//�������ģʽ
	ul = 0 ; //0Ϊ����ģʽ
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
//����˵��:
// strData : �������ݵ�BUF (OUT)
// iLen    : ����iLen���ֽ�
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
