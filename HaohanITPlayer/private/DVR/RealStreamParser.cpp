// RealStreamParser.cpp: implementation of the CRealStreamParser class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "RealStreamParser.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRealStreamParser::CRealStreamParser()
{
    
}

CRealStreamParser::~CRealStreamParser()
{

}

int	CRealStreamParser::ReadCacheData( BYTE* buf, int size )
{
	if( m_sk == NULL )
		return 0;
    if( m_sk->Receive( (char*)buf, size ) )
        return size;
    return 0;
}
