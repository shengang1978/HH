//-----------------------------------------------------------------------------
// ThreadSafeRefCount.cpp
// Copyright (c) 2002 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

//
//	A thread safe reference count class.  Also allows locking of
//	the reference count for exclusive access.
//
#include <locale>
#include "ThreadSafeRefCount.h"
#include "SonicThread.h"

#ifdef _WIN32
#pragma warning(disable:4127) // conditional expression is constant - for while (true) below
#endif

ThreadSafeRefCount::ThreadSafeRefCount()
	: RefCount(), mExclusive(false)
{
}


ThreadSafeRefCount::~ThreadSafeRefCount()
{
}


//
//	AddRef - Increments reference count.
//
//	IMPORTANT: This function will block if LockExclusive has
//	been called.
//
UInt32 ThreadSafeRefCount::AddRef()
{
	while (true)
	{
		{
			SystemSpinLock lock(mLock);

			if (!mExclusive)
				return(RefCount::AddRef());
		}

		SonicThread::Sleep(1);
	}
}


//
//	Release - Decrements reference count.
//
//	IMPORTANT: This function will block if LockExclusive has
//	been called.
//
UInt32 ThreadSafeRefCount::Release()
{
	while (true)
	{
		{
			SystemSpinLock lock(mLock);

			if (!mExclusive)
				return(RefCount::Release());
		}

		SonicThread::Sleep(1);
	}
}


//
//	LockExclusive - Locks ref counter for exclusive use.  Waits until
//	all outstanding references have been released and then locks the
//	reference counter, preventing others from acquiring a reference.
//
//	IMPORTANT: This function will block until all reference counts
//	have been released.
//
void ThreadSafeRefCount::LockExclusive()
{
	while (true)
	{
		{
			SystemSpinLock lock(mLock);

			if (GetCount() == 0)
			{
				mExclusive = true;
				return;
			}
		}

		SonicThread::Sleep(1);
	}
}


//
//	UnlockExclusive - Unlocks exclusive mode so that others may
//	acquire references.
//
void ThreadSafeRefCount::UnlockExclusive()
{
	SystemSpinLock lock(mLock);

	mExclusive = false;
}
