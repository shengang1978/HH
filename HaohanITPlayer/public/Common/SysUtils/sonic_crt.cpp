///////////////////////////////////////////////////////////////////////////////
// sonic_crt.cpp
// Copyright (c) 2005, Haohanit.  All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#include "sonic_crt.h"
#include "UnicodeUtilities.h"

#include <cstdio>
#include <ctime>

#ifdef _WIN32
#pragma warning(disable:4996) // disable deprecation warnings
#else
#include "UnicodeUtilities.h"
#endif

char * sonic::asctime_safe(
	const tm * tm,
	__out_bcount_z(bytes) char * buffer,
	size_t bytes)
{
	return strlcpy(buffer, asctime(tm), bytes);
}

char * sonic::ctime_safe(
	const time_t * time,
	__out_bcount_z(bytes) char * buffer,
	size_t bytes)
{
	return asctime_safe(localtime(time), buffer, bytes); // per the ISO C standard
}

FILE * sonic::fopen(
	const char * path,
	const char * mode)
{
	return ::fopen(path, mode);
}

void sonic::ftime(
	timeb * t)
{

#ifdef _WIN32

	_ftime(t);

#elif defined(OSXPORT)

	// no _timezone and timezone is a function. So do it the
	// hardway
	time_t rawtime;
	struct tm * timeinfo;
	struct timeval timeval;

	time( &rawtime );
	gettimeofday(&timeval, 0);
	timeinfo = localtime(&rawtime);
	t->time = timeval.tv_sec;
	t->millitm = timeval.tv_usec/1000L;
	t->timezone = timeinfo->tm_gmtoff;
	t->dstflag = timeinfo->tm_isdst;

#else

	::ftime(t);

#endif

}

char * sonic::itoa_safe(
	SInt64 i,
	__out_bcount_z(bytes) char * buffer,
	size_t bytes,
	int radix)
{
	UInt64 ui = i;
	if (
		radix == 10
		&& i < 0)
	{
		bytes--;
		assert(bytes > 1);
		*buffer++ = '-';
		ui = -i;
	};

	return uitoa_safe(ui, buffer, bytes, radix);
}

struct tm * sonic::localtime_safe(
	const time_t * time,
	tm * result)
{

#ifdef _WIN32

	*result = *localtime(time);
	return result;

#else

	return localtime_r(time, result);

#endif

}

struct tm * sonic::gmtime_safe(
	const time_t * time,
	tm * result)
{

#ifdef _WIN32

	*result = *gmtime(time);
	return result;

#else

	return gmtime_r(time, result);

#endif

}

char * sonic::uitoa_safe(
	UInt64 ui,
	__out_bcount_z(bytes) char * buffer,
	size_t bytes,
	int radix)
{
	assert(radix >= 2 && radix <= 16);
	std::string digits;
	if (ui == 0)
		digits = "0";
	else while (ui != 0)
	{
		char digit = static_cast<char>(ui % radix);
		digit += digit <= 9 ? '0' : 'A' - 10;
		digits.insert(0, 1, static_cast<char>(digit));
		ui /= radix;
	}

	return strlcpy(buffer, digits.c_str(), bytes);
}

size_t sonic::snprintf_safe(
	__out_bcount_z(bytes) char * buffer,
	size_t bytes,
	const char * format,
	...)
{
	va_list args;
	va_start(args, format);
	size_t r = vsnprintf_safe(buffer, bytes, format, args);
	va_end(args);
	return r;
}

int sonic::get_daylight()
{
	sonic::tzset();

#ifdef _WIN32

	#if defined(_MSC_VER) && _MSC_VER >= 1400 && !defined(__NTBUILD__)
	int hours;
	_get_daylight(&hours);
	return hours;

	#else

	return _daylight;

	#endif

#elif defined(OSXPORT)

	// no _timezone and timezone is a function. So do it the
	// hardway
	time_t rawtime;
	struct tm * timeinfo;

	time( &rawtime );
	timeinfo = localtime(&rawtime);
	return timeinfo->tm_isdst;

#else

	return daylight;

#endif

}

long sonic::get_timezone()
{
	sonic::tzset();

#ifdef _WIN32

	#if defined(_MSC_VER) && _MSC_VER >= 1400 && !defined(__NTBUILD__)

	long seconds;
	_get_timezone(&seconds);
	return seconds;

	#else

	return _timezone;

	#endif

#elif defined (__CYGWIN__)

	return _timezone;
#elif defined(macintosh) || defined(OSXPORT)

	// no _timezone and timezone is a function. So do it the
	// hardway
	time_t rawtime;
	struct tm * timeinfo;

	time( &rawtime );
	timeinfo = localtime(&rawtime);
	return timeinfo->tm_gmtoff;

#else

	return timezone;

#endif

}

void sonic::tzset()
{
	static bool is_tzset = false;
	if (!is_tzset)
	{

#ifdef _WIN32

		_tzset();

#else

		tzset();

#endif

		is_tzset = true;
	}
}

size_t sonic::vsnprintf_safe(
	__out_bcount_z(bytes) char * buffer,
	size_t bytes,
	const char * format,
	const va_list args)
{
	assert(buffer != 0);
	assert(bytes > 0);
	assert(format != 0);
#ifndef __stm__ // g++ complains, kernel 2.6.11, version 3.4.3 error: invalid operands of types `va_list' and `int' to binary `operator=='   isn't it void* ?
	assert(args != 0);
#endif

#ifdef _WIN32

	int result = _vsnprintf(buffer, bytes, format, args);

#else

	int result = vsnprintf(buffer, bytes, format, args);

#endif

	if (
		result < 0
		|| static_cast<size_t>(result) >= bytes)
	{
		assert(false);
		buffer[bytes - 1] = 0;
		return bytes - 1;
	}

	return static_cast<size_t>(result);
}

FILE * sonic::wfopen(
	const wchar_t * path,
	const wchar_t * mode)
{

#ifdef _WIN32

	return _wfopen(path, mode);

#else

	return ::fopen(
		unicode::to_string(path).c_str(),
		unicode::to_string(mode).c_str());

#endif

}