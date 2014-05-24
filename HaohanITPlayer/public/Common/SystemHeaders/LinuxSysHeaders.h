//-----------------------------------------------------------------------------
// LinuxSysHeaders.h
// Copyright (c) 2000 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

#ifndef	__LinuxSysHeaders_h
#define __LinuxSysHeaders_h

#ifndef __unix__
#define __unix__
#endif

#ifndef __linux__
#define __linux__
#endif

#define SONIC

#define __AUTHORSCRIPT_CE__
#define __SWORDFISH__
#define __need_NULL

#include <sys/types.h>

#include "SystemHeaders_C_Includes.h"
#include "SystemHeaders_CPP_Includes.h"
#include "CommonTypes.h"
#include "CommonSTLTypes.h"
#include "SystemHeaders_sal.h"

#include <cstddef>

#ifdef NULL
#undef NULL
#endif
#define NULL 0

#define PTHREAD_LIBRARY

#ifdef	THROW_NOTHING
#undef 	THROW_NOTHING
#define	THROW_NOTHING() throw()
#endif

#define _snprintf snprintf
// Strip out stuff that hasn't been implemented for Linux

#ifndef VIDEDIT_STRIP
#define VIDEDIT_STRIP
#endif

#ifndef SLIDESHOW_STRIP 
#define SLIDESHOW_STRIP 
#endif

#ifndef TIMELINE_STRIP 
#define TIMELINE_STRIP 
#endif

#ifndef SUBIMPORT_STRIP
#define SUBIMPORT_STRIP
#endif

#ifndef SCENEDET_STRIP
#define SCENEDET_STRIP
#endif

#ifndef MASTERING_STRIP
#define MASTERING_STRIP
#endif

#ifndef MOTIONMENU_STRIP
#define MOTIONMENU_STRIP
#endif

#ifndef IMAGERAPI_STATIC_LIB
#define IMAGERAPI_STATIC_LIB
#endif

#ifndef CODEC_STRIP
#define CODEC_STRIP
#endif

#ifndef CHAPTBUTTONS_STRIP
#define CHAPTBUTTONS_STRIP
#endif

//#ifndef STRIP_PLUSVR
//#define STRIP_PLUSVR
//#endif

#ifndef STRIP_QUICKTIME
#define STRIP_QUICKTIME
#endif

#ifndef USING_SONIC_GD
#define USING_SONIC_GD
#endif

#ifndef ALLOW_ANYEXTENSION_IMPORT
#define ALLOW_ANYEXTENSION_IMPORT
#endif

#ifndef NO_MENU_TEXT
#define NO_MENU_TEXT
#endif

#ifndef NO_MENU_SHADOWS
#define NO_MENU_SHADOWS
#endif

#ifndef SLIDESHOW_STRIP
#define SLIDESHOW_STRIP
#endif

#ifndef IMAPI_STRIP
#define IMAPI_STRIP
#endif

#ifndef PREVIEW_STRIP
#define PREVIEW_STRIP
#endif

#define SLIDESHOW_STRIP

#ifndef NO_RESTRICTED_OVERWRITE_MODE
#define NO_RESTRICTED_OVERWRITE_MODE
#endif

// Added to ensure Large File Support
//
#ifndef _LARGEFILE_SOURCE
#define _LARGEFILE_SOURCE
#endif

#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif

#ifdef _FILE_OFFSET_BITS
#undef _FILE_OFFSET_BITS
#endif
#define _FILE_OFFSET_BITS 64


#endif 	// _LinuxSysHeaders_h

