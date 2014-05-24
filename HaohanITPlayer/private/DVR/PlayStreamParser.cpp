// PlayStreamParser.cpp: implementation of the CPlayStreamParser class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "PlayStreamParser.h"
#include "ComSocket.h"
#include "./NetDef.h"
#include "DvrSDkErr.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPlayStreamParser::CPlayStreamParser()
{
	m_sk = NULL;
    m_fileLen = 0;
    m_currPos = 0;
    m_DataLeft = 0;
    m_Flag    = READ_DATA_OVER;  
}

CPlayStreamParser::~CPlayStreamParser()
{

}

void CPlayStreamParser::SetFileLenPara( __int64 fileLen )
{
    m_fileLen = fileLen;
    m_Flag    = 0;
    m_currPos = 0;
    m_DataLeft = 0;
}

__int64 CPlayStreamParser::GetCurrPos()
{
    return m_currPos;
}

int CPlayStreamParser::SeekPercent(int pos) //pos 1-100
{
    __int64 position = m_fileLen * (pos / 100.0);
    if(!CPlayStreamParser::MpegCacheSeek_Vs(m_sk, position))
    {
        return -1;
    }
    m_currPos = position;
    m_DataLeft = 0;
    return 0;
}

int	CPlayStreamParser::ReadCacheData( BYTE* buf, int size )
{
    if(m_Flag == READ_DATA_OVER)//已经结束
        return READ_DATA_OVER;
	if( m_sk == NULL )
		return 0;
    int ret = CPlayStreamParser::GetData_Vs(m_sk, (char*)buf, size);
    if( ret > 0 )
    {
        m_currPos += ret;
        if(m_currPos >=  m_fileLen)//结束
        {
            m_Flag = READ_DATA_OVER;
        }
        return ret;
    }
    return ret;
}

int CPlayStreamParser::GetData_Vs(CComSocket * comSocket, char* buf, int len )
{
    //headersize OK
    FILE_READ_REQUEST_MSG cmd64;
	cmd64.header.command		= ntohl(SYSTEM_VIDEO_READ_REQUEST); //NET_READ_REQUEST_64
	cmd64.header.length			= ntohl(sizeof(U32));
	cmd64.length		= ntohl(len) ;
	
    if( !comSocket->Send((char*)&cmd64, sizeof(cmd64)) )
        return HHV_ERROR_SEND;
    if( !comSocket->Receive((char*)&cmd64, sizeof(cmd64)) )
        return HHV_ERROR_RECV;
	
	if(SYSTEM_REQUEST_ACCEPT != ntohl(cmd64.header.command))
	{
		return 0;
	}
    int realLen =  ntohl(cmd64.length);
	if( !comSocket->Receive((char*)buf, realLen) )
        return  HHV_ERROR_RECV;
	
	return realLen;
}

#define HIGHINT(l)                  ( (int)( l >> 32 ) )
#define LOWINT(l)                   ( (int)( l ) )
bool CPlayStreamParser::MpegCacheSeek_Vs(CComSocket * comSocket, __int64 position )
{
    //headersize OK
	FILE_SEEK_REQUEST_MSG cmd64;
	int pos_hign = HIGHINT( position );
	int pos_low  = LOWINT( position );
	cmd64.header.command  =  htonl( SYSTEM_VIDEO_SEEK_REQUEST ) ;
	cmd64.header.length = htonl( 2 * sizeof(int) );
	cmd64.hpos   =  htonl(pos_hign);
	cmd64.lpos  =  htonl(pos_low) ;
	
    comSocket->Send((char*)&cmd64, sizeof(cmd64));
    comSocket->Receive((char*)&cmd64, sizeof(cmd64)- 2 * sizeof(int));
	
	if ( ntohl( cmd64.header.command ) == SYSTEM_REQUEST_ACCEPT )
	{
		return true;
	}
	return false;		
}
