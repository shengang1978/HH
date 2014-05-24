//-----------------------------------------------------------------------------
// WinFilePathUtils.cpp
// Eduard Kegulskiy
// Copyright (c) 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [wwt 20040914] - MBSC string is used. but only used internally
//SR FS: Reviewed [wwt 20040915]: safe input param; no MBCS; safely use dangerous API; no registry/sys folder/temp file 
#include <windows.h>
#include <iostream>
#include <atlbase.h>
#include "safe_ptr.h"


#include "WinFilePathUtils.h"

// this file will return a name that is guaranteed to be valid in the current code page
// it will use short name if the full name contains Unicode characters not supported by the code page
bool GetCodePageFilePath(const std::wstring& unicodePath, std::wstring& codePagePath)
{
#if defined(_WIN32)

	bool res = true;
	if( IsUnicodePath( unicodePath ) )
	{
		wchar_t shortName[MAX_PATH];
		memset( shortName, 0x0, sizeof(wchar_t)*MAX_PATH);

		DWORD result = ::GetShortPathNameW( unicodePath.c_str(), shortName, MAX_PATH);
		if(  result > 0 && result != ERROR_INVALID_PARAMETER)
		{
			codePagePath = shortName;
			res = true;		
		}
		else
		{
			codePagePath = L"";		
			res = false;	
		}
	}
	else
	{
		codePagePath = unicodePath;  // all characters in the path are present in system code page
		res = true;	
	}
	return res;
#else
#error
#endif
}

// this will determine if the file name (or any string) contains Unicode characters
// not supported by System code page
bool IsUnicodePath( const std::wstring& pathToCheck )
{
	// This was recommened by MS to correctly determine whether the path contains Unicode characters (including extended)

	int size = WideCharToMultiByte	(CP_ACP, WC_NO_BEST_FIT_CHARS, (LPCWSTR) pathToCheck.c_str(), -1, 0, 0, 0, 0);
	safe_ptr_array<char> buf(new char[size]);
	BOOL notConvertible = false;

	WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, (LPCWSTR) pathToCheck.c_str(), -1, buf.get(), size, 0, &notConvertible);
	return notConvertible ? true : false;
}
