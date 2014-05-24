//-----------------------------------------------------------------------------
// Seconds.cpp
// Copyright (c) 1997 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#include "Seconds.h"
#include "time_utils.h"

//----------------------------------------------------------------------
// Seconds::Seconds typical (and can be default) constructor
//----------------------------------------------------------------------
Seconds::Seconds(double inSeconds, double inEpsilon)
	: FuzzyTime(inSeconds, inEpsilon)
{
}

Seconds::Seconds(const std::string &theString, tc_mode timecodePref)
	: FuzzyTime(TCString2Seconds(theString, timecodePref))
{
}

void Seconds::GetTCString(std::string & theString, tc_mode timecodePref, bool showOddField /*= true*/) const
{
	Seconds2TCString(*this, theString, timecodePref, showOddField);
}

void Seconds::ParseTCString(const std::string &theString, tc_mode timecodePref, bool *pOutSuccess)
{
	*this = TCString2Seconds(theString, timecodePref, pOutSuccess);
}
