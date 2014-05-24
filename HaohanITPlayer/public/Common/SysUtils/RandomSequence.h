//-----------------------------------------------------------------------------
// RandomSequence.h
// Copyright (c) 2002 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#ifndef __RandomSequence__
#define __RandomSequence__

#include "CommonTypes.h"

class RandomSequence
{
public:
// **CodeWizzard** - Informational:  More Effective C++ item 5  - Be wary of user-defined conversion function
	RandomSequence(UInt32 length);
	~RandomSequence();

	UInt32 GetFirst();
	UInt32 GetNext(UInt32 n);

	UInt32 GetLength()
				{ return(mLength); }

protected:
	UInt32 FindMask(UInt32 length);

protected:
	UInt32	mLength;
	UInt32	mMask;
};

#endif
