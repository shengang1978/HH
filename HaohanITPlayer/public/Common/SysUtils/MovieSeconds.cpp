//-----------------------------------------------------------------------------
// MovieSeconds.cpp
// Copyright (c) 1997 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#include <algorithm>

#include "MovieSeconds.h"
#include <iostream>
#include "time_utils.h"
#include "MathMacros.h"
#include "SonicException.h"

MovieSeconds::MovieSeconds()
	: fFieldDuration(0.)
	, fFields(0)
	, fInitialized(false)
	, fVideoRate(eVideoRateUnknown)
	, fTCMode(TC_UNKNOWN)
{
}

MovieSeconds::MovieSeconds(movieSecondInitializer initializer)
	: fFieldDuration(0.)
	, fFields(0)
	, fInitialized(false)
	, fVideoRate(eVideoRateUnknown)
	, fTCMode(TC_UNKNOWN)
{
	if (initializer == timeZero)
		Set(0, eVideoRate525_60, ::VideoRateToTCMode(eVideoRate525_60));
}

MovieSeconds::MovieSeconds(const Seconds &time, const Seconds &fieldDuration)
	: fFieldDuration(0.)
	, fFields(0)
	, fInitialized(false)
	, fVideoRate(eVideoRateUnknown)
	, fTCMode(TC_UNKNOWN)
{
	Set(time, fieldDuration);
}

MovieSeconds::MovieSeconds(const Seconds &time, eVideoRate theVideoRate)
	: fFieldDuration(0.)
	, fFields(0)
	, fInitialized(false)
	, fVideoRate(eVideoRateUnknown)
	, fTCMode(TC_UNKNOWN)
{
	Set(time, theVideoRate, ::VideoRateToTCMode(theVideoRate));
}

MovieSeconds::MovieSeconds(const Seconds &time, tc_mode tcMode)
	: fFieldDuration(0.)
	, fFields(0)
	, fInitialized(false)
	, fVideoRate(eVideoRateUnknown)
	, fTCMode(TC_UNKNOWN)
{
	Set(time, TCModeToVideoRate(tcMode), tcMode);
}

MovieSeconds::MovieSeconds(double time, eVideoRate theVideoRate)
	: fFieldDuration(0.)
	, fFields(0)
	, fInitialized(false)
	, fVideoRate(eVideoRateUnknown)
	, fTCMode(TC_UNKNOWN)
{
	Set(Seconds(time), theVideoRate, ::VideoRateToTCMode(theVideoRate));
}

MovieSeconds::MovieSeconds(double time, tc_mode tcMode)
	: fFieldDuration(0.)
	, fFields(0)
	, fInitialized(false)
	, fVideoRate(eVideoRateUnknown)
	, fTCMode(TC_UNKNOWN)
{
	Set(Seconds(time), TCModeToVideoRate(tcMode), tcMode);
}

MovieSeconds::MovieSeconds(Fields numFields, eVideoRate theVideoRate)
	: fFieldDuration(0.)
	, fFields(0)
	, fInitialized(false)
	, fVideoRate(eVideoRateUnknown)
	, fTCMode(TC_UNKNOWN)
{
	Set(numFields, theVideoRate, ::VideoRateToTCMode(theVideoRate));
}

MovieSeconds::MovieSeconds(Fields numFields, tc_mode tcMode)
: fFieldDuration(0.)
, fFields(0)
, fInitialized(false)
, fVideoRate(eVideoRateUnknown)
, fTCMode(TC_UNKNOWN)
{
	Set(numFields, TCModeToVideoRate(tcMode), tcMode);
}

MovieSeconds::~MovieSeconds() 
{
}

bool MovieSeconds::IsValid()
{
	return fInitialized;
}

Seconds MovieSeconds::GetSeconds() const
{
	assert(fInitialized);

	return fFieldDuration * fFields;
}

double MovieSeconds::AsDouble() const
{
	assert(fInitialized);

	return GetSeconds().AsDouble();
}

bool MovieSeconds::NotZero()
{
	assert(fInitialized);

	return (fFields != 0);
}

bool MovieSeconds::IsZero()
{
	assert(fInitialized);

	return (fFields == 0);
}

bool MovieSeconds::IsPositive()
{
	assert(fInitialized);

	return (fFields > 0);
}


bool MovieSeconds::IsNegative()
{
	assert(fInitialized);

	return (fFields < 0);
}

