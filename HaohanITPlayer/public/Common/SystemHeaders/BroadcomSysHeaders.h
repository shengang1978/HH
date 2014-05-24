//-----------------------------------------------------------------------------
// Copyright (c) 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

#ifndef	__BroadcomSysHeaders_h
#define __BroadcomSysHeaders_h

#ifndef __mipsel__
#define __mipsel__
#endif

#ifndef __unix__
#define __unix__
#endif

#define SONIC

#define __AUTHORSCRIPT_CE__
#define __SWORDFISH__

#include "SystemHeaders_C_Includes.h"
#include "SystemHeaders_CPP_Includes.h"

#include <cstddef>

#include "CommonTypes.h"
#include "CommonSTLTypes.h"
#include "SystemHeaders_sal.h"

#undef NULL
#define NULL 0

/* 
* A note on this.....
* Under IRIS' page size is always 16k but since everything in SSE is designed
* around 4k page size, so we use 4k chunks unless SSE will get ported to the
* 64bit ABI
*/
#ifdef __IX64__
#define IRIXPageSize	getpagesize()
#else	// __IX64__
#define IRIXPageSize    4096
#endif	// __IX64__

#define PTHREAD_LIBRARY

#ifndef	THROW_NOTHING
#define THROW_NOTHING(x)
#endif

// RL

#undef 	THROW_NOTHING
#define	THROW_NOTHING() throw()

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

#ifndef STRIP_PLUSVR
#define STRIP_PLUSVR
#endif

#ifndef STRIP_DASHVR
#define STRIP_DASHVR
#endif

#ifndef STRIP_QUICKTIME
#define STRIP_QUICKTIME
#endif

/* KC 02.10.2006 
Change OK'd by V.J. since Broadcom Authorscript will 
only be using this file from Neutrino. Broadcom is now moving to
DirectFB graphics and we have a new SysSurface, etc. to accomodate that.
#ifndef USING_SONIC_GD
#define USING_SONIC_GD
#endif*/

#ifndef _DIRECTFB
#define _DIRECTFB
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

#ifndef NEUTRINO_STRIP
#define NEUTRINO_STRIP
#endif


#endif 	// __BroadcomSysHeaders_h

