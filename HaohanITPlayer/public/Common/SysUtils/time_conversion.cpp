//-----------------------------------------------------------------------------
// time_conversion.cpp
// Copyright (c) 1997 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS Needs Review [JAW  20040912] - Check use of char buffers - Add makeUDFHexDateStr, makeDateStr to risky list
//SR FS: Reviewed [JAW  20040913] 
//SR FS: Reviewed [wwt 20040914] 

#include <ctime>
#include <string>

#include "time_conversion.h"
#include "time_utils.h"
#include "Locale_CurrentTime.h"
#include "Locale_TimeDate.h"
#include "sonic_crt.h"

static time_t y2k() 
{
	static struct tm y2k_tm = {
		0, 		//	tm_sec;
		0, 		//	tm_min;
		0, 		//	tm_hour;
		1, 		//	tm_mday;
		0, 		//	tm_mon;
		100, 	//	tm_year;
		0, 		//	tm_wday; (don't care)
		0, 		//	tm_yday; (don't care)
		-1 		//	tm_isdst; (don't care)
	};
	static time_t y2k = 0;
	if (y2k == 0) 
		y2k = mktime(&y2k_tm);
	return y2k;
}

SInt32 time_tToAFPTime(time_t udfTime) {
	return static_cast<SInt32>(difftime(udfTime, y2k()));
}


time_t AFPTimeTotime_t(SInt32 afpTime){
	return y2k() + afpTime;
}

std::string makeUDFHexDateString(SInt32 afp_time)
{
	Locale::TimeDate timedate = Locale::AFPToTimeDate(afp_time);
	Locale::Date date = timedate.GetDate();
	Locale::Time time = timedate.GetTime();
	int year = date.GetYear() - 1980;
	int month = date.GetMonth();
	int day = date.GetDay();
	int hour = time.GetHours();
	int min = time.GetMinutes();
	int sec = time.GetSeconds() / 2;

	char buffer[64];
	sonic::snprintf_safe(
		buffer,
		sizeof(buffer),
		"%08x",
		year << 25 | month << 21 | day << 16 | hour << 11 | min << 5 | sec);

	return std::string(buffer);
}

std::string
makeDateString(SInt32 afp_time)
{
	Locale::TimeDate timedate = Locale::AFPToTimeDate(afp_time);
	Locale::TimeZone timezone = timedate.GetTimeZone();
	Locale::Date date = timedate.GetDate();
	Locale::Time time = timedate.GetTime();

	char buffer[64];
	sonic::snprintf_safe(
		buffer,
		sizeof(buffer),
		"%04d%02d%02d%02d%02d%02d00%c%02d", 
		date.GetYear(),
		date.GetMonth(), 
		date.GetDay(), 
		time.GetHours(),
		time.GetMinutes(),
		time.GetSeconds(),
		timezone.IsWest() ? '+' : '-',
		abs((timezone.GetHours() * 60 + timezone.GetMinutes()) / 15)); // units of 15 minutes

	return std::string(buffer);
}

SInt32 GetAFP_time()		//	return seconds since 1/1/2000
{
	return Locale::TimeDateToAFP(Locale::CurrentTime());
}
