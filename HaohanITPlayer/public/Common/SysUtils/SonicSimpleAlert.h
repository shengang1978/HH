//-----------------------------------------------------------------------------
// SonicSimpleAlert.h
// Copyright (c) 1997 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#ifndef __SonicSimpleAlert_h__
#define __SonicSimpleAlert_h__

#if defined (_WIN32)
#include "Windows/WinSonicSimpleAlert.h"
typedef WinSonicSimpleAlert SystemSonicSimpleAlert;
#elif defined (__linux__)
#include "Unix/LinuxSonicSimpleAlert.h"
typedef LinuxSonicSimpleAlert SystemSonicSimpleAlert;
#endif

class SonicSimpleAlert : public SystemSonicSimpleAlert
{

public:

	SonicSimpleAlert();

	void Alert(const SonicText & text, SInt16 iconSelector);
	bool AlertCancel(const SonicText & text, SInt16 iconSelector);
	bool AlertCancelOk(const SonicText & text, SInt16 iconSelector);
	bool AlertCancelRetry(const SonicText & text, SInt16 iconSelector);
	bool AlertYesNo(const SonicText & text, SInt16 iconSelector);
	virtual void Beep() const;
	virtual BaseSonicSimpleAlert * NewAlert() const;
	static void Register(const BaseSonicSimpleAlert * alert = 0);

protected:

	virtual SInt16 DoAlert(const SonicText & text, SInt16 iconSelector, SInt32 resourceID);

private:

	SonicSimpleAlert(const SonicSimpleAlert &);
	SonicSimpleAlert & operator =(const SonicSimpleAlert &);
};

#endif 	// __SonicSimpleAlert_h__
