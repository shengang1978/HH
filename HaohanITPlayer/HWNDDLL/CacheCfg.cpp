// CacheCfg.cpp: implementation of the CCacheCfg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CacheCfg.h"

extern BOOL ExtractFilePath( HMODULE hModule, CHAR* chPath );

BOOL CreateDirectory(char* path)
{
	SECURITY_ATTRIBUTES sa;
	sa.nLength              = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle       = 1;	
	
	return ::CreateDirectory( path, &sa );
}

void  ReadCacheConfig( CACHE_CONFIG& config ) 
{  	
	char buf[MAX_PATH]	= {0x00} ;
	char name[MAX_PATH] = {0x00} ;    
	
	ExtractFilePath(NULL, buf);
	//strcat( buf, "HwndDll\\");
	CreateDirectory(buf);
    wsprintf( name , "%sHWndDll.ini" , buf ) ;
	
	config.SpecialSplit_6	= GetPrivateProfileInt( "CONFIG" , "SpecialSplit_6", 0, name );
	if( config.SpecialSplit_6 == 0 )
	{
		wsprintf( buf , "%d", config.SpecialSplit_6 );
		WritePrivateProfileString( "CONFIG" , "SpecialSplit_6" , buf , name ) ; 
	}
}

CACHE_CONFIG	g_CacheIni ;
