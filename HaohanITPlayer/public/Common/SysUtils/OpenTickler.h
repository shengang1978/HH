//-----------------------------------------------------------------------------
// OpenTickler.h
// Copyright (c) 1998 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#ifndef OPENTICKLER
#define OPENTICKLER

#include "CommonTypes.h"

class OpenTicklerImplBase
// **CodeWizzard** - Violation:  Universal Coding Standards item 2  - Do not use protected data member
{
public:
	OpenTicklerImplBase();
	virtual ~OpenTicklerImplBase();

	virtual void Tickle(SInt64 count) = 0;
	virtual void Tickle(SInt32 phase,SInt64 count); 	//not pure virtual since many existing subclasses
														//do not have this method
														//phase is used to indicate overall progress, count is sub progress
	bool IsCancelled();

protected:
	bool mCancelled;
	SInt64 mPRCount;
};

class OpenTickler
{
public:
	OpenTickler();
	// Note user-defined conversion function (More Effective C++ item 5)
	OpenTickler(OpenTicklerImplBase* impPtr);
	OpenTickler(const OpenTickler&);
	OpenTickler& operator=(const OpenTickler&);

	void Tickle(SInt64 count) const;
	void Tickle(SInt32 phase,SInt64 count) const; //phase is used to indicate overall progress, count is sub progress

	OpenTicklerImplBase* GetImplPointer() const;
	void SetImplPointer(OpenTicklerImplBase* impPtr);

	bool IsCancelled();

private:
	OpenTicklerImplBase* mImpPtr;
};

#endif
