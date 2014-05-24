//-----------------------------------------------------------------------------
// time_conversion.h
// Copyright (c) 1997 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#ifndef __TIME_CONVERSION__
#define __TIME_CONVERSION__

#include <ctime>

SInt32 time_tToAFPTime(std::time_t udfTime);
std::time_t AFPTimeTotime_t(SInt32 afpTime);

std::string makeDateString(SInt32 afp_time);
std::string makeUDFHexDateString(SInt32 afp_time);

SInt32 GetAFP_time();

#endif

