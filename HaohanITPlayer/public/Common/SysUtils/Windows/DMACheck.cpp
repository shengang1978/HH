//-----------------------------------------------------------------------------
// DMACheck.cpp
// Copyright (c) 1991 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040915]: safe input param; no MBCS; use dangerous API to query registry only; no registry/sys folder/temp file 
#if defined (_WIN32)

#include "DMACheck.h"
#include "sonic_crt.h"

#define MAX_VALUE_NAME 255

SInt32 GetVersionOS()
{
   OSVERSIONINFO osv;
   osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

//                          dwMajorVersion  dwMinorVersion
//      Windows 95                       4               0
//      Windows 98                       4              10
//      Windows Me                       4              90
//      Windows NT 3.51                  3              51
//      Windows NT 4.0                   4               0
//      Windows 2000                     5               0
//      Windows XP                       5               1
//      Windows .NET Server              5               1

   SInt32 osVer = 0;
   if (GetVersionEx(&osv))
   {
      switch(osv.dwPlatformId)
      {  
         //case VER_PLATFORM_WIN32s: //Win32s on Windows 3.1. 
         //   break;
         case VER_PLATFORM_WIN32_WINDOWS: //WIN32 on 95 or 98 or ME  
			 
            if (osv.dwMinorVersion<10)
	           osVer=Win95;
	        else if (osv.dwMinorVersion<90)
			{
				if ((osv.dwBuildNumber & 0xFFFF) < 2222)
	               osVer=Win98;
                else
                   osVer=Win98SE;
			}
	        else
	           osVer=WinMe;
            break;

         case VER_PLATFORM_WIN32_NT: //Win32 on Windows NT/2000/XP/.NET server. 

	        //if (osv.dwMajorVersion == 3) //Windows NT 3.51
            if (osv.dwMajorVersion == 4) 
				osVer = WinNT4;
            else if (osv.dwMajorVersion == 5)
            {
               if (osv.dwMinorVersion == 0)
				   osVer = Win2K;
               else if (osv.dwMinorVersion == 1)
				   osVer = WinXP;
            }
			break;

         default: 
// **CodeWizzard** - Violation:  Universal Coding Standards item 34  - Always terminate a case statement with brea
			 osVer = 0;
      }
   }
   return osVer;
}

