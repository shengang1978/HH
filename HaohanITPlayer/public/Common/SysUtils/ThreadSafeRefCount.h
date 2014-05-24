//-----------------------------------------------------------------------------
// ThreadSafeRefCount.h
// Copyright (c) 2002 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//
//	A thread safe reference count class.  Also allows locking of
//	the reference count for exclusive access.
//

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#ifndef __THREADSAFEREFCOUNT_H__
#define __THREADSAFEREFCOUNT_H__

#include "RefCount.h"
#include "SonicSpinLock.h"

class ThreadSafeRefCount : public RefCount
{
public:
	ThreadSafeRefCount();
	~ThreadSafeRefCount();

	//
	//	AddRef - Increments reference count.
	//
	//	IMPORTANT: This function will block if LockExclusive has
	//	been called.
	//
	virtual UInt32 AddRef();

	//
	//	Release - Decrements reference count.
	//
	//	IMPORTANT: This function will block if LockExclusive has
	//	been called.
	//
	virtual UInt32 Release();

	//
	//	LockExclusive - Locks ref counter for exclusive use.  Waits until
	//	all outstanding references have been released and then locks the
	//	reference counter, preventing others from acquiring a reference.
	//
	//	IMPORTANT: This function will block until all reference counts
	//	have been released.
	//
	void LockExclusive();

	//
	//	UnlockExclusive - Unlocks exclusive mode so that others may
	//	acquire references.
	//
	void UnlockExclusive();

private:
	SpinLockFlag		mLock;
	bool				mExclusive;
};


//
//	Simple ref count holding class that acquires an exclusive lock
//	at construction time and releases it at destruction time.
//
class RefCountExclusiveLock
{
public:
// **CodeWizzard** - Informational:  More Effective C++ item 5  - Be wary of user-defined conversion function
	RefCountExclusiveLock(ThreadSafeRefCount &refCount)
		: mRefCount(refCount)
		{ mRefCount.LockExclusive(); }

	~RefCountExclusiveLock()
		{ mRefCount.UnlockExclusive(); }

private:
	RefCountExclusiveLock& operator=(const RefCountExclusiveLock&); // No assignment

	ThreadSafeRefCount	&mRefCount;
};


#endif	// __THREADSAFEREFCOUNT_H__
