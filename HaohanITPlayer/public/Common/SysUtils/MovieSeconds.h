//-----------------------------------------------------------------------------
// MovieSeconds.h
// Copyright (c) 1997 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#ifndef __MovieSeconds__
#define __MovieSeconds__

#include "time_const.h"
#include "Seconds.h"
#include "video_const.h"
#include "video_defs.h"


typedef enum {
	timeZero	= 0,
	timeNDef	= 1
} movieSecondInitializer;

class MovieSeconds
{
private:
	Seconds		fFieldDuration;
	Fields		fFields;
	bool		fInitialized;
	eVideoRate	fVideoRate;
	tc_mode		fTCMode;

public:
	// Use the following constructor to explicitly make uninitialized MovieSeconds
	// When you know they will be set before they are used.
	// Or set initialized to true to set a time of zero.
	explicit MovieSeconds();

	explicit MovieSeconds(movieSecondInitializer initializer);
	explicit MovieSeconds(const Seconds &time, const Seconds &fieldDuration);
	explicit MovieSeconds(const Seconds &time, eVideoRate theVideoRate);
	explicit MovieSeconds(const Seconds &time, tc_mode tcMode);
	explicit MovieSeconds(double time, eVideoRate theVideoRate);
	explicit MovieSeconds(double time, tc_mode tcMode);
	explicit MovieSeconds(Fields numFields, eVideoRate theVideoRate);
	explicit MovieSeconds(Fields numFields, tc_mode tcMode);
	virtual ~MovieSeconds();

	bool		IsValid();

	Seconds		GetSeconds() const;
	bool		NotZero();
	bool		IsZero();
	bool		IsNegative();
	bool		IsPositive();
	double		AsDouble() const;
	std::string	AsString() const;
	void		Set(const Seconds &time, eVideoRate theVideoRate, tc_mode tcMode);
	void		Set(const Fields &numFields, eVideoRate theVideoRate, tc_mode tcMode);
	void		Set(const Seconds &time, const Seconds &fieldDuration);

	eVideoRate GetVideoRate() const {return fVideoRate;}

	bool operator == (const MovieSeconds& inB) const;
	bool operator != (const MovieSeconds& inB) const;
	bool operator <  (const MovieSeconds& inB) const;
	bool operator <= (const MovieSeconds& inB) const;
	bool operator >  (const MovieSeconds& inB) const;
	bool operator >= (const MovieSeconds& inB) const;
	bool operator !   () 				 	 const;

	MovieSeconds & operator += (const MovieSeconds & rhs);
	MovieSeconds & operator -= (const MovieSeconds & rhs);

	// return newly constructed objects
// **CodeWizzard** - Violation:  Effective C++ item 19  - Differentiate among member functions, global functions and friend function
	MovieSeconds operator + (const MovieSeconds & inB) const;
// **CodeWizzard** - Violation:  Effective C++ item 19  - Differentiate among member functions, global functions and friend function
	MovieSeconds operator - (const MovieSeconds & inB) const;
	// Unary negate
// **CodeWizzard** - Violation:  Effective C++ item 19  - Differentiate among member functions, global functions and friend function
	MovieSeconds operator - ();
	// MovieSeconds times or divided by double yields MovieSeconds
// **CodeWizzard** - Violation:  Effective C++ item 19  - Differentiate among member functions, global functions and friend function
	MovieSeconds operator * (const double inB)	const;
// **CodeWizzard** - Violation:  Effective C++ item 19  - Differentiate among member functions, global functions and friend function
	MovieSeconds operator / (const double inB)	const;

	void SetFieldsFromSeconds(const Seconds &time);
		// Set fFields from s and fFieldDuration
	void AddFields(const Fields &numFields);
		// Adds numFields to the MovieSeconds
	SInt32 GetFields() const;
		// returns # of fields
	void SetFields(const Fields &numFields);
		// Sets the # of fields
	void RoundToFrame();
		// Rounds to the nearest frame

	static double FrameRateInSeconds(const eVideoRate rate);

};



	std::ostream& operator<< (std::ostream& os, const MovieSeconds& x);

#endif

