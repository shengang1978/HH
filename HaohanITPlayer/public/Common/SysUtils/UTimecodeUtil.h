//-----------------------------------------------------------------------------
// UTimecodeUtil.h
// Copyright (c) 1997 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------
/*
	File:		UTimecodeUtil.h

	Contains:

	Written by:

	Change History (most recent first):

				 x.x.x	spg		Initial file creation.
*/

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040915]: safe input param; no MBCS; no dangerous API; no registry/sys folder/temp file 

#ifndef __UTimecodeUtil_h
#define __UTimecodeUtil_h

#include <string>
 
namespace UTimecodeUtil
{
	double StringToTimecode(const std::string &inString,   bool isNTSC=true, bool dropFrameMode=false, bool useFract=false);
	void TimecodeToString(double timecode, std::string &outString, bool isNTSC=true, bool dropFrameMode=false, bool useFract=false);
}

#endif
