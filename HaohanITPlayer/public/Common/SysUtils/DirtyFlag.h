//-----------------------------------------------------------------------------
// DirtyFlag.h
// Copyright (c) 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//	Multi-client dirty flag.

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 


#ifndef __DIRTYFLAG__
#define __DIRTYFLAG__

#include "CommonTypes.h"

class DirtyFlagClient;

class DirtyFlag {
public:
	DirtyFlag();
	~DirtyFlag();

	//	Returns true if flag is dirty for this client
	bool IsDirty(DirtyFlagClient const &client) const;

	//	Marks flag dirty for all clients
	void MarkDirty();
	//	Marks flag clean for this client
	void MarkClean(DirtyFlagClient const &client);
	//	Marks flag clean for all clients
	void MarkAllClean();

protected:
	friend class DirtyFlagClient;
	
	typedef UInt32	FlagBits;
	static const UInt32 kMaxBitNumber = 31;

	DirtyFlag::FlagBits AllocateBit(UInt32 preferredBitNumber);

	FlagBits	mDirtyBits;
	FlagBits	mAllocatedBits;
};


class DirtyFlagClient {
public:
	DirtyFlagClient(DirtyFlag &flag, UInt32 preferredID = 0);
	~DirtyFlagClient();

protected:
	friend class DirtyFlag;

	DirtyFlag::FlagBits	bit;
};


#endif
