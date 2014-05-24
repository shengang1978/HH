// Utilities.h: 定义的通用函数接口
//
//////////////////////////////////////////////////////////////////////
#ifndef _UTILITIES_H
#define _UTILITIES_H

#include <windows.h>
#include <winbase.h>
#include "winsock2.h"
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string>
using std::string;

#define  REPLACE_LEFT    1 
#define  REPLACE_TOP     2
#define  REPLACE_RIGHT   4
#define  REPLACE_BOTTOM  8
#define  REPLACE_ALL     15


#define SAFE_DELETE_ARRAY(p) { if(p) { delete [](p);     (p)=NULL; } }	
class CUtilities
{
public:
	//创建目录
    static BOOL CreateDirectory(char* path);
	
	static bool ExtractFilePathB( HMODULE hModule, char* chPath );
	//取出exe程序所在的路径
	static bool ExtractAppPath( char* chPath );
	//根据文件名获得文件大小
	static DWORD PowerGetFileSize(const char *file);
	
	//压缩buf中的空格
	static BOOL Trim(char* buf);
	
	//时间转化函数
	static int LongToTime( unsigned long dat , SYSTEMTIME* sysTime ) ;
	static int TimeToLong(  SYSTEMTIME* sysTime  ) ;

    //创建全目录
    static BOOL CreateFullDirectory(char* path); 
	//调整wnd位置
	static RECT AjustWindowPos(HWND hwnd, int leftOffset, int topOffset, int rightOffset, int bottomOffset, int replacePara = REPLACE_ALL);

	//控件上画BMP图片   
	//参数： bmp图片路径， 控件窗口句柄
	static bool PaintBmp(char* ImagePath, HWND hwnd);

	static string ShowSelectDirectoryDlg(const string& strDlgTitle);

	static BOOL GetDirectoryFileNum(char* DirPath);
};

class CCritSec 
{	
    CCritSec(const CCritSec &refCritSec);
    CCritSec &operator=(const CCritSec &refCritSec);
	
    CRITICAL_SECTION m_CritSec;
	
public:
    CCritSec() {
        InitializeCriticalSection(&m_CritSec);
    };
	
    ~CCritSec() {
        DeleteCriticalSection(&m_CritSec);
    };
	
    void Lock() {
        EnterCriticalSection(&m_CritSec);
    };
	
    void Unlock() {
        LeaveCriticalSection(&m_CritSec);
    };
};

class CAutoLock {
    CAutoLock(const CAutoLock &refAutoLock);
    CAutoLock &operator=(const CAutoLock &refAutoLock);
	
protected:
    CCritSec * m_pLock;
	
public:
    CAutoLock(CCritSec * plock)
    {
        m_pLock = plock;
        m_pLock->Lock();
    };
	
    ~CAutoLock() {
        m_pLock->Unlock();
    };
};
#endif
