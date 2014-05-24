// CommClass.h: interface for the CCommClass class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMCLASS_H__991C364C_5B5F_47B9_9B6E_B899E2152F95__INCLUDED_)
#define AFX_COMMCLASS_H__991C364C_5B5F_47B9_9B6E_B899E2152F95__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "winsock2.h"
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>

struct DATE_INFO;
struct TIME_INFO;

class CCommClass  
{
public:
	static BOOL ExtractFilePath( TCHAR* chPath );
	
	static BOOL SetLocalComputerTime(LPSYSTEMTIME lpSystemTime);
	static VOID LongToTime( ULONG dat , DATE_INFO * date , TIME_INFO *time );
	static UINT TimeToLong(  DATE_INFO * date , TIME_INFO *time );
	static UINT DateToLong(  DATE_INFO * date );
	static UINT	GetCurrentTime();
	static unsigned long CTimeToULong(CTime time);

	static BOOL GetLocalIP( TCHAR* chIP, TCHAR* chHostName = NULL );	

	static BOOL CreateDirectory(TCHAR* path);
	static BOOL CreateFullDirectory(TCHAR* path);
	static BOOL CheckIP( std::string ip );

	static DWORD PowerGetFileSize(const TCHAR *file);
};


class CTimeSynch
{
public:
	BOOL	DoTimeSynch(DATE_INFO& di, TIME_INFO& ti);
};

int Send( SOCKET sk, char* buf, int size, int flag);
int	Recv( SOCKET sk, char* buf, int size, int flag);

SOCKET CreateSocket( int type, INT nTimeOut );
int Connect( SOCKET sk, char* remote, int port, int nTimeOut );
int SendAndRecv(SOCKET sk, char *msg, int msgSize, char *rsp, int rspSize);

#endif // !defined(AFX_COMMCLASS_H__991C364C_5B5F_47B9_9B6E_B899E2152F95__INCLUDED_)