static bool CheckRegKeyWMe(const char * keyName, const char * prefix, bool& dmaDisabled, std::vector<std::string>& dmaDisabledlist)
{
	dmaDisabled = false;

    char  achKey[MAX_VALUE_NAME]; 
    DWORD i;
    LONG retCode;

	char * keyName1 = "Config Manager\\Enum";

    HKEY hKey;
    HKEY hKey1;
// **CodeWizzard** - Violation:  More Effective C++ item 2  - Prefer C++-style cast
    if (ERROR_SUCCESS != RegOpenKeyExA(HKEY_DYN_DATA, keyName1,
                                      0, KEY_QUERY_VALUE|KEY_ENUMERATE_SUB_KEYS, &hKey1))
		return false;

	std::set<std::string> currentHardwareIDs;

	DWORD size;
	DWORD max_path = MAX_VALUE_NAME;

    for (i = 0, retCode = ERROR_SUCCESS; 
            retCode == ERROR_SUCCESS; i++) 
    {
	  max_path = MAX_VALUE_NAME;
		retCode = RegEnumKeyExA(hKey1, i, achKey, &max_path, 0, 0, 0, 0);
        if (retCode == ERROR_SUCCESS) 
        {
			char subkey[MAX_VALUE_NAME];
			sonic::strncpy_safe(subkey, keyName1, sizeof(subkey));
			sonic::strncat_safe(subkey, "\\", sizeof(subkey));
			sonic::strncat_safe(subkey, achKey, sizeof(subkey));
// **CodeWizzard** - Violation:  More Effective C++ item 2  - Prefer C++-style cast
			if (ERROR_SUCCESS != RegOpenKeyExA(HKEY_DYN_DATA, subkey,
                                              0, KEY_QUERY_VALUE, &hKey))
				continue;

			char hardWareKeyName[] = "HardWareKey";
			size = 0x0;
			if (ERROR_SUCCESS != RegQueryValueExA(hKey, hardWareKeyName, 0, 0, 0, &size))
				continue;

			char* pHardWareKeyValue = new char[size];
			if (ERROR_SUCCESS != RegQueryValueExA(hKey, hardWareKeyName, 0, 0,
				                                reinterpret_cast<LPBYTE>(pHardWareKeyValue), &size))
			{
				delete [] pHardWareKeyValue;
				continue;

          	} 
			RegCloseKey(hKey);

			std::string hardwareID(pHardWareKeyValue);
            std::string subHardwareID = hardwareID.substr(0,5);
			delete [] pHardWareKeyValue;
            if(subHardwareID != prefix)
				continue;
      
			size_t pos1 = 5;
			size_t pos2 = hardwareID.find_first_of('\\', pos1+1);
			if (pos2 == std::string::npos)
				continue;
			currentHardwareIDs.insert(hardwareID.substr(pos1, pos2-pos1));
		}
	}
    
	RegCloseKey(hKey1);

	if (currentHardwareIDs.size() == 0)
		return false;

    if (ERROR_SUCCESS != RegOpenKeyExA(HKEY_LOCAL_MACHINE, keyName,
                                      0, KEY_QUERY_VALUE|KEY_ENUMERATE_SUB_KEYS, &hKey1))
		return false;

    for (i = 0, retCode = ERROR_SUCCESS; 
            retCode == ERROR_SUCCESS; i++) 
    {
        max_path = MAX_VALUE_NAME;
		if( (retCode = RegEnumKeyExA(hKey1, i, achKey, &max_path, 0, 0, 0, 0)) == ERROR_SUCCESS )
        {
			//try to get the dma value
			char subkey[MAX_VALUE_NAME];
			sonic::strncpy_safe(subkey, keyName, sizeof(subkey));
			sonic::strncat_safe(subkey, "\\", sizeof(subkey));
			sonic::strncat_safe(subkey, achKey, sizeof(subkey));
			if (ERROR_SUCCESS != RegOpenKeyExA(HKEY_LOCAL_MACHINE, subkey,
                                              0, KEY_QUERY_VALUE|KEY_ENUMERATE_SUB_KEYS, &hKey))
				continue;

			
			for(DWORD j=0, retCode1 = ERROR_SUCCESS; retCode1==ERROR_SUCCESS; j++)
			{
				max_path = MAX_VALUE_NAME;
				if( (retCode1 = RegEnumKeyExA(hKey, j, achKey, &max_path, 0, 0, 0, 0)) == ERROR_SUCCESS )
				{
					char skey[MAX_VALUE_NAME];
					HKEY hsKey;
					sonic::strncpy_safe(skey, subkey, sizeof(skey));
					sonic::strncat_safe(skey, "\\", sizeof(skey));
					sonic::strncat_safe(skey, achKey, sizeof(skey));
					if(ERROR_SUCCESS != RegOpenKeyExA( HKEY_LOCAL_MACHINE, skey,
						0, KEY_QUERY_VALUE, &hsKey ))
						continue;

					char dmaValueName[] = "DMACurrentlyUsed";
					DWORD dmaValue = 0x0;
					size = sizeof(DWORD);
					if(ERROR_SUCCESS != RegQueryValueExA(hsKey, dmaValueName, 0, 0, (LPBYTE)&dmaValue, &size))
					{
						RegCloseKey(hsKey);
						continue;
					}

					if(dmaValue == 0x0) //DMA is disabled
					{
						char hardWareKeyName[] = "HardwareID";
						size = 0x0;
						if(ERROR_SUCCESS != RegQueryValueExA(hsKey, hardWareKeyName, 0, 0, 0, &size))
						{
							RegCloseKey(hsKey);
							continue;
						}

						char* pHardWareKeyValue = new char[size];
						if(ERROR_SUCCESS != RegQueryValueExA(hsKey, hardWareKeyName, 0, 0,
							(LPBYTE)pHardWareKeyValue, &size))
						{
							RegCloseKey(hsKey);
							delete [] pHardWareKeyValue;
							continue;
						}

						size = 0x0;
						char deviceDesc[] = "DeviceDesc";
						char * pDeviceDesc = 0;
						if(ERROR_SUCCESS == RegQueryValueExA(hsKey, deviceDesc, 0, 0, 0, &size))
						{
							pDeviceDesc = new char[size];
							RegQueryValueExA(hsKey, deviceDesc, 0, 0, (LPBYTE)pDeviceDesc, &size);
						}

						RegCloseKey(hsKey);

						std::string hardwareID(pHardWareKeyValue); 
						delete [] pHardWareKeyValue;
						size_t pos = hardwareID.find_first_of(',', 0);
						if (pos == std::string::npos)
						{
							delete [] pDeviceDesc;
							continue;
						}

						std::string subHardwareID = hardwareID.substr(0,pos);
						if(currentHardwareIDs.find(subHardwareID) ==  currentHardwareIDs.end())
						{
							delete [] pDeviceDesc;
							continue;
						}

						if(pDeviceDesc)
						{
							std::string szDeviceDesc(pDeviceDesc);
							dmaDisabledlist.push_back(szDeviceDesc);
							delete [] pDeviceDesc;
						}

						dmaDisabled = true;
					}	
				}
			}
			RegCloseKey(hKey);
		}
    } //for

	RegCloseKey(hKey1);
    return true;
}

