#ifndef _COMSOCKET_H
#define _COMSOCKET_H

#include <winsock2.h>


#define MAX_RECV_BUFFER (1024 * 1024)

#define UNKNOWN_SOCKET   0
#define CS_SOCKET        1
#define VS_SOCKET        2
#define DOG_SOCKET       3

#define SOCKET_DESCRIBE_LENGTH  128
class CComSocket
{
public:
    CComSocket( INT nTimeOut = 2, INT skRecvBufSize = 10 );
    ~CComSocket();

    INT Connect( CHAR* strRemote, UINT nPort );

    INT Close( );

    INT Send( CHAR* strData, INT iLen );

    //接收完整的iLen长度或出错负数
    BOOL Receive( CHAR* strData, INT iLen );

    //返回实际接收到的长度，有可能小于len
    INT Recv( CHAR* buf, INT len );

    sockaddr_in GetSockAddr();

    VOID SetSkDescription(CHAR* pDescription);
private:
    INT Create( );

    sockaddr_in m_sockaddr;

    sockaddr_in m_rsockaddr;

    BOOL m_bConnected;

    INT m_nTimeOut;  //超时时间 recv send等待的时间
	INT m_skRecvBufSize;//SOCKET接收缓冲
	
    CHAR m_skDescription[SOCKET_DESCRIBE_LENGTH]; 
public:
	SOCKET m_hSocket;
};


#endif 