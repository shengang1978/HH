//-----------------------------------------------------------------------------
// FieldDoubleConversion.h
// Copyright (c) 1999 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040915]: safe input param; no MBCS; no dangerous API; no registry/sys folder/temp file 

#ifndef __FIELDDOUBLECONVERSION__
#define __FIELDDOUBLECONVERSION__

#include <cmath>
#include "video_const.h"

inline
double FieldsToDouble(UInt32 fields, eVideoRate rate)
{
	if		(rate == eVideoRate525_60) 	return fields * videoFieldRate525_60;
	else if	(rate == eVideoRate625_50) 	return fields * videoFieldRate625_50;
	return 0;
}

inline
UInt32 DoubleToFields(double time, eVideoRate rate)
{
	if		(rate == eVideoRate525_60) 	return static_cast<UInt32>( std::floor((time / videoFieldRate525_60) + .5) );
	else if	(rate == eVideoRate625_50)	return static_cast<UInt32>( std::floor((time / videoFieldRate625_50) + .5) );
	return 0;
}

#endif
