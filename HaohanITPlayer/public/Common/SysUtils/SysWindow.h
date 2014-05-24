//-----------------------------------------------------------------------------
// SysWindow.h
// Copyright (c) 2001 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

// Include SysWindowForward.h in *.h files wherever possible
// in order to minimize Compiler dependencies. Include SysWindow.h within
// matching *.cpp files.

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#ifndef __SysWindow_h__
#define __SysWindow_h__

#if defined (__unix__)
	#include "Unix/UnixWindow.h"
#elif defined (_WIN32)
	#include "Windows/WinWindow.h"
#else
	#pragma warning(__FILE__ " >>>Warning: SysWindow is not defined for this platform")
#endif

#if defined (__unix__)
	typedef UnixWindow		SysWindow;

#elif defined (_WIN32)

	typedef WinWindow		SysWindow;

#endif


#endif // __SysWindow_h__