static bool CheckRegKeyWXP(bool& dmaDisabled, std::vector<std::string>& dmaDisabledlist)
{
	dmaDisabled = false;
	HKEY hKey;
	LONG retCode = ERROR_SUCCESS;
	//HKLM\HARDWARE\DEVICEMAP\Scsi
	if(ERROR_SUCCESS != RegOpenKeyExA( HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\Scsi",
		0, KEY_QUERY_VALUE|KEY_ENUMERATE_SUB_KEYS, &hKey ))
		return false;

	char keyBuf[MAX_VALUE_NAME];
	DWORD max_path = MAX_VALUE_NAME;

	retCode = RegEnumKeyExA(hKey, 0, keyBuf, &max_path, 0, 0, 0, 0);
	if(retCode != ERROR_SUCCESS) 
	{
		RegCloseKey(hKey);
		return false;
	}	

	HKEY hPortKey;
	for(DWORD nPort=1, retCode=ERROR_SUCCESS; retCode==ERROR_SUCCESS; nPort++)
	{
		//HKLM\HARDWARE\DEVICEMAP\Scsi\Scsi Port 0...
		if( ERROR_SUCCESS == RegOpenKeyExA(hKey, keyBuf, 0, KEY_QUERY_VALUE|KEY_ENUMERATE_SUB_KEYS, &hPortKey) )
		{
			size_t dmaValue = 0;
			DWORD size = sizeof(DWORD);
			//DMAEnabled
			if( ERROR_SUCCESS != RegQueryValueExA(hPortKey, "DMAEnabled", 0, 0, (LPBYTE)&dmaValue, &size) )
			{
				RegCloseKey(hPortKey);
				RegCloseKey(hKey);
				return false;
			}
			if(dmaValue == 0x0)
			{
				dmaDisabled = true;
				HKEY hBusKey;
				for(DWORD nBus=0,retCode=ERROR_SUCCESS;retCode==ERROR_SUCCESS;nBus++)
				{
					max_path = MAX_VALUE_NAME;
					keyBuf[0] = '\0';
					retCode = RegEnumKeyExA(hPortKey, nBus, keyBuf, &max_path, 0, 0, 0, 0);
					if( retCode == ERROR_SUCCESS )
					{
						//HKLM\HARDWARE\DEVICEMAP\Scsi\Scsi Port 0\Scsi Bus 0...
						if( ERROR_SUCCESS == RegOpenKeyExA(hPortKey, keyBuf, 0, KEY_QUERY_VALUE|KEY_ENUMERATE_SUB_KEYS, &hBusKey) )
						{
							HKEY hTargetKey;
							for(DWORD nTarget=0,retCode=ERROR_SUCCESS;retCode==ERROR_SUCCESS;nTarget++)
							{
								max_path = MAX_VALUE_NAME;
								keyBuf[0] = '\0';
								retCode = RegEnumKeyExA(hBusKey, nTarget, keyBuf, &max_path, 0, 0, 0, 0);
								if( retCode==ERROR_SUCCESS )
								{
									std::string targetValue(keyBuf);
									if(targetValue.find("Target Id")==std::string::npos)
										continue;
									//HKLM\HARDWARE\DEVICEMAP\Scsi\Scsi Port 0\Scsi Bus 0\Target Id 0...
									targetValue += "\\Logical Unit Id 0";
									if( ERROR_SUCCESS == RegOpenKeyExA(hBusKey, targetValue.c_str(), 0, KEY_QUERY_VALUE|KEY_ENUMERATE_SUB_KEYS, &hTargetKey) )
									{
										max_path = MAX_VALUE_NAME;
										keyBuf[0] = '\0';
										retCode = RegQueryValueExA(hTargetKey, "Identifier", 0, 0, (LPBYTE)keyBuf, &max_path);
										if( retCode==ERROR_SUCCESS )
										{
											dmaDisabledlist.push_back(std::string(keyBuf));
										}
										RegCloseKey(hTargetKey);
									}
								}
							}
							RegCloseKey(hBusKey);
						}
					}
				}
			}
			RegCloseKey(hPortKey);
		}
		max_path = MAX_VALUE_NAME;
		keyBuf[0] = '\0';
		retCode = RegEnumKeyExA(hKey, nPort, keyBuf, &max_path, 0, 0, 0, 0);
	}
	RegCloseKey(hKey);
	return true;
}

bool CheckDMAStatus(bool& dmaDisabled, std::vector<std::string> &dmaDisabledlist)
{
	dmaDisabled = false;
    bool ret;
    SInt32 osVer = GetVersionOS();
	dmaDisabledlist.clear();
    switch(osVer)
    {
 	   case Win98:
       case Win98SE:
	   case WinMe:
	    	ret = CheckRegKeyWMe( "Enum\\ESDI", "ESDI\\", dmaDisabled, dmaDisabledlist);
 			if(ret && dmaDisabled)
 				return true;
    		ret = CheckRegKeyWMe( "Enum\\SCSI", "SCSI\\", dmaDisabled, dmaDisabledlist);
	    	break;

	   case Win2K:
	   case WinXP:
			ret = CheckRegKeyWXP(dmaDisabled, dmaDisabledlist);
		    break;

	   default:
// **CodeWizzard** - Violation:  Universal Coding Standards item 34  - Always terminate a case statement with brea
		    ret = false;
    }

    return ret;
}

#endif

