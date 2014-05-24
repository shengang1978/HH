//-----------------------------------------------------------------------------
// WinGDIPlusHeaders.h
// Copyright (c) 2001 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

#ifndef __WINGDIPLUSHEADERS_H__
#define __WINGDIPLUSHEADERS_H__

// these includes are required to make GDIPlus compatible with our core

#ifdef _WIN32

#pragma warning(push)
#pragma warning(disable: 4263)
#pragma warning(disable: 4264)

	#include <Unknwn.h>
	#include <RPC.h>
	#include <RPCNSI.h>
	#include <RPCNDR.h>

	#ifdef NOMINMAX
		#include <algorithm>
		using std::min;
		using std::max;
	#endif

	#include <gdiplus.h>

#pragma warning(pop)

#endif // _WIN32

#endif
