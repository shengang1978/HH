// PlayStreamParser.h: interface for the CPlayStreamParser class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __PLAY_STREAM_PARSER_H
#define __PLAY_STREAM_PARSER_H

#include "StreamParser.h"

#define     READ_DATA_OVER    -128

class CPlayStreamParser  : public CStreamParser
{
public:
	CPlayStreamParser();
	virtual ~CPlayStreamParser();
    void    SetFileLenPara( __int64 fileLen );
    __int64 GetCurrPos();
    int     SeekPercent(int pos); //pos 1-100
private:
	virtual int	ReadCacheData( BYTE* buf, int size );
private:
	int	GetData_Vs(CComSocket * comSocket, char* buf, int len );
	bool MpegCacheSeek_Vs(CComSocket * comSocket, __int64 position );
private:
    __int64   m_fileLen;
    __int64   m_currPos;
    int       m_Flag;
};

#endif // !defined(AFX_PLAYSTREAMPARSER_H__69839624_8A5E_4E55_9B92_2CECA6C1019F__INCLUDED_)
