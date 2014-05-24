///////////////////////////////////////////////////////////////////////////////
// sonic_crt.h
// Copyright (c) 2005, Haohanit.  All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#ifndef __sonic_crt_h__
#define __sonic_crt_h__

#include "CommonTypes.h"
#include <sys/types.h>
#include <sys/timeb.h>
#include <cstdarg>
#include <cassert>

// A workaround to avoid compile errors due to conflicts with std::min/std::max.
#undef min
#undef max

// This file contains analogs of a number of deprecated functions in the CRT
// library.  It is not intended to replace all such functions, but instead only
// those which are considered to still be of some value by Sonic.  Most, if not
// all such functions, have been deprecated due to security concerns regarding
// buffer overruns.  Analogs of such functions can typically be found in this
// module with the extension of "_safe" added to the original function name.
//
// In other cases, functions may have been deprecated for reasons that aren't
// necessarily pertinent to Sonic usage, and they may be found in an essentially
// unmodified form, albeit in a new namespace.
//
// In general, if you need to use one of the functions provided in this module,
// give careful consideration as to whether or not the task can be accomplished
// in some other way first.

namespace sonic
{
	// Analogs of C file I/O functions.  These functions typically are used in
	// debug-only code, and should be avoided if at all possible.

	FILE * fopen(const char * path, const char * mode);
	FILE * wfopen(const wchar_t * path, const wchar_t * mode);
}

namespace sonic
{
	// Template-ized analogs of C-string copy functions.  In general, use of STL
	// strings is to be strongly preferred.  However, there are some situations
	// where the contents of STL strings need to be copied into a C-string to
	// communicate with legacy code, etc.  These functions are implemented as
	// templates to permit the copying of all STL string variants.
	//
	// Please note that the size_t used to describe the TOTAL destination buffer
	// size is in terms of bytes, NOT characters.  This is to permit the simple
	// usage of the sizeof() operator on the destination buffer, without regards
	// to character size or current buffer contents, in the case of concatenation
	// operations.  Also note that the versions of these functions that ensure
	// null-termination on buffer overflow will assert() since they are not
	// intended to truncate in normal usage.

	template<class T>
	T * strlcat(T * s1, const T * s2, std::size_t bytes)
	{
		// Analog of strncat():
		// 1. Total buffer size is specified in bytes.
		// 2. Buffer is null-terminated on overflow.
		// 3. Unused buffer is not null-padded.
		size_t size = bytes / sizeof(T);
		size_t size1 = ( strlen(s1) > size ?  size : strlen(s1) ); //std::min(strlen(s1), size);
		strlcpy(s1 + size1, s2, (size - size1) * sizeof(T));
		return s1;
	}

	template<class T>
	T * strlcpy(T * s1, const T * s2, std::size_t bytes)
	{
		// Analog of strncpy():
		// 1. Total buffer size is specified in bytes.
		// 2. Buffer is null-terminated on overflow.
		// 3. Unused buffer is not null-padded.
		size_t size = bytes / sizeof(T);
		std::size_t i = 0;
		for (std::size_t size1 = size ? size - 1 : 0; i < size1 && s2[i]; s1[i] = s2[i], i++);
		assert(size == 0 || s2[i] == 0); // assert on overflow
		if (i < size) s1[i] = 0;
		return s1;
	}

	template<class T>
	std::size_t strlen(const T * s)
	{
		// Analog of strlen()
		std::size_t i = 0;
		for (; s[i]; i++);
		return i;
	}

	template<class T>
	T * strncat_safe(T * s1, const T * s2, std::size_t bytes)
	{
		// Analog of strncat():
		// 1. Total buffer size is specified in bytes.
		// 2. Buffer is null-terminated on overflow.
		// 3. Unused buffer is null-padded.
		size_t size = bytes / sizeof(T);
		size_t size1 = ( strlen(s1) > size ?  size : strlen(s1) ); //std::min(strlen(s1), size);
		strncpy_safe(s1 + size1, s2, (size - size1) * sizeof(T));
		return s1;
	}

