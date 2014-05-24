//-----------------------------------------------------------------------------
// time_utils.cpp
// Copyright (c) 1997 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS Needs Review [JAW 20040910] - Check use of sprintf to char buffers - Add FloatToTCString, Seconds2TCString, get_timestamp_string, FloatToFF to risky list
//SR FS Needs Review [JAW 20040910] - Check use of sprintf to char buffers - Add subFrameFloatToTC, FloatToSecs to risky list
//SR FS Reviewed [JAW 20040924] - Advise against use of sprintf
//SR FS Reviewed [wwt 20040914] 

#include "time_utils.h"

#if !defined (STRIP_FOR_LONGHORN) && !defined (OSXPORT) 
  #include "pstring.h"
#endif

#include "safe_ptr.h"
#include "Seconds.h"
#include "MathMacros.h"
#include "SonicException.h"
#include "SonicTextBase.h"
#include "UnicodeUtilities.h"
#include "sonic_crt.h"

#if defined (WIN32)
#pragma warning(push)
#pragma warning(disable:4201)
#include <mmsystem.h>
#pragma warning(pop)
#endif

#include <ctime>
#if defined __linux__
#include <sys/time.h>
#endif
#include <iostream>
#include <sstream>
#include <cassert>
#include <ctime>
#include <algorithm>


const SonicText TimecodeText::invalidMode				(SonicTextBase::TimecodeText - 0, "invalid timecode mode");
const SonicText TimecodeText::invalidDropFrameNumber	(SonicTextBase::TimecodeText - 1, "invalid frame number for dropframe mode");
const SonicText TimecodeText::invalidFrameNumber		(SonicTextBase::TimecodeText - 2, "invalid frame number");
const SonicText TimecodeText::invalidString				(SonicTextBase::TimecodeText - 4, "invalid timecode string");
const SonicText TimecodeText::outOfRange				(SonicTextBase::TimecodeText - 5, "timecode value out of range");
const SonicText TimecodeText::invalidSampleRate			(SonicTextBase::TimecodeText - 7, "invalid sampling rate");
const SonicText TimecodeText::invalidVideoRate			(SonicTextBase::TimecodeText - 8, "invalid video rate");

const char		kNegativeTimeSign	=	0x2d;
const double	kSubframeCount		=	80.0;


static double RoundToNearestFrame(double time)
{
	SInt32 i = static_cast<SInt32>((((time * 30.0) + 0.5)));
	double rTime = static_cast<double>(i) / 30.0;
	return rTime;
}

static double FramesPerSecondToFramesPerSecondCounted(tc_mode mode, double framesPerSecond)
{
	double framesPerSecondCounted;
	switch (mode)
	{
	case TC_SMPTE_29_NDF:
		framesPerSecondCounted = NDF_FRAMESSECOND;
		break;
	case TC_SMPTE_23976:
		framesPerSecondCounted = F24_FRAMESSECOND;
		break;
	case TC_SMPTE_5994:
		framesPerSecondCounted = F60_FRAMESSECOND;
		break;
	default:
		framesPerSecondCounted = framesPerSecond;
		break;
	}

	return framesPerSecondCounted;
}

