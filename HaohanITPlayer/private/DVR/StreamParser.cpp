// StreamParser.cpp: implementation of the CStreamParser class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "StreamParser.h"
#include <assert.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStreamParser::CStreamParser()
{
	m_sk = NULL;
	m_DataLeft = 0;
	memset( m_StreamData, 0x00, sizeof(m_StreamData) );
	memset( m_synTempBuf, 0x00, sizeof(m_synTempBuf) );	
}

CStreamParser::~CStreamParser()
{

}

void CStreamParser::SetSocketPara( CComSocket* sk )
{
	m_sk = sk;
}

void CStreamParser::ClearBuf()
{
    m_DataLeft = 0;
}

//ÿ֡������������β��ʶ
//int CStreamParser::GetOneFrame(  BYTE * dst , FRAME_HEADER * frameHeader )
//{
//    int ret =  GetOneFrameAndTail( dst , frameHeader  );
//    if( ret <= 0 )
//        return ret;
//    DWORD *tail = (DWORD*)(dst + ret - 4);
//    while( *tail != SYN_TAIL )
//    {
//        ret =  GetOneFrameAndTail( dst , frameHeader  );
//        if( ret <= 0 )
//            return ret;
//        tail = (DWORD*)(dst + ret - 4);
//    }
//    return ret - 4;



//
//    GetOneFrame
//    ���ڲ�����������һ֡(����4���ֽڵ�β��)�����ʧȥͬ�������Զ�����ͬ������    
//
//int  CStreamParser::GetOneFrameAndTail( BYTE * dst , FRAME_HEADER  * frameHeader  )
//����ͬ��ͷ����ͬ��β
int  CStreamParser::GetOneFrame( BYTE * dst , FRAME_HEADER  * frameHeader  )
{   
    FRAME_HEADER * head ;
    BYTE *pBuf ;      
    int ret  ;
    int  len = 0 ;   
    int retval = 0;

    //�����һ��֡ͷ��������
	if( m_DataLeft < sizeof(FRAME_HEADER) )
    {                            
        BYTE* pData = m_StreamData + m_DataLeft ; 
		
        ret = ReadCacheData( pData , sizeof(FRAME_HEADER) - m_DataLeft ) ;
        if( ret <= 0 )
		{
			//TRACE(_T("ReadCacheData return:%d  \r\n",ret);
			return ret;
		}
        m_DataLeft  +=  ret ;           
    }
	head = ( FRAME_HEADER * ) m_StreamData ; 
    pBuf = ( BYTE * )( head + 1 ) ;
	while(1)
	{	  
		if ( head->SyncFlag == SYN_HEADER )	
		{

			if( m_DataLeft < sizeof(FRAME_HEADER) )//�����в���һ��֡ͷ�ĳ���
			{
				ReadCacheData( m_StreamData + m_DataLeft, sizeof(FRAME_HEADER) - m_DataLeft );
				m_DataLeft = sizeof( FRAME_HEADER );
			}
			len = htonl( head->FrameLength) ; 
			ret         = len ; 

			memcpy( frameHeader, head, sizeof(FRAME_HEADER) );
			m_DataLeft -= sizeof( FRAME_HEADER );
			if( ret <= 0 )
			{
			  //TRACE(_T("֡�����쳣Ϊ���� ret = %d\r\n", ret );
			  return ret;
			}
			if( ret > (MAX_FRAME_LENGTH - 1024 * 1024) )
			{
			  //TRACE(_T("̫֡�� ret = %d\r\n", ret );
			  return -1;
			}	       

			if( len <= m_DataLeft )//���������������
			{    
			  //����ʣ������
			  assert( len > 0 );
			  memcpy( dst , pBuf , len    ) ;
                
			  //Adjust the internal buffer
			  m_DataLeft   -=  len ;
			  pBuf         +=  len ;
			  assert( m_DataLeft >= 0 );
			  memmove( m_StreamData , pBuf , m_DataLeft ) ;
			  if( ret <= 0 )
			  {
				//TRACE(_T("GetOneFrame::ret = %d\r\n", ret);
			  }
			  return ret ;		  
			}				    

			//data is not enough, we should read more
			if(m_DataLeft < 0)
			  m_DataLeft = 0;
			assert( m_DataLeft >= 0 );
			if( m_DataLeft > 0 )
				memcpy( dst , pBuf , m_DataLeft ) ;				  

			len -= m_DataLeft ;
			dst += m_DataLeft ;                    
			m_DataLeft  = 0     ;
			while( len > 0 )//��������ѭ��
			{
				//��������ֱ�Ӵ�Cache�ж�����
				retval = ReadCacheData( dst, len );
				if( retval <= 0 )
					return retval;
				len -= retval;
				dst += retval;
			}
			return ret;
		}

		//֡û��ͬ���ϣ���ʼ����ͬ������
        ret =  MpegStreamSync( NULL );
		if( ret <= 0 )
		{
			//TRACE(_T("GetOneFrame::MpegStreamSync return 0\r\n");
			return ret ;
		}

		head = ( FRAME_HEADER * ) m_StreamData ; 
		pBuf = ( BYTE * )( head + 1 ) ;		    		
	} 	
}

int  CStreamParser::MpegStreamSync(  __int64 * checked )
{
	BYTE *ptr ;   
	FRAME_HEADER * head ;
    DWORD synced ;          
    int   count ;
	int   ret = 0;
    
    //Copy to temp buffer 
	if( m_DataLeft < 0 )
	{
		//TRACE( "CStreamParser::MpegStreamSync m_Dataleft = %d\r\n\r\n", m_DataLeft );
		return 0;
	}
    assert( m_DataLeft >= 0 );
    memcpy( m_synTempBuf ,  m_StreamData , m_DataLeft ) ;           
    head = ( FRAME_HEADER * ) m_StreamData ; 
    head->SyncFlag = SYN_HEADER ;
    
    count  = m_DataLeft   ;            
    //shift  = 0xFFFFFFFF   ;
    synced = 0            ;
	//DWORD	SyncFlag = SYN_HEADER;
    while(1)
    {
		ptr   = m_synTempBuf     ;  		    
        while( count > 4  )
		{
			if( *(DWORD*)ptr == SYN_HEADER )
			{
				assert( count >= 0 );
				memcpy( m_StreamData , ptr  ,  count ) ;
				m_DataLeft  =  count ;  
				
				//int test = ( synced - 4 );
				if( checked )
					*checked = ( synced ) ;
				return 1 ;
			}
			ptr++;
			count--;
			synced++ ;//ͬ�������ֽ��������ӵ�ǰ�ֽ�ֱ���ҵ�SYN_HEADER�ֽ��ӵ����ֽ���
		}
		//Copyʣ�µ��ĸ��ֽ�
		memcpy( m_synTempBuf, ptr, count );
		ptr = m_synTempBuf + count;
		ret =  ReadCacheData( ptr , MAX_STREAM_BUFFER_LEN - count ) ; //ͬ��ʱ���������
        if( ret <= 0 )
			return ret;
		count += ret;
	}
	//return 0 ;
}


int	CStreamParser::ReadCacheData( BYTE* buf, int size )
{	
	return 0;
}
