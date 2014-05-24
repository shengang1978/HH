// TraceLog.cpp: implementation of the CDVRTraceLog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TraceLog.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
FILE*	CDVRTrace::m_fileTrace = NULL	;
BOOL	CDVRTrace::m_bInited = FALSE;
int     CDVRTrace::LogLevel = 100;

CCritSec CDVRTrace::m_csTrace;

CDVRTrace::CDVRTrace()
{
	memset( this, 0x00, sizeof(CDVRTrace) );
	LogLevel = 1;
}
CDVRTrace::~CDVRTrace(){
	UnInit();
}
void CDVRTrace::Init()
{
	CreateTraceFile( m_fileTrace, "PlayerLog.txt" );
	m_bInited = TRUE;
}

void CDVRTrace::UnInit()
{
	if( m_fileTrace != NULL )
		fclose(m_fileTrace ); 
	m_bInited = FALSE;
}

void CDVRTrace::Trace(char* msg)
{
	if( !m_bInited )
	{
		Init();
		m_bInited = TRUE;
	}
	WriteTraceFile( m_fileTrace, msg );
	if( ftell(m_fileTrace) >= MAX_TRACE_FILE_LENGTH )
		UnInit();
}

void CDVRTrace::CreateTraceFile( FILE*& fileTrace, char* shortFileName )
{
	char path[256] = {0x00};
	//char *s = "D:\\";
	memcpy(path, "D:\\", strlen("D:\\"));
	//Functions::GetExeFilePath( path);
	strcat(path, "Player_Log\\");
	//Functions::CreateDirectory(path);
	SYSTEMTIME	systime;
	GetLocalTime(&systime);

	char buf[256] = {0x00};
	sprintf( buf, "%04d%02d%02d %02d%02d%02d_%04d", systime.wYear, systime.wMonth, systime.wDay, 
													systime.wHour, systime.wMinute, systime.wSecond,
													GetTickCount());
	
	strcat(path, buf);
	strcat(path, shortFileName);

	fileTrace = fopen( path,"w+b");
}
void CDVRTrace::WriteTraceFile( FILE*& fileTrace, char* msg )
{
	CHAR	buf[1024] = {0x00};
	SYSTEMTIME systime;
	GetLocalTime(&systime);
	//sprintf( buf, "%04d-%02d-%02d %02d:%02d:%02d  ",systime.wYear, systime.wMonth, systime.wDay, systime.wHour, systime.wMinute, systime.wSecond);
	sprintf( buf, "%04d-%02d-%02d %02d:%02d:%02d.%03d  ",systime.wYear, systime.wMonth, systime.wDay, systime.wHour, systime.wMinute, systime.wSecond, systime.wMilliseconds);
	strcat( buf, msg );
	fputs( buf, fileTrace );
	fflush( fileTrace );	
}

void TRACE_LOG(LPCSTR fmt , ... )
{
	if( CDVRTrace::LogLevel < 1 )
	{
		CAutoLock_CS lc(&CDVRTrace::m_csTrace);
		va_list argPtr   ;	
		CHAR  msg[1024] = {0x00}  ;	
		va_start( argPtr, fmt );   
		vsprintf( msg, fmt, argPtr );  
		va_end  ( argPtr ); 
		//OutputDebugString( msg );
		CDVRTrace::Trace( msg );
	}
}

void TRACE_ERROR(LPCSTR fmt , ... )
{
	if( CDVRTrace::LogLevel < 10 )
	{
		CAutoLock_CS lc(&CDVRTrace::m_csTrace);
		va_list argPtr   ;	
		CHAR  msg[1024] = {0x00}  ;	
		va_start( argPtr, fmt );   
		vsprintf( msg, fmt, argPtr );  
		va_end  ( argPtr ); 
		//OutputDebugString( msg );
		CDVRTrace::Trace( msg );
	}
}

void TRACE_WARN(LPCSTR fmt , ... )
{
	if( CDVRTrace::LogLevel < 3 )
	{
		CAutoLock_CS lc(&CDVRTrace::m_csTrace);
		va_list argPtr   ;	
		CHAR  msg[1024] = {0x00}  ;	
		va_start( argPtr, fmt ); 
		vsprintf( msg, fmt, argPtr );  
		va_end  ( argPtr ); 
		//OutputDebugString( msg );
		CDVRTrace::Trace( msg );
	}
}

void TRACE_INFO(LPCSTR fmt , ... )
{
	if( CDVRTrace::LogLevel < 2 )
	{
		CAutoLock_CS lc(&CDVRTrace::m_csTrace);
		va_list argPtr   ;	
		CHAR  msg[1024] = {0x00}  ;	
		va_start( argPtr, fmt );  
		vsprintf( msg, fmt, argPtr );  
		va_end  ( argPtr ); 
		//OutputDebugString( msg );
		CDVRTrace::Trace( msg );
	}
}

void TRACE_DEBUG(LPCSTR fmt , ... )
{
	if( CDVRTrace::LogLevel < 1 )
	{
		CAutoLock_CS lc(&CDVRTrace::m_csTrace);
		va_list argPtr   ;	
		CHAR  msg[1024] = {0x00}  ;	
		va_start( argPtr, fmt );   
		vsprintf( msg, fmt, argPtr );  
		va_end  ( argPtr ); 
		//OutputDebugString( msg );
		CDVRTrace::Trace( msg );
	}
}

void TRACE_CLOSE()
{
	
}