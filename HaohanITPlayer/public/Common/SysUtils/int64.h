//-----------------------------------------------------------------------------
// int64.h
// Copyright (c) 1994 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW  20040912] - scanhexa added to risky list
//SR FS: Reviewed  [wwt 20040915] - tohexa is partial safe
//SR FS Reviewed [JAW 20040924] - Added comments for risky routines

#if !defined (__INT64__) | !defined (__BYTE_POS__)
#define __INT64__
#define __BYTE_POS__

#include "CommonTypes.h"

inline void intDiv(
	SInt64 value,
	UInt32 blockSize,	// Must be power of 2
	UInt32 *blockp,		// = bp / blocksize
	SInt32 *offsetp				// = bp % blocksize
){ *blockp = (SInt32)(value/(SInt64)blockSize); *offsetp = (SInt32)(value%(SInt64)blockSize); };

inline void intDivRoundUp(
	SInt64 value,
	UInt32 blockSize,	// Must be power of 2
	UInt32 *blockp		// = ceiling(bp / blocksize)
){
	SInt32 offset;	
	intDiv(value+(blockSize-1), blockSize, blockp, &offset);
};

inline void intMul(
	SInt64 value,
	UInt32 blockSize,	// Must be power of 2
	UInt32 block,
	SInt32 offset
){ value = blockSize*block + offset; };

#endif


