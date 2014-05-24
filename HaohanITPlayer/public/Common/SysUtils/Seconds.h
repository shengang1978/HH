//-----------------------------------------------------------------------------
// Seconds.h
// Copyright (c) 1997 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------
// Seconds.h

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#ifndef __Seconds__
#define __Seconds__

#include "FuzzyTime.h"
#include "time_const.h"

class Seconds : public FuzzyTime
{
public :
	Seconds(double inSeconds = 0., double inEpsilon = FuzzyTime::kDefaultEpsilon);
	Seconds(const std::string &theString, tc_mode timecodePref);

	// Note: most of the operators are available from the FuzzyTime base class, however,
	// the non-member +-*/ operators (declared below) and thus the supporting +=, -=, *=, and /=
	// are defined here to support "a + b" operations where the result is a new Seconds object.
	Seconds& operator+=(const Seconds& inRHS) { FuzzyTime::operator+=(inRHS); return *this; }
	Seconds& operator-=(const Seconds& inRHS) { FuzzyTime::operator-=(inRHS); return *this; }
	Seconds& operator*=(const Seconds& inRHS) { FuzzyTime::operator*=(inRHS); return *this; }
	Seconds& operator/=(const Seconds& inRHS) { FuzzyTime::operator/=(inRHS); return *this; }

	const Seconds operator+()                 { return Seconds(*this); }
	const Seconds operator-()                 { return Seconds(-AsDouble(), GetEpsilon()); }

	// Convert this to a timecode string
	void GetTCString(std::string & theString, tc_mode timecodePref, bool showOddField = true) const;

	// Parse a timecode string to set this value. This method throws when pOutSuccess is zero,
	// otherwise it returns false if any errors (such as unknown tc_mode, invalid time string, etc).
	void ParseTCString(const std::string &theString, tc_mode timecodePref, bool *pOutSuccess = 0);
};

// non-member functions
inline const Seconds operator+(const Seconds& inLHS, const Seconds& inRHS) { return Seconds(inLHS) += inRHS; }
inline const Seconds operator-(const Seconds& inLHS, const Seconds& inRHS) { return Seconds(inLHS) -= inRHS; }
inline const Seconds operator*(const Seconds& inLHS, const Seconds& inRHS) { return Seconds(inLHS) *= inRHS; }
inline const Seconds operator/(const Seconds& inLHS, const Seconds& inRHS) { return Seconds(inLHS) /= inRHS; }

// For debugging and test harness
inline std::ostream& operator<<(std::ostream& ioStream, const Seconds& inSeconds) { ioStream << inSeconds.AsDouble(); return ioStream; }

#endif
