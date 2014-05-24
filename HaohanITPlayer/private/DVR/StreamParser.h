// StreamParser.h: interface for the CStreamParser class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _STREAM_PARSER_H
#define _STREAM_PARSER_H

#include "ComSocket.h"

typedef  unsigned char			U8  ;
typedef  unsigned short			U16 ;
typedef  unsigned int			U32 ;
typedef  unsigned __int64		U64 ;

#define	 DATA_SYNCH_ERROR		-15	

#define  MAX_STREAM_BUFFER_LEN		(1*1024)   ////decoder临时缓冲，为了分析头位置
#define  MAX_FRAME_LENGTH		    (2*1024*1024)

#define  SYN_HEADER                 (0xAA8888AA)
#define  SYN_TAIL                   (0xBB6666BB)
  

#pragma pack(1)

struct FRAME_HEADER
{
	U32 SyncFlag;
	U32 FrameTime;
	U8 FrameType;//帧类型
	U8	FrameRate;//帧率
	U16 Width;//宽度
	U16 Height;//高度	
	U32	MetaLength;
	U32 FrameLength;
};

#pragma pack()

class CStreamParser  
{
public:
	CStreamParser();
	virtual ~CStreamParser();

	VOID SetSocketPara( CComSocket* sk );
    INT GetOneFrame(  BYTE * pBuf , FRAME_HEADER * type ) ;
	//INT GetOneFrameAndTail(  BYTE * pBuf , FRAME_HEADER * type ) ;
    VOID ClearBuf(); 
private:
	INT	MpegStreamSync(  __int64 * count   ) ; 	
protected:
	virtual INT	ReadCacheData( BYTE* buf, int size );
protected:
	CComSocket* m_sk;
	INT m_DataLeft    ;

	BYTE m_StreamData [ MAX_STREAM_BUFFER_LEN ] ; 
    BYTE m_synTempBuf [ MAX_STREAM_BUFFER_LEN * 100 ] ;	//由于m_synTemBuf溢出，导致后面数据被覆盖
};

#endif 
