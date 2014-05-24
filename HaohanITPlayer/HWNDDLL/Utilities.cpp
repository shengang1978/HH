// Utilities.cpp: implementation of the CUtilities class.
//
//////////////////////////////////////////////////////////////////////
#pragma warning(disable:4786)
#include "stdafx.h"
#include "shlwapi.h"
#include "Utilities.h"
#include <string>
#include <vector>
using namespace std;
#pragma comment(lib,"ws2_32.lib")

BOOL CUtilities::CreateDirectory(char* path)
{
	SECURITY_ATTRIBUTES sa;
	sa.nLength              = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle       = 1;	
	
	return ::CreateDirectory( path, &sa );
}
bool CUtilities::ExtractFilePathB(  HMODULE hModule, char* chPath )
{
	char path[256] = {0X00};
	GetModuleFileName(hModule,(char*)path,sizeof(path));
	char* pos = strrchr( path, '\\' );
	if( pos == NULL)
		return FALSE;
	*(pos + 1) = 0;
	strncpy( chPath, path, (pos + 1) - path );
	return TRUE;
}
bool CUtilities::ExtractAppPath( char* chPath )
{
	char path[256] = {0X00};
	GetModuleFileName(NULL,(char*)path,sizeof(path));
	//CString strPath = path;
	char* pos = strrchr( path, '\\' );
	if( pos == NULL)
		return FALSE;
	*(pos + 1) = 0;
	strncpy( chPath, path, (pos + 1) - path );
	return true;
}

DWORD CUtilities::PowerGetFileSize(const char *file)
{
	struct _stat buf;
	int result = 0;
	result = _stat( file, &buf );
	if( result != 0 )
		return 0;
	else
		return buf.st_size;	
}

BOOL CUtilities::Trim(char* buf)
{
	int size = strlen( buf );
	char* tempbuf = new char[size + 1];
	memset( tempbuf, 0x00, size + 1 );
	int i = 0;
	int j = 0;
	while( buf[i] != '\0' ) 
	{
		if( buf[i] != ' ' )
			tempbuf[j++] = buf[i];
		i++;
	}
	strcpy( buf, tempbuf );
	delete[] tempbuf;
	return TRUE;
}

int CUtilities::LongToTime( unsigned long dat , SYSTEMTIME* sysTime )
{
	int  days[13] = { 0, 31 , 29 , 31 , 30 , 31 , 30 , 31 , 31 , 30 , 31 , 30 , 31 } ;
    int  i ;
	
    sysTime->wSecond	= WORD(dat % 60) ;
    dat					= dat / 60 ;        //dat is minute
    
    sysTime->wMinute = WORD(dat % 60) ;
    dat          = dat / 60 ;        //dat is hour  
    
    sysTime->wHour   = WORD(dat % 24) ;
    dat          = dat / 24 ;        //dat is day   
	
    i            = 0    ;	
	
	bool bLeapYear = true; //从2000.1.1日开始
	while( dat >= (DWORD)( bLeapYear ? 366 :365 ))// 存在闰年问题
    {
        if( bLeapYear )
            dat -= 366 ;
        else
            dat -= 365 ;
        i++ ;
		bLeapYear = (2000+i)%400==0 || ( (2000+i)%4==0 && (2000+i)%100!=0 ) ;
    }
    
    sysTime->wYear  = 2000 + i ;
    if( !bLeapYear ) 
        days[2] = 28 ;
    
    for( i = 1 ; i < 13 ; i++ )
    {
        if( dat < (DWORD)days[i] )	
            break;
        dat -= days[i] ;    
    }
    sysTime->wMonth  =  i  ;
    sysTime->wDay    = (WORD)(dat + 1) ;
	return 0;
}
int CUtilities::TimeToLong(  SYSTEMTIME* sysTime  )
{
	int  days[13] = { 0, 31 , 29 , 31 , 30 , 31 , 30 , 31 , 31 , 30 , 31 , 30 , 31 } ;
    int  day  , i ;
    unsigned long  seconds ;
    
    seconds =    0 ;
    day     =    0 ;
    
	bool bLeapYear = sysTime->wYear%400 == 0 || ( sysTime->wYear%4 == 0 && sysTime->wYear%100 != 0 ) ;
    if( !bLeapYear )
        days[2] = 28 ;
    
    for( i = 0 ; i < ( sysTime->wYear - 2000 ) ; i++ )
    {
        bLeapYear = (2000+i)%400 == 0 || ( (2000+i)%4 == 0 && (2000+i)%100 != 0 ) ;
		if( bLeapYear )
            day += 366 ;
        else
            day += 365 ;
    }    
    for( i = 1 ; i < sysTime->wMonth ; i++ )  
    {
        day += days[i] ;
    }       
    day += ( sysTime->wDay - 1 );     
    
    seconds = ( day * 24 + sysTime->wHour )*3600 + sysTime->wMinute * 60 + sysTime->wSecond ;
    
    return seconds ;    
}

