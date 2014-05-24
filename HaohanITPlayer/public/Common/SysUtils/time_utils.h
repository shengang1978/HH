//-----------------------------------------------------------------------------------
// time_utils.h
// Copyright (c) 1997 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------------

/*-----------------------------------------------------------------------------------
    Name        : time-code-utils.h

    Description : Header file containing class definition(s) for the following class(es):
                  TBCBorderView.

    Author      : Richard Kimberly
    Date        : Mon, March 3, 1997

    Revised By  : wrb 

-----------------------------------------------------------------------------------*/


//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#ifndef __TIMECODE_UTILS_H__
#define __TIMECODE_UTILS_H__

#include "CommonTypes.h"
#include "time_const.h"
#include "video_const.h"
#include <string>
#include <cmath>

class Seconds;
class SonicText;


namespace TimecodeText
{
	extern const SonicText invalidMode;
	extern const SonicText invalidDropFrameNumber;
	extern const SonicText invalidFrameNumber;
	extern const SonicText invalidString;
	extern const SonicText outOfRange;
	extern const SonicText invalidSampleRate;
	extern const SonicText invalidVideoRate;
}


// TCStringParser is a class which will parse a timecode string assumed to be of the form:
//
//		         FF
//		      SS:FF		or		  SS;FF
//		   MM:SS:FF		or     MM;SS;FF
//		HH:MM:SS:FF		or	HH;MM;SS;FF
//
//  - Any of the forms may append either ".ss" or "*" but not both.
//		".ss" indicates 0-79 subframe units
//		"*" asterisk indicates that the string refers to field 2
//
//  - All forms can substitute the empty string for the number 0.
//		(e.g. One hour and 4 subframes may be represented as:  "1:::.4")
//
//	- String may begin with a minus sign '-' to indicate a negative value
//
//	- Strings may not mix colons and semi-colons.
//
// The timecode string must be consistent with the specified mode.
// The following errors are thrown if the TC string is badly formed or the mode is not valid:
//
//		-4800	// not a mode of smpte time code
//		-4801	// not Drop Frame
//		-4802	// frame values not permitted by indicated format
//		-4804	// bogus syntax in TC string
//		-4805	// bogus number in TC string
//

class TCStringParser 
{
public:
	TCStringParser(const std::string& TCString, tc_mode tcMode, bool validate = true);
	virtual ~TCStringParser();

	tc_mode TCMode() const				{ return mTCMode; }
	SInt32	Hours()	const				{ return mHours; }
	SInt32	Minutes() const				{ return mMinutes; }
	SInt32	Seconds() const				{ return mSeconds; }
	SInt32	Frames() const				{ return mFrames; }
	SInt32	Subframes() const			{ return mSubframes; }
	bool	IsFieldTwo() const			{ return mFieldTwoFlag; }
	bool	IsNegative() const			{ return mIsNegative; }

private:
	tc_mode	mTCMode;
	SInt32	mHours;
	SInt32	mMinutes;
	SInt32	mSeconds;
	SInt32	mFrames;
	SInt32	mSubframes;
	bool	mFieldTwoFlag;
	bool	mIsNegative;
};


/*
 * TCStringToFloat:
 * 		Converts a Time Code String of the Format HH:MM:SS:FF.sss into
 * 			a Floating Point result in seconds, where HH are hours, MM
 *			are minutes, SS are seconds and FF are frames.  An optional
 * 			subframe field sss may be present, delineated by a period '.'
 */
extern double TCStringToFloat(const std::string& tcString, tc_mode type, double samplingRate);
extern double FFtoFloat(const std::string& str, tc_mode mode);
extern double SecsToFloat(const std::string& str);
extern double SamplesToFloat(const std::string& str, double samplingRate);
extern double TCToFloat(const std::string& str, tc_mode mode);
extern double TCToFloat(const TCStringParser& tc, tc_mode mode);
extern double TimecodeStringToFrames(const std::string& str, tc_mode);

extern double TCFramesPerSecond(tc_mode mode);
extern double TCFramesPerFoot(tc_mode mode);
extern double TCOneSecond(tc_mode mode);

extern SInt32 FloatToTCString(double value, std::string & str, tc_mode mode, bool fraction, 
							  double samplingRate, bool inShowHours = false);

extern SInt32 FloatToTC(double value, std::string & str, tc_mode mode, bool fraction, bool inShowHours = false);

// blj 9/6/99 - add optional 'inShowHours' to force showing hours even on CD times for SonicHD
extern SInt32 subFrameFloatToTC(double value, std::string & str, tc_mode mode, bool fraction, double subFrameCount, bool subFrameAsterisk, bool inShowHours = false);

extern void FloatToSecs(double value, std::string & str);
extern SInt32 FloatToFF(double value, std::string & str, tc_mode mode, bool fraction);

