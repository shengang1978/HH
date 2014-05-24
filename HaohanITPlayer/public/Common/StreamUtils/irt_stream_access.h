//-----------------------------------------------------------------------------
// IRTStreamAccess.h
// Copyright (c) 2002 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [DDT 20040916] 
//SR FS: Reviewed [wwt 20040929] - second pass

#ifndef __IRTStreamAccess_h__
#define __IRTStreamAccess_h__

#include <comdef.h>
#include "strmif.h"

// {B6367AA3-31B7-4a5f-9FE2-42DC93C0E841}
EXTERN_GUID(IID_IRTStreamAccess, 
0xb6367aa3, 0x31b7, 0x4a5f, 0x9f, 0xe2, 0x42, 0xdc, 0x93, 0xc0, 0xe8, 0x41);

DECLARE_INTERFACE_(IRTStreamAccess, IUnknown)
{
	STDMETHOD(Read)(THIS_ __out_bcount_nz(nbytes) char *data, size_t nbytes, size_t *bytes_read) PURE;
	STDMETHOD(Write)(THIS_ const char *data, size_t nbytes, size_t *bytes_written) PURE;
	
	STDMETHOD(ReadFromPos)(THIS_ UInt64 pos, __out_bcount_nz(nbytes) char *data, size_t nbytes, size_t *bytes_read) PURE;
	STDMETHOD(WriteToPos)(THIS_ UInt64 pos, const char *data, size_t nbytes, size_t *bytes_written) PURE;
	
	STDMETHOD_(UInt64, GetWritePos)(THIS) PURE;
	STDMETHOD_(UInt64, GetReadPos)(THIS) PURE;
	STDMETHOD_(UInt64, GetContentBytes)(THIS) PURE;
	
	STDMETHOD_(void, PushTimeStamp)(THIS_ UInt64 nBytePos, double dStartTime) PURE;
	STDMETHOD_(void, PushEndOfStream)(THIS) PURE;
	
	STDMETHOD_(bool, HasMetadata)(THIS) PURE;
};

_COM_SMARTPTR_TYPEDEF(IRTStreamAccess, IID_IRTStreamAccess);

#endif
