//-----------------------------------------------------------------------------
// DirtyFlag.h
// Copyright (c) 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//	Multi-client dirty flag.

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#include "DirtyFlag.h"
#include "DVDErrors.h"
#include "SonicException.h"


DirtyFlag::DirtyFlag()
	: mDirtyBits(0), mAllocatedBits(0)
{
}


DirtyFlag::~DirtyFlag()
{
}


bool DirtyFlag::IsDirty(DirtyFlagClient const &client) const
{
	return((mDirtyBits & client.bit) != 0);
}

void DirtyFlag::MarkDirty()
{
	mDirtyBits = mAllocatedBits;
}

void DirtyFlag::MarkClean(DirtyFlagClient const &client)
{
	mDirtyBits &= !client.bit;
}

void DirtyFlag::MarkAllClean()
{
	mDirtyBits = 0;
}

DirtyFlag::FlagBits DirtyFlag::AllocateBit(UInt32 preferredBitNumber)
{
	if (preferredBitNumber > kMaxBitNumber)
		throw SonicException(DVDError::invalidParam);

	FlagBits bit = (1 << preferredBitNumber);
	if ((mAllocatedBits & bit) != 0)
	{
		bit = 1;
		while (bit != 0 && (mAllocatedBits & bit) != 0)
			bit <<= 1;

		if (bit == 0)
			throw SonicException(DVDError::invalidParam);
	}

	mAllocatedBits |= bit;
	return(bit);
}

DirtyFlagClient::DirtyFlagClient(DirtyFlag &flag, UInt32 preferredID)
{
	bit = flag.AllocateBit(preferredID);
}

DirtyFlagClient::~DirtyFlagClient()
{
}

