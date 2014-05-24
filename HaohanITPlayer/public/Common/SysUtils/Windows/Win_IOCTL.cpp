//-----------------------------------------------------------------------------
// Win_IOCTL.cpp
// Copyright (c) 1999 - 2004, Haohanit.  All rights reserved.
//
//-----------------------------------------------------------------------------

//SR FS Needs Review [JAW  20040912] make sure buffers to system calls are big enough
//SR FS: Reviewed [JAW  20040912] - AOK, but old fashioned
//SR FS: Reviewed [wwt 20040915]: safe input param; no MBCS; no dangerous API; no registry/sys folder/temp file 
//SR FS Reviewed [JAW 20040924] - Advise against use of printf and other archane programming practices

/* based on MSDN
HOWTO: Ejecting Removable Media in Windows NT
Windows32 SDK
Article ID: Q165721 
*/

#if defined (_WIN32)

#pragma warning(push)
#pragma warning(disable:4201)

#include "Win_IOCTL.h"
#include "sonic_crt.h"
#include <winioctl.h>

#pragma warning(pop)

// Original Prototypes
/*
   BOOL EjectVolume(TCHAR cDriveLetter);
   HANDLE OpenVolume(TCHAR cDriveLetter);
   BOOL LockVolume(HANDLE hVolume);
   BOOL DismountVolume(HANDLE hVolume);
   BOOL PreventRemovalOfVolume(HANDLE hVolume, BOOL fPrevent);
   BOOL AutoEjectVolume(HANDLE hVolume);
   BOOL CloseVolume(HANDLE hVolume);
*/

static const char* szVolumeFormat = "\\\\.\\%c:";
static const char* szRootFormat = "%c:\\";
static const char* szErrorFormat = "Error %d: %s\n";

// some Windows NT system stuff we may use
bool WinIOCTL::IsWindowsNT()
{
	OSVERSIONINFO sysVer;

	sysVer.dwOSVersionInfoSize = sizeof(sysVer);
	if (GetVersionEx(&sysVer))
		if (sysVer.dwPlatformId == VER_PLATFORM_WIN32_NT)
			return true;
	return false;
}

static void ReportError(LPCTSTR /*szMsg*/)
{
	//_tprintf(szErrorFormat, GetLastError(), szMsg);
}

WinIOCTL::WinIOCTL()
{
}

WinIOCTL::WinIOCTL(char driveLetter)
{
	OpenVolume(driveLetter);
}

WinIOCTL::WinIOCTL(const char* driveName)
{
	OpenVolume(driveName[0]); // just use first char for now could be something else
}

WinIOCTL::~WinIOCTL()
{
	CloseVolume();
}

HANDLE WinIOCTL::OpenVolume(char cDriveLetter)
{
	UINT uDriveType;
    char szVolumeName[8];
    char szRootName[5];
    DWORD dwAccessFlags;

	wiLastError = 0;
	wiDriveLetter = cDriveLetter;
//SR SC: T, I, D [JAW 20040924] - rewrite using iostreams instead of sprintf
//SR DI: DP0, E0, D0 [JAW 20040924] {dread_score: 0}  - no threat, just old fashioned
//SR MC: No Issue [wwt 20040915] - cDriveLetter only has one TCHAR. very safe
	sonic::snprintf_safe(szRootName, sizeof(szRootName), szRootFormat, cDriveLetter);

    uDriveType = GetDriveTypeA(szRootName);
    switch(uDriveType) {
    case DRIVE_REMOVABLE:
        dwAccessFlags = GENERIC_READ | GENERIC_WRITE;
        break;
    case DRIVE_CDROM:
        dwAccessFlags = GENERIC_READ;
        break;
    default:
// **CodeWizzard** - Informational:  Effective C++ item 2  - Prefer iostream.h to stdio.
 //SR SC: T, I, D [JAW 20040924] - rewrite using iostreams instead of sprintf
//SR DI: DP0, E0, D0 [JAW 20040924] {dread_score: 0}  - no threat, just old fashioned
       printf("Cannot eject.  Drive type is incorrect.\n");
        return INVALID_HANDLE_VALUE;
    }

 //SR SC: T, I, D [JAW 20040924] - rewrite using iostreams instead of sprintf
	sonic::snprintf_safe(szVolumeName, sizeof(szVolumeName), szVolumeFormat, cDriveLetter);

    hVolume = CreateFileA(  szVolumeName, 
							dwAccessFlags,
                            FILE_SHARE_READ | FILE_SHARE_WRITE,
                            0,
                            OPEN_EXISTING,
                            0,
                            0);

    if (hVolume == INVALID_HANDLE_VALUE)
	{
		wiLastError = GetLastError();
        ReportError(TEXT("CreateFile"));
	}
    return hVolume;
}

