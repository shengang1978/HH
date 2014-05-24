//-----------------------------------------------------------------------------
// WinSonicSimpleAlert.h
// Copyright (c) 1997 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [wwt 20040914] 
//SR FS: Reviewed [DDT 20040928] Second pass.
//SR FS: Reviewed [JAW 20040928] 

#ifndef __WinSonicSimpleAlert_h__
#define __WinSonicSimpleAlert_h__

#include "BaseSonicSimpleAlert.h"

class WinSonicSimpleAlert : public BaseSonicSimpleAlert
{

public:

	WinSonicSimpleAlert();
	virtual void Beep() const;

protected:

	UINT DecodeIconSelector(SInt16 iconSelector) const;
	UINT DecodeResourceID(SInt16 resourceID) const;
	SInt16 DecodeReturnValue(SInt32 value) const;
	unicode::string8 DecodeTitle(SInt16 iconSelector) const;
	virtual SInt16 DoAlert(const SonicText & text, SInt16 iconSelector, SInt32 resourceID);

private:

	WinSonicSimpleAlert(const WinSonicSimpleAlert &);
	WinSonicSimpleAlert & operator =(const WinSonicSimpleAlert &);
};

#endif // __WinSonicSimpleAlert_h__
