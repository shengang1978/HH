//-----------------------------------------------------------------------------
// GetDateTimeString.cpp
// Copyright (c) 1998 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040915]: safe input param; no MBCS; no dangerous API; no registry/sys folder/temp file 

#include "GetDateTimeString.h"
#include "time_utils.h"
#include "sonic_crt.h"

#include <cstdio>
#include <ctime>

std::string GetDateTimeString()
{
	time_t now = time(0);
	char buffer[64];
	return sonic::ctime_safe(&now, buffer, sizeof(buffer));
}