void MovieSeconds::Set(const Seconds &time, const Seconds &fieldDuration)
{
	double	dif23967=std::abs(fieldDuration.AsDouble() - videoFieldRate23976);	// 23.976
	double	dif24  = std::abs(fieldDuration.AsDouble() - videoFieldRate6_24);	// 24
	double	dif525 = std::abs(fieldDuration.AsDouble() - videoFieldRate525_60);	// 25
	double	dif625 = std::abs(fieldDuration.AsDouble() - videoFieldRate625_50);	// 29.97
	double	dif630 = std::abs(fieldDuration.AsDouble() - videoFieldRate630_60);	// 30
	double	dif50  = std::abs(fieldDuration.AsDouble() - videoFieldRate50);		// 50
	double	dif5994= std::abs(fieldDuration.AsDouble() - videoFieldRate5994);	// 59.94
	double	dif60  = std::abs(fieldDuration.AsDouble() - videoFieldRate60);		// 60
	double compareDiff = 1;

	if (dif23967 < .001)
	{
		compareDiff = dif23967;
		fInitialized = true;
		fVideoRate = eVideoRate23976;
	}
	else if (dif24 < .001)
	{
		compareDiff = dif24;
		fInitialized = true;
		fVideoRate = eVideoRate6_24;
	}
	else if (dif525 < .001)
	{
		compareDiff = dif525;
		fInitialized = true;
		fVideoRate = eVideoRate525_60;
	}
	else if (dif625 < .001)
	{
		compareDiff = dif625;
		fInitialized = true;
		fVideoRate = eVideoRate625_50;
	}
	else if (dif630 < .001)
	{
		compareDiff = dif630;
		fInitialized = true;
		fVideoRate = eVideoRate630_60;
	}
	else if (dif50 < .001)
	{
		compareDiff = dif50;
		fInitialized = true;
		fVideoRate = eVideoRate50;
	}
	else if (dif5994 < .001)
	{
		compareDiff = dif5994;
		fInitialized = true;
		fVideoRate = eVideoRate5994;
	}
	else if (dif60 < .001)
	{
		compareDiff = dif60;
		fInitialized = true;
		fVideoRate = eVideoRate60;
	}

	assert(fInitialized);
	if (fInitialized)
	{
		if (compareDiff > .0000001)
		{
			assert(0);
			std::cout << "Suspicious field duration " << fieldDuration.AsDouble() << std::endl;
		}

		fFieldDuration = fieldDuration;
		SetFieldsFromSeconds(time);
		fTCMode = ::VideoRateToTCMode(fVideoRate);
	}
}

void MovieSeconds::Set(const Seconds &time, eVideoRate theVideoRate, tc_mode tcMode)
{
	fVideoRate = theVideoRate;
	fFieldDuration = VideoRateToFieldDuration(theVideoRate);

	fInitialized = true;
	SetFieldsFromSeconds(time);
	fTCMode = tcMode;
}

void MovieSeconds::Set(const Fields &numFields, eVideoRate theVideoRate, tc_mode tcMode)
{
	fVideoRate = theVideoRate;
	fFieldDuration = VideoRateToFieldDuration(theVideoRate);

	fInitialized = true;
	SetFields(numFields);
	fTCMode = tcMode;
}

void MovieSeconds::SetFields(const Fields &numFields)
{
	assert(fInitialized);

	fFields = numFields;
}

SInt32 MovieSeconds::GetFields() const
{
	assert(fInitialized);

	return fFields;
}

void MovieSeconds::RoundToFrame()
{
	assert(fInitialized);

	// If odd number of fields, add 1
	if (fFields % 2 == 1)
		fFields++;
}


void MovieSeconds::SetFieldsFromSeconds(const Seconds &time)
{
	assert(fInitialized);

	fFields = static_cast<SInt32>(ROUND((time / fFieldDuration).AsDouble()));
}

void MovieSeconds::AddFields(const Fields &numFields)
{
	assert(fInitialized);

	fFields += numFields;
}

bool MovieSeconds::operator == (const MovieSeconds& inB) const
{
	assert(fInitialized);
	assert(inB.fInitialized);
	assert(fVideoRate == inB.fVideoRate);

	return (fFields == inB.fFields);
}

bool MovieSeconds::operator != (const MovieSeconds& inB) const
{
	assert(fInitialized);
	assert(inB.fInitialized);
	assert(fVideoRate == inB.fVideoRate);

	return (fFields != inB.fFields);
}

bool MovieSeconds::operator < (const MovieSeconds& inB) const
{
	assert(fInitialized);
	assert(inB.fInitialized);
	assert(fVideoRate == inB.fVideoRate);

	return (fFields < inB.fFields);
}

