//-----------------------------------------------------------------------------
// Win_IOCTL.h
// Copyright (c) 2002 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [wwt 20040914] 
//SR FS: Reviewed [DDT 20040928] Second pass.
//SR FS: Reviewed [JAW 20040928] 

#ifndef __WINIOCTL__
#define __WINIOCTL__

#if defined (_WIN32)

class WinIOCTL {
public:
	WinIOCTL();
// **CodeWizzard** - Informational:  More Effective C++ item 5  - Be wary of user-defined conversion function
	WinIOCTL(char driveLetter);
// **CodeWizzard** - Informational:  More Effective C++ item 5  - Be wary of user-defined conversion function
	WinIOCTL(const char* deviceName);
	virtual ~WinIOCTL();

	static bool IsWindowsNT();

	bool EjectVolume();
	bool LockVolume();
	bool UnlockVolume();
	bool DismountVolume();
	bool PreventRemovalOfVolume(bool fPrevent);
	bool AutoEjectVolume();
	bool CloseVolume();
	DWORD GetLastError() { return wiLastError;};

	// some power management test code, should move to sysutils
	static bool UsingACPower();
	static bool UsingLowBattery();
private:
	// No copying
	WinIOCTL(const WinIOCTL& wio);
	WinIOCTL& operator =(const WinIOCTL& wio);

	HANDLE OpenVolume(char cDriveLetter);
	HANDLE hVolume;
	TCHAR wiDriveLetter;
	DWORD wiLastError;
};

#else

class WinIOCTL {
public:
 	WinIOCTL() {}
	virtual ~WinIOCTL() {};
	static bool IsWindowsNT() { return false; }
};

#endif // defined (_WIN32)

#endif // __WINIOCTL__


