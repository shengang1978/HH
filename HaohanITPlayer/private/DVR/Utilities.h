// Utilities.h: �����ͨ�ú����ӿ�
//
//////////////////////////////////////////////////////////////////////
#ifndef _UTILITIES_H
#define _UTILITIES_H
#include "../stdafx.h"
#include <windows.h>
#include <winbase.h>
#include "winsock2.h"
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

class CUtilities
{
public:
	//ȡ��exe�������ڵ�·��
	static bool ExtractAppPath( TCHAR* chPath );
	static BOOL CreateDirectory(TCHAR* path);
	//�����ļ�������ļ���С
	static DWORD PowerGetFileSize(const TCHAR *file);
	
	//ѹ��buf�еĿո�
	static BOOL Trim(TCHAR* buf);
	
	//ʱ��ת������
	static int LongToTime( unsigned long dat , SYSTEMTIME* sysTime ) ;
	static int TimeToLong(  SYSTEMTIME* sysTime  ) ; 
};

HMODULE GetCurrentModule();
void OutputDebugString_DL(LPCTSTR fmt , ... );
#endif