bool WinIOCTL::CloseVolume()
{
	HANDLE h = hVolume;
 	hVolume = INVALID_HANDLE_VALUE; 
	BOOL ok = CloseHandle(h);
	if (!ok)
		wiLastError = GetLastError();
	return (ok != 0);
}

/*
	Routines to manage the Windows SCSI driver information model
	We need this try to map Padus' Bus:ID to the drive letters which VXBLOCK wants 

	We can identify a WindowsBus by the PathID
	We can Identify a WindowsDevice by PathID+TargetID

	When we do a IOCTL_SCSI_GET_INQUIRY_DATA for a drive letter, 
	we get all of the information for its Bus
	??? How do we determine the letter?
	??? How do we know which of the n devs on that bus we asked for?
 
*/

bool WinIOCTL::LockVolume()
{
	const UInt32 LOCK_TIMEOUT        =10000;      // 10 Seconds
	const UInt32 LOCK_RETRIES        =20;
	DWORD dwBytesReturned;
	DWORD dwSleepAmount;
	SInt32 nTryCount;

    if (hVolume == INVALID_HANDLE_VALUE)
		return false;
	dwSleepAmount = LOCK_TIMEOUT / LOCK_RETRIES;

	// Do this in a loop until a timeout period has expired
    for (nTryCount = 0; nTryCount < LOCK_RETRIES; nTryCount++) {
        if (DeviceIoControl(hVolume, FSCTL_LOCK_VOLUME, 0, 0, 0, 0, &dwBytesReturned, 0))
			return TRUE;
		Sleep(dwSleepAmount);
	}
	wiLastError = GetLastError();
    return FALSE;
}

bool WinIOCTL::UnlockVolume()
{
	DWORD dwBytesReturned;
    if (hVolume == INVALID_HANDLE_VALUE)
		return false;
	BOOL ok = DeviceIoControl(hVolume, FSCTL_UNLOCK_VOLUME, 0, 0, 0, 0, &dwBytesReturned, 0);
	if (!ok)
		wiLastError = GetLastError();
	return (ok != 0);
}

bool WinIOCTL::DismountVolume()
{
	DWORD dwBytesReturned;
    if (hVolume == INVALID_HANDLE_VALUE)
		return false;
	BOOL ok = DeviceIoControl(hVolume, FSCTL_DISMOUNT_VOLUME, 0, 0, 0, 0, &dwBytesReturned, 0);
	if (!ok)
		wiLastError = GetLastError();
	return (ok != 0);
}

bool WinIOCTL::PreventRemovalOfVolume(bool fPreventRemoval)
{
    if (hVolume == INVALID_HANDLE_VALUE)
		return false;
	DWORD dwBytesReturned;
	PREVENT_MEDIA_REMOVAL PMRBuffer;
    PMRBuffer.PreventMediaRemoval = fPreventRemoval;

    return (0 != DeviceIoControl(hVolume,
		IOCTL_STORAGE_MEDIA_REMOVAL,
		&PMRBuffer, sizeof(PREVENT_MEDIA_REMOVAL),
		0, 0,
		&dwBytesReturned,
		0));
}

bool WinIOCTL::AutoEjectVolume()
{
    if (hVolume == INVALID_HANDLE_VALUE)
		return false;
	DWORD dwBytesReturned;      
	BOOL ok = DeviceIoControl(hVolume, IOCTL_STORAGE_EJECT_MEDIA, 0, 0, 0, 0, &dwBytesReturned, 0);
	if (!ok)
		wiLastError = GetLastError();
	return (ok != 0);
}

