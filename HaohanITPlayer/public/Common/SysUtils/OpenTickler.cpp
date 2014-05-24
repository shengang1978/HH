//-----------------------------------------------------------------------------
// OpenTickler.cpp
// Copyright (c) 1998 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#include "OpenTickler.h"

OpenTicklerImplBase::OpenTicklerImplBase()
	: mCancelled(false)
	, mPRCount(-1)
{
}

OpenTicklerImplBase::~OpenTicklerImplBase()
{
}

bool OpenTicklerImplBase::IsCancelled()
{
	return mCancelled;
}

void OpenTicklerImplBase::Tickle(SInt32 /*phase*/,SInt64 /*count*/)
{
	//do nothing
}

OpenTickler::OpenTickler()
	: mImpPtr(0)
{
}

// Note user-defined conversion function (More Effective C++ item 5)
OpenTickler::OpenTickler(OpenTicklerImplBase* impPtr)
	: mImpPtr(impPtr)
{
}

OpenTickler::OpenTickler(const OpenTickler& t)
	: mImpPtr(t.mImpPtr)
{
}

OpenTickler& OpenTickler::operator=(const OpenTickler& t)
{
	mImpPtr = t.mImpPtr;
	return *this;
}

void OpenTickler::Tickle(SInt64 count) const
{
	if (mImpPtr == 0) return;
	mImpPtr->Tickle(count);
}

void OpenTickler::Tickle(SInt32 phase,SInt64 count) const
{
	if (mImpPtr == 0) return;
	mImpPtr->Tickle(phase,count);
}


OpenTicklerImplBase* OpenTickler::GetImplPointer() const
{
// **CodeWizzard** - Severe Violation:  Effective C++ item 29  - Avoid returning "handles" to internal data from const member function
	return mImpPtr;
}

void OpenTickler::SetImplPointer(OpenTicklerImplBase* impPtr)
{
	mImpPtr = impPtr;
}

bool OpenTickler::IsCancelled()
{
	if (mImpPtr)
		return mImpPtr->IsCancelled();
	else
		return false;
}

