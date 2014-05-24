//-----------------------------------------------------------------------------
// CallbackTypes.h
// Copyright (c) 1997 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------
// Callback types
// Abstract base classes for function callbacks.

//SR FS: Reviewed [JAW 20040911]
//SR FS: Reviewed [wwt 20040914] 

#ifndef __CallbackTypes__
#define __CallbackTypes__


class StringCallback
{

	public:
		virtual SInt32 StatusUpdate(SInt32 statusVal, const char *statusString, void* refCon) = 0;
		
} ;

#endif

