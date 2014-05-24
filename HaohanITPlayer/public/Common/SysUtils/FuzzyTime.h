//-----------------------------------------------------------------------------
// FuzzyTime.h
// Copyright (c) 1998 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------
/*
	File:		FuzzyTime.h

	Contains:	Wrapper class for times kept by segments, fades, etc.  Rounding errors
				make if almost impossible to determine when two segment/fade times are
				equal.  This class allows for a "fudge-factor" when comparing two times.

	Written by:	Jim Roth

	Change History (most recent first):

				11/18/98	jmr		Initial file creation.
*/

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040915]: safe input param; no MBCS; no dangerous API; no registry/sys folder/temp file 
#ifndef __FuzzyTime_h__
#define __FuzzyTime_h__

class FuzzyTime
// **CodeWizzard** - Violation:  Universal Coding Standards item 16  - Do not specify public data member
{
public:
	static const double kDefaultEpsilon;

// **CodeWizzard** - Informational:  More Effective C++ item 5  - Be wary of user-defined conversion function
	FuzzyTime(double inTime = 0.0, double inEpsilon = kDefaultEpsilon);
	FuzzyTime(const FuzzyTime&);
	FuzzyTime& operator=(const FuzzyTime&);
	virtual	~FuzzyTime();

	double	AsDouble() const;

	void	SetDouble(double inTime);

	double	GetEpsilon() const;

	void	SetEpsilon(double inEpsilon);

	bool	operator! () const;

	FuzzyTime&	operator+=(const FuzzyTime& inRHS)		{ fTime += inRHS.fTime; return *this; }
	FuzzyTime&	operator-=(const FuzzyTime& inRHS)		{ fTime -= inRHS.fTime; return *this; }
	FuzzyTime&	operator*=(const FuzzyTime& inRHS)		{ fTime *= inRHS.fTime; return *this; }
	FuzzyTime&	operator/=(const FuzzyTime& inRHS)		{ fTime /= inRHS.fTime; return *this; }

	// Unary operators
	const FuzzyTime	operator+() { return FuzzyTime(*this); }
	const FuzzyTime	operator-() { return FuzzyTime(-fTime, fEpsilon); }

	// Declare friends
	friend bool operator==(const FuzzyTime& inLHS, const FuzzyTime& inRHS);
	friend bool operator<(const FuzzyTime& inLHS, const FuzzyTime& inRHS);
	friend const FuzzyTime operator-(const FuzzyTime& inLHS);

private:

	double	fTime;		// The actual time that this class is to represent.

	double	fEpsilon;	// The "fudge-factor" that comparison
						// operators use when determining if
						// two times are close enough together
						// to be considered equal.

};

// non-member operators to support lhs implicit type conversions
bool operator==(const FuzzyTime& inLHS, const FuzzyTime& inRHS);
bool operator!=(const FuzzyTime& inLHS, const FuzzyTime& inRHS);
bool operator<(const FuzzyTime& inLHS, const FuzzyTime& inRHS);
bool operator>(const FuzzyTime& inLHS, const FuzzyTime& inRHS);
bool operator<=(const FuzzyTime& inLHS, const FuzzyTime& inRHS);
bool operator>=(const FuzzyTime& inLHS, const FuzzyTime& inRHS);
const FuzzyTime	operator+(const FuzzyTime& inLHS, const FuzzyTime& inRHS);
const FuzzyTime	operator-(const FuzzyTime& inLHS, const FuzzyTime& inRHS);
const FuzzyTime	operator*(const FuzzyTime& inLHS, const FuzzyTime& inRHS);
const FuzzyTime	operator/(const FuzzyTime& inLHS, const FuzzyTime& inRHS);

#endif

