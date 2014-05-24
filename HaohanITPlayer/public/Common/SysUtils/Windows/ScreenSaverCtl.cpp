//-----------------------------------------------------------------------------
// ScreenSaverCtl.cpp
// Copyright (c) 2002 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//	(C) 2002 Haohanit
//----------------------------------------------------------------------------

//SR FS Needs Review [JAW  20040912] - Check void * parmateres to OS call
//SR FS: Reviewed [JAW  20040912] AOK
//SR FS: Reviewed [wwt 20040914] 

#if defined (_WIN32)

#include "ScreenSaverCtl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScreenSaverCtl::CScreenSaverCtl()
{

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

CScreenSaverCtl::~CScreenSaverCtl()
{

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

void CScreenSaverCtl::enable() {
	BOOL res;
	res = SystemParametersInfo(SPI_SETSCREENSAVEACTIVE,
			TRUE,
			0L,
			0);

	return;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

void CScreenSaverCtl::disable() {
	BOOL	res;

	res = SystemParametersInfo(SPI_SETSCREENSAVEACTIVE,
			FALSE,
			0L,
			0);
	return;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

bool CScreenSaverCtl::is_enabled() {
	BOOL	res, active = FALSE;
	bool	return_val = false;

	res = SystemParametersInfo(SPI_GETSCREENSAVEACTIVE,
			TRUE,
			&active,
			0);

	if (res != 0) {
		return_val = (active == TRUE)?true:false;
	}

	return return_val;
}

#endif
