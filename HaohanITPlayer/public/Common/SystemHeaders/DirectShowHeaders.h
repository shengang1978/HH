//-----------------------------------------------------------------------------
// DirectShowHeaders.h
// Copyright (c) 2003 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

#ifndef DirectShowHeaders_H
#define DirectShowHeaders_H

#ifdef _WIN32

// Turn off warnings that occur in DirectShow headers

#pragma warning(push)
#pragma warning(disable:4263 4264 4265 4312)
#pragma warning(disable:4625)
#pragma warning(disable:4626)
#include <streams.h>
#pragma warning(pop)

#pragma comment(lib, "winmm")

#endif

#endif
