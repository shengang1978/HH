//-----------------------------------------------------------------------------
// WinFilePathUtils.h
// Eduard Kegulskiy
// Copyright (c) 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------
//SR FS: Reviewed [wwt 20040914] 
//SR FS: Reviewed [DDT 20040928] Second pass.
//SR FS: Reviewed [JAW 20040928] 

#if defined _WIN32

// this file will return a name that is guaranteed to be valid in the current code page
// it will use short name if the full name contains Unicode characters not supported by the code page
bool GetCodePageFilePath(const std::wstring& unicodePath, std::wstring& codePagePath);

// this will determine if the file name (or any string) contains Unicode characters
// not supported by System code page
bool IsUnicodePath( const std::wstring& pathToCheck );
#endif
