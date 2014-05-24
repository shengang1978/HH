//-----------------------------------------------------------------------------
// ieee80.cpp
// Copyright (c) 2002 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS Needs Review [JAW  20040912] - Add double_to_extended80  to list of risky routines due to use of store2 and store8
//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#include "ieee80.h"
#include "ByteSwapper.h"

void 
ieee80::double_to_extended80(unsigned char *outRate, double inRate) 
{
	UInt16 expn;
	SInt64 onebit;
	SInt64 fraction;

	if (inRate>=32000.0 && inRate < 64000.0)
		expn = 0xe;
	else if (inRate < 128000.0)
		expn = 0xf;
	else
		expn = 0x10;

	onebit = (static_cast<SInt64>(1)) << (63 - (expn + 2));
	fraction = static_cast<SInt64> (inRate * (static_cast<double>(onebit)));
	fraction <<= 1;
	expn += 0x4000;
	store2(reinterpret_cast<unsigned char *>(&outRate[0]), expn);
	store8(reinterpret_cast<unsigned char *>(&outRate[2]) - 2, static_cast<UInt64>(fraction));
}

void 
ieee80::extended80_to_double(double *outRate, const unsigned char* inRate) 
{
	UInt16 expn = load2(inRate);
	UInt64 mant = load8(&inRate[2]);
	SInt32 answer;
	SInt64 fraction;
	SInt64 bit48 = (static_cast<SInt64>(1)) << 48;

	expn -= 0x4000;
	answer = static_cast<SInt32>(mant >> (64 - (expn + 2)));
	fraction = mant >> (64 - (expn + 2 + 48));		// 48 bits of fraction
	fraction &= (bit48 - 1);		// Mask off integer part
	*outRate = static_cast<double>(answer) + (static_cast<double>(fraction)) / (static_cast<double>(bit48));
}
