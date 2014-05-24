// Utilities.cpp: implementation of the CUtilities class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Utilities.h"
#pragma comment(lib,"ws2_32.lib")

#if _MSC_VER >= 1300    // for VC 7.0
// from ATL 7.0 sources
#ifndef _delayimp_h
extern "C" IMAGE_DOS_HEADER __ImageBase;
#endif
#endif

HMODULE GetCurrentModule()
{
#if _MSC_VER < 1300    // earlier than .NET compiler (VC 6.0)
	
	// Here's a trick that will get you the handle of the module
	// you're running in without any a-priori knowledge:
	// http://www.dotnet247.com/247reference/msgs/13/65259.aspx
	
	MEMORY_BASIC_INFORMATION mbi;
	static int dummy;
	VirtualQuery( &dummy, &mbi, sizeof(mbi) );
	
	return reinterpret_cast<HMODULE>(mbi.AllocationBase);
	
#else    // VC 7.0
	
	// from ATL 7.0 sources
	
	return reinterpret_cast<HMODULE>(&__ImageBase);
#endif
}

bool CUtilities::ExtractAppPath( TCHAR* chPath )
{
	TCHAR path[256] = {0X00};
	GetModuleFileName(GetCurrentModule(),(TCHAR*)path,sizeof(path));
	//CString strPath = path;
	TCHAR* pos = _tcsrchr( path, '\\' );
	if( pos == NULL)
		return FALSE;
	*(pos + 1) = 0;
	_tcsncpy( chPath, path, (pos + 1) - path );
	return true;
}

BOOL CUtilities::CreateDirectory(TCHAR* path)
{
	SECURITY_ATTRIBUTES sa;
	sa.nLength              = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle       = 1;	
	
	return ::CreateDirectory( path, &sa );
}

DWORD CUtilities::PowerGetFileSize(const TCHAR *file)
{
	struct _stat buf;
	int result = 0;
	result = _tstat( file, &buf );
	if( result != 0 )
		return 0;
	else
		return buf.st_size;	
}

BOOL CUtilities::Trim(TCHAR* buf)
{
	int size = _tcslen( buf );
	TCHAR* tempbuf = new TCHAR[size + 1];
	memset( tempbuf, 0x00, (size + 1)*sizeof(TCHAR) );
	int i = 0;
	int j = 0;
	while( buf[i] != '\0' ) 
	{
		if( buf[i] != ' ' )
			tempbuf[j++] = buf[i];
		i++;
	}
	_tcscpy( buf, tempbuf );
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

void OutputDebugString_DL(LPCTSTR fmt , ... )
{
	va_list argPtr   ;	
	TCHAR  msg[1024] = {0x00}  ;
	
	va_start( argPtr, fmt );   
	_vstprintf( msg, fmt, argPtr );  
	va_end  ( argPtr ); 
	OutputDebugString( msg );
}

