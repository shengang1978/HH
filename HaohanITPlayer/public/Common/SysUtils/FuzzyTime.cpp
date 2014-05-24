//-----------------------------------------------------------------------------
// FuzzyTime.cpp
// Copyright (c) 1998 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------
/*
	File:		FuzzyTime.cp

	Contains:	Class to allow time comparisons within some epsilon.
				Used to deal with floating point rounding errors.

	Written by:	Jim Roth

	Change History (most recent first):

				11/18/98	jmr		Initial file creation.
*/

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040915]: safe input param; no MBCS; no dangerous API; no registry/sys folder/temp file 

#include "FuzzyTime.h"

// Initialize the default to about 1 sample time at the highest
//		sampling rate we support which is 192000
//	1 / 192000 = .000005208
const double FuzzyTime::kDefaultEpsilon = 0.000005;


#pragma mark -- constructor/destructor


/**************************************************************************
	FuzzyTime()														[public]

	constructors
**************************************************************************/
FuzzyTime::FuzzyTime(
	double inTime,
	double inEpsilon)
:	fTime(inTime),
	fEpsilon(inEpsilon)
{
}

FuzzyTime::FuzzyTime(
	const FuzzyTime& copyMe)
:	fTime(copyMe.fTime),
	fEpsilon(copyMe.fEpsilon)
{
}

FuzzyTime&
FuzzyTime::operator=(
	const FuzzyTime& copyMe)
{
	if (&copyMe == this) 
		return *this;
	fTime = copyMe.fTime;
	fEpsilon = copyMe.fEpsilon;

	return *this;
}

/**************************************************************************
	~FuzzyTime()													[public]

	destructor
**************************************************************************/
FuzzyTime::~FuzzyTime()
{
}

#pragma mark -
#pragma mark -- operator overloads

/**************************************************************************
	operator==()													[non-member]

**************************************************************************/
bool
operator==(const FuzzyTime& inLHS, const FuzzyTime& inRHS)
{
	bool isEqual = false;

	// See if the epsilon intervals overlap
	if (inRHS.fTime > inLHS.fTime) {
		if (inRHS.fTime - inRHS.fEpsilon <= inLHS.fTime + inLHS.fEpsilon)
			isEqual = true;
	}
	else {
		if (inRHS.fTime + inRHS.fEpsilon >= inLHS.fTime - inLHS.fEpsilon)
			isEqual = true;
	}
	return isEqual;
}

/**************************************************************************
	operator<()														[non-member]

**************************************************************************/
bool
operator<(const FuzzyTime& inLHS, const FuzzyTime& inRHS)
{
	bool isLessThan = (inLHS.fTime < inRHS.fTime && !(inLHS == inRHS));
	return isLessThan;
}

//
// The following are all rerived from operator< and operator==
//

/**************************************************************************
	operator<=()													[non-member]

**************************************************************************/
bool
operator<=(const FuzzyTime& inLHS, const FuzzyTime& inRHS)
{
	bool isLessThanEqual = (inLHS < inRHS || inLHS == inRHS);
													 
	return isLessThanEqual;													 
}

/**************************************************************************
	operator>()														[non-member]

**************************************************************************/
bool
operator>(const FuzzyTime& inLHS, const FuzzyTime& inRHS)
{
	return !(inLHS <= inRHS);
}

/**************************************************************************
	operator>=()													[non-member]

**************************************************************************/
bool
operator>=(const FuzzyTime& inLHS, const FuzzyTime& inRHS)
{
	return !(inLHS < inRHS);												 
}

/**************************************************************************
operator!=()														[non-member]

**************************************************************************/
bool
operator!=(const FuzzyTime& inLHS, const FuzzyTime& inRHS)
{
	return !(inLHS == inRHS);
}

/**************************************************************************
operator!()															[public]

**************************************************************************/
bool
FuzzyTime::operator!() const
{
	return *this == FuzzyTime(0., fEpsilon);
}

//
#pragma mark -
#pragma mark -- accessors
//

/**************************************************************************
	AsDouble()														[public]

	accessor to fTime
**************************************************************************/
double
FuzzyTime::AsDouble() const
{
	return fTime;
}

/**************************************************************************
	SetDouble()														[public]

	accessor to fTime
**************************************************************************/
void
FuzzyTime::SetDouble(double inTime)
{
	fTime = inTime;
}

/**************************************************************************
	GetEpsilon()													[public]

	accessor to fEpsilon
**************************************************************************/
double
FuzzyTime::GetEpsilon() const
{
	return fEpsilon;
}

/**************************************************************************
	SetEpsilon()													[public]

	accessor to fEpsilon
**************************************************************************/
void
FuzzyTime::SetEpsilon(double inEpsilon)
{
	fEpsilon = inEpsilon;
}

//
#pragma mark -
#pragma mark -- math
//

/**************************************************************************
operator+()															[non-member]

**************************************************************************/
const FuzzyTime	operator+(const FuzzyTime& inLHS, const FuzzyTime& inRHS)
{
	return FuzzyTime(inLHS) += inRHS;
}

/**************************************************************************
operator-()															[non-member]

**************************************************************************/
const FuzzyTime	operator-(const FuzzyTime& inLHS, const FuzzyTime& inRHS)
{
	return FuzzyTime(inLHS) -= inRHS;
}

/**************************************************************************
operator*()															[non-member]

**************************************************************************/
const FuzzyTime	operator*(const FuzzyTime& inLHS, const FuzzyTime& inRHS)
{
	return FuzzyTime(inLHS) *= inRHS;
}

/**************************************************************************
operator/()															[non-member]

**************************************************************************/
const FuzzyTime	operator/(const FuzzyTime& inLHS, const FuzzyTime& inRHS)
{
	return FuzzyTime(inLHS) /= inRHS;
}
