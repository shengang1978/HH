// RealStreamParser.h: interface for the CRealStreamParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REALSTREAMPARSER_H__0A29A002_2E03_4B93_98E9_C6715784246B__INCLUDED_)
#define AFX_REALSTREAMPARSER_H__0A29A002_2E03_4B93_98E9_C6715784246B__INCLUDED_

#include "StreamParser.h"

class CRealStreamParser : public CStreamParser
{
public:
	CRealStreamParser();
	virtual ~CRealStreamParser();
private:
	virtual int	ReadCacheData( BYTE* buf, int size );
};

#endif // !defined(AFX_REALSTREAMPARSER_H__0A29A002_2E03_4B93_98E9_C6715784246B__INCLUDED_)
 