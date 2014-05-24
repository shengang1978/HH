//-----------------------------------------------------------------------------
// SysWindowForward.h
// Copyright (c) 2002 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

// Include this instead of sysWindow.h in *.h files wherever possible
// in order to minimize Compiler dependencies. Include SysBlitter.h within
// matching *.cpp files.

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#ifndef __SysWindowForward_h__
#define __SysWindowForward_h__

#if defined (__unix__)

	class UnixWindow;
	typedef UnixWindow		SysWindow;


#elif defined (_WIN32)

	class WinWindow;
	typedef WinWindow		SysWindow;


#endif


#endif // __SysWindowForward_h__