TCStringParser::TCStringParser(const std::string& TCString, tc_mode tcMode, bool validate)
:	mTCMode(tcMode),
	mHours(0),
	mMinutes(0),
	mSeconds(0),
	mFrames(0),
	mSubframes(0),
	mFieldTwoFlag(false),
	mIsNegative(false)
{
	if (	tcMode != TC_UNKNOWN
		&&	tcMode != TC_SMPTE_30_NDF   /* only test consistency for the time code types */
		&& 	tcMode != TC_SMPTE_29_DF
		&&	tcMode != TC_SMPTE_CD
		&&	tcMode != TC_DATA_CD
		&&	tcMode != TC_SMPTE_24
		&&  tcMode != TC_SMPTE_23976
		&&	tcMode != TC_SMPTE_25
		&&	tcMode != TC_SMPTE_29_NDF
		&&  tcMode != TC_SMPTE_30_DF
		&&  tcMode != TC_SMPTE_50
		&&  tcMode != TC_SMPTE_5994
		&&  tcMode != TC_SMPTE_50
		&&	tcMode != TC_SMPTE_60
		&&	tcMode != TC_SMPTE_5994_DF)
	{
		throw SonicException(TimecodeText::invalidMode);
	}

	std::string rawTimeStr = TCString;

#ifdef macintosh
	// Replace Macintosh bullet (option-8) with period
	replace(rawTimeStr.begin(), rawTimeStr.end(), '?, '.');
	// Replace Macintosh non-breaking space character (option-space)with normal space
	replace(rawTimeStr.begin(), rawTimeStr.end(), '?, ' ');
	// Replace Macintosh em-dash character (shift-option-dash) with normal hyphen
	replace(rawTimeStr.begin(), rawTimeStr.end(), '?, '-');
#endif

	// Look for minus sign at front of stream and strip it off
	std::string::size_type minusPos = rawTimeStr.find('-');
	mIsNegative = minusPos != std::string::npos;
	if (mIsNegative)
		minusPos++; // start after minus sign
	else
		minusPos = 0; // reset pos to start of string

	std::string str = rawTimeStr.substr(minusPos);


	size_t colons = 0;
	size_t semicolons = 0;
	std::string::size_type periodPos = std::string::npos;
	std::string::size_type asteriskPos = std::string::npos;

	// Scan the string and count non-numeric characters
	// Verify that we don't have mixed colons and semi-colons
	//	and that no colons/semicolons follow a period.
	for (std::string::iterator i = str.begin(); i != str.end(); ++i) {
		switch (*i) {
			case ':':
				colons++;
				if (	colons > 3
					||	semicolons > 0
					||	periodPos != std::string::npos
					||  asteriskPos != std::string::npos
					)
					throw SonicException(TimecodeText::invalidString);		// Garbled string
				break;

			case ';':
				semicolons++;
				if (	colons > 0
					||	semicolons > 3
					||	periodPos != std::string::npos
					||  asteriskPos != std::string::npos
					)
					throw SonicException(TimecodeText::invalidString);		// Garbled string
				break;

			case '.':
				if (periodPos != std::string::npos || asteriskPos != std::string::npos)
					throw SonicException(TimecodeText::invalidString);		// Garbled string
				periodPos = i - str.begin();
				break;

			case '*':
				if (periodPos != std::string::npos || asteriskPos != std::string::npos)
					throw SonicException(TimecodeText::invalidString);		// Garbled string
				asteriskPos = i - str.begin();
				break;

			case ' ':
				break;

			default:
				// Make sure the character is a number
				if (!isdigit(static_cast<unsigned char>(*i)))
					throw SonicException(TimecodeText::invalidString);
				break;
		}
	}

	// Now convert any semicolons to colons
	if (semicolons > 0) {
		colons = semicolons;
		replace(str.begin(), str.end(), ';', ':');
	}
	char colon = 0;
	std::istringstream ss(str);

	if (colons > 2) {	// Hours
		ss >> mHours;
		ss.clear(); // Clear error in case there wasn't a number
		ss >> colon;
		if (colon != ':')
			throw SonicException(TimecodeText::invalidString);
	}

	if (colons > 1) {	// Minutes
		ss >> mMinutes;
		ss.clear(); // Clear error in case there wasn't a number
		colon = 0;
		ss >> colon;
		if (colon != ':')
			throw SonicException(TimecodeText::invalidString);
	}

	if (colons > 0) {	// Seconds
		ss >> mSeconds;
		ss.clear(); // Clear error in case there wasn't a number
		colon = 0;
		ss >> colon;
		if (colon != ':')
			throw SonicException(TimecodeText::invalidString);
	}

	// Frames
	ss >> mFrames;
	ss.clear(); // Clear error in case there wasn't a number

	if (asteriskPos != std::string::npos)
		mFieldTwoFlag = true;

	if (periodPos != std::string::npos) {
		char period = 0;
		ss >> period >> mSubframes;
	}

	if (validate)
	{
		// Now perform a series of tests, prinicpally on the frame number, to 
		//	determine inconsistency with the time code mode

		if (tcMode == TC_SMPTE_29_DF || tcMode == TC_SMPTE_30_DF || tcMode == TC_SMPTE_5994_DF) {
			if (colons >= 2) {
				// There is a minutes value
				if (mMinutes % 10 != 0)	// if we're not on minute 00,10,20...
				{
					if (mSeconds == 0) /* and we're on the first second */
					{
						if (tcMode == TC_SMPTE_5994_DF)
						{
							if (mFrames < 4)
								throw SonicException(TimecodeText::invalidDropFrameNumber);
						}
						else
							if (mFrames < 2)	// Frame number is 0 or 1
								throw SonicException(TimecodeText::invalidDropFrameNumber);
					}
				}
			}
		}

		// Limits for all types
		if (mHours > 23 || mSeconds > 59 || mSubframes > 79)
			throw SonicException(TimecodeText::invalidString);
		if (mMinutes > 59 && !(tcMode == TC_DATA_CD || tcMode == TC_SMPTE_CD))
			throw SonicException(TimecodeText::invalidString);

	// We should never see a frame value greater than indicated 
	//	for each of the smpte types
// **CodeWizzard** - Violation:  Universal Coding Standards item 35  - Always provide a default branch for switch statement
		switch (tcMode) {
			case TC_DATA_CD:
			case TC_SMPTE_CD:
				if (mFrames > 74)
					throw SonicException(TimecodeText::invalidFrameNumber);
				if (mHours > 0 || mMinutes > 99 || mSubframes > 0)
					throw SonicException(TimecodeText::invalidString);
				break;

			case TC_SMPTE_30_NDF:
			case TC_SMPTE_29_DF:
			case TC_SMPTE_29_NDF:
			case TC_SMPTE_30_DF:
				if (mFrames > 29)
					throw SonicException(TimecodeText::invalidFrameNumber);
				break;

			case TC_SMPTE_24:
			case TC_SMPTE_23976:
				if (mFrames > 23)
					throw SonicException(TimecodeText::invalidFrameNumber);
				break;

			case TC_SMPTE_25:
				if (mFrames > 24)
					throw SonicException(TimecodeText::invalidFrameNumber);
				break;
				
			case TC_SMPTE_50:
				if (mFrames > 49)
					throw SonicException(TimecodeText::invalidFrameNumber);
				break;

			case TC_SMPTE_5994:
			case TC_SMPTE_5994_DF:
			case TC_SMPTE_60:
				if (mFrames > 59)
					throw SonicException(TimecodeText::invalidFrameNumber);
				break;

			default:
				throw SonicException(TimecodeText::invalidFrameNumber);
				break;	
		}
	}
}


TCStringParser::~TCStringParser()
{
}


/*
 * TCStringToFloat:
 * 		Converts a Time Code String of the Format HH:MM:SS:FF.sss into
 * 			a Floating Point result in seconds, where HH are hours, MM
 *			are minutes, SS are seconds and FF are frames.  An optional
 * 			subframe field sss may be present, delineated by a period '.'
 */
double TCStringToFloat(const std::string& inTCString, tc_mode mode, double samplingRate)
{
	// Make a copy so we can tweak it
	std::string rawTimeStr = inTCString;

#ifdef macintosh
	// Replace Macintosh non-breaking space character (option-space)with normal space
	replace(rawTimeStr.begin(), rawTimeStr.end(), '?, ' ');
	// Replace Macintosh em-dash character (shift-option-dash) with normal hyphen
	replace(rawTimeStr.begin(), rawTimeStr.end(), '?, '-');
#endif

	std::string::size_type pos = rawTimeStr.find('-');
	bool minusFlag = pos != std::string::npos;
	if (minusFlag)
		pos++; // start after minus sign
	else
		pos = 0; // reset pos to start of string

	std::string timeString = rawTimeStr.substr(pos);

	double value = 0.0;

	switch (mode) {
		default:
		case TC_UNKNOWN:
		case TC_MEASURE_RATE:
		case TC_BEATS:
		case TC_MEASURES:
		case TC_MINUTES:
		case TC_UNKNOWN_RATE:
// **CodeWizzard** - Violation:  Universal Coding Standards item 34  - Always terminate a case statement with brea
			throw SonicException(TimecodeText::invalidMode);

		case TC_16FF:
		case TC_16MM_24:
		case TC_16MM_25:
		case TC_16MM_29:
		case TC_16MM_30:
		case TC_35FF:
		case TC_35MM_24:
		case TC_35MM_25:
		case TC_35MM_29:
		case TC_35MM_30:
			value = FFtoFloat(timeString, mode);
			break;

		case TC_SAMPLES :
			value = SamplesToFloat(timeString, samplingRate);
			break;

		case TC_SECONDS :
			value = SecsToFloat(timeString);
			break;

		case TC_SMPTE_30_NDF:
		case TC_SMPTE_29_NDF:
		case TC_SMPTE_30_DF:
		case TC_SMPTE_29_DF:
		case TC_SMPTE_CD:
		case TC_DATA_CD:
		case TC_SMPTE_23976:
		case TC_SMPTE_24:
		case TC_SMPTE_25:
		case TC_SMPTE_50:
		case TC_SMPTE_5994:
		case TC_SMPTE_5994_DF:
		case TC_SMPTE_60:
			value = TCToFloat(timeString, mode);
			break;
	}

	/* must be greater than 0.0 and less than MAX */
	if (value < 0.0 || value >= MAXIMUM_TIME_VALUE)
		throw SonicException(TimecodeText::outOfRange);
	if (minusFlag)
		value = -value;

	return value;
}


/*
 * FFtoFloat: convert a string in Feet and Frames into a floating point
 *       	value in seconds equivalent.  The String must be valid feet
 *       	and frames format.	(FFFF:ff.ss) where F are Feet digits
 *       	f are frame digits and sss are subframe digits.
 *       	The Subframe digits are optional.
 */
double FFtoFloat(const std::string& str, tc_mode mode)
{
	std::string ffString(str);
#ifdef macintosh
	// Allow a Macintosh bullet character (option-8) to be used for the subframe delimiter
	replace(ffString.begin(), ffString.end(), '?, '.');
#endif

	std::istringstream ss(ffString);
	SInt32 feet = 0;
	SInt32 frames = 0;
	SInt32 subframes = 0;

	ss >> feet;
	ss.clear(); // Clear error in case there wasn't a number
	if (ss.str().find(':') != std::string::npos) {
		char colon = 0;
		ss >> colon	>> frames;
		ss.clear(); // Clear error in case there wasn't a number

		if (ss.str().find('.') != std::string::npos) {

			char period = 0;
			ss >> period >> subframes;
		}
	}

	double FramesPerFoot = TCFramesPerFoot(mode);  /* Different for 16mm film from 35mm film */
	double FramesPerSecond = TCFramesPerSecond(mode);

	/* feet */
	double totalFrames = feet * FramesPerFoot;
	totalFrames += frames;

	/* sub-frames */
	totalFrames += static_cast<double>(subframes) / kSubframeCount;
	// Return value in seconds
	return totalFrames / FramesPerSecond;
}


/************************************************************************/
/* SecsToFloat: convert from a string to a floating point number.		*/
/************************************************************************/
double SecsToFloat(const std::string& str)
{
	double value = 0.0; // Initialize to 0 in case there's nothing to parse in the string

	std::istringstream iss(str);
	iss >> value;

	return value;
}


/*
 * SamplesToFloat: convert from samples to a floating point number.
 */
double SamplesToFloat(const std::string& str, double samplingRate)
{
	double value = SecsToFloat(str);

	// convert seconds to samples
	if (samplingRate <= 0.0)
		throw SonicException(TimecodeText::invalidSampleRate);

	value /= samplingRate;

	return value;
}


/*
 * TCToFloat: convert from specified string to a floating point
 *       	number.  Make sure that the value is not greater
 *       	than the maximum allowed value.
 *
 *       	No white space allowed. '? or '.' must appear before subframes.
 *       	Valid timecode string only, please.
 */
double TCToFloat(const std::string& str, tc_mode mode)
{
	// parses the TC string into fields of minutes, seconds etc as appropriate for the TC format
	// It will throw if the string is invalid
	TCStringParser tc(str, mode);
	return TCToFloat(tc, mode);
}


/*
 * TCToFloat: convert from parsed string to a floating point number.  
 */
double TCToFloat(const TCStringParser& tc, tc_mode mode)
{
	// this is the number of frames which go by in a second
	// of clock or real time;  by default it is set according
	// to the format of the time code.
	double	framesPerSecond = TCFramesPerSecond(mode);

	// here is where we count up the number of frames implied by the time code string
	double	frameCount = 0.0;

	/* we changed the following to allow for different combinations of smpte format (DF/NDF)
	and frame rate.....first we caculate the number of frames implied by the smpte, and
	then we apply to this a frame rate to calculate a number of seconds */

	/*
	* Calculate frames.
	*	1. Proceeding from right to left, multiply each x[i] by its
	*		positional weight.
	*/

	// handle DF seperately
	if (mode == TC_SMPTE_29_DF || mode == TC_SMPTE_30_DF) {
		frameCount = tc.Hours() * DF_1HOUR;

		SInt32 tensOfMinutes = tc.Minutes() / 10;
		frameCount += tensOfMinutes * DF_10MINUTES;

		SInt32 onesOfMinutes = tc.Minutes() % 10;
		if (onesOfMinutes > 0) {
			// there are 1800 frames in the first minute of every 10 minutes block in df
			frameCount += 1800.0; 

			// but 2 frames are dropped at the beginning of each of the other minutes
			for (SInt32 mins = onesOfMinutes - 1; mins > 0; --mins)
				frameCount += 1798.0;
		}

		// seconds
		//
		// there are 28 frames in the first second of each minute
		// unless we're on a 10 minute boundary, in which case 
		// there are 30 frames as well as 30 frames in each
		// subsequent second
		SInt32 seconds = tc.Seconds();
		if (seconds > 0) {
			if (onesOfMinutes > 0) {
				frameCount += 28.0;
				--seconds;
			}
			if (seconds != 0)
				frameCount += seconds * 30.0;
		}

		// frames
		SInt32 frames = tc.Frames();
		if (frames != 0) {
			// if we're on the first second
			// of a minute, but not in the first
			// minute of a 10 minute block, then
			// subtract 2 from the number of frames shown
			if (onesOfMinutes > 0 && tc.Seconds() == 0)
				frames -= 2;
			frameCount += frames;
		}

		// sub-frames
		frameCount += tc.Subframes() / kSubframeCount;
		if (tc.IsFieldTwo())
			frameCount += 0.5;
	}
	else if (mode == TC_SMPTE_5994_DF) {	// drop frame concept extended to the 59.94 fps case
		frameCount = tc.Hours() * DF_5994_1HOUR;

		SInt32 tensOfMinutes = tc.Minutes() / 10;
		frameCount += tensOfMinutes * DF_5994_10MINUTES;

		SInt32 onesOfMinutes = tc.Minutes() % 10;
		if (onesOfMinutes > 0) {
			// there are 3600 frames in the first minute of every 10 minutes block in df
			frameCount += 3600.0; 

			// but 4 frames are dropped at the beginning of each of the other minutes
			for (SInt32 mins = onesOfMinutes - 1; mins > 0; --mins)
				frameCount += 3596.0;
		}

		// seconds
		//
		// there are 56 frames in the first second of each minute
		// unless we're on a 10 minute boundary, in which case 
		// there are 60 frames as well as 60 frames in each
		// subsequent second
		SInt32 seconds = tc.Seconds();
		if (seconds > 0) {
			if (onesOfMinutes > 0) {
				frameCount += 56.0;
				--seconds;
			}
			if (seconds != 0)
				frameCount += seconds * 60.0;
		}

		// frames
		SInt32 frames = tc.Frames();
		if (frames != 0) {
			// if we're on the first second
			// of a minute, but not in the first
			// minute of a 10 minute block, then
			// subtract 4 from the number of frames shown
			if (onesOfMinutes > 0 && tc.Seconds() == 0)
				frames -= 4;
			frameCount += frames;
		}

		// sub-frames
		frameCount += tc.Subframes() / kSubframeCount;
		if (tc.IsFieldTwo())
			frameCount += 0.5;
	}
	else
	{
		// this is the number of frames that a particular
		// time code format implies will go by in a "second" of
		// time .... it's not used for DF because of all the 
		// special cases there
		double framesPerSecondCounted = FramesPerSecondToFramesPerSecondCounted(mode, framesPerSecond);

		// subframes, if any
		if (tc.IsFieldTwo())
			frameCount = 0.5;
		else
			frameCount = tc.Subframes() / kSubframeCount;

		// frames, if any
		frameCount += tc.Frames();

		// seconds, if any
		frameCount += tc.Seconds() * framesPerSecondCounted;

		// minutes, if any
		frameCount += tc.Minutes() * 60 * framesPerSecondCounted;

		// hours, if any
		frameCount += tc.Hours() * 3600 * framesPerSecondCounted;

	}

// **Prefix** - time_utils.cpp; Line: 540; warning (20): dividing by zero using 'framesPerSecond'
	if (tc.IsNegative())
		return -frameCount / framesPerSecond;

	return frameCount / framesPerSecond;
}


double TimecodeStringToFrames(const std::string& str, tc_mode mode)
{
	std::string localStr = str;

	TCStringParser tc(localStr, mode);

	// Only these two modes are implemented
	if (mode != TC_SMPTE_30_NDF && mode != TC_SMPTE_25)
		throw SonicException(TimecodeText::invalidMode);

	double framesPerSecond = TCFramesPerSecond(mode);

	double retFrames = tc.Subframes() / kSubframeCount;
	if (tc.IsFieldTwo())
		retFrames += 0.5;

	//	get frames
	retFrames += tc.Frames();

	//	get seconds
	retFrames += tc.Seconds() * framesPerSecond;

	//	get minutes
	retFrames += tc.Minutes() * 60 * framesPerSecond;

	//	get hours
	retFrames += tc.Hours() * 3600 * framesPerSecond;

	return retFrames;
}


double TCFramesPerSecond(tc_mode mode)
{
	/* use the global time display if requested */
	mode = TCGlobalMode(mode);

	switch (mode) {
		case TC_SAMPLES:
		case TC_SECONDS:
		default:
			return 0.0;

		case TC_16MM_24:
		case TC_16FF:
		case TC_16MM_29:
		case TC_35MM_24:
		case TC_35FF:
		case TC_35MM_29:
		case TC_SMPTE_24:
		case TC_SMPTE_23976:
			return (F24_FRAMESSECOND * TCOneSecond(mode));

		case TC_16MM_25:
		case TC_35MM_25:
		case TC_SMPTE_25:
			return (F25_FRAMESSECOND * TCOneSecond(mode));

		case TC_SMPTE_50:
			return (F50_FRAMESSECOND * TCOneSecond(mode));

		case TC_16MM_30:
		case TC_35MM_30:
			return (F30_FRAMESSECOND * TCOneSecond(mode));

		case TC_SMPTE_30_NDF:
		case TC_SMPTE_29_NDF:
			return (NDF_FRAMESSECOND * TCOneSecond(mode));

		case TC_SMPTE_29_DF:
		case TC_SMPTE_30_DF:
			return (DF_FRAMESSECOND * TCOneSecond(mode));

		case TC_SMPTE_5994_DF:
			return (F5994_FRAMESSECOND * TCOneSecond(mode));

		case TC_SMPTE_5994:
		case TC_SMPTE_60:
			return (F60_FRAMESSECOND * TCOneSecond(mode));

		case TC_SMPTE_CD:
		case TC_DATA_CD:
			return CD_FRAMESSECOND;
	}
}


double TCFramesPerFoot(tc_mode mode)
{
	/* use the global time display if requested */
	mode = TCGlobalMode(mode);

	switch (mode) {
		default:
			return 0.0;

		case TC_16FF:
		case TC_16MM_24:
			return F16_FRAMESFOOT;

		case TC_16MM_25:
			return F16_FRAMESFOOT;

		case TC_16MM_29:
			return F16_FRAMESFOOT;

		case TC_16MM_30:
			return F16_FRAMESFOOT;

		case TC_35FF:
		case TC_35MM_24:
			return F35_FRAMESFOOT;

		case TC_35MM_25:
			return F35_FRAMESFOOT;

		case TC_35MM_29:
			return F35_FRAMESFOOT;

		case TC_35MM_30:
			return F35_FRAMESFOOT;
	}
}


double TCOneSecond(tc_mode mode)
{
	/* use the global time display if requested */
	mode = TCGlobalMode(mode);

// **CodeWizzard** - Violation:  Universal Coding Standards item 35  - Always provide a default branch for switch statement
	switch (mode) {
		case TC_SAMPLES:
		case TC_SECONDS:
		case TC_SMPTE_24:
		case TC_SMPTE_25:
		case TC_SMPTE_50:
		case TC_16MM_24:
		case TC_16MM_25:
		case TC_16FF:
		case TC_35MM_24:
		case TC_35MM_25:
		case TC_35FF:
		case TC_35MM_30:
		case TC_16MM_30:
		case TC_SMPTE_30_NDF:
		case TC_SMPTE_60:
		case TC_SMPTE_CD:
		case TC_DATA_CD:
		case TC_SMPTE_29_DF:
		case TC_SMPTE_5994_DF:
			return 1;

		case TC_SMPTE_29_NDF:
		case TC_35MM_29:
		case TC_16MM_29:
			return (DF_FRAMESSECOND / NDF_FRAMESSECOND);

		case TC_SMPTE_5994: // non DF by default
			return (F5994_FRAMESSECOND / F60_FRAMESSECOND);

		case TC_SMPTE_23976:
			return (F23976_FRAMESSECOND / F24_FRAMESSECOND);

		case TC_SMPTE_30_DF:
			return (NDF_FRAMESSECOND / DF_FRAMESSECOND);
	}
	return 1;
}


// blj 9/6/99 - add optional 'inShowHours' to force showing hours even on CD times for SonicHD
SInt32 FloatToTCString(double value, std::string & str, tc_mode mode, bool fraction, 
					   double samplingRate, bool inShowHours)
{
	str = "";

	//value must be less than MAX
	if (value >= MAXIMUM_TIME_VALUE)
	{
		str = "n/a";
		return -1;
	}

	// use the global time display if requested
	mode = TCGlobalMode(mode);
	SInt32 retval = 0;

	switch (mode) 
	{
	case TC_SAMPLES	:
		{
			// USE constant until global time code value
			// Finite precision arith. folks. Don't forget to add half
			// less we wind up with improper truncation!
			// Ed, 11-12-92
			// JT 00.11.07 convert SInt32 to UInt64
			UInt64 samples = static_cast<UInt64> (static_cast<float> (value * samplingRate) + 0.5);

			std::ostringstream out;
			out << samples;
			str = out.str();

			retval = 0;
			break;
		}

	case TC_SECONDS :
		FloatToSecs(value, str);
		break;

	case TC_16MM_24:
	case TC_16FF:
	case TC_16MM_25:
	case TC_16MM_29:
	case TC_16MM_30:
	case TC_35MM_24:
	case TC_35FF:
	case TC_35MM_25:
	case TC_35MM_29:
	case TC_35MM_30:
		retval = FloatToFF(value, str, mode, fraction);
		break;

	case TC_SMPTE_30_NDF:
	case TC_SMPTE_23976:
	case TC_SMPTE_24:
	case TC_SMPTE_25:
	case TC_SMPTE_CD:
	case TC_DATA_CD:
	case TC_SMPTE_29_DF:
	case TC_SMPTE_29_NDF:
	case TC_SMPTE_30_DF:
	case TC_SMPTE_50:
	case TC_SMPTE_5994:
	case TC_SMPTE_5994_DF:
	case TC_SMPTE_60:
		retval = FloatToTC(value, str, mode, fraction, inShowHours); // blj 9/6/99 - add 'inShowHours'
		break;

	default :
		str = "n/a";
		retval = -1;
		break;
	}

	return retval;
}


/********************************************************************************/
/* FloatToTC: convert from floating point number to the specified				*/
/*       	mode of timecode.  Make sure that the value is not greater			*/
/*       	than the maximum allowed value.										*/
/*       	             														*/
/*       	If fraction is true then add .x, where x is frames/kSubframeCount.	*/
/*       	Otherwise timecode is truncated to the frame.						*/
/********************************************************************************/
SInt32 FloatToTC(double value, std::string & str, tc_mode mode, bool fraction, bool inShowHours)
{
	return subFrameFloatToTC(value, str, mode, fraction, kSubframeCount, false, inShowHours);
}


// blj 9/6/99 - add optional 'inShowHours' to force showing hours even on CD times for SonicHD
SInt32 subFrameFloatToTC(double value, std::string & str, tc_mode mode, bool fraction, 
						 double subFrameCount, bool subFrameAsterisk, bool inShowHours)
{
	#define CONFORM_ALL_CD_TIMES_TO_SMPTE 1
	// ensure that all CD times are an integral number of SMPTE frames

	// save if negative value
  	SInt32 isNegative = false;
	if (value < 0.0)
	{
		isNegative = true;
		value = value * -1.0;
	}

	if (value > 360000.0)
	{
		// Very large times take a while in here... Catch anything over 100 hours.
		str = "99:99:99:99";
		return -1;
	}

	double frames = 0; /* frames at first (later set to the number of frames indicated 
					   by the value is the frame RATE....the number of frames which go by 
					   in a clock tick....usually the same as... */

	double framesPerSecond = 0;  /* which is the number of frames in a second 
								 as implied by the time code format */
	double tmp = 0.0;
	SInt32 hours = 0, tens = 0, minutes = 0, seconds = 0, dfframes = 0, subframes = 0;
	std::ostringstream out;

	SInt32 j = 0;
	const double theDF_10MINUTES = DF_10MINUTES; // NOTE: DF_10MINUTES	is SMPTE defined as exactly: 17982.0
	const double theDF_5994_10MINUTES = DF_5994_10MINUTES;

	// depending on type, generate correct frames
	switch (mode)
	{
	case TC_SMPTE_23976:
	case TC_SMPTE_50:
	case TC_SMPTE_5994:
	case TC_SMPTE_60:
	case TC_SMPTE_24:
	case TC_SMPTE_25:
	case TC_SMPTE_30_NDF:
	case TC_SMPTE_CD:
	case TC_DATA_CD:
	case TC_SMPTE_29_NDF:
		frames = TCFramesPerSecond(mode);
		framesPerSecond = FramesPerSecondToFramesPerSecondCounted(mode, frames);

		if (mode == TC_SMPTE_CD)
		{
#if defined(CONFORM_ALL_CD_TIMES_TO_SMPTE) && CONFORM_ALL_CD_TIMES_TO_SMPTE
			// Round to SMPTE timecode frame first
			// **CodeWizzard** - Severe Violation:  User Defined item 208  - Don't check floats for equality; check for greater than or less than
			if (value != 0.0)
				value = RoundToNearestFrame(value);
#endif

			// The number of frames = frames per second * value in seconds
			frames = frames * value;

			/* 5/6/94 - The industry doesn't round up CD "subframes" 
			and neither should we. In other words, "fraction" is FALSE. 
			However, if we rounded to SMPTE above we have to do one more check... */

#if defined(CONFORM_ALL_CD_TIMES_TO_SMPTE) && CONFORM_ALL_CD_TIMES_TO_SMPTE
			// Since we rounded to SMPTE above, there can be no harm in making sure
			// that frames does not end in .99999999 so that the subsequent truncation to
			// ntegral value will not be one frame off.
			// Without the check below, we have seen conversion errors!  An actual SMPTE
			// time that always seemed to fail was 00:20:00:22		-	Sagar 11/11/97
			frames += (0.25 / subFrameCount);
#endif
		}
		else
		{
			//	The number of frames = frames per second * value in seconds
			frames = frames * value;

			//	Prevent rounding errors:
			// **CodeWizzard** - Severe Violation:  User Defined item 208  - Don't check floats for equality; check for greater than or less than
			if (value != 0.0)
			{
				if (fraction || subFrameAsterisk)
					frames += (0.5 / subFrameCount); //	partial frames, add 1/2 of smallest granule
				else
					frames += 0.5; // whole frames, add 1/2 frame
			}
		}

		// blj 9/6/99 - add 'inShowHours' check to allow SonicHD to show hours on CD times
		if (inShowHours || (mode != TC_SMPTE_CD && mode != TC_DATA_CD))
		{
			 /* hours */
			j = static_cast<SInt32>(frames / (framesPerSecond * 3600.0));

			if (isNegative)
				out << kNegativeTimeSign;

			out.flags(std::ios_base::right);
			out.fill('0');
			out.width(2);
			out << static_cast<int>(j) << ':';

			frames = frames - static_cast<float>(j * (framesPerSecond * 3600.0));

			/* minutes */
			j = static_cast<SInt32>(frames / (framesPerSecond * 60.0));

			out.flags(std::ios_base::right);
			out.fill('0');
			out.width(2);
			out << j << ':';

			frames = frames - static_cast<float>(j * (framesPerSecond * 60.0));
		}
		else
		{
			/* only minutes  for CD time */
			j = static_cast<SInt32>(frames / (framesPerSecond * 60.0));

			out << "   ";
			out.flags(std::ios_base::right);
			out.fill('0');
			out.width(2);
			out << j << ':';

			frames = frames - static_cast<float>(j * (framesPerSecond * 60.0));
		}

		/* seconds */
		j = static_cast<SInt32>(frames / framesPerSecond);

		out.flags(std::ios_base::right);
		out.fill('0');
		out.width(2);
		out << j << ':';

		frames = frames - static_cast<float>(j) * framesPerSecond;

		/* frames */
		j = static_cast<SInt32>(frames);

		out.flags(std::ios_base::right);
		out.fill('0');
		out.width(2);
		out << j;

		frames = frames - static_cast<float>(j);

		j = static_cast<SInt32>(frames * subFrameCount);
		if (fraction != 0) // add != 0 to avoid warning on Win32
		{
			/* frames / kSubframeCount */
			out << '.';
			out.flags(std::ios_base::right);
			out.fill('0');
			out.width(2);
			out << j;
		}
		else
		{
			// Asterisk form : field 1, end of string, field 2, asterisk added
			if (subFrameAsterisk)
			{
				if (j >= (subFrameCount / 2) )
					out << '*';
			}
		}

		break;

	case TC_SMPTE_29_DF:
	case TC_SMPTE_30_DF: // JT.00.10.10, DF_30 not in SSHD 
		if (mode == TC_SMPTE_30_DF)
			frames = NDF_FRAMESSECOND; // 30.0
		else
			frames = DF_FRAMESSECOND; // 30.0/1.001)

		// blj 10/22/99 - this is causing a slight error in the subframe area. 
		// 01:00:02:00.00 is 3602.00000 for the 'value' in
		// Following calculations turn out 01:00:02:00.04 => 4 subframes wrong??
		frames = value * frames; /* now calculate the actual number of 
									frames implied by the floating point value */
		
		/* add (1/2 frame) to insure appropriate "rounding" */
		//	Prevent rounding errors:
		// **CodeWizzard** - Severe Violation:  User Defined item 208  - Don't check floats for equality; check for greater than or less than
		if (value != 0.0) // JT.00.10.10, 
		{
			if (fraction || subFrameAsterisk) 
			{	//	partial frames, add 1/2 of smallest granule :
				frames += (0.5 / subFrameCount);
			}
			else 
			{	//	whole frames, add 1/2 frame :
				frames += 0.5;
			}
		}

		while (frames >= DF_1HOUR) // get hours 
		{
			frames -= DF_1HOUR;
			hours++;
		}

		while (frames >= theDF_10MINUTES) // get tens of mintues
		{
			frames -= theDF_10MINUTES;
			tens++;
		}

		/* get units of minutes */
		/* first minute has 1800, others have 1798 frames */
		if (frames >= 1800.0) 
		{
			// first minute has 1800,  meaning 00;00;00;00 and  00;00;00;01 are vaild times
			minutes  = 1;
			frames -= 1800.0;

			while (frames > 1798.00) 
			{
				minutes++;
				frames -= 1798.00;
			}
		}

		/* seconds */
		/* test if this is NOT a first minute, i.e.  NOT 00,10,20,30,40 or 50
			- in that case only 28 frames go into the first "second"*/
		if ((minutes != 0) && (frames >= 28.0)) 
		{
			seconds = 1;
			frames -= 28.0;
		}

		/* rest of "seconds" including first "second" occuring on 10 minute boundaries */
		while (frames >= 30.00) 
		{
			seconds++;
			frames -= 30.00;
		}
		minutes += tens * 10;  /* now add together the 10's and units of minutes */

		/* frames now */
		dfframes  = static_cast<SInt32>(frames);

		/* subframes */
		if (fraction || subFrameAsterisk) 
		{
			/* frames / kSubframeCount */
			tmp = frames - dfframes;
			subframes = static_cast<SInt32>(tmp * subFrameCount);
		}

		/* because drop frame "drops" the first two frames
			in each minute, except for minutes 00, 10, 20, 30, 40, & 50, 
			here we increment the frame number by 2 if we are on the
			the first or 0'th second in a minute, 
			unless we're on a 10 minute boundary */
		if ((seconds == 0) && (minutes % 10 != 0))
			dfframes += 2;

		if (isNegative)
			out << '-';

		out.flags(std::ios_base::right);
		out.fill('0');
		out.width(2);
		out << hours << ';';
		out.flags(std::ios_base::right);
		out.fill('0');
		out.width(2);
		out << minutes << ';';
		out.flags(std::ios_base::right);
		out.fill('0');
		out.width(2);
		out << seconds << ';';
		out.flags(std::ios_base::right);
		out.fill('0');
		out.width(2);
		out << dfframes;

		/* add subframe number */
		if (fraction)
		{
			out << '.';
			out.flags(std::ios_base::right);
			out.fill('0');
			out.width(2);
			out << subframes;
		}
		else
		{
			if (subFrameAsterisk)
			{
				if (subframes >= (subFrameCount / 2))
					out << '*';
			}
		}

		break;
	case TC_SMPTE_5994_DF: // this is not per spec. but some assets have that. Let's extrapolate according to their time codes: they drop 4 frames instead of 2....
		frames = F5994_FRAMESSECOND;
		// blj 10/22/99 - this is causing a slight error in the subframe area. 
		// 01:00:02:00.00 is 3602.00000 for the 'value' in
		// Following calculations turn out 01:00:02:00.04 => 4 subframes wrong??
		frames = value * frames; /* now calculate the actual number of 
									frames implied by the floating point value */
		
		/* add (1/2 frame) to insure appropriate "rounding" */
		//	Prevent rounding errors:
		// **CodeWizzard** - Severe Violation:  User Defined item 208  - Don't check floats for equality; check for greater than or less than
		if (value != 0.0) // JT.00.10.10, 
		{
			if (fraction || subFrameAsterisk) 
			{	//	partial frames, add 1/2 of smallest granule :
				frames += (0.5 / subFrameCount);
			}
			else 
			{	//	whole frames, add 1/2 frame :
				frames += 0.5;
			}
		}

		while (frames >= DF_5994_1HOUR) // get hours 
		{
			frames -= DF_5994_1HOUR;
			hours++;
		}

		while (frames >= theDF_5994_10MINUTES) // get tens of mintues
		{
			frames -= theDF_5994_10MINUTES;
			tens++;
		}

		/* get units of minutes */
		/* first minute has 3600, others have 3596 frames */
		if (frames >= 3600) 
		{
			// first minute has 3600,  meaning 00;00;00;00 and  00;00;00;01  (...02 and 03) are valid times
			minutes  = 1;
			frames -= 3600.0;

			while (frames > 3596.00) 
			{
				minutes++;
				frames -= 3596.00;
			}
		}

		/* seconds */
		/* test if this is NOT a first minute, i.e.  NOT 00,10,20,30,40 or 50
			- in that case only 56 frames go into the first "second"*/
		if ((minutes != 0) && (frames >= 56.0)) 
		{
			seconds = 1;
			frames -= 56.0;
		}

		/* rest of "seconds" including first "second" occuring on 10 minute boundaries */
		while (frames >= 60.00) 
		{
			seconds++;
			frames -= 60.00;
		}
		minutes += tens * 10;  /* now add together the 10's and units of minutes */

		/* frames now */
		dfframes  = static_cast<SInt32>(frames);

		/* subframes */
		if (fraction || subFrameAsterisk) 
		{
			/* frames / kSubframeCount */
			tmp = frames - dfframes;
			subframes = static_cast<SInt32>(tmp * subFrameCount);
		}

		/* because drop frame "drops" the first four frames
			in each minute, except for minutes 00, 10, 20, 30, 40, & 50, 
			here we increment the frame number by 4 if we are on the
			the first or 0'th second in a minute, 
			unless we're on a 10 minute boundary */
		if ((seconds == 0) && (minutes % 10 != 0))
			dfframes += 4;

		if (isNegative)
			out << '-';

		out.flags(std::ios_base::right);
		out.fill('0');
		out.width(2);
		out << hours << ';';
		out.flags(std::ios_base::right);
		out.fill('0');
		out.width(2);
		out << minutes << ';';
		out.flags(std::ios_base::right);
		out.fill('0');
		out.width(2);
		out << seconds << ';';
		out.flags(std::ios_base::right);
		out.fill('0');
		out.width(2);
		out << dfframes;

		/* add subframe number */
		if (fraction)
		{
			out << '.';
			out.flags(std::ios_base::right);
			out.fill('0');
			out.width(2);
			out << subframes;
		}
		else
		{
			if (subFrameAsterisk)
			{
				if (subframes >= (subFrameCount / 2))
					out << '*';
			}
		}

		break;
	default:
		break;
	}
  
	str = out.str();
  	return 0;
}


/************************************************************************/
/* FloatToSecs: convert from floating number to seconds string.			*/
/*       		Insure that the floating number is not greater than the */
/*       		maximum allowed value.									*/
/************************************************************************/
void FloatToSecs(double value, std::string & str)
{
	std::ostringstream out;
	out << value;
	str = out.str();
}


/********************************************************************************/
/* FloatToFF: convert from floating point number to the feet and 				*/
/*       	frames equivalent.  Make sure that the value is not greater			*/
/*       	than the maximum allowed value.										*/
/*       	             														*/
/*       	If fraction is true then add .x, where x is frames/kSubframeCount.	*/
/*       	Otherwise timecode is truncated to the frame.						*/
/********************************************************************************/

SInt32 FloatToFF(double value, std::string & str, tc_mode mode, bool fraction)
{
	/* save if negative value */
	bool isNegative = (value < 0.0);
	if (isNegative)
		value = -value;

	/* Calculate the number of feet based on the number of seconds */
	double FramesPerSecond = TCFramesPerSecond(mode);
	double FramesPerFoot = TCFramesPerFoot(mode);

	// **Prefix** - time_utils.cpp; Line: 671; warning (20): dividing by zero using 'FramesPerFoot'
	double FeetPerSecond = FramesPerSecond / FramesPerFoot;

#if DEBUG_FloatToFF
	fprintf(stderr, "FloatToFF: FramesPerFoot :%i ;", FramesPerFoot);
	fprintf(stderr, " FeetPerSecond :%i ;", FeetPerSecond);
#endif

	double totalFeet = FeetPerSecond * value;
	SInt32 wholeFeet = static_cast<SInt32>(totalFeet);
	double tmpFrames = (totalFeet - wholeFeet) * FramesPerFoot;

	/* whole frames, add 1/2 frame */
	// **CodeWizzard** - Severe Violation:  User Defined item 208  - Don't check floats for equality; check for greater than or less than
	if (!fraction && (value != 0.0))
		tmpFrames += 0.5;

	SInt32 frames = static_cast<SInt32>(tmpFrames);
	while (frames >= FramesPerFoot)
	{
		wholeFeet++;
		frames -= static_cast<SInt32>(FramesPerFoot);
		tmpFrames -= FramesPerFoot;
	}

	double tmpSubframes = (tmpFrames - frames) * kSubframeCount;

	/* partial frames, add 1/2 of smallest granule */
	// **CodeWizzard** - Severe Violation:  User Defined item 208  - Don't check floats for equality; check for greater than or less than
	if (fraction && (value != 0.0))
		tmpSubframes = tmpSubframes + 0.5;

	SInt32 subframes = static_cast<SInt32>(tmpSubframes);
	while (subframes >= kSubframeCount)
	{
		frames++;
		while (frames >= FramesPerFoot)
		{
			wholeFeet++;
			frames -= static_cast<SInt32>(FramesPerFoot);
		}

		subframes -= static_cast<SInt32>(kSubframeCount);
		tmpSubframes -= kSubframeCount;
	}

#if DEBUG_FloatToFF
	fprintf(stderr, "  \ntotalFeet :%g \n", totalFeet);
	fprintf(stderr, "  wholeFeet :%i \n", wholeFeet);
	fprintf(stderr, "  tmp :%g \n", tmp); 
	fprintf(stderr, "  frames :%i \n", frames);
	fprintf(stderr, "  subframes :%i \n", subframes);
#endif

	std::ostringstream out;
	if (isNegative)
		out << kNegativeTimeSign;

	out.flags(std::ios_base::right);
	out.fill('0');
	out.width(2);
	out << wholeFeet << ':';
	out.flags(std::ios_base::right);
	out.fill('0');
	out.width(2);
	out << frames;


	/* add subframe number */
	if (fraction)
	{
		out << '.';
		out.flags(std::ios_base::right);
		out.fill('0');
		out.width(2);
		out << subframes;
	}

	str = out.str();
  	return 0;
}

void Seconds2TCString(const Seconds & theValue, std::string & theString, tc_mode timecodePref, bool showOddField)
{
	double samplingRate = 48000/1.001;

	double frame_per_sec = TCFramesPerSecond(timecodePref);

	//need to make sure that seconds is top field aligned since FloatToTCString cannot handle fields
	//appropriately for our purposes - add the field afterwards
	Fields theFields = static_cast<Fields>(floor(2.0*frame_per_sec*theValue.AsDouble() + .5)); //convert to fields
	bool oddField = false ;
	if (theFields % 2 == 1) //top field should be even valued
	{
		theFields--;
		oddField = true ;
	}

	Seconds secs(static_cast<double>(theFields) / (2.0 * frame_per_sec));

	SInt32 err = FloatToTCString(secs.AsDouble(), theString, timecodePref, false, samplingRate);
	if (err)
{
		std::cout << "Seconds2TCString:Error  performing conversion err = " << err << std::endl;
		theString =  "??:??:??:??";
		return;
	}

	//¥¥ Temporary fix to make sure that time code strings don't
	//¥¥ have a mix and match bunch of ";" and ":" seperators.
	size_t theStringLength = theString.size();
	if (timecodePref == TC_SMPTE_29_DF || timecodePref == TC_SMPTE_5994_DF)
	{
		for (size_t i = 0; i < theStringLength; i++)
			if (theString[i] == ':')
				theString[i] = ';';
	}
	else
	{
		for (size_t i = 0; i < theStringLength; i++)
			if (theString[i] == ';')
				theString[i] = ':';
	}

	//now see if we need to add an asterisk to represent the bottom field
	if (oddField && showOddField) //add asterisk 
		theString += '*' ;

	return ;
}


Seconds TCString2Seconds(const std::string & inString, tc_mode timecodePref, bool* pOutSuccess)
{
	if (pOutSuccess) 
		*pOutSuccess = true;

	std::string theString(inString);
	size_t theStringLength = theString.size();

	//SSE TCStringToFloat does not interpret the * on the end of a TC String as bottom field
	//- this is required for re encodes in Telecine modes
	//- interpret (do not want to change SSE) and add to resulting float
	SInt32 bottom_field = 0 ;
	for (size_t i = 0; i < theStringLength; i++)
		if (theString[i] == '*')
			bottom_field = 1;

	if (bottom_field)
		theString.erase(theStringLength - 1, 1);

	//¥¥ Temporary fix to make sure that time code strings don't
	//¥¥ have a mix and match bunch of ";" and ":" seperators.
	if (timecodePref == TC_SMPTE_29_DF)
		for (size_t i = 0; i < theStringLength; i++)
			if (theString[i] == ':')
				theString[i] = ';';
	else
		// **Prefix** - time_utils.cpp; Line: 1563; warning (246): Local declaration of 'i' hides declaration of the same name in an outer scope:  see previous declaration at line 1542 of c:\_authorscript_lh.20040616\_longhorn.139544\lib\SysUtils\time_utils.cpp.
		for (size_t i = 0; i < theStringLength; i++)
			if (theString[i] == ';')
				theString[i] = ':';

  	Seconds start;
  	try
  	{
  		const double samplingRate = 48000 / 1.001;
  		start = TCStringToFloat(theString, timecodePref, samplingRate);
  	}
	catch (std::exception&)
  	{
  		if (pOutSuccess)
  			*pOutSuccess = false;
  		else
  			throw;
  	}
  
	//add field duration
	double frame_per_sec;
	switch (timecodePref)
	{
	case TC_SMPTE_23976:
		frame_per_sec = F23976_FRAMESSECOND;
		break;

	case TC_SMPTE_24:
		frame_per_sec = F24_FRAMESSECOND;
		break;

	case TC_SMPTE_25:
		frame_per_sec = F25_FRAMESSECOND;
		break;

	case TC_SMPTE_29_NDF:
	case TC_SMPTE_29_DF:
		frame_per_sec = DF_FRAMESSECOND;
		break;

	case TC_SMPTE_30_NDF:
	case TC_SMPTE_30_DF:
		frame_per_sec = F30_FRAMESSECOND;
		break;

	case TC_SMPTE_50:
		frame_per_sec = F50_FRAMESSECOND;
		break;

	case TC_SMPTE_5994:
	case TC_SMPTE_5994_DF:
		frame_per_sec = F5994_FRAMESSECOND;
		break;

	case TC_SMPTE_60:
		frame_per_sec = F60_FRAMESSECOND;
		break;

	default:
		throw SonicException(TimecodeText::invalidMode);
		break;
	}

	//due to a calculation error in TCStringToFloat TCToFloat in cautils
	//.. and not wanting to correct that routine at the moment since it is used by just about eveyone
	// I am going to correct it here - BWR 1/29/98
	//Calculation generates a frame count which is then converted to seconds - however,
	//the frame count comes out fractional - I think this is wrong so I an going to 
	// convert back to frames here, round and then reconvert to seconds
	start = (static_cast<double>(floor(start.AsDouble() * frame_per_sec + 0.5))) / frame_per_sec ;

	if (bottom_field)
		start += (1 / (2 * frame_per_sec)) ;

	return start;
}


tc_mode TCGlobalMode(tc_mode mode)
{
	if (mode == TC_GLOBAL || mode == TC_MEASURE_RATE)
	{
		// mode = (UInt16) (**gSonicConfig).timeDisplay;
		// **CodeWizzard** - Informational:  Effective C++ item 2  - Prefer iostream.h to stdio.
		printf("WARNING!!!: INVALID USE OF GLOBAL TIMECODE\n");
		mode = TC_SMPTE_29_NDF;
	}

	return mode ;
}


//-----------------------------------------------------------------------------
//	FUNCTION	:	GetMilliSeconds
//-----------------------------------------------------------------------------
UInt64	GetMilliSeconds() {

#if defined (macintosh)

	UnsignedWide curTimeUSec;
	Microseconds(&curTimeUSec);
	UInt64 curTime;
	curTime = curTimeUSec.hi;
	curTime <<= 32;
	curTime |= curTimeUSec.lo;

	return curTime / 1000;

#elif defined(__unix__)

	UInt64			curTime = 0;
	struct timeval	timer;

#ifdef	__IX__
	gettimeofday(&timer);
#else
	struct timezone tz;
	gettimeofday(&timer, &tz);
#endif

	curTime = (timer.tv_sec * 1000) + (timer.tv_usec / 1000);

	return curTime;

#elif defined (_WIN32)

//	return GetTickCount(); //*10;	//ugh. only 1/18sec resolution

	return ::timeGetTime();		//1 ms resolution

#endif
}


void TCStringToHMSF(const std::string& tcString, tc_mode tcMode, SInt32& hours, SInt32& minutes, SInt32& seconds, SInt32& frames, bool& fieldTwo)
{
	TCStringParser parser(tcString, tcMode);
	hours	= parser.Hours();
	minutes = parser.Minutes();
	seconds = parser.Seconds();
	frames	= parser.Frames();
	fieldTwo= parser.IsFieldTwo();
}


void SecondsToHMSF(const Seconds& time, tc_mode tcMode, SInt32& hours, SInt32& minutes, SInt32& seconds, SInt32& frames, bool& oddField)
{
	std::string tcString;
	Seconds2TCString(time, tcString, tcMode);
	TCStringToHMSF(tcString, tcMode, hours, minutes, seconds, frames, oddField);
}

void HMSFToTCString(std::string& tcString, tc_mode tcMode, const SInt32& hours, const SInt32& minutes, const SInt32& seconds, const SInt32& frames, const bool& oddField)
{
	Seconds time;
	HMSFToSeconds(time, tcMode, hours, minutes, seconds, frames, oddField);
	Seconds2TCString(time, tcString, tcMode);
}

void HMSFToSeconds(Seconds& time, tc_mode tcMode, const SInt32& hours, const SInt32& minutes, const SInt32& seconds, const SInt32& frames, const bool& isFieldTwo)
{
	const double framesPerSecond = TCFramesPerSecond(tcMode);
	double fieldsPerSecondCounted = FramesPerSecondToFramesPerSecondCounted(tcMode, framesPerSecond)*2;

	// subframes, if any
	double fields;
	if (isFieldTwo)
		fields = 1.;
	else
		fields = 0.;

	// frames, if any
	fields += frames*2.;

	// seconds, if any
	fields += seconds*fieldsPerSecondCounted;

	// minutes, if any
	fields += minutes*60*fieldsPerSecondCounted;

	// hours, if any
	fields += hours*3600*fieldsPerSecondCounted;

	time = fields/(framesPerSecond*2.);
}




#define PAL_TC_DESC			"PAL\n"
#define PAL_TC_DESC_LEN		4
#define NDF_29_TC_DESC		"29.97 NDF\n"
#define NDF_29_TC_DESC_LEN	10
#define DF_29_TC_DESC		"29.97 DF\n"
#define DF_29_TC_DESC_LEN	9
#define NDF_30_TC_DESC		"30 NDF\n"
#define NDF_30_TC_DESC_LEN	7
#define DF_30_TC_DESC		"30 DF\n"
#define DF_30_TC_DESC_LEN	6

// SonicError mpeg_audio_stream_file::parse_timestamp_string(char* str)
SInt32 parse_timestamp_string(const std::string & stamp_str, double* time_stamp, tc_mode* timecode_mode)
{
	if (!time_stamp || !timecode_mode)
		return -1;

	*timecode_mode = TC_UNKNOWN;
	bool found = false;

	size_t i = 0;
	for (i = 0; i < stamp_str.size(); i++)
	{
		if (stamp_str[i] == '\n' || stamp_str[i] == '\r')
		{
			found = true;
			break;
		}
	}

	if (!found)
		return -1;

	i++; // Include the EOL
	if ((i == PAL_TC_DESC_LEN) && stamp_str.substr(i, PAL_TC_DESC_LEN - 1) == PAL_TC_DESC)
	{
		*timecode_mode = TC_SMPTE_25;
	}
	else if ((i == NDF_29_TC_DESC_LEN) && stamp_str.substr(i, NDF_29_TC_DESC_LEN - 1) == NDF_29_TC_DESC)
	{
		*timecode_mode = TC_SMPTE_29_NDF;
	}
	else if ((i == DF_29_TC_DESC_LEN) && stamp_str.substr(i, DF_29_TC_DESC_LEN - 1) == DF_29_TC_DESC)
	{
		*timecode_mode = TC_SMPTE_29_DF;
	}
	else if ((i == NDF_30_TC_DESC_LEN) && stamp_str.substr(i, NDF_30_TC_DESC_LEN - 1) == NDF_30_TC_DESC)
	{
		*timecode_mode = TC_SMPTE_30_NDF;
	}
	else if ((i == DF_30_TC_DESC_LEN) && stamp_str.substr(i, DF_30_TC_DESC_LEN - 1) == DF_30_TC_DESC)
	{
		*timecode_mode = TC_SMPTE_30_DF;
	}
	else
		return -1;

	*time_stamp = TCToFloat(stamp_str.substr(i, stamp_str.size() - i), *timecode_mode);
	return 0;
}


// Make a timestamp string from a time value and timecode mode.
SInt32 get_timestamp_string(double value, tc_mode mode, std::string & stamp_str)
{
	std::string tc_string;
	SInt32 retval = FloatToTC(value, tc_string, mode, true);
	if (retval)
		return retval;

	stamp_str = "";
	switch (mode)
	{
	case TC_SMPTE_25 :
		stamp_str = PAL_TC_DESC;
		break;

	case TC_SMPTE_29_NDF :
		stamp_str = NDF_29_TC_DESC;
		break;

	case TC_SMPTE_29_DF :
		stamp_str = DF_29_TC_DESC;
		break;

	case TC_SMPTE_30_NDF :
		stamp_str = NDF_30_TC_DESC;
		break;

	case TC_SMPTE_30_DF :
		stamp_str = DF_30_TC_DESC;
		break;

	default:
		break;
	}

	stamp_str += tc_string;
	return 0;
}


SInt32 TickCountSixtiethSec() 
{
#ifdef _WIN32   
	DWORD ticks = GetTickCount(); // Number of ms since windows started (don't ask me 
  								  // why anyone would want to know in 60ths of a second.
  								  // I guess macintoshes sometimes run for 49.7 days)
  								  // Perhaps we should start measuring font point sizes in
  								  // 1024ths of a cubit
  	return (ticks * 1000) / 60;
#else
	return(TickCount());
#endif
}

eVideoRate TCModeToVideoRate(tc_mode tcMode)
{
	eVideoRate videoRate;

	switch(tcMode)
	{
	case TC_SMPTE_23976:
		videoRate = eVideoRate23976;
		break;

	case TC_SMPTE_24:
		videoRate = eVideoRate6_24;
		break;

	case TC_SMPTE_25:
		videoRate = eVideoRate625_50;
		break;

	case TC_SMPTE_29_NDF:
	case TC_SMPTE_29_DF:
		videoRate = eVideoRate525_60;
		break;
		
	case TC_SMPTE_30_NDF:
	case TC_SMPTE_30_DF:
		videoRate = eVideoRate630_60;
		break;

	case TC_SMPTE_50:
		videoRate = eVideoRate50;
		break;

	case TC_SMPTE_5994:
	case TC_SMPTE_5994_DF:
		videoRate = eVideoRate5994;
		break;

	case TC_SMPTE_60:
		videoRate = eVideoRate60;
		break;

	default:
		throw SonicException(TimecodeText::invalidMode);
		break;
	}

	return videoRate;
}

tc_mode VideoRateToTCMode(eVideoRate videoRate)
{
	tc_mode tcMode;
	switch (videoRate)
	{
	case eVideoRate23976:
		tcMode = TC_SMPTE_23976;
		break;

	case eVideoRate6_24:
		tcMode = TC_SMPTE_24;
		break;

	case eVideoRate625_50:
		tcMode = TC_SMPTE_25;
		break;

	case eVideoRate525_60:
		tcMode = TC_SMPTE_29_NDF;
		break;

	case eVideoRate630_60:
		tcMode = TC_SMPTE_30_NDF;
		break;

	case eVideoRate50:
		tcMode = TC_SMPTE_50;
		break;

	case eVideoRate5994:
		tcMode = TC_SMPTE_5994;
		break;

	case eVideoRate60:
		tcMode = TC_SMPTE_60;
		break;

	case eVideoRateUnknown:
	default:
		throw SonicException(TimecodeText::invalidMode);
		break;
	}

	return tcMode;
}

double VideoRateToFrameDuration(eVideoRate videoRate)
{
	double frameRate;
	switch(videoRate)
	{
	case eVideoRate525_60:
		frameRate = videoFrameRate525_60;
		break;
	case eVideoRate625_50:
		frameRate = videoFrameRate625_50;
		break;
	case eVideoRate630_60:
		frameRate = videoFrameRate630_60;
		break;
	case eVideoRate6_24:
		frameRate = videoFrameRate6_24;
		break;
	case eVideoRate23976:
		frameRate = videoFrameRate23976;
		break;
	case eVideoRate5994:
		frameRate = videoFrameRate5994;
		break;
	case eVideoRate50:
		frameRate = videoFrameRate50;
		break;
	case eVideoRate60:
		frameRate = videoFrameRate60;
		break;

	default:
		throw SonicException(TimecodeText::invalidVideoRate);
	}

	return frameRate;
}

double VideoRateToFieldDuration(eVideoRate videoRate)
{
	double fieldRate;
	switch(videoRate)
	{
	case eVideoRate525_60:
		fieldRate = videoFieldRate525_60;
		break;
	case eVideoRate625_50:
		fieldRate = videoFieldRate625_50;
		break;
	case eVideoRate630_60:
		fieldRate = videoFieldRate630_60;
		break;
	case eVideoRate6_24:
		fieldRate = videoFieldRate6_24;
		break;
	case eVideoRate23976:
		fieldRate = videoFieldRate23976;
		break;
	case eVideoRate5994:
		fieldRate = videoFieldRate5994;
		break;
	case eVideoRate50:
		fieldRate = videoFieldRate50;
		break;
	case eVideoRate60:
		fieldRate = videoFieldRate60;
		break;

	default:
		throw SonicException(TimecodeText::invalidVideoRate);
	}

	return fieldRate;
}


typedef std::map<Seconds, eVideoRate> FrameRateToeVideoRateMap;
typedef FrameRateToeVideoRateMap::value_type FrameRateToeVideoRateMapEntry;
static FrameRateToeVideoRateMapEntry sFrameRateToeVideoRateMapEntries[] =
{
	FrameRateToeVideoRateMapEntry(60.,	eVideoRate60),
	FrameRateToeVideoRateMapEntry(59.94,eVideoRate5994),
	FrameRateToeVideoRateMapEntry(50.,	eVideoRate50),
	FrameRateToeVideoRateMapEntry(30.,	eVideoRate630_60),
	FrameRateToeVideoRateMapEntry(29.97,eVideoRate525_60),
	FrameRateToeVideoRateMapEntry(25.,	eVideoRate625_50),
	FrameRateToeVideoRateMapEntry(24.,	eVideoRate6_24),
	FrameRateToeVideoRateMapEntry(23.97,eVideoRate23976),
	FrameRateToeVideoRateMapEntry(23.98,eVideoRate23976)
};

static SInt32 kNumFrameRateToeVideoRateMapEntries = sizeof( sFrameRateToeVideoRateMapEntries)/sizeof (FrameRateToeVideoRateMapEntry);
static const FrameRateToeVideoRateMap sFrameRateToeVideoRateMap(sFrameRateToeVideoRateMapEntries, sFrameRateToeVideoRateMapEntries + kNumFrameRateToeVideoRateMapEntries);

eVideoRate FindNearestVideoRate(const Seconds& frameRate)
{
	eVideoRate videoRate = eVideoRateUnknown;

	// Round value to 2 decimal places, then look up in map
	double roundedRate = static_cast<SInt32>((frameRate.AsDouble()*100 + .5))/100.;
	FrameRateToeVideoRateMap::const_iterator mapIter = sFrameRateToeVideoRateMap.find(roundedRate);

	if (mapIter != sFrameRateToeVideoRateMap.end())
	{
		videoRate = (*mapIter).second;
	}
	return videoRate;
}

#ifdef	__unix__
SInt32 GetTickCount()
{
	struct timeval tv;
	gettimeofday(&tv, 0);

	// printf("TickCount returning secs= %d\n",tv.tv_sec*60);
	// printf("TickCount returning usecs = %d\n",tv.tv_usec);

	UInt64 mytime = tv.tv_sec*1000000 + tv.tv_usec;
	return (mytime);

	// return 0;
}
#endif	// __unix__


#if !defined (STRIP_FOR_LONGHORN) && !defined (OSXPORT) 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Deprecated functions.  DO NOT USE

/************************************************************************/
// DEPRECATED!!!
/* TCStringToFloat:      												*/
/* 		Converts a Pascal-style Time Code String of the Format HH:MM:SS:ff¥sss into  */
/* 			a Floating Point result in seconds, where HH are hours, MM  */
/*			are minutes, ss are seconds and ff are frames.  An optional */
/* 			subframe field may be present, delineated by the bullet ¥sss*/
/************************************************************************/
SInt32 TCStringToFloat(double* value, const unsigned char* str, tc_mode mode, double samplingRate)
{
	// Make a string out of the pascal string
	std::string timeStr(reinterpret_cast<const char*>(str + 1), *str);

	/* call the C++ version */
	*value = TCStringToFloat(timeStr, mode, samplingRate);
	return 0;
}


/************************************************************************/
/* SamplesToFloat: convert from samples to a floating point number.		*/
/************************************************************************/
// Deprecated - DO NOT USE
SInt32 SamplesToFloat(double* value, const char *str, double samplingRate)
{
	std::string s(str);
	*value = SamplesToFloat(s, samplingRate);
	return 0;
}


// Deprecated!  NO NOT USE
SInt32 CTCStringToFloat(double* value, const char* timeStr, tc_mode mode, double samplingRate)
{
	std::string timeString(timeStr);
	*value = TCStringToFloat(timeString, mode, samplingRate);
	return 0;
}


/************************************************************************/
/* TCToFloat: convert from specified string to a floating point			*/
/*       	number.  Make sure that the value is not greater			*/
/*       	than the maximum allowed value.								*/
/*       	             												*/
/*       	No white space allowed. ?must appear before subframes.		*/
/*       	Valid timecode string only, please.							*/
/************************************************************************/
// Deprecated -- DO NOT USE
SInt32 TCToFloat(double* value, const char *str, tc_mode mode)
{
	*value = TCToFloat(std::string(str), mode);
	return 0;
}


// Depredcated  DO NOT USE
double TimecodeStringToFrames(const char* str, tc_mode mode)
{
	return TimecodeStringToFrames(std::string(str), mode);
}


// Depredcated  DO NOT USE
// blj 9/6/99 - add optional 'inShowHours' to force showing hours even on CD times for SonicHD
SInt32 FloatToTCString(double value, unsigned char * str, size_t strlength,
						tc_mode mode, bool fraction, double samplingRate, 
						bool inShowHours)
{
	SInt32	retval = 0;
	//SInt32	samples;

	if (str == 0)
		return(-1);

	str[0] = 0;

	/* value must be less than MAX */
	if (value >= MAXIMUM_TIME_VALUE) {
		sonic::strlcpy(reinterpret_cast<char *>(str), "n/a", strlength);
		c2pstr(reinterpret_cast<char *>(str));
	}

	/* use the global time display if requested */
	mode = TCGlobalMode(mode);

	switch (mode) 
	{
	case TC_SAMPLES	:
		{
			/* USE constant until global time code value */
			// Finite precision arith. folks. Don't forget to add half
			// less we wind up with improper truncation!
			// Ed, 11-12-92
			// JT 00.11.07 convert SInt32 to UInt64
//SR SC: T, I, D [JAW 20040924] - rewrite using iostreams instead of sprintf
//SR DI: DP0, E0, D0 [JAW 20040924] {dread_score 0.0} - no threat, just old fashioned
			UInt64 samples = static_cast<UInt64> (static_cast<float> (value * samplingRate) + 0.5);
// **CodeWizzard** - Informational:  Effective C++ item 2  - Prefer iostream.h to stdio.
			sonic::snprintf_safe(reinterpret_cast<char*>(str), strlength,"%llu", samples);
			retval = 0;
			break;
		}
	case TC_SECONDS :
		FloatToSecs(value, reinterpret_cast<char *>(str), strlength, fraction);
		break;

	case TC_16MM_24:
	case TC_16FF:
	case TC_16MM_25:
	case TC_16MM_29:
	case TC_16MM_30:
	case TC_35MM_24:
	case TC_35FF:
	case TC_35MM_25:
	case TC_35MM_29:
	case TC_35MM_30:
		retval = FloatToFF(value, reinterpret_cast<char *>(str), strlength, mode, fraction);
		break;

	case TC_SMPTE_30_NDF:
	case TC_SMPTE_23976:
	case TC_SMPTE_24:
	case TC_SMPTE_25:
	case TC_SMPTE_CD:
	case TC_DATA_CD:
	case TC_SMPTE_29_DF:
	case TC_SMPTE_29_NDF:
	case TC_SMPTE_30_DF:
	case TC_SMPTE_50:
	case TC_SMPTE_5994:
	case TC_SMPTE_60:
		retval = FloatToTC(value, reinterpret_cast<char *>(str), strlength, mode, fraction, inShowHours); // blj 9/6/99 - add 'inShowHours'
		break;

	default :
// **CodeWizzard** - Violation:  Universal Coding Standards item 34  - Always terminate a case statement with brea
		sonic::strlcpy(reinterpret_cast<char*>(str), "n/a", strlength);
		retval = -1;
	} /* switch */

	/* convert back to Pascal string */
	c2pstr(reinterpret_cast<char *>(str));

	return(retval);
} /* FloatToTCString */


/********************************************************************************/
// Depredcated  DO NOT USE
/* FloatToTC: convert from floating point number to the specified				*/
/*       	mode of timecode.  Make sure that the value is not greater			*/
/*       	than the maximum allowed value.										*/
/*       	             														*/
/*       	If fraction is true then add .x, where x is frames/kSubframeCount.	*/
/*       	Otherwise timecode is truncated to the frame.						*/
/********************************************************************************/

SInt32 FloatToTC(double value, char* str, size_t strlength, tc_mode mode, bool fraction, bool inShowHours)
{
		SInt32 retval;

		retval = subFrameFloatToTC(value, str, strlength, mode, fraction, kSubframeCount, false, inShowHours);
		return retval;
}


// Deprecated -- DO NOT USE
// blj 9/6/99 - add optional 'inShowHours' to force showing hours even on CD times for SonicHD
SInt32 subFrameFloatToTC(double value, char* str, size_t strlength, tc_mode mode, bool fraction,
							double subFrameCount, bool subFrameAsterisk,
							bool inShowHours)
{
  	SInt32		 j, intframes;
  	SInt32		 isNegative = false;
	double	frames; /* frames at first (later set to the 
						number of frames indicated by the value
						is the frame RATE....the number of frames which
						go by in a clock tick....usually the same as... */
	double	framesPerSecond;  /* which is the number of frames in a second
								as implied by the time code format */
	double	tmp;
	SInt32 	hours = 0, tens = 0, minutes = 0, seconds = 0,
	        dfframes = 0, subframes = 0;
	SInt32 	incr = 0;				/* increment for indices into string */

#define CONFORM_ALL_CD_TIMES_TO_SMPTE 1
/* ensure that all CD times are an integral number of SMPTE frames */

	/* save if negative value */
	if (value < 0.0) {
		isNegative = true;
		value = value * -1.0;
		incr = 1;
	}

	if (value > 360000.0)
	{
		// Very large times take a while in here... Catch anything over 100 hours.
// **CodeWizzard** - Informational:  Effective C++ item 2  - Prefer iostream.h to stdio.
		sonic::snprintf_safe(str, strlength, "99:99:99:99");
//SR SC: T, I, D [JAW 20040924] - rewrite using iostreams instead of sprintf
//SR DI: DP0, E0, D0 [JAW 20040924] {dread_score 0.0} - no threat, just old fashioned
		return -1;
	}

	/* depending on type, generate correct frames */
// **CodeWizzard** - Violation:  Universal Coding Standards item 35  - Always provide a default branch for switch statement
	switch (mode) {
	case TC_SMPTE_23976:
	case TC_SMPTE_24:
	case TC_SMPTE_25:
	case TC_SMPTE_30_NDF:
	case TC_SMPTE_CD:
	case TC_DATA_CD:
	case TC_SMPTE_29_NDF:
	case TC_SMPTE_50:
	case TC_SMPTE_5994:
	case TC_SMPTE_60:
		frames = TCFramesPerSecond(mode);
		if (mode == TC_SMPTE_29_NDF)
			framesPerSecond = NDF_FRAMESSECOND;
		else
			framesPerSecond = frames;

		if (mode == TC_SMPTE_CD) {

#if defined(CONFORM_ALL_CD_TIMES_TO_SMPTE) && CONFORM_ALL_CD_TIMES_TO_SMPTE
			/* Round to SMPTE timecode frame first */
// **CodeWizzard** - Severe Violation:  User Defined item 208  - Don't check floats for equality; check for greater than or less than
			if (value != 0.0) {
				value = RoundToNearestFrame(value);
			}
#endif /* defined(CONFORM_ALL_CD_TIMES_TO_SMPTE) && CONFORM_ALL_CD_TIMES_TO_SMPTE */

			//	The number of frames = frames per second * value in seconds
			frames = frames * value;

			//	5/6/94 -	The industry doesn't round up CD "subframes" and neither should we.
			//				In other words, "fraction" is FALSE.
			//	However, if we rounded to SMPTE above we have to do one more check...

#if defined(CONFORM_ALL_CD_TIMES_TO_SMPTE) && CONFORM_ALL_CD_TIMES_TO_SMPTE
			//	Since we rounded to SMPTE above, there can be no harm in making sure
			//	that frames does not end in .99999999 so that the subsequent truncation to
			//	integral value will not be one frame off.
			//	Without the check below, we have seen conversion errors!  An actual SMPTE
			//	time that always seemed to fail was 00:20:00:22		-	Sagar 11/11/97
			frames += (0.25 / subFrameCount);
#endif /* defined(CONFORM_ALL_CD_TIMES_TO_SMPTE) && CONFORM_ALL_CD_TIMES_TO_SMPTE */

		}
		else {
			//	The number of frames = frames per second * value in seconds
			frames = frames * value;

			//	Prevent rounding errors:
// **CodeWizzard** - Severe Violation:  User Defined item 208  - Don't check floats for equality; check for greater than or less than
			if (value != 0.0) {
				if (fraction || subFrameAsterisk) {
					//	partial frames, add 1/2 of smallest granule :
					frames += (0.5 / subFrameCount);
				}
				else {
					//	whole frames, add 1/2 frame :
					frames += 0.5;
				}
			}
		}

		// blj 9/6/99 - add 'inShowHours' check to allow SonicHD to show hours on CD times
		if (inShowHours || (mode != TC_SMPTE_CD && mode != TC_DATA_CD)) {
			/* hours */
			j = static_cast<SInt32>(frames / (framesPerSecond * 3600.0));

			if (isNegative == false)
// **CodeWizzard** - Informational:  Effective C++ item 2  - Prefer iostream.h to stdio.
//SR SC: T, I, D [JAW 20040924] - rewrite using iostreams instead of sprintf
//SR DI: DP0, E0, D0 [JAW 20040924] {dread_score 0.0} - no threat, just old fashioned
				sonic::snprintf_safe(str, strlength, "%02d:", j);
			else
// **CodeWizzard** - Informational:  Effective C++ item 2  - Prefer iostream.h to stdio.
//SR SC: T, I, D [JAW 20040924] - rewrite using iostreams instead of sprintf
//SR DI: DP0, E0, D0 [JAW 20040924] {dread_score 0.0} - no threat, just old fashioned
				sonic::snprintf_safe(str, strlength, "%c%02d:", kNegativeTimeSign, j);

			frames = frames - static_cast<float>(j * (framesPerSecond * 3600.0));

			/* minutes */
			j = static_cast<SInt32>(frames / (framesPerSecond * 60.0));
// **CodeWizzard** - Informational:  Effective C++ item 2  - Prefer iostream.h to stdio.
//SR SC: T, I, D [JAW 20040924] - rewrite using iostreams instead of sprintf
//SR DI: DP0, E0, D0 [JAW 20040924] {dread_score 0.0} - no threat, just old fashioned
			sonic::snprintf_safe(&str[3 + incr], strlength-(3 + incr), "%02d:", j);
			frames = frames - static_cast<float>(j * (framesPerSecond * 60.0));
		} else {
			/* only minutes  for CD time */
			j = static_cast<SInt32>(frames / (framesPerSecond * 60.0));
// **CodeWizzard** - Informational:  Effective C++ item 2  - Prefer iostream.h to stdio.
//SR SC: T, I, D [JAW 20040924] - rewrite using iostreams instead of sprintf
//SR DI: DP0, E0, D0 [JAW 20040924] {dread_score 0.0} - no threat, just old fashioned
			sonic::snprintf_safe(str, strlength, "   %02d:", j);
			frames = frames - static_cast<float>(j * (framesPerSecond * 60.0));
		}

		/* seconds */
		j = static_cast<SInt32>(frames / framesPerSecond);
// **CodeWizzard** - Informational:  Effective C++ item 2  - Prefer iostream.h to stdio.
//SR SC: T, I, D [JAW 20040924] - rewrite using iostreams instead of sprintf
//SR DI: DP0, E0, D0 [JAW 20040924] {dread_score 0.0} - no threat, just old fashioned
		sonic::snprintf_safe(&str[6 + incr], strlength-(6 + incr), "%02d:", j);
		frames = frames - static_cast<float>(j) * framesPerSecond;

		/* frames */
		intframes = static_cast<SInt32>(frames);
		j = static_cast<SInt32>(intframes);
// **CodeWizzard** - Informational:  Effective C++ item 2  - Prefer iostream.h to stdio.
//SR SC: T, I, D [JAW 20040924] - rewrite using iostreams instead of sprintf
//SR DI: DP0, E0, D0 [JAW 20040924] {dread_score 0.0} - no threat, just old fashioned
		sonic::snprintf_safe(&str[9 + incr], strlength-(9 + incr), "%02d", j);
		frames = frames - static_cast<float>(j);

		j = static_cast<SInt32>(frames * subFrameCount);
		if (fraction != 0) {			// add != 0 to avoid warning on Win32
			/* frames / kSubframeCount */
// **CodeWizzard** - Informational:  Effective C++ item 2  - Prefer iostream.h to stdio.
//SR SC: T, I, D [JAW 20040924] - rewrite using iostreams instead of sprintf
//SR DI: DP0, E0, D0 [JAW 20040924] {dread_score 0.0} - no threat, just old fashioned
			sonic::snprintf_safe(&str[11 + incr], strlength-(11 + incr), ".%02d", j);
		}
		else
		{
			// Asterisk form : field 1, end of string, field 2, asterisk added
			if (subFrameAsterisk)
			{
				if (j >= (subFrameCount / 2) )
// **CodeWizzard** - Informational:  Effective C++ item 2  - Prefer iostream.h to stdio.
//SR SC: T, I, D [JAW 20040924] - rewrite using iostreams instead of sprintf
//SR DI: DP0, E0, D0 [JAW 20040924] {dread_score 0.0} - no threat, just old fashioned
					::sonic::snprintf_safe(&str[11 + incr], strlength-(11 + incr), "%c", '*');
			}
		}
		break;

	case TC_SMPTE_29_DF:
	case TC_SMPTE_30_DF:	// JT.00.10.10, DF_30 not in SSHD 
		const double theDF_10MINUTES = DF_10MINUTES;		// 	NOTE: DF_10MINUTES	is SMPTE defined as exactly: 17982.0
		if (mode == TC_SMPTE_30_DF)
		{
			frames = NDF_FRAMESSECOND;	// 30.0
		}
		else
		{
			frames = DF_FRAMESSECOND;	// 30.0/1.001)
		}

		// blj 10/22/99 - this is causing a slight error in the subframe area. 
		// 01:00:02:00.00 is 3602.00000 for the 'value' in
		// Following calculations turn out 01:00:02:00.04 => 4 subframes wrong??

		frames = value * frames; /* now calculate the actual number of 
									frames implied by the floating point value */
		/* add (1/2 frame) to insure appropriate "rounding" */
		//	Prevent rounding errors:
// **CodeWizzard** - Severe Violation:  User Defined item 208  - Don't check floats for equality; check for greater than or less than
		if (value != 0.0) // JT.00.10.10, 
		{
			if (fraction || subFrameAsterisk) 
			{	//	partial frames, add 1/2 of smallest granule :
				frames += (0.5 / subFrameCount);
			}
			else 
			{	//	whole frames, add 1/2 frame :
				frames += 0.5;
			}
		}

		while (frames >= DF_1HOUR) 			// get hours 
		{
			frames -= DF_1HOUR;
			hours++;
		}

		while (frames >= theDF_10MINUTES) 	// get tens of mintues
		{
			frames -= theDF_10MINUTES;
			tens++;
		}

		/* get units of minutes */
		/* first minute has 1800, others have 1798 frames */
		if (frames >= 1800.0) 
		{
			// first minute has 1800,  meaning 00;00;00;00 and  00;00;00;01 are vaild times
			minutes  = 1;
			frames -= 1800.0;

			while (frames > 1798.00) 
			{
				minutes++;
				frames -= 1798.00;
			}
		}

		/* seconds */
		/* test if this is NOT a first minute, i.e.  NOT 00,10,20,30,40 or 50
			- in that case only 28 frames go into the first "second"*/
		if ((minutes != 0) && (frames >= 28.0)) 
		{
			seconds = 1;
			frames -= 28.0;
		}

		/* rest of "seconds" including first "second" occuring on 10 minute boundaries */
		while (frames >= 30.00) 
		{
			seconds++;
			frames -= 30.00;
		}
		minutes += tens * 10;  /* now add together the 10's and units of minutes */

		/* frames now */
		dfframes  = static_cast<SInt32>(frames);

		/* subframes */
		if (fraction || subFrameAsterisk) 
		{
			/* frames / kSubframeCount */
			tmp = frames - dfframes;
			subframes = static_cast<SInt32>(tmp * subFrameCount);
		}

	/* because drop frame "drops" the first two frames
			in each minute, except for minutes 00, 10, 20, 30, 40, & 50, 
			here we increment the frame number by 2 if we are on the
			the first or 0'th second in a minute, 
			unless we're on a 10 minute boundary */
		if ((seconds == 0) && (minutes % 10 != 0))
			dfframes += 2;

// **CodeWizzard** - Informational:  Effective C++ item 2  - Prefer iostream.h to stdio.
//SR SC: T, I, D [JAW 20040924] - rewrite using iostreams instead of sprintf
//SR DI: DP0, E0, D0 [JAW 20040924] {dread_score 0.0} - no threat, just old fashioned
		sonic::snprintf_safe(str, strlength, "%s%02d;%02d;%02d;%02d", (isNegative ? "-" : ""), hours, minutes, seconds, dfframes);

		/* add subframe number */
		if (fraction)
// **CodeWizzard** - Informational:  Effective C++ item 2  - Prefer iostream.h to stdio.
//SR SC: T, I, D [JAW 20040924] - rewrite using iostreams instead of sprintf
//SR DI: DP0, E0, D0 [JAW 20040924] {dread_score 0.0} - no threat, just old fashioned
			sonic::snprintf_safe(&str[11 + incr], strlength-(11 + incr), ".%02d", subframes);
		else
		{
			if (subFrameAsterisk)
			{
				if (subframes >= (subFrameCount / 2))
// **CodeWizzard** - Informational:  Effective C++ item 2  - Prefer iostream.h to stdio.
//SR SC: T, I, D [JAW 20040924] - rewrite using iostreams instead of sprintf
//SR DI: DP0, E0, D0 [JAW 20040924] {dread_score 0.0} - no threat, just old fashioned
					sonic::snprintf_safe(&str[11 + incr], strlength-(11 + incr), "%c", '*');
			}
		}

		break;
	} /* switch */
  
  	return(0);
}


// Deprecated!  NO NOT USE
/************************************************************************/
/* FloatToSecs: convert from floating number to seconds string.			*/
/*       		Insure that the floating number is not greater than the */
/*       		maximum allowed value.									*/
/************************************************************************/

void
FloatToSecs(double value, char *str, size_t strlength,  bool fraction)
{
	SInt32 i = static_cast<SInt32>(value);

// **CodeWizzard** - Severe Violation:  User Defined item 208  - Don't check floats for equality; check for greater than or less than
	if (i == value)
// **CodeWizzard** - Informational:  Effective C++ item 2  - Prefer iostream.h to stdio.
//SR SC: T, I, D [JAW 20040924] - rewrite using iostreams instead of sprintf
//SR DI: DP0, E0, D0 [JAW 20040924] {dread_score 0.0} - no threat, just old fashioned
		sonic::snprintf_safe(str, strlength, "%.1f", value);
	else {
		if ((fraction != 0) == true)
// **CodeWizzard** - Informational:  Effective C++ item 2  - Prefer iostream.h to stdio.
//SR SC: T, I, D [JAW 20040924] - rewrite using iostreams instead of sprintf
//SR DI: DP0, E0, D0 [JAW 20040924] {dread_score 0.0}  - no threat, just old fashioned
			sonic::snprintf_safe(str, strlength, "%.6f", value);
		else
// **CodeWizzard** - Informational:  Effective C++ item 2  - Prefer iostream.h to stdio.
//SR SC: T, I, D [JAW 20040924] - rewrite using iostreams instead of sprintf
//SR DI: DP0, E0, D0 [JAW 20040924] {dread_score 0.0}  - no threat, just old fashioned
			sonic::snprintf_safe(str, strlength, "%.4f", value);
	}
}


/********************************************************************************/
// DEPRECATED!!!
/* FloatToFF: convert from floating point number to the feet and 				*/
/*       	frames equivalent.  Make sure that the value is not greater			*/
/*       	than the maximum allowed value.										*/
/*       	             														*/
/*       	If fraction is true then add .x, where x is frames/kSubframeCount.	*/
/*       	Otherwise timecode is truncated to the frame.						*/
/********************************************************************************/

SInt32 FloatToFF(double value, char* str, size_t strlength , tc_mode mode, bool fraction)
{
	/* save if negative value */
//	UInt32 incr;					// increment for indices into string
	bool isNegative = (value < 0.0);
	if (isNegative) {
		value = -value;
//		incr = 1;
	}
//	else
//		incr = 0;

	/* Calculate the number of feet based on the number of seconds */
	double FramesPerSecond = TCFramesPerSecond(mode);
	double FramesPerFoot = TCFramesPerFoot(mode);
// **Prefix** - time_utils.cpp; Line: 671; warning (20): dividing by zero using 'FramesPerFoot'
	double FeetPerSecond = FramesPerSecond / FramesPerFoot;

#if DEBUG_FloatToFF
	fprintf(stderr,"FloatToFF: FramesPerFoot :%i ;", FramesPerFoot);
	fprintf(stderr," FeetPerSecond :%i ;", FeetPerSecond);
#endif /* DEBUG_FloatToFF */

	double totalFeet =  FeetPerSecond * value;

	SInt32 wholeFeet = static_cast<SInt32>(totalFeet);
	double tmpFrames = (totalFeet - wholeFeet) * FramesPerFoot;

		/* whole frames, add 1/2 frame */
// **CodeWizzard** - Severe Violation:  User Defined item 208  - Don't check floats for equality; check for greater than or less than
	if (!fraction && (value != 0.0))
		tmpFrames += 0.5;

	SInt32 frames = static_cast<SInt32>(tmpFrames);
	while (frames >= FramesPerFoot) {
		wholeFeet++;
		frames -= static_cast<SInt32>(FramesPerFoot);
		tmpFrames -= FramesPerFoot;
	}

	double tmpSubframes = (tmpFrames  - frames) * kSubframeCount;

		/* partial frames, add 1/2 of smallest granule */
// **CodeWizzard** - Severe Violation:  User Defined item 208  - Don't check floats for equality; check for greater than or less than
	if (fraction && (value != 0.0))
		tmpSubframes = tmpSubframes + 0.5;

	SInt32 subframes = static_cast<SInt32>(tmpSubframes);
	while (subframes >= kSubframeCount) {
		frames++;
		while (frames >= FramesPerFoot) {
			wholeFeet++;
			frames -= static_cast<SInt32>(FramesPerFoot);
		}
		subframes -= static_cast<SInt32>(kSubframeCount);
		tmpSubframes -= kSubframeCount;
	}


#if DEBUG_FloatToFF
	fprintf(stderr,"  \ntotalFeet :%g \n", totalFeet);
	fprintf(stderr,"  wholeFeet :%i \n", wholeFeet);
	fprintf(stderr,"  tmp :%g \n", tmp); 
	fprintf(stderr,"  frames :%i \n", frames);
	fprintf(stderr,"  subframes :%i \n", subframes);
#endif /* DEBUG_FloatToFF */

	if (!isNegative)
// **CodeWizzard** - Informational:  Effective C++ item 2  - Prefer iostream.h to stdio.
//SR SC: T, I, D [JAW 20040924] - rewrite using iostreams instead of sprintf
//SR DI: DP0, E0, D0 [JAW 20040924] {dread_score 0.0} - no threat, just old fashioned
		sonic::snprintf_safe(str, strlength, "%02d:%02d", wholeFeet, frames);
	else
// **CodeWizzard** - Informational:  Effective C++ item 2  - Prefer iostream.h to stdio.
//SR SC: T, I, D [JAW 20040924] - rewrite using iostreams instead of sprintf
//SR DI: DP0, E0, D0 [JAW 20040924] {dread_score 0.0} - no threat, just old fashioned
		sonic::snprintf_safe(str, strlength, "%c%02d:%02d", kNegativeTimeSign, wholeFeet, frames);

		/* add subframe number */
	if (fraction)
// Bug#2989 98/02/26		sprintf(&str[strlen(str) + incr], ".%02d", subframes);
// **CodeWizzard** - Informational:  Effective C++ item 2  - Prefer iostream.h to stdio.
//SR SC: T, I, D [JAW 20040924] - rewrite using iostreams instead of sprintf
//SR DI: DP0, E0, D0 [JAW 20040924] {dread_score 0.0} - no threat, just old fashioned
		sonic::snprintf_safe(&str[strlen(str)], strlength-strlen(str), ".%02d", subframes);

  	return 0;
}


// Deprecated!  NO NOT USE
/************************************************************************/
/* KindOfTimeCode:      												*/
/* 		returns the mode of time code string:							*/
/* 				TC_UNKNOWN -	can not figure it out.					*/
/* 				TC_SECONDS -	seconds									*/
/* 				TC_SMPTE_30_NDF, TC_SMPTE_29_DF - SMPTE (uses global flag to  */
/* 											distinguish NDF from DF.)	*/
/************************************************************************/

SInt32 KindOfTimeCode(const char* str, tc_mode sysMode)
{
	const char *tmpStr = 0;
	SInt32	tcType, nPeriods, nMinuses, nBullets;

	nPeriods = 0;
	nMinuses = 0;
	nBullets = 0;

	// look for only legal characters: '?, ' ', ':', ';', '.' and numbers */
	/* only allow 'S' or 's'  at end */
	for (size_t i = 0; i < strlen(str); i++) {
		if (str[i] == '.')
			++nPeriods;
		else if (str[i] == '?)
			++nBullets;
		else if ((str[i] == '?) || (str[i] == '-'))
			++nMinuses;

		if (((str[i] < '0')  ||
		     (str[i] > '9')) &&
	 	    ((str[i] != ':') &&
			 (str[i] != ';') &&
			 (str[i] != '.') &&
			 (str[i] != '?) &&
			 (str[i] != '-') &&
			 (str[i] != '?) &&
			 (str[i] != ' ')))
		{
			if ((i + 1 == strlen(str)) &&
			    (str[i] != 'S') &&
				(str[i] != 's'))
			return(TC_UNKNOWN);
		}
	}
	if (	(nPeriods > 1)
		||	(nBullets > 1)
		||	(nMinuses > 1)
		)
		return(TC_UNKNOWN);

	/* test for samples */
	/* only allowed at end */
	if ((strchr(str, 's') != 0) ||
	    (strchr(str, 'S') != 0))
	{
		return(TC_SAMPLES);
	}

	/* based on global TimeCode flag return SMPTE mode	*/
	// tcType = (SInt32) (**gSonicConfig).timeDisplay;
	tcType = static_cast<SInt32>(sysMode);

	/* look for ';' for Drop Frame */
	tmpStr = strchr(const_cast<char*>(str), ';');
	if (tmpStr != 0)
	{
		if (tcType == TC_SMPTE_30_DF)	/* use screw case only if the global is set to the screw case*/
			return(TC_SMPTE_30_DF);
		else 
			return(TC_SMPTE_29_DF);
	}

	/* look for ':' and '.' */
	tmpStr = strchr(const_cast<char*>(str), ':');

	/* there is a ':' in string, check for no '.' */
	if (tmpStr != 0) {
		if (nPeriods > 0)
			return(TC_UNKNOWN);

		/* if flag is DF then return it */
		if (	tcType == TC_SMPTE_29_DF	|| 
				tcType == TC_SMPTE_CD	||
				tcType == TC_DATA_CD	||
				tcType == TC_SMPTE_23976	||
				tcType == TC_SMPTE_24	||
				tcType == TC_SMPTE_25	||
				tcType == TC_SMPTE_29_NDF ||
				tcType == TC_SMPTE_30_DF ||
				tcType == TC_SMPTE_50	||
				tcType == TC_SMPTE_5994	||
				tcType == TC_SMPTE_60	||
				tcType == TC_16FF		||
				tcType == TC_16MM_24	||
				tcType == TC_16MM_25	||
				tcType == TC_16MM_29	||
				tcType == TC_16MM_30	||
				tcType == TC_35FF		||
				tcType == TC_35MM_24	||
				tcType == TC_35MM_25	||
				tcType == TC_35MM_29	||
				tcType == TC_35MM_30)
		{
			return(tcType);
		}

		/* if not any SMPTE mode then assume NDF */
		return(TC_SMPTE_30_NDF);
	}

	/* there are no ':' or ';' then look for seconds */

	if (nPeriods > 0) {
		/* can not have a '? in a seconds string */
		if (nBullets > 0)
			return(TC_UNKNOWN);

		/* return seconds */
		return(TC_SECONDS);
	}

	/* unless explictly SAMPLES, no ':', ';',  or '.' - assume seconds */
	if (tcType == TC_SAMPLES)
		return(tcType);

	return(TC_SECONDS);

} /* KindOfTimeCode */



// Deprecated - DO NOT USE

void Seconds2TCString(const Seconds &theValue, char* theString, size_t strlength, tc_mode timecodePref, bool showOddField)
{
	// Now written in terms of common code
	std::string theSTLString;
	Seconds2TCString(theValue, theSTLString, timecodePref, showOddField);

	sonic::strlcpy(theString, theSTLString.c_str(), strlength);
}

// Deprecated - DO NOT USE
// SonicError mpeg_audio_stream_file::parse_timestamp_string(char* str)
SInt32 parse_timestamp_string(const char* stamp_str, double* time_stamp, tc_mode* timecode_mode)
{
	*timecode_mode = TC_UNKNOWN;

	const char *search = stamp_str;
	SInt32 i;

	for (i = 0; (*search != '\n') && (*search != '\r'); search++, i++)
	{
		if (*search == '\0')
			return -1;
	}
	i++;			// Include the EOL
	search++;

	if ((i == PAL_TC_DESC_LEN) && (strncmp(stamp_str, PAL_TC_DESC, PAL_TC_DESC_LEN-1) == 0))
	{
		*timecode_mode = TC_SMPTE_25;
	}
	else if ((i == NDF_29_TC_DESC_LEN) && (strncmp(stamp_str, NDF_29_TC_DESC, NDF_29_TC_DESC_LEN-1) == 0))
	{
		*timecode_mode = TC_SMPTE_29_NDF;
	}
	else if ((i == DF_29_TC_DESC_LEN) && (strncmp(stamp_str, DF_29_TC_DESC, DF_29_TC_DESC_LEN-1) == 0))
	{
		*timecode_mode = TC_SMPTE_29_DF;
	}
	else if ((i == NDF_30_TC_DESC_LEN) && (strncmp(stamp_str, NDF_30_TC_DESC, NDF_30_TC_DESC_LEN-1) == 0))
	{
		*timecode_mode = TC_SMPTE_30_NDF;
	}
	else if ((i == DF_30_TC_DESC_LEN) && (strncmp(stamp_str, DF_30_TC_DESC, DF_30_TC_DESC_LEN-1) == 0))
	{
		*timecode_mode = TC_SMPTE_30_DF;
	}
	else
		return -1;

	return static_cast<SInt32>(TCToFloat(time_stamp, search, *timecode_mode));
}


// Deprecated - DO NOT USE
SInt32 get_timestamp_string(double value, tc_mode mode, char* stamp_str, size_t stamp_str_length)
{
	char tc_string[64];
	tc_string[0] = 0;

	SInt32 retval = FloatToTC(value, tc_string,64, mode, true);
	if (retval)
		return retval;

	*stamp_str = '\0';
	switch(mode)
	{
	case TC_SMPTE_25 :
		sonic::strlcpy(stamp_str, PAL_TC_DESC, stamp_str_length);
		break;
	case TC_SMPTE_29_NDF :
		sonic::strlcpy(stamp_str, NDF_29_TC_DESC, stamp_str_length);
		break;
	case TC_SMPTE_29_DF :
		sonic::strlcpy(stamp_str, DF_29_TC_DESC, stamp_str_length);
		break;
	case TC_SMPTE_30_NDF :
		sonic::strlcpy(stamp_str, NDF_30_TC_DESC, stamp_str_length);
		break;
	case TC_SMPTE_30_DF :
		sonic::strlcpy(stamp_str, DF_30_TC_DESC, stamp_str_length);
		break;
	default:
		break;
	}

	sonic::strlcat(stamp_str, tc_string, stamp_str_length);
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif
