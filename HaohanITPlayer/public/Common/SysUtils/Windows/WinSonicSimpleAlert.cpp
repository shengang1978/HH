//-----------------------------------------------------------------------------
// WinSonicSimpleAlert.cpp
// Copyright (c) 1997 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

#if defined (_WIN32)

#include "WinSonicSimpleAlert.h"
#include "SonicThread.h"
#include "UnicodeUtilities.h"

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914]  MBCS in used and used internally

WinSonicSimpleAlert::WinSonicSimpleAlert()
{
}

void WinSonicSimpleAlert::Beep() const
{
	MessageBeep(MB_ICONASTERISK);
}

UINT WinSonicSimpleAlert::DecodeIconSelector(SInt16 iconSelector) const
{
	switch (iconSelector)
	{

	default:
	case kSonicAlertNoteIcon:
		return MB_ICONINFORMATION;

	case kSonicAlertCautionIcon:
		return MB_ICONEXCLAMATION;

	case kSonicAlertStopIcon:
		return MB_ICONSTOP;
	}
}

UINT WinSonicSimpleAlert::DecodeResourceID(SInt16 resourceID) const
{
	switch (resourceID)
	{

	default:
	case kSonicAlertResourceID:
		return MB_OK;

	case kSonicAlertCancelOkResourceID:
		return MB_OKCANCEL;

	case kSonicAlertCancelResourceID:
		return MB_OKCANCEL;

	case kSonicAlertYesNoID:
		return MB_YESNO;

	case kSonicAlertCancelRetryID:
		return MB_RETRYCANCEL;
	}
}

SInt16 WinSonicSimpleAlert::DecodeReturnValue(SInt32 value) const
{
	switch (value)
	{

	case IDNO:
		return kReturnNo;

	case IDABORT:
	case IDCANCEL:
		return kReturnCancel;

	case IDIGNORE:
	case IDOK:
	case IDRETRY:
	case IDYES:
	default:
		return kReturnOk;
	}
}

unicode::string8 WinSonicSimpleAlert::DecodeTitle(SInt16 iconSelector) const
{
	switch (iconSelector)
	{

	default:
	case kSonicAlertNoteIcon:
		return Note.GetText();

	case kSonicAlertCautionIcon:
		return Caution.GetText();

	case kSonicAlertStopIcon:
		return Stop.GetText();
	}
}

SInt16 WinSonicSimpleAlert::DoAlert(const SonicText & text, SInt16 iconSelector, SInt32 resourceID)
{
	return
		DecodeReturnValue(
			MessageBoxW(
				GetForegroundWindow (),
				unicode::to_wstring(text.GetText()).c_str(),
				unicode::to_wstring(DecodeTitle(iconSelector)).c_str(),
				DecodeIconSelector(iconSelector) | DecodeResourceID(static_cast<SInt16>(resourceID))));
}

#endif
