//-----------------------------------------------------------------------------
// WinRegistryUtils.h
// Copyright (c) 2002 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------
//SR FS: Reviewed [wwt 20040914] 
//SR FS: Reviewed [DDT 20040928] Second pass.
//SR FS: Reviewed [JAW 20040928] 

#if defined _WIN32

//gets a DWORD from the registry.  Returns false if key or value does not exist.  Any other errors, throws. 
bool WinRegGetDWORD(const HKEY &key,const std::string &subkey,const std::string &valueName, SInt32 &retDWORD);

//sets a DWORD from the registry.  Any errors, throws. 
void WinRegSetDWORD(const HKEY &key,const std::string &subkey,const std::string &valueName, SInt32 setDWORD);

//check the key exists or not in the registry. Returns false if key does not exist. Any other errors, throws
bool WinRegOpenKey(const HKEY &key,const std::string &subkey);

#endif
