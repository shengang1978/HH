//-----------------------------------------------------------------------------
// CustomSysHeaders.h
// Copyright (c) 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------
// Settings for AuhthorScript for Longhorn logical stripping
//-----------------------------------------------------------------------------

#ifndef __CustomSysHeaders_h__
#define __CustomSysHeaders_h__

#define __LONGHORN__

#pragma warning(disable : 4100)


// WIN32="TRUE"
#ifndef WIN32
#define WIN32
#endif

// _WIN32="TRUE"
#ifndef _WIN32
#define _WIN32
#endif

// __GNUC__="FALSE"
#ifdef __GNUC__
#undef __GNUC__
#endif

// __unix__="FALSE"
#ifdef __unix__
#undef __unix__
#endif

// __linux__="FALSE"
#ifdef __linux__
#undef __linux__
#endif

// __MWERKS__="FALSE"
#ifdef __MWERKS__
#undef __MWERKS__
#endif

// macintosh="FALSE"
#ifdef macintosh
#undef macintosh
#endif

// MACINTOSH="FALSE"
#ifdef MACINTOSH
#undef MACINTOSH
#endif

// __SAILFISH__="FALSE"
#ifdef __SAILFISH__
#undef __SAILFISH__
#endif

// __SWORDFISH__="FALSE"
#ifdef __SWORDFISH__
#undef __SWORDFISH__
#endif

// __AUTHORSCRIPT_CE__="FALSE"
#ifdef __AUTHORSCRIPT_CE__
#undef __AUTHORSCRIPT_CE__
#endif

// ADVANCEDSTYLES_STRIP="TRUE"
#ifndef ADVANCEDSTYLES_STRIP
#define ADVANCEDSTYLES_STRIP
#endif

// TRANSRATER_STRIP="TRUE"
#ifndef TRANSRATER_STRIP
#define TRANSRATER_STRIP
#endif

// IFOIMPORT_STRIP="TRUE"
#ifndef IFOIMPORT_STRIP
#define IFOIMPORT_STRIP
#endif

// MASTERING_STRIP="TRUE"
#ifndef MASTERING_STRIP
#define MASTERING_STRIP
#endif

// SCENEDET_STRIP="TRUE"
#ifndef SCENEDET_STRIP
#define SCENEDET_STRIP
#endif

// ENCODER_FEATURE_QUICKTIME="FALSE"
#ifdef ENCODER_FEATURE_QUICKTIME
#undef ENCODER_FEATURE_QUICKTIME
#endif

// STRIP_QUICKTIME="TRUE"
#ifndef STRIP_QUICKTIME
#define STRIP_QUICKTIME
#endif

// STRIP_3RDPARTY="TRUE"
#ifndef STRIP_3RDPARTY
#define STRIP_3RDPARTY
#endif

// STRIP_DASHVR="TRUE"
#ifndef STRIP_DASHVR
#define STRIP_DASHVR
#endif

// STRIP_PLUSVR="TRUE"
#ifndef STRIP_PLUSVR
#define STRIP_PLUSVR
#endif

// STRIP_PADUS="TRUE"
#ifndef STRIP_PADUS
#define STRIP_PADUS
#endif

// STRIP_PRIMO="TRUE"
#ifndef STRIP_PRIMO
#define STRIP_PRIMO
#endif

// STRIP_VFR="TRUE"
#ifndef STRIP_VFR
#define STRIP_VFR
#endif

// STRIP_VR="TRUE"
#ifndef STRIP_VR
#define STRIP_VR
#endif

// STRIP_PVR="TRUE"
#ifndef STRIP_PVR
#define STRIP_PVR
#endif

// STRIP_RTTD="TRUE"
#ifndef STRIP_RTTD
#define STRIP_RTTD
#endif

// STRIP_IMAPI2="FALSE"
#ifdef STRIP_IMAPI2
#undef STRIP_IMAPI2
#endif

// USE_IMAPI2="TRUE"
#ifndef USE_IMAPI2
#define USE_IMAPI2
#endif

// USING_SONIC_GD="FALSE"
#ifdef USING_SONIC_GD
#undef USING_SONIC_GD
#endif

// ICODECAPI_STRIP="TRUE"
#ifndef ICODECAPI_STRIP
#define ICODECAPI_STRIP
#endif

// VIDEDIT_STRIP="FALSE"
#ifdef VIDEDIT_STRIP
#undef VIDEDIT_STRIP
#endif

// DVD_AUDIO="FALSE"
#ifdef DVD_AUDIO
#undef DVD_AUDIO
#endif

// IMAPI_STRIP="FALSE"
#ifdef IMAPI_STRIP
#undef IMAPI_STRIP
#endif

// CODEC_STRIP="FALSE"
#ifdef CODEC_STRIP
#undef CODEC_STRIP
#endif

// SLIDESHOW_STRIP="FALSE"
#ifdef SLIDESHOW_STRIP
#undef SLIDESHOW_STRIP
#endif

// MOTIONMENU_STRIP="FALSE"
#ifdef MOTIONMENU_STRIP
#undef MOTIONMENU_STRIP
#endif

// SUBIMPORT_STRIP="FALSE"
#ifdef SUBIMPORT_STRIP
#undef SUBIMPORT_STRIP
#endif

// TIMELINE_STRIP="FALSE"
#ifdef TIMELINE_STRIP
#undef TIMELINE_STRIP
#endif

// CHAPTBUTTONS_STRIP="FALSE"
#ifdef CHAPTBUTTONS_STRIP
#undef CHAPTBUTTONS_STRIP
#endif


#endif
