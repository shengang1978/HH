//-----------------------------------------------------------------------------
// WinRegistryUtils.cpp
// Copyright (c) 2002 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS Needs Review [JAW  20040912] - Check for risky registry routines, anything which can update regsitry
//SR FS: Reviewed [JAW  20040913] - any privledge issues cause errors to be passed back
//SR FS: Reviewed [wwt 20040915]: safe input param; no MBCS; safely use dangerous API; no registry/sys folder/temp file 
#if defined _WIN32

#include "WinRegistryUtils.h"
#include "SystemException.h"
#include "UnicodeUtilities.h"

bool WinRegGetDWORD(const HKEY &key,const std::string &subkey,const std::string &valueName, SInt32 &retDWORD)
//returns false if key does not exist
{
	DWORD  dwType;
	UInt32  retCode;
	DWORD  cbData;
	HKEY   hKey;
	SInt32  *buf;
	unsigned char  buffer[MAX_PATH];

	retCode = RegOpenKeyExA(key, subkey.c_str(), 0, KEY_READ, &hKey);
	if (retCode == ERROR_CANTOPEN || retCode == ERROR_FILE_NOT_FOUND)
		return false;

	if (retCode) throw SystemException::Win32(retCode);

	cbData = MAX_PATH-1;
	retCode = RegQueryValueExA( hKey, valueName.c_str(), 0, &dwType, buffer, &cbData);
	::RegCloseKey(hKey);
	if (retCode == ERROR_CANTOPEN || retCode == ERROR_FILE_NOT_FOUND)
		return false;

	if (retCode) throw SystemException::Win32(retCode);

	buf = reinterpret_cast<SInt32 *>(buffer);
	retDWORD = *buf;
	return true;
}


void WinRegSetDWORD(const HKEY &key,const std::string &subkey,const std::string &valueName, SInt32 setDWORD)
{

	DWORD		retCode;
	HKEY		hkResult;
	DWORD		dwDisp;
	retCode = RegCreateKeyExA(key, subkey.c_str(), 0, 0, REG_OPTION_NON_VOLATILE, KEY_WRITE, 0, &hkResult, &dwDisp);
	if (retCode) throw SystemException::Win32(retCode);

	SInt32		valuelen = sizeof(setDWORD);
	retCode = RegSetValueExA(hkResult, valueName.c_str(), 0, REG_DWORD, reinterpret_cast<BYTE*>(&setDWORD), valuelen);
	RegCloseKey(hkResult);
	if (retCode) throw SystemException::Win32(retCode);
}

bool WinRegOpenKey(const HKEY &key,const std::string &subkey)
//returns false if key does not exist
{
	DWORD  retCode;
	HKEY   hKey;

	retCode = RegOpenKeyExA(key, subkey.c_str(), 0, KEY_READ, &hKey);
	if (retCode == ERROR_CANTOPEN || retCode == ERROR_FILE_NOT_FOUND)
		return false;

	if (retCode) throw SystemException::Win32(retCode);

	RegCloseKey(hKey);
	return true;
}
#endif
