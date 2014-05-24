//-----------------------------------------------------------------------------
// WinComUtils.cpp
// Copyright (c) 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040915]: safe input param; no MBCS; safely use dangerous API; no registry/sys folder/temp file 

#if defined (_WIN32)

#include "WinComUtils.h"
#include "FileSpec.h"
#include <comdef.h>

typedef HRESULT (STDAPICALLTYPE* DllGetClassObjectFunc)(REFCLSID rClsID, REFIID riid, void ** ppv);

static HRESULT GetLastHresult() { return HRESULT_FROM_WIN32(GetLastError()); }

//____ SonicCreateInstance() __________________________________________________________________________
//
//	Description: Explicitly loads a module that implements rClsID.  CLSCTX_INPROC is implied.
//
//  Parameters: rClsID - class to instantiate
//				file - FileSpec of module that implements rClsID
//
//  Returns: HRESULT
//
HRESULT SonicCreateInstance(IN REFCLSID rClsID, IN LPUNKNOWN pUnkOuter, IN const FileSpec & file, IN REFIID riid, OUT LPVOID FAR * ppv)
{
	HRESULT hr;
	HMODULE hModule = LoadLibraryExW(file.GetNativePath().c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	if (hModule == NULL)
		hr = GetLastHresult();
	else
	{
#pragma warning(push)
#pragma warning(disable:4191)		// unsafe conversion from type of expression to type required
		DllGetClassObjectFunc pDllGetClassObjectFunc = (DllGetClassObjectFunc)GetProcAddress(hModule, "DllGetClassObject");
#pragma warning(pop)
		if (pDllGetClassObjectFunc == NULL)
			hr = GetLastHresult();
		else
		{
			IClassFactoryPtr pIClassFactory;
			hr = pDllGetClassObjectFunc(rClsID, IID_IClassFactory, reinterpret_cast<void **>(&pIClassFactory));
			if (SUCCEEDED(hr))
				hr = pIClassFactory->CreateInstance(pUnkOuter, riid, ppv);
		}

		if (FAILED(hr))
			FreeLibrary(hModule);
	}

	return hr;
}

#endif
