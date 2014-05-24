//-----------------------------------------------------------------------------
// WinLibLoader.h
// Copyright (c) 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------
//SR FS: Reviewed [wwt 20040914] 
//SR FS: Reviewed [DDT 20040928] Second pass.
//SR FS: Reviewed [JAW 20040928] 

#ifndef __WinLibLoader_h__
#define __WinLibLoader_h__

#include <string>
#include "WTypes.h"
#include <vector>

class WinLibLoader
{
public:
	WinLibLoader(); //use the calling .exe path as the base path  loading 
	WinLibLoader(const std::wstring &moduleName); //uses the name dll module path as the base for loading 
	virtual ~WinLibLoader();

	bool LoadRelative(const std::wstring &libPath); //returns false if lib does not exist, throws otherwise
	bool LoadAbsolute(const std::wstring &libPath); //returns false if lib does not exist, throws otherwise

	void GetModulePath(std::wstring &outModulePath) { outModulePath = mLibPath; }

private:
	WinLibLoader(const WinLibLoader &rhs);
	WinLibLoader &operator=(const WinLibLoader &rhs);

	void BuildLibPath(const std::wstring &libName, std::wstring &outLibPath);
	HRESULT LoadLib(const std::wstring &lib);

	//utility
	bool LibExists(const std::wstring fullPath);

	std::wstring mLibPath;
	std::vector<std::wstring> mLibArray;
};

#endif
