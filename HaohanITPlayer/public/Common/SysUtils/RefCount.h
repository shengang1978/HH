//-----------------------------------------------------------------------------
// RefCount.h
// Copyright (c) 2002 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//
//	A trivial reference counting class that's really intended as
//	a base for other more complex classes.
//

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#ifndef __REFCOUNT_H__
#define __REFCOUNT_H__

#include "SonicCounter.h"
#include <cassert>

class RefCount
{

public:

	RefCount() : mCounter(0) {}
	virtual ~RefCount() {};
	virtual UInt32 AddRef() { return mCounter.Increment(); }
	virtual UInt32 Release() { assert(mCounter.Count() > 0); return mCounter.Decrement(); }
	inline UInt32 GetCount() { return mCounter.Count(); }

private:

	SonicCounter mCounter;
};


//
//	Simple ref count holding class that acquires a reference
//	at construction time and releases it at destruction time.
//
class RefCountHolder
{

public:

	RefCountHolder(RefCount &refCount) : mRefCount(refCount) { mRefCount.AddRef(); }
	~RefCountHolder() { mRefCount.Release(); }

private:

	RefCountHolder(const RefCountHolder &);
	RefCountHolder & operator =(const RefCountHolder &);

	RefCount & mRefCount;
};


#endif	// __REFCOUNT_H__
