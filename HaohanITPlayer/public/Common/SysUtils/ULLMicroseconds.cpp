//-----------------------------------------------------------------------------
// ULLMicroseconds.cpp
// Copyright (c) 1997 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#include <ctime>

#if defined __unix__
#include <sys/time.h>
#endif

#if defined (macintosh)

#include "ULLMicroseconds.h"

#include <Timer.h>

UInt64 ULLMicroseconds()
{
	UnsignedWide uw;
	Microseconds(&uw);
	UInt64 ull = uw.hi;
	return (ull<<32) + uw.lo;
}

#elif defined (_WIN32)

UInt64 ULLMicroseconds()
{
	LARGE_INTEGER lpFrequency, lpCount;
	UInt64 freq, count;

	if (QueryPerformanceFrequency(&lpFrequency)) {
		QueryPerformanceCounter(&lpCount);
		freq = lpFrequency.QuadPart;
		count = lpCount.QuadPart;
		return  (static_cast<UInt64> (count * static_cast<UInt64>(1000000) / freq));
	} else {
		return  (static_cast<UInt64>(clock()) * static_cast<UInt64>(1000000) / static_cast<UInt64>(CLOCKS_PER_SEC));
	}
}

#elif defined(__unix__)

UInt64 ULLMicroseconds()
{
	timeval tv;

	UInt64 micros = 0;

	if (gettimeofday(&tv, 0) == 0) {
		micros = tv.tv_sec * 1000000;
		micros += tv.tv_usec;
	}

	return micros;
}

#endif
