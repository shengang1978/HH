// LogClass.cpp: implementation of the CLogClass class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LogClass.h"

#define MAX_TRACE_FILE_LENGTH	(8*1024*1024)
class CTraceLog
{
public:
	CTraceLog(){
		memset( this, 0x00, sizeof(CTraceLog) );
		m_bInited = FALSE;
	}
	~CTraceLog(){
		UnInit();
		m_bInited = FALSE;
	}
	void Init()
	{
		char path[256] = {0x00};
		//ExtractFilePath( path);
        CUtilities::ExtractFilePathB(NULL, path);
		strcat( path, "LOG_TEST\\");                                                          
		CUtilities::CreateDirectory((LPTSTR)path);
		char fileName[128] = {0x00};
		SYSTEMTIME	sysTime;
		GetLocalTime(&sysTime);
		
#if defined(_DEBUG)
		sprintf( fileName, "TEST %04d%02d%02d-%02d%02d%02d.txt", sysTime.wYear, sysTime.wMonth, sysTime.wDay,
			sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
#else
		sprintf( fileName, "VideoAnalisys.txt");
#endif
		strcat(path, fileName);
		
		m_fileTrace = fopen( path,"w+b");
		m_bInited = TRUE;
	}
	void UnInit(){
		if( m_fileTrace != NULL )
		{
			fclose(m_fileTrace ); 
			m_bInited = FALSE;
		}
	} 
	void Trace(char* msg)
	{
		if( !m_bInited )
		{
			Init();
			m_bInited = TRUE;
		}
		CHAR	buf[1024] = {0x00};
		SYSTEMTIME systime;
		GetLocalTime(&systime);
		sprintf( buf, "%04d-%02d-%02d %02d:%02d:%02d  ",systime.wYear, systime.wMonth, systime.wDay, systime.wHour, systime.wMinute, systime.wSecond);
		strcat( buf, msg );
		fputs( buf, m_fileTrace );
		fflush( m_fileTrace );	
		if( ftell(m_fileTrace) >= MAX_TRACE_FILE_LENGTH )
			UnInit();
	}
	
private:
	FILE*	m_fileTrace	;
	BOOL	m_bInited;
};
void TRACE_LOG(LPCTSTR fmt , ... )
{
    static CTraceLog		g_TraceSDK;
    static CCritSec	    g_csTraceSDK;
	CAutoLock lc(&g_csTraceSDK);
	va_list argPtr   ;	
	CHAR  msg[1024] = {0x00}  ;
	
	va_start( argPtr, fmt );   
	vsprintf( msg, fmt, argPtr );  
	va_end  ( argPtr );
    //if( CLocalConfig::GetTraceToFile() )
	g_TraceSDK.Trace( msg );
    OutputDebugString( (LPTSTR)msg );
}
