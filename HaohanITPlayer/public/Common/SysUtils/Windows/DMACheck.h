//-----------------------------------------------------------------------------
// DMACheck.h
// Copyright (c) 1991 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [wwt 20040914] 
//SR FS: Reviewed [DDT 20040928] Second pass.

//SR FS: Reviewed [JAW 20040928] 


#if defined (_WIN32)

#ifndef __DMACHECK__
#define __DMACHECK__

#define Win95		1 //Windows 95
#define Win98		2 //Windows 98
#define Win98SE	    3 //Windows 98 SE
#define WinMe		4 //Windows ME
#define WinNT4      5 //Windows NT 4.0
#define Win2K		6 //Windows 2000
#define WinXP		7 //Windows XP

extern bool CheckDMAStatus(bool& dmaDisabled, std::vector<std::string>& list);
extern SInt32 GetVersionOS();


#endif __DMACHECK__

#endif
