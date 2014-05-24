//-----------------------------------------------------------------------------
// SonicSimpleAlert.cpp
// Copyright (c) 1997 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#include "SonicSimpleAlert.h"
#include "safe_ptr.h"
#include "UnicodeUtilities.h"

static safe_ptr<BaseSonicSimpleAlert> gAlert;

SonicSimpleAlert::SonicSimpleAlert() :  SystemSonicSimpleAlert() 
{
}

void SonicSimpleAlert::Alert(const SonicText & text, SInt16 iconSelector)
{
	DoAlert(text, iconSelector, kSonicAlertResourceID);
}

bool SonicSimpleAlert::AlertCancel(const SonicText & text, SInt16 iconSelector)
{
	return DoAlert(text, iconSelector, kSonicAlertCancelResourceID) == kReturnOk;
}

bool SonicSimpleAlert::AlertCancelOk(const SonicText & text, SInt16 iconSelector)
{
	return DoAlert(text, iconSelector, kSonicAlertCancelOkResourceID) == kReturnOk;
}

bool SonicSimpleAlert::AlertCancelRetry(const SonicText & text, SInt16 iconSelector)
{
	return DoAlert(text, iconSelector, kSonicAlertCancelRetryID) == kReturnOk;
}

bool SonicSimpleAlert::AlertYesNo(const SonicText & text, SInt16 iconSelector)
{
	return DoAlert(text, iconSelector, kSonicAlertYesNoID) == kReturnOk;
}

SInt16 SonicSimpleAlert::DoAlert(const SonicText & text, SInt16 iconSelector, SInt32 resourceID) 
{
	return
		gAlert.get()
			? gAlert->DoAlert(text, iconSelector, resourceID)
			: SystemSonicSimpleAlert::DoAlert(text, iconSelector, resourceID);
}

void SonicSimpleAlert::Beep() const 
{
	if (gAlert.get())
		gAlert->Beep();
	else
		SystemSonicSimpleAlert::Beep();
}

BaseSonicSimpleAlert * SonicSimpleAlert::NewAlert() const 
{ 
	return new SonicSimpleAlert(); 
}

void SonicSimpleAlert::Register(const BaseSonicSimpleAlert * alert) 
{
	gAlert = alert ? alert->NewAlert() : 0;
}
