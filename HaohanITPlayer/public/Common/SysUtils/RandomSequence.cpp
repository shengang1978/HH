//-----------------------------------------------------------------------------
// RandomSequence.cpp
// Copyright (c) 2002 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#include "RandomSequence.h"


//	A pseudo-random sequence generator that uniquely generates the numbers between 0 and N-1.
//
//	Taken from Graphics Gems (Glassner, 1990) article 'A digital "dissolve" effect'.  The 
//	base algorithm generates a non-repeating sequence of numbers from 1-N if N is a power of 2.


//	Mask values
static UInt32 gMaskTable[32] = {
	0x00000003,			// 2^2 (generates 1-3)
	0x00000006,			// 2^3 (generates 1-7)
	0x0000000C,			// 2^4 (and so on...)
	0x00000014,			// 2^5
	0x00000030,			// 2^6
	0x00000060,			// 2^7
	0x000000B8,			// 2^8
	0x00000110,			// 2^9
	0x00000240,			// 2^10
	0x00000500,			// 2^11
	0x00000CA0,			// 2^12
	0x00001B00,			// 2^13
	0x00003500,			// 2^14
	0x00006000,			// 2^15
	0x0000B400,			// 2^16
	0x00012000,			// 2^17
	0x00020400,			// 2^18
	0x00072000,			// 2^19
	0x00090000,			// 2^20
	0x00140000,			// 2^21
	0x00300000,			// 2^22
	0x00400000,			// 2^23
	0x00D80000,			// 2^24
	0x01200000,			// 2^25
	0x03880000,			// 2^26
	0x07200000,			// 2^27
	0x09000000,			// 2^28
	0x14000000,			// 2^29
	0x32800000,			// 2^30
	0x48000000,			// 2^31
	0xA3000000,			// 2^32
};



RandomSequence::RandomSequence(UInt32 length)
	: mLength(length), mMask(0)
{
	mMask = FindMask(length);
}


RandomSequence::~RandomSequence()
{
}


UInt32 RandomSequence::GetFirst()
{
	return(rand() % mLength);		// pick any random number in the sequence as the start point
}


UInt32 RandomSequence::GetNext(UInt32 n)
{
	n += 1;		// add one because algorithm generates 1-N

	do {
		if ((n & 1) != 0)
			n = (n >> 1) ^ mMask;
		else
			n = (n >> 1);
	} while (n > mLength);

	n -= 1;		// remove the one we added in the beginning

	return(n);
}


UInt32 RandomSequence::FindMask(UInt32 length)
{
	UInt32 max = 4;
	UInt32 i = 0;
	while (max <= length && i < 31) {
		++i;
		max *= 2;
	}
	return(gMaskTable[i]);
}

