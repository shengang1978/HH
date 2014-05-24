//-----------------------------------------------------------------------------
// WinHeaders.h
// Copyright (c) 2005, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------
#ifndef __WinHeaders_h__
#define __WinHeaders_h__

#define _HAS_ITERATOR_DEBUGGING	0

#pragma warning(push)

#if _M_AMD64 

// Do not enable the COMPILE_64_BIT until we have the corresponding ASM code to avoid the compile problems.
#ifndef COMPILE_64_BIT
#define COMPILE_64_BIT
#endif

// Set the platform if not already set...
// For 64-bit, we are targetting WinXP (0x0501) or higher platform.
#if !defined(_WIN32_WINNT) && !defined(WINVER)
	#define WINVER			0x0501
	#define _WIN32_WINNT	WINVER 
#else
	#if !defined(_WIN32_WINNT)
		#define _WIN32_WINNT  WINVER
	#elif !defined(WINVER)
		#define WINVER  _WIN32_WINNT
	#endif
#endif

#ifndef STRIP_QUICKTIME
#define STRIP_QUICKTIME
#endif

#ifdef _X86_
#error _X86_ defined for _M_AMD64!
#endif

#pragma warning(disable:4996)	// avoid "warning C4996: 'wcscpy' was declared deprecated" for 64-bit target

#elif _M_IX86

//
// Until MyDVD 4.0, we always set this to 0400. MyDVD 4.0 want to use 0500, so all 4.0 modules including this
// header file need to define this for themselves and include this header last.
//
#if !defined(_WIN32_WINNT)
#define _WIN32_WINNT 0x0400
#endif

#else

#error Unknown architecture!

#endif

#define NOMINMAX
#define _SECURE_SCL_DEPRECATE 0					// No STL deprecated 

//
//	For MFC targets, _AFXDLL is defined.
//		MFC targets must define afx.h before windows.h
//
#ifdef _AFXDLL
	// Make sure we don't want LEAN_AND_MEAN - WTL projects define this and don't want afx.h
#pragma warning(disable:4625)			// copy constructor could not be generated because a base class copy constructor is inaccessible
#pragma warning(disable:4626)			// assignment operator could not be generated because a base class assignment operator is inaccessible
	#ifndef NO_WIN32_LEAN_AND_MEAN
	#include <afx.h>	
	#endif
#else
	#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

#include <windows.h>

#pragma warning(pop)

#endif	// __WinHeaders_h__
