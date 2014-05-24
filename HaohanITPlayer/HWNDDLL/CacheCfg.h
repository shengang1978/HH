// CacheCfg.h: interface for the CCacheCfg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CACHECFG_H__92A97221_0C4A_4553_9588_9ABC3F6AC0E9__INCLUDED_)
#define AFX_CACHECFG_H__92A97221_0C4A_4553_9588_9ABC3F6AC0E9__INCLUDED_

struct CACHE_CONFIG 
{
	int SpecialSplit_6;
	CACHE_CONFIG(){
		memset(this, 0x00, sizeof(CACHE_CONFIG) );
	}
};
void	ReadCacheConfig( CACHE_CONFIG& config ) ;
extern CACHE_CONFIG			g_CacheIni;

#endif // !defined(AFX_CACHECFG_H__92A97221_0C4A_4553_9588_9ABC3F6AC0E9__INCLUDED_)
