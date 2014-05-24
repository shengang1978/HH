//-----------------------------------------------------------------------------
// time_const.h
// Copyright (c) 1997 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------
/*-----------------------------------------------------------------------------------
    Name        : time_const.h

    Description : Header file containing duplicate constants and types.

    Author      : Richard Kimberly
    Date        : Thu, March 6, 1997

    Revised By  : 

-----------------------------------------------------------------------------------*/

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#ifndef __TIME_CONSTANTS__
#define __TIME_CONSTANTS__

typedef SInt16 TimeFormat;
typedef SInt32 Fields;
typedef SInt32 Frames;

/* constants for time code conversion routines */
/* TC_UNKNOWN is a flag for converting user input to float */
/*            when the type of the string is not known.    */
/* TC_GLOBAL is a flag for converting float to the global  */
/*            flag specifying the type of string.   	   */

typedef enum {
	TC_UNSPECIFIED				= -1,	// Means "unspecified" to UI code looking at PGCOverview's TCDisplayFormat field
	TC_GLOBAL					= 0,
	TC_UNKNOWN					= 0,
	TC_SECONDS					= 1,
	TC_SMPTE_30_NDF				= 2,	/* 30 fps */
	TC_SMPTE_29_DF				= 3,	/* 29.97 fps */
	TC_SMPTE_CD					= 4,	/* 75 fps */
	TC_SMPTE_24					= 5,	/* 24 fps */
	TC_SMPTE_25					= 6,	/* 25 fps */
	TC_SAMPLES					= 7,
	TC_16FF						= 8,	/* 16MM code at 60hz */
	TC_35FF						= 9,	/* 35MM code at 60hz */
	TC_16MM_24					= 10,	/* Not used */
	TC_16MM_25					= 11,	/* 16MM code at 50hz */
	TC_16MM_30					= 12,	/* Not used */
	TC_35MM_24					= 13,	/* Not used */
	TC_35MM_25					= 14,	/* 35MM code at 50hz */
	TC_35MM_30					= 15,	/* Not used */
	TC_BEATS					= 16,	/* Not used */
	TC_MEASURES					= 17,	/* Not used */
	TC_MINUTES					= 18,	/* Not used */
	TC_SMPTE_29_NDF				= 19,	/* ndf code at 29.97 fps */
	TC_SMPTE_30_DF				= 20,	/* df code at 30 fps */
	TC_16MM_29					= 21,	/* 16MM code at 59.94hz */
	TC_35MM_29					= 22,	/* 35MM code at 59.94hz */
	TC_MEASURE_RATE				= 23,	/* measure rate */
	TC_DATA_CD					= 24,	/* 75 fps - no SMPTE rounding */
	TC_UNKNOWN_RATE				= 25,	/* Probably an error */
	TC_SMPTE_23976				= 26,	/* 23.976 fps*/
	TC_SMPTE_50					= 27,	/* 50 fps */
	TC_SMPTE_5994				= 28,	/* 59.94 fps per spec non drop frame timecode*/
	TC_SMPTE_60					= 29,	/* 60 fps */
	TC_SMPTE_5994_DF			= 30,	/* 59.94 fps in drop frame timecode extension of the spec*/

	TC_LONG_VAL = 0xFFFFFFFF	// force enum to sizeof(SInt32)
} tc_mode;

// For compatilbility with original naming conventions

#define CD_FRAMESSECOND			75.0
#define F24_FRAMESSECOND		24.0
#define F25_FRAMESSECOND		25.0
#define F30_FRAMESSECOND		30.0
#define NDF_FRAMESSECOND		30.0
#define F50_FRAMESSECOND		50.0
#define F60_FRAMESSECOND		60.0

#define DF_FRAMESSECOND			(30.0/1.001)
#define F23976_FRAMESSECOND		(24.0/1.001)
#define	F5994_FRAMESSECOND		(60.0/1.001)

#define DF_10MINUTES			17982.0  // cannot use (18000.0/1.001)
#define DF_1HOUR				DF_10MINUTES * 6.0
#define DF_5994_10MINUTES		35964.0
#define DF_5994_1HOUR			DF_5994_10MINUTES * 6.0
#define F16_FRAMESFOOT			40.0
#define F35_FRAMESFOOT			16.0

#define K_FRACTIONFRAMES		true
#define K_NOTFRACTIONFRAMES		false

#define MAXIMUM_TIME_VALUE		90000.0	/* must be less than 25 hours in seconds */

enum {
	K_TC_MENU_ID 				= 8000,
	K_TC_SMPTE_MENUID 			= 8001
};

	// MENU 8000 (K_TC_MENU_ID) items
enum {
	K_TC_MENU_SMPTE_NDF			= 1,
	K_TC_MENU_SMPTE_DF30 		= 2,
	K_TC_MENU_SMPTE_NDF29 		= 3,
	K_TC_MENU_SMPTE_DF			= 4,
	K_TC_MENU_SMPTE_25			= 5,
	K_TC_MENU_SMPTE_24			= 6,
	K_TC_MENU_35MM_24			= 8,
	K_TC_MENU_35MM_29			= 9,
	K_TC_MENU_35MM_25			= 10,
	K_TC_MENU_16MM_24			= 11,
	K_TC_MENU_16MM_29			= 12,
	K_TC_MENU_16MM_25			= 13,
	K_TC_MENU_SECONDS			= 15,
	K_TC_MENU_SAMPLES			= 16
};

	// MENU 8001 (K_TC_SMPTE_MENUID) items
enum {
	K_TC_SMPTE_GLOBAL			= 1,
	K_TC_SMPTE_NDF				= 3,
	K_TC_SMPTE_DF_30			= 4,
	K_TC_SMPTE_NDF_29			= 5,  
	K_TC_SMPTE_DF				= 6,
	K_TC_SMPTE_25				= 7,
	K_TC_SMPTE_24				= 8
};

#endif
