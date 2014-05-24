//-----------------------------------------------------------------------------
// log2er.h
// Copyright (c) 2002 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#ifndef _LOG2ER_
#define _LOG2ER_

template<UInt32 n>
struct log2er
{
};

template<>
struct log2er<2>
{
	enum
	{
		value = 1
	};
};

template<>
struct log2er<4>
{
	enum
	{
		value = 2
	};
};

template<>
struct log2er<8>
{
	enum
	{
		value = 3
	};
};

template<>
struct log2er<16>
{
	enum
	{
		value = 4
	};
};

template<>
struct log2er<32>
{
	enum
	{
		value = 5
	};
};

template<>
struct log2er<64>
{
	enum
	{
		value = 6
	};
};

template<>
struct log2er<128>
{
	enum
	{
		value = 7
	};
};

template<>
struct log2er<256>
{
	enum
	{
		value = 8
	};
};

#endif
