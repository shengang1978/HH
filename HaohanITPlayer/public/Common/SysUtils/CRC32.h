//-----------------------------------------------------------------------------
// CRC32.h
// Copyright (c) 2005, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//	Functions for calculating 32-bit CRC using the following polynomial:
//
//	x^32 + x^26 + x^23 + x^22 + x^16 + x^12 + x^11 + x^10 + x^8 + x^7 + x^5 + x^4 + x^2 + x + 1
//
//	This polynomial is used for PNG and HD-DVD archive files.
//
//	This code is derived from sample code in Portable Network Graphics (PNG)
//	Specification (Second Edition) (http://www.w3.org/TR/PNG/#D-CRCAppendix)


#include "CommonTypes.h"


/* Return the CRC of the bytes buf[0..len-1]. */
UInt32 CalculateCRC(UInt8 const *buf, size_t len);


/* Update a running CRC with the bytes buf[0..len-1]--the CRC
should be initialized to all 1's, and the transmitted value
is the 1's complement of the final running CRC (see
implementation of CalculateCRC() routine). */
UInt32 UpdateCRC(UInt32 crc, UInt8 const *buf, size_t len);
