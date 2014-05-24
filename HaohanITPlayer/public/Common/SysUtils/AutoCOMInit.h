//-----------------------------------------------------------------------------
// AutoCOMInit.h
// Copyright (c) 2003 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

#if !defined(_AutoCOMInit_h_)
#define _AutoCOMInit_h_

//
// Use this class as an automatic and exception-safe mechanism for calling ::CoInitialize() and ::CoUninitialize(). See
// usage in most of the progress dialogs.
//
class AutoCOMInit
{
public:
	explicit AutoCOMInit(bool inIsThread = true);
	~AutoCOMInit();

private:
	const bool mIsThread;

	AutoCOMInit(const AutoCOMInit&);					// No copying allowed
	const AutoCOMInit& operator=(const AutoCOMInit&);	// No assignment allowed
};

#endif	// !defined(_AutoCOMInit_h_)
