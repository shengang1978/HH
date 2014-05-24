// TraceLog.h: interface for the CDVRTraceLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRACELOG_H__D7A5AAB8_C515_4799_A276_85E1A305958E__INCLUDED_)
#define AFX_TRACELOG_H__D7A5AAB8_C515_4799_A276_85E1A305958E__INCLUDED_

#include "Sync_Locks.h"

#define MAX_TRACE_FILE_LENGTH	(8*1024*1024)

class CDVRTrace
{
public:
	CDVRTrace();
	~CDVRTrace();
	static void Trace(char* msg);

private:
	static void Init();

	static void UnInit();
private:
	static void CreateTraceFile( FILE*& fileTrace, char* shortFileName );
	static void WriteTraceFile( FILE*& fileTrace, char* msg );
private:
	static FILE* m_fileTrace	;
	static BOOL	 m_bInited;

public:
	static CCritSec	m_csTrace;
	static int LogLevel;
};

void TRACE_LOG(LPCSTR fmt , ... );
void TRACE_ERROR(LPCSTR fmt , ... );
void TRACE_WARN(LPCSTR fmt , ... );
void TRACE_INFO(LPCSTR fmt , ... );
void TRACE_DEBUG(LPCSTR fmt , ... );
void TRACE_CLOSE();

#endif // !defined(AFX_TRACELOG_H__D7A5AAB8_C515_4799_A276_85E1A305958E__INCLUDED_)
