//-----------------------------------------------------------------------------
// SonicStatusTickler.h
// Copyright (c) 1998 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#ifndef __SonicStatusTickler_h__
#define __SonicStatusTickler_h__

#include "SonicStatus.h"
#include "Tickler.h"

class SonicStatusTickler : public SonicStatus
{

public:

	SonicStatusTickler(Tickler & tickler);
	virtual ~SonicStatusTickler();
	virtual void ThrowIfCancelling();

protected:

	virtual void UpdateNotify(); // user must override to get update notifications

private:
	// No copying
	SonicStatusTickler(const SonicStatusTickler&);
	SonicStatusTickler operator=(const SonicStatusTickler&);

	Tickler	* m_tickler;
};

class SonicStatusTickleImpl : public TickleImplBase
{

public:

	SonicStatusTickleImpl(SonicStatus * status);
	virtual void Tickle();
	virtual void Tickle(double fraction);
	virtual void Message(const SonicText & text);

protected:

	SonicStatus * m_status;
};

#endif // __SonicStatusTickler_h__