bool MovieSeconds::operator <= (const MovieSeconds& inB) const
{
	assert(fInitialized);
	assert(inB.fInitialized);
	assert(fVideoRate == inB.fVideoRate);

	return (fFields <= inB.fFields);
}


bool MovieSeconds::operator > (const MovieSeconds& inB) const
{
	assert(fInitialized);
	assert(inB.fInitialized);
	assert(fVideoRate == inB.fVideoRate);

	return (fFields > inB.fFields);
}

bool MovieSeconds::operator >= (const MovieSeconds& inB) const
{
	assert(fInitialized);
	assert(inB.fInitialized);
	assert(fVideoRate == inB.fVideoRate);

	return (fFields >= inB.fFields);
}

bool MovieSeconds::operator ! () const
{
	assert(fInitialized);

	return (fFields == 0);
}

MovieSeconds& MovieSeconds::operator += (const MovieSeconds & rhs)
{
	assert(fInitialized);
	assert(rhs.fInitialized);
	assert(fVideoRate == rhs.fVideoRate);

	fFields += rhs.fFields;

	return *this;
}

MovieSeconds& MovieSeconds::operator -= (const MovieSeconds & rhs)
{
	assert(fInitialized);
	assert(rhs.fInitialized);
	assert(fVideoRate == rhs.fVideoRate);

	fFields -= rhs.fFields;

	return *this;
}

MovieSeconds MovieSeconds::operator + (const MovieSeconds & inB) const
{
	assert(fInitialized);
	assert(inB.fInitialized);
	assert(fVideoRate == inB.fVideoRate);

	MovieSeconds	outMovieSeconds = *this;

	outMovieSeconds.Set(fFields + inB.fFields, fVideoRate, fTCMode);

	return outMovieSeconds;
}

MovieSeconds MovieSeconds::operator - (const MovieSeconds & inB) const
{
	assert(fInitialized);
	assert(inB.fInitialized);
	assert(fVideoRate == inB.fVideoRate);

	MovieSeconds	outMovieSeconds =  *this;

	outMovieSeconds.Set(fFields - inB.fFields, fVideoRate, fTCMode);

	return outMovieSeconds;
}

MovieSeconds MovieSeconds::operator- ()
{
	assert(fInitialized);;

	MovieSeconds	outMovieSeconds =  *this;

	outMovieSeconds.Set(-fFields, fVideoRate, fTCMode);

	return outMovieSeconds;
}

MovieSeconds MovieSeconds::operator* (const double inB) const
{
	assert(fInitialized);

	MovieSeconds	outMovieSeconds =  *this;
	Seconds			timeInSeconds = GetSeconds();

	timeInSeconds = timeInSeconds * inB;

	outMovieSeconds.Set(timeInSeconds, fVideoRate, fTCMode);

	return outMovieSeconds;
}

MovieSeconds MovieSeconds::operator/ (const double inB) const
{
	assert(fInitialized);

	MovieSeconds	outMovieSeconds = *this;
	Seconds			timeInSeconds = GetSeconds();

	timeInSeconds = timeInSeconds / inB;

	outMovieSeconds.Set(timeInSeconds, fVideoRate, fTCMode);

	return outMovieSeconds;
}

std::string MovieSeconds::AsString() const
{
	Seconds curSeconds = GetSeconds();
	std::string theString;
	curSeconds.GetTCString(theString, fTCMode, true);

	return theString;
}

double MovieSeconds::FrameRateInSeconds(const eVideoRate rate)
{
	double fps = 0.0;
	switch(rate)
	{
		case eVideoRate525_60:
			fps = 30.0/1.001;
			break;
		case eVideoRate625_50:
			fps = 25.0;
			break;
		case eVideoRate630_60:
			fps = 30.0;
			break;
		case eVideoRate6_24:
			fps = 24.0;
			break;
		case eVideoRate23976:
			fps = 24.0/1.001;
			break;
		case eVideoRateUnknown:
			fps = 0.0;
			break;
		case eVideoRate5994:
			fps = 60.0/1.001;
			break;
		case eVideoRate50:
			fps = 50.0;
			break;
		case eVideoRate60:
			fps = 60.0;
			break;

	}

	return fps;
}

std::ostream& operator<< (std::ostream&os, const MovieSeconds& x)
{
	os << x.AsString() << "  (" << x.AsDouble() << "  " << x.GetFields() << ")";
	return os;
}
