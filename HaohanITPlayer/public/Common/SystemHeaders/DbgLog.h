

#ifndef __DEBUG_LOG_H__
#define __DEBUG_LOG_H__

#include "CommonTypes.h"

#define	LOG_NONE							  0			
#define	LOG_ST_Player						 (1)
#define	LOG_ST_Renderer					(1 << 1)
#define	LOG_AWT_Bridge					(2 << 1)
#define	LOG_TextSprite					(3 << 1)
#define	LOG_VideoSprite					(4 << 1)
#define	LOG_SoundSprite					(5 << 1)
#define	LOG_BDJ_Module					(6 << 1)
#define	LOG_STImageSprite				(7 << 1)
#define	LOG_STSound						(8 << 1)
#define LOG_ST_PlaybackControlEngine	(9 << 1)

extern	char	dbg_buffer[];

extern	UInt32	DebugFlag;

void PrintLog(const char *fmt, ...);

#define PRINT_LOG				PrintLog

#define LOG(dbg_flag, fmt)			\
{									\
	if (dbg_flag & DebugFlag)		\
	{								\
		PRINT_LOG fmt;				\
	}								\
}									\

#endif