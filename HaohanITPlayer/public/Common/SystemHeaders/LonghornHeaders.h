//-----------------------------------------------------------------------------
// LonghornHeaders.h
// Copyright (c) 2005, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------
#ifndef __LonghornHeaders_h__
#define __LonghornHeaders_h__

#ifndef WIN32
#define WIN32
#endif
#ifndef _WIN32
#define _WIN32
#endif
#ifdef __GNUC__
#undef __GNUC__
#endif
#ifdef __unix__
#undef __unix__
#endif
#ifdef __linux__
#undef __linux__
#endif
#ifdef __MWERKS__
#undef __MWERKS__
#endif
#ifdef __MACTYPES__
#undef __MACTYPES__
#endif
#ifdef macintosh
#undef macintosh
#endif
#ifdef MACINTOSH
#undef MACINTOSH
#endif
			
#ifndef STRIP_FOR_LONGHORN
#define STRIP_FOR_LONGHORN
#endif

#ifdef __SAILFISH__
#undef __SAILFISH__
#endif
#ifdef __SWORDFISH__
#undef __SWORDFISH__
#endif
#ifdef __AUTHORSCRIPT_CE__
#undef __AUTHORSCRIPT_CE__
#endif
#ifndef ADVANCEDSTYLES_STRIP
#define ADVANCEDSTYLES_STRIP
#endif
#ifndef TRANSRATER_STRIP
#define TRANSRATER_STRIP
#endif
#ifndef IFOIMPORT_STRIP
#define IFOIMPORT_STRIP
#endif
#ifndef MASTERING_STRIP
#define MASTERING_STRIP
#endif
#ifndef SCENEDET_STRIP
#define SCENEDET_STRIP
#endif
#ifdef ENCODER_FEATURE_QUICKTIME
#undef ENCODER_FEATURE_QUICKTIME
#endif
#ifndef STRIP_QUICKTIME
#define STRIP_QUICKTIME
#endif
#ifndef STRIP_3RDPARTY
#define STRIP_3RDPARTY
#endif
#ifndef STRIP_DASHVR
#define STRIP_DASHVR
#endif
#ifndef STRIP_PLUSVR
#define STRIP_PLUSVR
#endif
#ifndef STRIP_PADUS
#define STRIP_PADUS
#endif
#ifndef STRIP_PRIMO
#define STRIP_PRIMO
#endif
#ifndef STRIP_VFR
#define STRIP_VFR
#endif
#ifndef STRIP_VR
#define STRIP_VR
#endif
#ifndef STRIP_PVR
#define STRIP_PVR
#endif
#ifndef STRIP_RTTD
#define STRIP_RTTD
#endif

#ifdef STRIP_IMAPI2
#undef STRIP_IMAPI2
#endif
#ifndef USE_IMAPI2
#define USE_IMAPI2
#endif
#ifdef USING_SONIC_GD
#undef USING_SONIC_GD
#endif
#ifdef ICODECAPI_STRIP
#undef ICODECAPI_STRIP
#endif
#ifdef VIDEDIT_STRIP
#undef VIDEDIT_STRIP
#endif

#ifdef IMAPI_STRIP
#undef IMAPI_STRIP
#endif
#ifdef CODEC_STRIP
#undef CODEC_STRIP
#endif
#ifdef SLIDESHOW_STRIP
#undef SLIDESHOW_STRIP
#endif
#ifdef MOTIONMENU_STRIP
#undef MOTIONMENU_STRIP
#endif
#ifdef SUBIMPORT_STRIP
#undef SUBIMPORT_STRIP
#endif
#ifdef TIMELINE_STRIP
#undef TIMELINE_STRIP
#endif
#ifdef CHAPTBUTTONS_STRIP
#undef CHAPTBUTTONS_STRIP
#endif

#ifndef UNICODE
#error UNICODE not defined!
#endif
#ifndef _UNICODE
#error _UNICODE not defined!
#endif

#endif
