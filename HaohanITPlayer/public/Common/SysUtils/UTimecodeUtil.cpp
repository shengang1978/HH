//-----------------------------------------------------------------------------
// UTimecodeUtil.cpp
// Copyright (c) 1997 - 2005, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------
/*
	File:		UTimecodeUtil.cp

	Contains:

	Written by:

	Change History (most recent first):

				 x.x.x	spg		Initial file creation.
*/

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040915]: safe input param; MBCS internally; no dangerous API; no registry/sys folder/temp file 

#include "UTimecodeUtil.h"
#include "StringUtilities.h"
#include "time_utils.h"
#include "sonic_crt.h"

typedef enum TimeStates
{
	kNoTime = 0,
	kFractTime = 1,
	kFrameTime = 2,
	kSecondsTime = 3,
	kMinutesTime = 4,
	kHoursTime = 5,
	kInvalidTime = 6
} TimeStates;


static char SeparatorChar(bool, bool dropFrameMode)
{
	return (dropFrameMode ? ';' : ':');
}

static void BindRange(ptrdiff_t &theValue, SInt32 minVal, SInt32 maxVal)
{
	if (theValue > maxVal)
		theValue = maxVal;

	if (theValue < minVal)
		theValue = minVal;
}

static void ValidateTimecodeString(std::string& inOutString, bool isNTSC, bool dropFrameMode, bool useFract)
{
	const char sepCharColon = ':';
	const char sepCharSemiCln = ';';
	const char sepCharFract = '.';

	SInt32		timeValues[6] = {0,0,0,0,0,0};	// Array of time values, [unused, frame, seconds, minutes, hours]
	SInt32 		tcLen = static_cast<SInt32>(inOutString.length());		// Length of the current time code string

	if (tcLen > 0) {
		// If there is nothing entered, we assume the user wants to leave it blank

		std::string		fieldStr;						// String containing current field
		SInt32		timeState = kHoursTime;			// Time state of the current field
		SInt32		breakPos = 1;		// End position of the current field
		SInt32		fieldStart = 0;					// Start position of the current field

		const ptrdiff_t foundColon = inOutString.find(sepCharColon);
		const ptrdiff_t foundSemi = inOutString.find(sepCharSemiCln);
		const ptrdiff_t foundFract = inOutString.find(sepCharFract);
		const ptrdiff_t n = std::string::npos;

		bool hasHardBreaks = (foundColon > n) || (foundSemi > n) || (foundFract > n);

		if (hasHardBreaks)
			breakPos = 0;

		// First parse the existing string
		while (timeState) {

			if (hasHardBreaks) {
				// Set the end of the current field to the next colon or semi-colon
				std::string::size_type tempBreakPos = inOutString.find(sepCharColon, breakPos + 1);
				if (tempBreakPos == std::string::npos)
					tempBreakPos = inOutString.find(sepCharSemiCln, breakPos + 1);
				if (tempBreakPos == std::string::npos)
					tempBreakPos = inOutString.find(sepCharFract, breakPos + 1);
				if (tempBreakPos == std::string::npos && kFractTime == timeState) {
					--timeState;
					continue;	//	no field exists for frame fraction
				}
				breakPos = static_cast<SInt32>(tempBreakPos);
			}
			else			// Set the end of the current field to the next 2 digits
				breakPos += 2;

			if (breakPos <= 0 || breakPos > tcLen)
				breakPos = tcLen + 1;

			// Set the field string to the next field, and convert it to a number
			fieldStr.assign(inOutString, fieldStart,  breakPos - fieldStart);
			timeValues[timeState] = StringUtils::StringToInt(fieldStr);

			// Update loop variables
			timeState = timeState - 1;
			fieldStart = breakPos;
			if (hasHardBreaks) fieldStart++;
		}

		// Set within limits
		SInt32	maxFrames = 24;		// Pal frames per second
		if (isNTSC)
			maxFrames = 29;		// NTSC frames per second

		BindRange(reinterpret_cast<ptrdiff_t &>(timeValues[kHoursTime]), 0, 23);
		BindRange(reinterpret_cast<ptrdiff_t &>(timeValues[kMinutesTime]), 0, 59);
		BindRange(reinterpret_cast<ptrdiff_t &>(timeValues[kSecondsTime]), 0, 59);
		BindRange(reinterpret_cast<ptrdiff_t &>(timeValues[kFrameTime]), 0, maxFrames);
		BindRange(reinterpret_cast<ptrdiff_t &>(timeValues[kFractTime]), 0, 79);

		if (dropFrameMode) {
			// Drop frame algorithm is:
			// Every minute, drop frames 0 and 1,
			// Except every tenth minute, when you keep them

			if ((timeValues[kFrameTime] == 0 || timeValues[kFrameTime] == 1) &&
				(timeValues[kSecondsTime] == 0) &&
				(timeValues[kMinutesTime] % 10 != 0)) {

					timeValues[kFrameTime] = 2;
				}
		}

		// And reconsitute the string
		char sep = SeparatorChar(isNTSC,dropFrameMode);
		char	tcCString[30];

		if (useFract) 
			sonic::snprintf_safe(tcCString,30, "%02ld%c%02ld%c%02ld%c%02ld.%02ld", timeValues[kHoursTime], sep, timeValues[kMinutesTime], sep,
			// **CodeWizzard** - Informational:  Effective C++ item 2  - Prefer iostream.h to stdio.
			timeValues[kSecondsTime], sep, timeValues[kFrameTime], timeValues[kFractTime]);
		else
			sonic::snprintf_safe(tcCString,30, "%02ld%c%02ld%c%02ld%c%02ld", timeValues[kHoursTime], sep, timeValues[kMinutesTime], sep,
			// **CodeWizzard** - Informational:  Effective C++ item 2  - Prefer iostream.h to stdio.
			timeValues[kSecondsTime], sep, timeValues[kFrameTime]);
		inOutString = tcCString;

	}
}

// This is obsolete for HD
static tc_mode GetTCMode(bool isNTSC, bool dropFrameMode)
{
	tc_mode tcMode;
	if (isNTSC) 
		tcMode = (dropFrameMode ? TC_SMPTE_29_DF : TC_SMPTE_29_NDF);
	else
		tcMode = TC_SMPTE_25;

	return tcMode;
}

// -----------------------------------

static double ValidStringToTimecode(
	const std::string &inString,
	bool isNTSC, 
	bool dropFrameMode,
	bool /*useFract*/)
{
	double samplingRate = 0.0;	// not used for our purposes

	tc_mode tcMode = GetTCMode(isNTSC, dropFrameMode);

	return TCStringToFloat(inString, tcMode, samplingRate);
}

// -----------------------------------
double UTimecodeUtil::StringToTimecode(
	const std::string &inString,
	bool isNTSC, 
	bool dropFrameMode,
	bool useFract)
{
	std::string validString = inString;
	ValidateTimecodeString(validString,isNTSC,dropFrameMode,useFract);
	return ValidStringToTimecode(validString,isNTSC,dropFrameMode,useFract);
}

// -----------------------------------
void UTimecodeUtil::TimecodeToString(
	double inTimecode, 
	std::string &outString, 
	bool isNTSC, 
	bool dropFrameMode, 
	bool useFract)
{
	double samplingRate = 0.0;	// not used for our purposes

	tc_mode tcMode = GetTCMode(isNTSC, dropFrameMode);
	::FloatToTCString(inTimecode, outString, tcMode, useFract, samplingRate);
}
