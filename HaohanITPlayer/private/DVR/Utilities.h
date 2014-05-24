// Utilities.h: 定义的通用函数接口
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
	//取出exe程序所在的路径
	static bool ExtractAppPath( TCHAR* chPath );
	static BOOL CreateDirectory(TCHAR* path);
	//根据文件名获得文件大小
	static DWORD PowerGetFileSize(const TCHAR *file);
	
	//压缩buf中的空格
	static BOOL Trim(TCHAR* buf);
	
	//时间转化函数
	static int LongToTime( unsigned long dat , SYSTEMTIME* sysTime ) ;
	static int TimeToLong(  SYSTEMTIME* sysTime  ) ; 
};

HMODULE GetCurrentModule();
void OutputDebugString_DL(LPCTSTR fmt , ... );
#endif
