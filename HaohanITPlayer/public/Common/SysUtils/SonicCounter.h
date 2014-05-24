//-----------------------------------------------------------------------------
// SonicCounter.h
// Copyright (c) 1998 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#ifndef __SonicCounter_h__
#define __SonicCounter_h__

#include "CommonTypes.h"

#if defined(__unix__)
	#if defined(__bcm__)
		#define __KERNEL__
		#include <asm/atomic.h>
		#undef __KERNEL__
	#elif !defined (__powertv__) && !defined (macintosh) && !defined(OSXPORT)
		#include <asm/atomic.h>
	#endif

#elif defined(_WIN32)
#pragma warning(push)
#pragma warning( disable : 4996 ) 
	#include <windows.h>
#pragma warning(pop)
#endif

class SonicCounter // thread-safe counter
{

public:

	// NOTE: on Linux and some versions of Windows, the results of Decrement() and Increment()
	// reflect the actual result of the operation ONLY when a value of zero is returned.  Other
	// values may reflect the results of concurrent operations.

#if defined __unix__ || defined macintosh

	#if defined OSXPORT
		SonicCounter(SInt32 count = 0) : m_count(count) {}
		SInt32 Count() const { return static_cast<SInt32>(m_count); }
		SInt32 Decrement() { DecrementAtomic(&m_count); return(Count()); }
		SInt32 Increment() { IncrementAtomic(&m_count); return(Count()); }
		
    #elif defined __stm__ || defined macintosh || defined __bcm__

	SonicCounter(SInt32 count = 0) : m_count(count) {}
	SInt32 Count() const { return m_count; }
	SInt32 Decrement() { return --m_count; }
	SInt32 Increment() { return ++m_count; }

    #else

	SonicCounter(SInt32 count = 0) { atomic_set(&m_count, count); }
	SInt32 Count() const { return static_cast<SInt32>(atomic_read(&m_count)); }
	SInt32 Decrement() { return atomic_dec_and_test(&m_count) ? 0 : static_cast<SInt32>(atomic_read(&m_count)); }
	SInt32 Increment() { return atomic_inc_and_test(&m_count) ? 0 : static_cast<SInt32>(atomic_read(&m_count)); }

    #endif

#elif defined _WIN32

	SonicCounter(SInt32 count = 0) : m_count(count) {}
	SInt32 Count() const { return static_cast<SInt32>(m_count); }
	SInt32 Decrement() { return static_cast<SInt32>(InterlockedDecrement(&m_count)); }
	SInt32 Increment() { return static_cast<SInt32>(InterlockedIncrement(&m_count)); }

#endif

private:

#if defined __unix__ || defined macintosh

    #if defined OSXPORT || defined __stm__ || defined macintosh || defined __bcm__

	SInt32 m_count;

    #else

	atomic_t m_count;

    #endif

#elif defined _WIN32

	LONG volatile m_count;

#endif

};

#endif // __SonicCounter_h__