//2009-06-02 added创建全目录
BOOL CUtilities::CreateFullDirectory(char* path)
{
	if( NULL == path )
	{
		return FALSE;
	}

	string cstrPath = path;
	//cstrPath.TrimLeft();
	//cstrPath.TrimRight();

	vector<string> vecCStr;
	vecCStr.push_back(cstrPath);

	int nFindPos = cstrPath.find_last_of('\\');
	while( 0 < nFindPos )
	{
		cstrPath = cstrPath.substr(0, nFindPos);
		if( nFindPos > 0 )
		if( ':' != cstrPath[nFindPos - 1] )
		{		
			vecCStr.push_back(cstrPath);
		}
		
		nFindPos = cstrPath.find_last_of('\\');
	}

	//LPTSTR p = NULL;
	int nSize = vecCStr.size();
	int i = 0;
	while( i < nSize )
	{
		cstrPath = vecCStr.back();
		vecCStr.pop_back();

		//p = cstrPath.GetBuffer(cstrPath.GetLength());

		CreateDirectory( (char*)cstrPath.c_str() );

		//cstrPath.ReleaseBuffer();

		i++;
	}

	return TRUE;
}

//用来初始化wnd位置
RECT CUtilities::AjustWindowPos(HWND hwnd, int leftOffset, int topOffset, int rightOffset, int bottomOffset, int replacePara)
{	
	RECT rect;	
	::GetClientRect(hwnd, &rect);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	if(replacePara & REPLACE_LEFT) //left
		rect.left = leftOffset;
	else
		rect.left += leftOffset;

	if(replacePara & REPLACE_TOP)  //top
		rect.top = topOffset;
	else
		rect.top += topOffset;

	if(replacePara & REPLACE_RIGHT) //right
		rect.right = rightOffset;
	else
		rect.right = rect.left + width + rightOffset;

	if(replacePara & REPLACE_BOTTOM) //bottom
		rect.bottom = bottomOffset;
	else
		rect.bottom = rect.top + height + bottomOffset;	
	//::MoveWindow(hwnd, rect.left, rect.top, rect.right, rect.bottom, true);
	return rect;
}

//控件上画BMP图片   
//参数： bmp图片路径， 控件窗口句柄
bool CUtilities::PaintBmp(char* ImagePath, HWND hwnd)
{
	if(!ImagePath || !hwnd)
		return false;
	HBITMAP m_Logo;
    m_Logo  = (HBITMAP)LoadImage(NULL, ImagePath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);
    BITMAP     bm ;
    HDC        hdcImage ;

    hdcImage = CreateCompatibleDC( NULL ) ;
    SelectObject( hdcImage, m_Logo ) ;
    GetObject( m_Logo , sizeof(bm), &bm );

    HDC     hdcDest;
	hdcDest = ::GetDC(hwnd);
	
	RECT rt;
	::GetClientRect(hwnd, &rt);
	int width = rt.right - rt.left;
	int height = rt.bottom - rt.top;

    StretchBlt( hdcDest, 0 , 0 , width , height,
        hdcImage, 0 , 0 , bm.bmWidth , bm.bmHeight , SRCCOPY);

    DeleteDC  (hdcImage);
    DeleteObject(m_Logo);
	return true;
}

string CUtilities::ShowSelectDirectoryDlg(const string& strDlgTitle)
{ 	
	char   szDir[MAX_PATH];	
	BROWSEINFO   bi;	
	ITEMIDLIST   *pidl = NULL;
	
	bi.hwndOwner   =   NULL;	
	bi.pidlRoot   =   NULL;	
	bi.pszDisplayName   =   szDir;	
	bi.lpszTitle   =   strDlgTitle.c_str();	
	bi.ulFlags   =   BIF_RETURNONLYFSDIRS;	
	bi.lpfn   =   NULL;	
	bi.lParam   =   0;	
	bi.iImage   =   0;	
	
	pidl = SHBrowseForFolder(&bi);	
	if(pidl == NULL)		
		return _T("");	
	if( !SHGetPathFromIDList(pidl, szDir) )		
		return _T("");	
	return   string(szDir);	
}

BOOL CUtilities::GetDirectoryFileNum(char* DirPath)
{
	if(!DirPath)
		return FALSE;
	char buf[1000] = {0X00};
	strncpy(buf, DirPath, 1000);
	
    char strbuf[1000];
	//strcpy(m_FilePath, buf);
	strcpy(strbuf,buf);
    //PathCompactPath(NULL, strbuf, 400);	

	bool fFinished = false;
	strcat(buf, "\\*.jpg");
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;	
	hFind = FindFirstFile(buf, &FindFileData); /*20060418-103919-04000401.jpg"*/	
	
	if (hFind == INVALID_HANDLE_VALUE) 
	{ 
		printf("No .jpg files found."); 
		return 0;
	} 	
   
	CString path;
	CString str;
	FILE* pfile = NULL; 
	int i = 0;
	while (!fFinished)
	{		
		path = FindFileData.cFileName; 		
		if (!FindNextFile(hFind, &FindFileData)) 
		{
			if (GetLastError() == ERROR_NO_MORE_FILES) 
			{ 
				fFinished = true; 
			} 
			else 
			{ 
				printf("Couldn't find next file."); 
				fFinished = true; 
			} 
		}	
		i++;		
    }
    FindClose(hFind);
	return i;
}
