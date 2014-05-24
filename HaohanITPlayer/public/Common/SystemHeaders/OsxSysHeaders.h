//-----------------------------------------------------------------------------
// OsxSysHeaders.h
// Copyright (c) 2006, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

#ifndef	__OsxSysHeaders_h
#define __OsxSysHeaders_h

// on OS X Debug must have a value!=0, just being defined wreaks havoc
#ifdef DEBUG
#undef DEBUG
#define DEBUG	1
#endif

// HVH - Endian should be based on processor not OS since OSX now runs on Intel platforms
// HVH - This should use ByteSwapper.h
#ifdef __BIG_ENDIAN__
#define _BIG_ENDIAN_
#endif

#ifndef USE_NAMESPACE
#define USE_NAMESPACE
#endif

#ifndef ENABLE_EXACT_COPY
#define ENABLE_EXACT_COPY
#endif

#include <unistd.h>
#include <string.h>
#include <stdlib.h>

// Carbon.h is roughly equivalent to windows.h (unless you are an Objective-C guy, and even they you still can include it)

#include <Carbon/Carbon.h>

// remainder is copied from LinuxSysHeaders.h for now,
// +++ but see the code for gcc char_traits at the end of the file, I am not completely sure if that is correct

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

// Depricated. should use Sonic_crt.h
#define _snprintf snprintf

// Strip out stuff that hasn't been implemented for Linux
// HVH - As we bring AuthorScript online for OSX we need to enable most of these features.
// HVH - These settings are based on a CE style box
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

#ifndef STRIP_QUICKTIME
#define STRIP_QUICKTIME
#endif

//	HVH - What graphics library should we use for OSX
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

// HVH - let's move this to a separate file
// +++ bad hack to add some missing stuff to OS X gcc char_traits
namespace std {
  template<>
    struct char_traits<unsigned char>
    {
      typedef unsigned char char_type;
      typedef unsigned long int_type;
      typedef streampos 	pos_type;
      typedef streamoff 	off_type;
      typedef mbstate_t 	state_type;

      static void 
      assign(char_type& __c1, const char_type& __c2)
      { __c1 = __c2; }

      static bool 
      eq(const char_type& __c1, const char_type& __c2)
      { return __c1 == __c2; }

      static bool 
      lt(const char_type& __c1, const char_type& __c2)
      { return __c1 < __c2; }

      static int 
      compare(const char_type* __s1, const char_type* __s2, size_t __n)
      { return memcmp(__s1, __s2, __n); }

      static size_t
      length(const char_type* __s)
      { return strlen((const char*)__s); }

      static const char_type* 
      find(const char_type* __s, size_t __n, const char_type& __a)
      { return static_cast<const char_type*>(memchr(__s, __a, __n)); }

      static char_type* 
      move(char_type* __s1, const char_type* __s2, size_t __n)
      { return static_cast<char_type*>(memmove(__s1, __s2, __n)); }

      static char_type* 
      copy(char_type* __s1, const char_type* __s2, size_t __n)
      {  return static_cast<char_type*>(memcpy(__s1, __s2, __n)); }

      static char_type* 
      assign(char_type* __s, size_t __n, char_type __a)
      { return static_cast<char_type*>(memset(__s, __a, __n)); }

      static char_type 
      to_char_type(const int_type& __c)
      { return static_cast<char_type>(__c); }

      // To keep both the byte 0xff and the eof symbol 0xffffffff
      // from ending up as 0xffffffff.
      static int_type 
      to_int_type(const char_type& __c)
      { return static_cast<int_type>(static_cast<unsigned char>(__c)); }

      static bool 
      eq_int_type(const int_type& __c1, const int_type& __c2)
      { return __c1 == __c2; }

      static int_type 
      eof() { return static_cast<int_type>(EOF); }

      static int_type 
      not_eof(const int_type& __c)
      { return (__c == eof()) ? 0 : __c; }
  };
  template<>
    struct char_traits<unsigned short>
    {
      typedef unsigned short	char_type;
      typedef unsigned long	int_type;
      typedef streamoff 	off_type;
      typedef streampos 	pos_type;
      typedef mbstate_t 	state_type;
      
      static void 
      assign(char_type& __c1, const char_type& __c2)
      { __c1 = __c2; }

      static bool 
      eq(const char_type& __c1, const char_type& __c2)
      { return __c1 == __c2; }

      static bool 
      lt(const char_type& __c1, const char_type& __c2)
      { return __c1 < __c2; }

      static int 
      compare(const char_type* __s1, const char_type* __s2, size_t __n)
      {
		for (size_t __i = 0; __i < __n; ++__i)
		  if (!eq(__s1[__i], __s2[__i]))
			return lt(__s1[__i], __s2[__i]) ? -1 : 1;
		return 0; 
	  }

      static size_t
      length(const char_type* __s)
      {
		const char_type* __p = __s; 
		while (*__p) ++__p; 
		return (__p - __s); 
	  }

      static const char_type* 
      find(const char_type* __s, size_t __n, const char_type& __a)
      {
		for (const char_type* __p = __s; size_t(__p - __s) < __n; ++__p)
		  if (*__p == __a) return __p;
		return 0;
	  }

      static char_type* 
      move(char_type* __s1, const char_type* __s2, int_type __n)
      { return (char_type*) memmove(__s1, __s2, __n * sizeof(char_type)); }

      static char_type* 
      copy(char_type* __s1, const char_type* __s2, size_t __n)
      { return (char_type*) memcpy(__s1, __s2, __n * sizeof(char_type)); }

      static char_type* 
      assign(char_type* __s, size_t __n, char_type __a)
      {
		for (char_type* __p = __s; __p < __s + __n; ++__p) 
		  assign(*__p, __a);
		return __s; 
	  }

      static char_type 
      to_char_type(const int_type& __c) { return char_type(__c); }

      static int_type 
      to_int_type(const char_type& __c) { return int_type(__c); }

      static bool 
      eq_int_type(const int_type& __c1, const int_type& __c2)
      { return __c1 == __c2; }

      static int_type 
      eof() { return static_cast<int_type>(-1); }

      static int_type 
      not_eof(const int_type& __c)
      { return eq_int_type(__c, eof()) ? 0 : __c; }
  };
}

#endif