	template<class T>
	T * strncpy_safe(T * s1, const T * s2, std::size_t bytes)
	{
		// Analog of strncpy():
		// 1. Total buffer size is specified in bytes.
		// 2. Buffer is null-terminated on overflow.
		// 3. Unused buffer is null-padded.
		size_t size = bytes / sizeof(T);
		std::size_t i = 0;
		for (std::size_t size1 = size ? size - 1 : 0; i < size1 && s2[i]; s1[i] = s2[i], i++);
		assert(size == 0 || s2[i] == 0); // assert on overflow
		while (i < size) s1[i++] = 0;
		return s1;
	}

	template<class T>
	T * strncpy_unsafe(T * s1, const T * s2, std::size_t bytes)
	{
		// Analog of strncpy():
		// 1. Total buffer size is specified in bytes.
		// 2. Buffer is NOT null-terminated on overflow - CAUTION!
		// 3. Unused buffer is null-padded.
		size_t size = bytes / sizeof(T);
		std::size_t i = 0;
		for (; i < size && s2[i]; s1[i] = s2[i], i++);
		while (i < size) s1[i++] = 0; // add null padding
		return s1;
	}
}

namespace sonic
{
	// Analogs of sprintf().  Though STL is to be preferred for creating
	// formatted string data, there are times when sprintf is far easier to use,
	// provided that it can be done with some degree of safety.
	//
	// All sprintf() analogs return the length of the resulting formatted string
	// and ensure null-termination.  Buffer overflow will be accompanied by an
	// assert() since they are not intended to truncate in normal usage.
	//
	// Due to differences in sizes of numeric types on various platforms, it is
	// strongly recommended that all numeric arguments be explicitly cast to
	// standard C types and used in conjunction with appropriate size prefixes
	// in the format specification string.  Use of vendor or platform-specific
	// format specifications should be avoided to maximize portability.
	//
	// Note that no wide-character versions have been provided since sprintf()
	// is not intended to create formatted localized text, but instead is a
	// convenient (if somewhat dangerous) tool for creating formatted ASCII
	// C-string data for internal usage which may be subsequently converted to
	// whatever string encoding as required.

	size_t snprintf_safe(__out_bcount_z(bytes) char * buffer, size_t bytes, const char * format, ...);
	size_t vsnprintf_safe(__out_bcount_z(bytes) char * buffer, size_t bytes, const char * format, const va_list args);

	// The following two routines will return the digits of any number up to
	// 64 bits in size in any radix between 2 and 16.  Note that the buffer
	// will contain no leading zeroes or denotation of radix, and that a leading
	// minus sign will be prepended only for a signed decimal number.  
	char * itoa_safe(SInt64 i, __out_bcount_z(bytes) char * buffer, size_t bytes, int radix);
	char * uitoa_safe(UInt64 ui, __out_bcount_z(bytes) char * buffer, size_t bytes, int radix);
}

namespace sonic
{
	// Analogs of C time functions.
	//
	// All timezone-related functions implicitly call tzset() in order to
	// guarantee valid return values.  tzset() is a wrapper that guarantees
	// the underlying CRT implementation will be called at most once.

#ifdef _WIN32

	typedef _timeb timeb;
#else

	typedef struct timeb timeb;

#endif

	char * asctime_safe(const tm * tm, __out_bcount_z(bytes) char * buffer, size_t bytes);
	char * ctime_safe(const time_t * time, __out_bcount_z(bytes) char * buffer, size_t bytes);
	void ftime(timeb * t);
	int get_daylight();
	long get_timezone();
	struct tm * localtime_safe(const time_t * time, tm * result);
	struct tm * gmtime_safe(const time_t * time, tm * result);
	void tzset();
}

#endif // __sonic_crt_h__
