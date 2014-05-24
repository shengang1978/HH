//-----------------------------------------------------------------------------
// WinComUtils.h
// Copyright (c) 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040915]: safe input param; no MBCS; no dangerous API; no registry/sys folder/temp file 

#if defined (_WIN32)

#ifndef __WinComUtils__
#define __WinComUtils__

#include <comdef.h>

class FileSpec;

//____ SonicCreateInstance() __________________________________________________________________________
//
//	Description: Explicitly loads a module that implements rClsID
//

HRESULT SonicCreateInstance(IN REFCLSID rClsID, IN LPUNKNOWN pUnkOuter, IN const FileSpec & file, IN REFIID riid, OUT LPVOID FAR * ppv);

#endif __WinComUtils__

#endif
