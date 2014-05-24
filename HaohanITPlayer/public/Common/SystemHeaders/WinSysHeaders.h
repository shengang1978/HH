//-----------------------------------------------------------------------------
// WinSysHeaders.h
// Copyright (c) 1997 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------
#ifndef __WinSysHeaders_h__
#define __WinSysHeaders_h__

#include "SystemHeaders_Warnings.h"
#include "WinHeaders.h"
#include "SystemHeaders_sal.h"

#include "SystemHeaders_C_Includes.h"
#include "SystemHeaders_CPP_Includes.h"

#include "CommonTypes.h"
#include "CommonSTLTypes.h"

#ifndef	THROW_NOTHING
#define THROW_NOTHING()
#endif

#if defined ST_DEMO
	#ifndef STRIP_QUICKTIME
		#define STRIP_QUICKTIME
	#endif
#endif

// Disable unused argument, unused variable, and unreachable code warnings for the release builds
#ifndef _DEBUG
#pragma warning(disable:4100 4101 4189 4702)
#endif

#endif