// client passes in the time code he wants to use.
void Seconds2TCString(const Seconds & theValue, std::string & theString, tc_mode timeCodePref, bool showOddField = true);

// client passes in the time code he wants to use.
Seconds TCString2Seconds(const std::string & inString, tc_mode timeCodePref, bool * pOutSuccess = 0);

extern tc_mode TCGlobalMode(tc_mode mode);
extern UInt64 GetMilliSeconds();

// Conversions to hours, minutes, seconds, frames
extern void TCStringToHMSF(const std::string& tcString, tc_mode tcMode, SInt32& hours, SInt32& minutes, SInt32& seconds, SInt32& frames, bool& oddField);
extern void SecondsToHMSF(const Seconds& time, tc_mode tcMode, SInt32& hours, SInt32& minutes, SInt32& seconds, SInt32& frames, bool& isFieldTwo);
extern void HMSFToTCString(std::string& tcString, tc_mode tcMode, const SInt32& hours, const SInt32& minutes, const SInt32& seconds, const SInt32& frames, const bool& oddField);
extern void HMSFToSeconds(Seconds& time, tc_mode tcMode, const SInt32& hours, const SInt32& minutes, const SInt32& seconds, const SInt32& frames, const bool& isFieldTwo);

// Parse a timestamp string and return the time value and timecode mode.  e.g.  "29.97 NDF\n01:00:00:00"
extern SInt32 parse_timestamp_string(const std::string & stamp_str, double* time_stamp, tc_mode* timecode_mode);

// Make a timestamp string from a time value and timecode mode.
extern SInt32 get_timestamp_string(double value, tc_mode mode, std::string & stamp_str);

SInt32 TickCountSixtiethSec();	//good old TickCount style here

eVideoRate TCModeToVideoRate(tc_mode tcMode);
tc_mode VideoRateToTCMode(eVideoRate videoRate);
double VideoRateToFrameDuration(eVideoRate videoRate);
double VideoRateToFieldDuration(eVideoRate videoRate);

// This function rounds the frameRate passed in to 2 decimal places
// and maps that to eVideoRate.  If the function does not find a video rate 
// it returns eVideoRateUnknown. 
eVideoRate FindNearestVideoRate(const Seconds& frameRate);

#ifdef _WIN32
#define TickCount GetTickCount
//#pragma message(">>>WIN32 - kludge");
//void Delay(UInt32 numTicks, UInt32 *finalTicks);
#endif

#ifdef	__unix__

#define TickCount	GetTickCount

extern SInt32 GetTickCount();

#endif	// __unix__


#ifndef STRIP_FOR_LONGHORN
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Deprecated functions.  DO NOT USE

extern SInt32 TCStringToFloat(double* value, const unsigned char * str, tc_mode type, double samplingRate);
extern SInt32 SamplesToFloat(double* value, const char* str, double samplingRate);
extern SInt32 CTCStringToFloat(double* value, const char* timeStr, tc_mode mode, double samplingRate);
extern SInt32 TCToFloat(double* value, const char* str, tc_mode mode);
extern double TimecodeStringToFrames(const char* str, tc_mode);

// blj 9/6/99 - add optional 'inShowHours' to force showing hours even on CD times for SonicHD
extern	SInt32 FloatToTCString(double value, unsigned char * str, size_t strlength, tc_mode mode, bool fraction, 
							   double samplingRate, bool inShowHours = false);

// blj 9/6/99 - add optional 'inShowHours' to force showing hours even on CD times for SonicHD
extern SInt32 FloatToTC(double value, char* str, size_t strlength, tc_mode mode, bool fraction, bool inShowHours = false);

// blj 9/6/99 - add optional 'inShowHours' to force showing hours even on CD times for SonicHD
extern SInt32 subFrameFloatToTC(double value, char* str, size_t strlength, tc_mode mode, bool fraction, double subFrameCount, bool subFrameAsterisk, bool inShowHours = false);

extern void FloatToSecs(double value, char *str, size_t strlength, bool fraction);
extern SInt32 FloatToFF(double value, char* str, size_t strlength, tc_mode mode, bool fraction);
extern SInt32 KindOfTimeCode(const char *str, tc_mode sysMode);

//client passes in the time code he wants to use.
void Seconds2TCString(const Seconds &theValue, char* theString, size_t strlength, tc_mode timeCodePref, bool showOddField = true);

// Parse a timestamp string and return the time value and timecode mode.  e.g.  "29.97 NDF\n01:00:00:00"
extern SInt32 parse_timestamp_string(const char* stamp_str, double* time_stamp, tc_mode* timecode_mode);

// Make a timestamp string from a time value and timecode mode.
extern SInt32 get_timestamp_string(double value, tc_mode mode, char* stamp_str, size_t stamp_str_length);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif


#endif	// __TIMECODE_UTILS_H__