bool WinIOCTL::EjectVolume()
{
       bool fRemoveSafely = false;
       bool fAutoEject = false;
       if (hVolume == INVALID_HANDLE_VALUE)
           return FALSE;

       // Lock and dismount the volume.
       if (LockVolume() && DismountVolume()) {
           fRemoveSafely = TRUE;

           // Set prevent removal to false and eject the volume.
           if (PreventRemovalOfVolume(FALSE) &&
               AutoEjectVolume())
               fAutoEject = true;
       }
//SR SC: T, I, D [JAW 20040924] - rewrite using iostreams instead of printf
//SR DI: DP0, E0, D0 [JAW 20040924] {dread_score: 0}  - no threat, just old fashioned

       if (fAutoEject)
           printf("Media in Drive %c has been ejected safely.\n",
// **CodeWizzard** - Informational:  Effective C++ item 2  - Prefer iostream.h to stdio.
                  wiDriveLetter);
       else {
           if (fRemoveSafely)
               printf("Media in Drive %c can be safely removed.\n",
// **CodeWizzard** - Informational:  Effective C++ item 2  - Prefer iostream.h to stdio.
               wiDriveLetter);
       }
       return true;
}

/*

The old code for drive letter that did not work quite right
We got messed up when several drives when one a single bus

char DeviceTarget::FindDriveLetter()
{
#if defined (_WIN32)
	// what we are looking for
	const char* decscrip = (const char*)&mUnit->what();
	UInt8 targetID = mUnit->id().target; 

	DWORD dwNumDrives = MAX_NUM_WINDOWS_DRIVES; // we should query OS for this
	for (char driveLetter = 'C'; driveLetter <= 'Z'; driveLetter++)
	{
		char buffer[MAX_INQUIRY_DATA_SIZE];
		if (VolumeInquiry(driveLetter, buffer))
		{
			// the inquiry refers to all data on the bus with drive letter driveLetter
			// we step through them all to see if we get a TargetID and InquiryData match

			// WARNING/BUG this algorithm fails if two devices with same inquiry data
			// are on the same bus, the lower drive letter gets returned

			PSCSI_ADAPTER_BUS_INFO busInfoP = (PSCSI_ADAPTER_BUS_INFO) buffer;
			for (SInt32 bus = 0; bus < busInfoP->NumberOfBuses; bus++)
			{
				UInt32 inqOffset = busInfoP->BusData[bus].InquiryDataOffset;
				while (inqOffset != 0)
				{
					PSCSI_INQUIRY_DATA inquiryPtr = (PSCSI_INQUIRY_DATA) (buffer + inqOffset);
					if (strncmp((char *)&inquiryPtr->InquiryData[0], decscrip, 20) == 0)
					{
						if (inquiryPtr->TargetId == targetID)
							return driveLetter;
					}
					inqOffset = inquiryPtr->NextInquiryDataOffset; 
				}
			}
		}
	}

#endif

	return 0; // no match
}
*/

/*
#define MAX_INQUIRY_DATA_SIZE 4096 // arbitrary
#define MAX_NUM_WINDOWS_DRIVES 26

bool WinIOCTL::VolumeInquiry(TCHAR cDriveLetter, char* buffer)
{
	bool success = false;
    if (hVolume == INVALID_HANDLE_VALUE)
		return false;

	// this may be very illegal
	DWORD dwBytesReturned;

	if (DeviceIoControl(hVolume,
		IOCTL_SCSI_GET_ADDRESS,	// try this to get port or driver information
		0, 0,
		buffer, MAX_INQUIRY_DATA_SIZE,
		&dwBytesReturned, 0))
		{
			// use valid result
			success = true;
		}

	if (DeviceIoControl(hVolume,
		IOCTL_SCSI_GET_INQUIRY_DATA,
		0, 0,
		buffer, MAX_INQUIRY_DATA_SIZE,
		&dwBytesReturned, 0))
		{
			// use valid result
			success = TRUE;
		}
	return success;
}
*/

// some power management test code, should move to sysutils

bool WinIOCTL::UsingACPower()
{
	SYSTEM_POWER_STATUS powerStatus;
	BOOL PowerCheckOK = GetSystemPowerStatus(&powerStatus);

	// if PowerCheck fails, assume AC power
	return (PowerCheckOK && powerStatus.ACLineStatus == 1);
}

bool WinIOCTL::UsingLowBattery()
{
	SYSTEM_POWER_STATUS powerStatus;
	BOOL PowerCheckOK = GetSystemPowerStatus(&powerStatus);

	return (PowerCheckOK && (powerStatus.ACLineStatus) == 0 && (powerStatus.BatteryFlag != 1));
}

#endif // #if defined (_WIN32)



