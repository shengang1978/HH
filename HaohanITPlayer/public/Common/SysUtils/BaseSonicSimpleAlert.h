//-----------------------------------------------------------------------------
// BaseSonicSimpleAlert.h
// Copyright (c) 1997 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW 20040911]
//SR FS: Reviewed [wwt 20040913] 

#ifndef __BaseSonicSimpleAlert_h__
#define __BaseSonicSimpleAlert_h__
 
/*
 If you wish to override the alert handler, subclass BaseSonicSimpleAlert
 and override the DoAlert() method.
 
 Add the following code to you startup.
 
 XXXSimpleAlert* alert = new XXXSimpleAlert();
 SonicSimpleAlert::Register(alert);
 
 Note - SonicSimpleAlert takes care of the descruction of the newly created XXXSimpleAlert.
*/

#include "SonicText.h"

class BaseSonicSimpleAlert
{

public:

	enum
	{
		kSonicAlertNoteIcon				= 0,
		kSonicAlertCautionIcon			= 1,
		kSonicAlertStopIcon				= 2
	};


	enum
	{
		kReturnPending = 0,
		kReturnOk = 1,
		kReturnCancel = 2,
		kReturnNo = 2,
		kReturnUnknown = 3
	};

	virtual ~BaseSonicSimpleAlert();
	virtual void Beep() const = 0;
	virtual BaseSonicSimpleAlert * NewAlert() const = 0;

protected:

	friend class SonicSimpleAlert;

	enum
	{
		kSonicAlertResourceID			= 1,
		kSonicAlertCancelOkResourceID	= 2,
		kSonicAlertCancelResourceID		= 3,
		kSonicAlertYesNoID				= 4,
		kSonicAlertCancelRetryID		= 5
	};

	BaseSonicSimpleAlert();
	virtual SInt16 DoAlert(const SonicText & text, SInt16 iconSelector, SInt32 resourceID) = 0;

	static const SonicText Note;
	static const SonicText Caution;
	static const SonicText Stop;
};

#endif 	// __BaseSonicSimpleAlert_h__
