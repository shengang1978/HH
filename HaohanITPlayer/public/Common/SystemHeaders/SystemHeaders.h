//-----------------------------------------------------------------------------
// SystemHeaders.h
// Copyright (c) 2003 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------
//
// Cross platform "System Headers" file.
//
//SR FS: Reviewed [DDT 20040924] 
//SR FS: Reviewed [JHL 20041018]
#if defined (_WIN32)

	#include "WinSysHeaders.h"

#elif defined (__mipsel__)

	#include "BroadcomSysHeaders.h"

#elif defined (__linux__)

	#include "LinuxSysHeaders.h"

#elif defined (OSXPORT)

	#include "OsxSysHeaders.h"

#elif defined (macintosh)

	#include "MacSysHeaders.h"

#endif


