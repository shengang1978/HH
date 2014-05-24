//-----------------------------------------------------------------------------
// WinLibLoader.cpp
// Copyright (c) 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040915]: safe input param; no MBCS; safely use dangerous API; no registry/sys folder/temp file 
#if defined (_WIN32)

#include "WinLibLoader.h"
#include "shlwapi.h"
#include "SystemException.h"

WinLibLoader::WinLibLoader()
//use the calling .exe path as the base path  loading 
	:	mLibPath()
	,	mLibArray()
{
	//find the path of our module
	HMODULE hModule = ::GetModuleHandleW(0);
	wchar_t libPath[MAX_PATH];		
	::GetModuleFileNameW( hModule, libPath, MAX_PATH );

	//now strip of the plugin file name
	::PathRemoveFileSpecW(libPath);

	mLibPath = libPath;
};


WinLibLoader::WinLibLoader(const std::wstring &moduleName)
//uses the name dll module path as the base for loading 
	:	mLibPath()
	,	mLibArray()
{

	//find the path of our module
	HMODULE hModule = ::GetModuleHandleW(moduleName.c_str());
	wchar_t libPath[MAX_PATH];		
	::GetModuleFileNameW( hModule, libPath, MAX_PATH );

	//now strip of the plugin file name
	::PathRemoveFileSpecW(libPath);

	mLibPath = libPath;
}

WinLibLoader::~WinLibLoader()
{
}

bool WinLibLoader::LoadRelative(const std::wstring &libPath)
{
	std::wstring fullLibPath; 
	BuildLibPath(libPath,fullLibPath);
	bool exists = LibExists(fullLibPath);
	if (!exists)
		return false;

	HRESULT res = LoadLib(fullLibPath);
	if (!SUCCEEDED(res))
		throw SystemException::HRESULT(res);

	return true;
}

bool WinLibLoader::LoadAbsolute(const std::wstring &libPath)
{
	bool exists = LibExists(libPath);
	if (!exists)
		return false;
	HRESULT res = LoadLib(libPath);
	if (!SUCCEEDED(res))
		throw SystemException::HRESULT(res);

	return true;
}

void WinLibLoader::BuildLibPath(const std::wstring &libName, std::wstring &outLibPath)
{
	//would like to bring in FilePathUtils here, but adding the lib to the project has too many dependancies
	// - we are WIN32 only, so we can go ahead and build the path ourselves.
	std::wstring fullPath = mLibPath;
	fullPath += L"\\";
	fullPath += libName;
	outLibPath = fullPath;
}

HRESULT WinLibLoader::LoadLib(const std::wstring &lib)
{
	HMODULE hModule = LoadLibraryExW(lib.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	if (hModule == NULL)
		return GetLastError();
	else
		return S_OK;
}

bool WinLibLoader::LibExists(const std::wstring fullPath)
{
	//can't use FileSpec here - don't want the fsapi dependancy here - part of the lightweight CommonUtilsBasic vcproj
	HRESULT errCode = S_OK;
	try 
	{
		WIN32_FIND_DATAW fileData;
		memset(&fileData, 0x0, sizeof(WIN32_FIND_DATAW));
		HANDLE fileH = ::FindFirstFileW( fullPath.c_str(), &fileData);

		if( fileH == INVALID_HANDLE_VALUE )
			errCode = ERROR_FILE_NOT_FOUND;
		else
			::FindClose( fileH );
	}
	catch (std::exception& /*inEx*/) {
		errCode = ERROR_FILE_NOT_FOUND;		
	}
	
	return errCode == S_OK;

}



#endif //WIN32
