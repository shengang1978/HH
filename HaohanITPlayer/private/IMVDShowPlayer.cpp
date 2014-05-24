#include "StdAfx.h"
#include "IMVDShowPlayer.h"
#include <InitGuid.h>
#include "hddvdc1guids.h"
//#include "MediaVaultSourceGuids.h"
//#include "SonicHDAudioDecoderPrivateGuids.h"
//#include "SonicHDVideoDecoderPrivateGuids.h"
//#include "IParserTrickPlay.h"
//#include "IntelDXVAGUIDS.h"
//#include "Dxva2api.h"
//#include "tcaes128.h"
#include "Propvarutil.h"
#include "Dvdmedia.h"
#include <math.h>
//#include "IMVLicenseDB.h"
#include "Dwmapi.h"
#include "EVRPresenter.h"
//#include "IMVDirectoryUtility.h"
#include "OPErrorTypes.h"
#include "dvr/DVRPlayer.h"
#include "comutil.h"
#include "dvr/TraceLog.h"

//temporary stack for dialog box before exit - 1 MB is linker default on Windows
#define SECURITY_STACK_SIZE	1024 * 1024		

#ifdef MODULE_CHECK //check for unsigned modules
#include "ModuleCheck.h"
#include "Sonic_StrUncloak.h"

//cloaked dialogs used by security features need the application name
//we will load this when CIMVDShowPlayer is instantiated
WCHAR   appExeName[MAX_PATH] = {0};

//Environment error detected.
unsigned char EnvErrorCloaked[] =
{
    0x6E, 0xAD, 0x1E, 0x86, 0x0C, 0x19, 0xD2, 0x2C,
    0x10, 0xD6, 0x48, 0xA3, 0x38, 0x51, 0x50, 0xD1,
    0x92, 0x3C, 0x0A, 0xC6, 0x1C, 0x03, 0x68, 0x33,
    0x81, 0xF5, 0xCB, 0xC3, 0x34, 0x61, 0xBE, 0xD3,
    0x4E, 0xB3, 0x1E, 0xF1, 0x81, 0xBF, 0xEC, 0x29,
    0x69, 0x26, 0xF7, 0xC0, 0xB1, 0xC6, 0xE6, 0xB0,
    0x57, 0x87, 0x15, 0x4C, 0xE4, 0xE7, 0x54, 0xA8,
    0x8B, 0x5C, 0xAC, 0x21, 0x6B, 0xE4, 0x0A, 0x0B,
    0x92, 0xB2, 0xB9, 0xAB, 0x53, 0xBE, 0xA8, 0xCB,
    0xC9, 0xF1, 0x62, 0xF4,
};
#endif //MODULE_CHECK //check for unsigned modules

// SONIC MediaVault Source Filter
//const WCHAR kSonicMediaVaultSourceFileName[] = L"MVSource.dll";
//
//// SONIC HDDemuxer Filter
//// {250D7B7D-4344-41fd-A284-7299B7765F18}
//DEFINE_GUID(CLSID_SonicHDDemuxer, 
//            0x250d7b7d, 0x4344, 0x41fd, 0xa2, 0x84, 0x72, 0x99, 0xb7, 0x76, 0x5f, 0x18);
//const WCHAR kSonicHDDemuxerFileName[] = L"SonicHDDemuxer.dll";
//const WCHAR C_SZ_SONIC_HD_DEMUXER[] = L"Sonic HD Demuxer";
//
//// SONIC Audio Decoder Filter
//// {6F3B764A-B207-4f55-9DF9-6852D77C3203}
//DEFINE_GUID(CLSID_SonicAudioDecoder, 
//            0x6f3b764a, 0xb207, 0x4f55, 0x9d, 0xf9, 0x68, 0x52, 0xd7, 0x7c, 0x32, 0x3);
//const WCHAR kSonicCinemasterAudioFileName[] = L"CinemasterAudio.dll";
//const WCHAR C_SZ_SONIC_AUDIO_DECODER[] = L"Sonic Cinemaster Audio Decoder 5.0 for MV";
//
//// SONIC Video Decoder Filter
//// {87BE77E3-BAC1-41b9-8555-D366C314A533}
//DEFINE_GUID(CLSID_SonicVideoDecoder, 
//            0x87be77e3, 0xbac1, 0x41b9, 0x85, 0x55, 0xd3, 0x66, 0xc3, 0x14, 0xa5, 0x33);
//const WCHAR kSonicCinemasterVideoFileName[] = L"CinemasterVideo.dll";
//const WCHAR C_SZ_SONIC_VIDEO_DECODER[] = L"Sonic Cinemaster VideoDecoder 5.0 for MV";

// MV initialization params
//MV_SPParams_t MvSpParams = 
//{
//    {
//        {0xe1,0x29,0xf2,0x7c,0x51,0x03,0xbc,0x5c,0xc4,0x4b,0xcd,0xf0,0xa1,0x5e,0x16,0x0d,0x44,0x50,0x66,0xff},
//        "CoreMedia",
//        "HRVMV001"
//    },
//    "",
//    0,
//    0,
//    0
//};
//void MvCallback(MV_CbInfo_t *pInfo)
//{
//}

const unsigned char EPIDParams[] = {
    0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x89, 0x57, 0x3f, 0x17, 0x47, 0x30, 0x8c, 0x43, 0xd5, 0xee, 0x41, 0x97, 0x96, 0x19, 0x72, 
    0xbb, 0x86, 0x88, 0xed, 0x4b, 0xef, 0x04, 0xab, 0xae, 0xc3, 0x8e, 0xec, 0x51, 0xc3, 0xd3, 0x09, 0xf9, 0x24, 0xe5, 0xd9, 0xbc, 
    0x67, 0x7f, 0x81, 0x0d, 0xf0, 0x25, 0x58, 0xf7, 0x53, 0x13, 0xa9, 0x8a, 0xa6, 0x10, 0x47, 0x65, 0x5d, 0x73, 0x9e, 0xf1, 0x94, 
    0xeb, 0x05, 0xb1, 0xa7, 0x11, 0x00, 0x00, 0x12, 0x97, 0x05, 0x53, 0xd7, 0xc8, 0x81, 0xf7, 0x78, 0xc2, 0x2c, 0x37, 0xb6, 0xc0, 
    0x16, 0x3e, 0x68, 0x24, 0x3a, 0x84, 0x78, 0x1c, 0x0a, 0xdf, 0x9b, 0xb3, 0xed, 0x21, 0xc4, 0x46, 0xe5, 0xa7, 0xa3, 0x92, 0x00, 
    0x3a, 0x2e, 0x39, 0x0e, 0x10, 0xd8, 0xac, 0x47, 0xcb, 0x29, 0xc8, 0xf1, 0x2c, 0x7f, 0x11, 0x99, 0x2a, 0x18, 0xb7, 0xef, 0x73, 
    0x48, 0xa6, 0xbe, 0x70, 0xa6, 0x8b, 0x97, 0x34, 0x8a, 0xb1, 0x02, 0x16, 0x7a, 0x61, 0x53, 0xdd, 0xf6, 0xe2, 0x89, 0x15, 0xa0, 
    0x94, 0xf1, 0xb5, 0xdc, 0x65, 0x21, 0x15, 0x62, 0xe1, 0x7d, 0xc5, 0x43, 0x89, 0xee, 0xb4, 0xef, 0xc8, 0xa0, 0x8e, 0x34, 0x0f, 
    0x04, 0x82, 0x27, 0xe1, 0xeb, 0x98, 0x64, 0xc2, 0x8d, 0x8f, 0xdd, 0x0e, 0x82, 0x40, 0xae, 0xd4, 0x31, 0x63, 0xd6, 0x46, 0x32, 
    0x16, 0x85, 0x7a, 0xb7, 0x18, 0x68, 0xb8, 0x17, 0x02, 0x81, 0xa6, 0x06, 0x20, 0x76, 0xe8, 0x54, 0x54, 0x53, 0xb4, 0xa9, 0xd8, 
    0x44, 0x4b, 0xaa, 0xfb, 0x1c, 0xfd, 0xae, 0x15, 0xca, 0x29, 0x79, 0xa6, 0x24, 0xa4, 0x0a, 0xf6, 0x1e, 0xac, 0xed, 0xfb, 0x10, 
    0x41, 0x08, 0x66, 0xa7, 0x67, 0x36, 0x6e, 0x62, 0x71, 0xb7, 0xa6, 0x52, 0x94, 0x8f, 0xfb, 0x25, 0x9e, 0xe6, 0x4f, 0x25, 0xe5, 
    0x26, 0x9a, 0x2b, 0x6e, 0x7e, 0xf8, 0xa6, 0x39, 0xae, 0x46, 0xaa, 0x24, 0x00, 0x03, 0xdf, 0xfc, 0xbe, 0x2f, 0x5c, 0x2e, 0x45, 
    0x49, 0x7a, 0x2a, 0x91, 0xba, 0xd1, 0x3e, 0x01, 0xec, 0x5f, 0xc2, 0x15, 0x14, 0x10, 0xb3, 0x28, 0x5e, 0x56, 0xcc, 0x26, 0x51, 
    0x24, 0x93, 0x0e, 0x6c, 0x99, 0x96, 0x38, 0xe0, 0x7d, 0x68, 0x8c, 0xb7, 0x97, 0x23, 0xf4, 0xac, 0x4d, 0xbc, 0x5e, 0x01, 0x15, 
    0xff, 0x45, 0x60, 0x08, 0x13, 0xcd, 0x59, 0xd7, 0x73, 0xb0, 0x0c, 0x20, 0x5e, 0xab, 0xaa, 0x24, 0x31, 0xe2, 0x2a, 0xa2, 0x53, 
    0x8a, 0xf7, 0x86, 0xd5, 0x19, 0x78, 0xc5, 0x55, 0x9c, 0x08, 0xb7, 0xe2, 0xf4, 0xd0, 0x37, 0x74, 0x93, 0x56, 0x62, 0x7b, 0x95, 
    0xcc, 0x2c, 0xb0, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbc, 0xe6, 
    0xfa, 0xad, 0xa7, 0x17, 0x9e, 0x84, 0xf3, 0xb9, 0xca, 0xc2, 0xfc, 0x63, 0x25, 0x51, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x5a, 0xc6, 
    0x35, 0xd8, 0xaa, 0x3a, 0x93, 0xe7, 0xb3, 0xeb, 0xbd, 0x55, 0x76, 0x98, 0x86, 0xbc, 0x65, 0x1d, 0x06, 0xb0, 0xcc, 0x53, 0xb0, 
    0xf6, 0x3b, 0xce, 0x3c, 0x3e, 0x27, 0xd2, 0x60, 0x4b, 0x07, 0x78, 0x3b, 0x0d, 0xfe, 0x4a, 0xa3, 0x19, 0x49, 0xb0, 0xce, 0xaf, 
    0x3f, 0x74, 0x0f, 0x32, 0x16, 0x0c, 0x8b, 0x46, 0x94, 0x5b, 0xa5, 0xb0, 0xe4, 0x8a, 0xda, 0xd8, 0x88, 0x32, 0x90, 0x53, 0x08, 
    0xf7, 0xa2, 0xaa, 0xba, 0x62, 0xb3, 0xfe, 0x29, 0x80, 0xc9, 0x5b, 0x63, 0x53, 0xc8, 0x24, 0x3c, 0x7c, 0x1f, 0x4c, 0xda, 0xcd, 
    0xe5, 0x5f, 0xa2, 0x36, 0x93, 0x04, 0x3c, 0x3a, 0xbc, 0x2e, 0x02, 0x10, 0x9a, 0xf4, 0x06, 0x32, 0x30, 0x89, 0xcb, 0x95, 0xe9, 
    0x55, 0x0e, 0x9d, 0xaf, 0x0e, 0x98, 0xcd, 0xca, 0xdc, 0xb1, 0xff, 0xfc, 0xd1, 0x45, 0x66, 0xbb, 0x86, 0x46, 0x1e, 0x8c, 0x30, 
    0x04, 0x78, 0x53, 0xe1, 0x3f, 0x96, 0xc5, 0xe4, 0x15, 0x23, 0x7b, 0x1f, 0x3f, 0x2c, 0xd3, 0x95, 0x40, 0xbc, 0x7a, 0x31, 0x1f, 
    0x14, 0x38, 0x9e, 0x1a, 0xa5, 0xd6, 0x63, 0x10, 0x91, 0xe4, 0xd3, 0x00, 0xb4, 0x02, 0xbc, 0x47, 0xfa, 0xa6, 0x29, 0x82, 0x0b, 
    0xb1, 0xd5, 0xff, 0xf2, 0xe6, 0xb0, 0xc6, 0xae, 0xe8, 0x7b, 0x91, 0xd9, 0xee, 0x66, 0x07, 0x1f, 0xfd, 0xa2, 0xe7, 0x02, 0x66, 
    0xdd, 0x05, 0x2e, 0xf8, 0xc6, 0xc1, 0x6a, 0xef, 0x3c, 0xc1, 0x95, 0xf6, 0x26, 0xce, 0x5e, 0x55, 0xd1, 0x64, 0x13, 0x28, 0xb1, 
    0x18, 0x57, 0xd8, 0x1b, 0x84, 0xfa, 0xec, 0x7e, 0x5d, 0x99, 0x06, 0x49, 0x05, 0x73, 0x35, 0xa9, 0xa7, 0xf2, 0xa1, 0x92, 0x5f, 
    0x3e, 0x7c, 0xdf, 0xac, 0xfe, 0x0f, 0xf5, 0x08, 0xd0, 0x3c, 0xae, 0xcd, 0x58, 0x00, 0x5f, 0xd0, 0x84, 0x7e, 0xea, 0x63, 0x57, 
    0xfe, 0xc6, 0x01, 0x56, 0xda, 0xf3, 0x72, 0x61, 0xda, 0xc6, 0x93, 0xb0, 0xac, 0xef, 0xaa, 0xd4, 0x51, 0x6d, 0xca, 0x71, 0x1e, 
    0x06, 0x73, 0xea, 0x83, 0xb2, 0xb1, 0x99, 0x4a, 0x4d, 0x4a, 0x0d, 0x35, 0x07, 0x6b, 0x17, 0xd1, 0xf2, 0xe1, 0x2c, 0x42, 0x47, 
    0xf8, 0xbc, 0xe6, 0xe5, 0x63, 0xa4, 0x40, 0xf2, 0x77, 0x03, 0x7d, 0x81, 0x2d, 0xeb, 0x33, 0xa0, 0xf4, 0xa1, 0x39, 0x45, 0xd8, 
    0x98, 0xc2, 0x96, 0x4f, 0xe3, 0x42, 0xe2, 0xfe, 0x1a, 0x7f, 0x9b, 0x8e, 0xe7, 0xeb, 0x4a, 0x7c, 0x0f, 0x9e, 0x16, 0x2b, 0xce, 
    0x33, 0x57, 0x6b, 0x31, 0x5e, 0xce, 0xcb, 0xb6, 0x40, 0x68, 0x37, 0xbf, 0x51, 0xf5, 0x93, 0xca, 0x4a, 0x6a, 0x3e, 0xe3, 0x16, 
    0xef, 0x63, 0x8b, 0xfb, 0x84, 0xf7, 0xff, 0xae, 0x27, 0xbe, 0xaf, 0x88, 0xcd, 0xdf, 0x63, 0x0a, 0xe4, 0x29, 0x96, 0x12, 0x32, 
    0xe7, 0xf4, 0xc7, 0x36, 0xdd, 0xf8, 0x71, 0x21, 0xf5, 0x73, 0x9e, 0x3d, 0x9f, 0xbf, 0xa1, 0x14, 0x90, 0xa7, 0x67, 0x1b, 0x24, 
    0xdf, 0xde, 0x96, 0x83, 0x05, 0x83, 0x6d, 0x6e, 0x34, 0xf4, 0xa1, 0xd0, 0xb8, 0x36, 0x5d
};

//void EnvDlgAndExit(void)
//{
//		WCHAR wMsgBuf[MAX_PATH];
//
//		SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
//
//		MultiByteToWideChar( CP_ACP             //UINT CodePage
//			, MB_PRECOMPOSED                     //DWORD dwFlags
//			, Sonic_StrUncloak(EnvErrorCloaked)  //LPCSTR lpMultiByteStr
//			, -1                                 //int cbMultiByte - null term str
//			, wMsgBuf                            //LPWSTR lpWideCharStr
//			, MAX_PATH                           //int cchWideChar        
//			);
//		
//		MessageBox(HWND_DESKTOP,wMsgBuf,appExeName,MB_ICONERROR | MB_SYSTEMMODAL | MB_TOPMOST | MB_SETFOREGROUND);
//		
//		TerminateProcess((HANDLE)-1,0);
//		_exit(0);
//
//		//backup controlled crash (in case the attacker bypassed the TerminateProcess() and exit()
//		_asm
//		{
//			mov ebx,0
//			push ebx
//			pop esp
//			mov ebp, esp
//			mov eax, ebx
//			jmp eax
//		}
//		//note: if we get hear, we have nothing on the stack to return to, and will crash
//}
//
////obfuscated function pointer to Error Dialog
//ENVDLGANDEXIT pfnEnvDlgAndExit = (ENVDLGANDEXIT)XORV3(EnvDlgAndExit, XOR_ENVDLGANDEXIT);

#define USE_LOCALLOAD 1
typedef HRESULT (STDAPICALLTYPE* DllGetClassObjectFunc)(REFCLSID rClsID, REFIID riid, void ** ppv);
static HRESULT LocalLoad(REFCLSID rClsID, LPUNKNOWN pUnkOuter, LPCWSTR lpFileName, REFIID riid, LPVOID FAR * ppv)
{
    HRESULT hr = S_OK;
    HMODULE hModule = LoadLibraryExW(lpFileName, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    if (hModule == NULL)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        return hr;
    }
    DllGetClassObjectFunc pDllGetClassObjectFunc = (DllGetClassObjectFunc)GetProcAddress(hModule, "DllGetClassObject");
    if (pDllGetClassObjectFunc == NULL)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
    }
    else
    {
        IClassFactory* pIClassFactory;
        hr = pDllGetClassObjectFunc(rClsID, IID_IClassFactory, reinterpret_cast<void **>(&pIClassFactory));
        if (SUCCEEDED(hr))
        {
            hr = pIClassFactory->CreateInstance(pUnkOuter, riid, ppv);
            pIClassFactory->Release();
        }
    }

    if (FAILED(hr))
        FreeLibrary(hModule);

    return hr;
}

// Release the format block for a media type.
static void _FreeMediaType(AM_MEDIA_TYPE& mt)
{
    if (mt.cbFormat != 0)
    {
        CoTaskMemFree((PVOID)mt.pbFormat);
        mt.cbFormat = 0;
        mt.pbFormat = NULL;
    }
    if (mt.pUnk != NULL)
    {
        // pUnk should not be used.
        mt.pUnk->Release();
        mt.pUnk = NULL;
    }
}

CIMVDShowPlayer::CIMVDShowPlayer()
:m_state(PlayerState_Uninitialized)
,m_hVideoWnd(NULL)
,m_videoWidth(0)
,m_videoHeight(0)
,m_dRate(1.0f)
,m_mvSession(0)
,m_pEPID(0)
,m_bProtectAudio(FALSE)
,m_bProtectedStream(FALSE)
,m_bIsIMVPlatform(false)
//,m_pMvAPI(0)
,m_hFile(NULL)
{
    IMV_TRACE(_T("CIMVDShowPlayer::CIMVDShowPlayer()\n"));

#if defined(MODULE_CHECK)
	//get app name for cloaked dialog messages
	WCHAR   seps[] = L"\\/";
	WCHAR   szModName[MAX_PATH];
	WCHAR   *pFirst = NULL;
	WCHAR   *pNext = NULL;
	WCHAR   *context = NULL;
	// retrieves the path of the executable file of the current process.
	DWORD dwBytesCopied = GetModuleFileName(NULL,szModName,MAX_PATH);
	if(dwBytesCopied)
	{
		//skip past the path
		pFirst = wcstok_s(szModName,seps,&context);
		do
		{
			pNext = wcstok_s(NULL, seps, &context);

			if(pNext)	//we want the last seperator
				pFirst = pNext;
		}while(pNext);
	}

	if(pFirst)
	{
		wcscpy_s(appExeName, MAX_PATH, pFirst);
	}
#endif //MODULE_CHECK

    GetModuleFileNameW(_AtlBaseModule.GetModuleInstance(), m_wszFiltersPath, MAX_PATH);
    size_t len = wcslen(m_wszFiltersPath);
    while (len > 0 && m_wszFiltersPath[len - 1] != '\\')
        len--;
    m_wszFiltersPath[len - 1] = '\0';

    // Ensure the delay loaded MV DLLs are on the system
    wchar_t sdk_dll[MAX_PATH];
    swprintf_s(sdk_dll,MAX_PATH,L"%s\\%s", m_wszFiltersPath, L"MVClientSDK.DLL");
    //m_pMvAPI = new SonicMVClient(sdk_dll, lstrlenW(sdk_dll)); 
    //if (m_pMvAPI && m_pMvAPI->DLLLoaded())
    //{
    //    MV_Version_t version = {0};
    //    MV_RESULT mvStatus = m_pMvAPI->MV_GetVersion(&version);

    //    if (MV_SUCCESSFUL == mvStatus)
    //    {
    //        IMV_TRACE(_T("CIMVDShowPlayer MV_GetVersion OK ! \n"));

    //        TCAES128 tcenc;
    //        tcenc.RandomNumber((UInt8 *)m_seed, MV_PRNG_SEED_BITS_MIN/8);

    //        // Initialize MV
    //        strcpy_s((char *)&MvSpParams.crlDir, sizeof(MvSpParams.crlDir), IMV_DIRECTORY);
    //        MvSpParams.pEpidParamCert = (UInt8*)EPIDParams;
    //        MvSpParams.prngSeedBits = MV_PRNG_SEED_BITS_MIN;
    //        MvSpParams.pSeed = m_seed;

    //        MV_RESULT mvStatus = m_pMvAPI->MV_Initialize(&MvSpParams, MvCallback, 1);
    //        m_bIsIMVPlatform = (mvStatus == MV_SUCCESSFUL);
    //        if(m_bIsIMVPlatform)
    //        {
    //            IMV_TRACE(_T("CIMVDShowPlayer MV_Initialize OK\n"));
    //        }
    //        else
    //        {
    //            IMV_TRACE(_T("CIMVDShowPlayer MV_Initialize Failed\n"));
    //            delete m_pMvAPI;
    //            m_pMvAPI = 0;
    //        }
    //    }
    //    else
    //    {
    //        IMV_TRACE(_T("CIMVDShowPlayer MV_GetVersion Failed\n"));
    //    }
    //}
    //else if (m_pMvAPI)
    //{
    //    IMV_TRACE(_T("CIMVDShowPlayer MVClient.dll not found !!!\n"));

    //    delete m_pMvAPI;
    //    m_pMvAPI = 0;
    //}
}

CIMVDShowPlayer::~CIMVDShowPlayer(void)
{
    IMV_TRACE(_T("CIMVDShowPlayer::~CIMVDShowPlayer()\n"));
    TearDownGraph();
	if (m_spDVRPlayer.get() != NULL)
	{
		m_spDVRPlayer->Destory();
		m_spDVRPlayer.reset(0);
	}
	if(m_bIsIMVPlatform)
	{        
		//m_pMvAPI->MV_DeInitialize();
		//
		//delete m_pMvAPI;
		//m_pMvAPI = 0;
    }
}

HRESULT CIMVDShowPlayer::InitializeGraph()
{
    IMV_TRACE(_T("CIMVDShowPlayer::InitializeGraph()\n"));
    TearDownGraph();
    // Create the Filter Graph Manager.
	/*if (m_spDVRPlayer.get() == NULL)
		m_spDVRPlayer.reset(new CDVRPlayer);
	m_spDVRPlayer->Init(m_hVideoWnd, m_hVideoWnd);*/

    HRESULT hr = m_pFilterGraph.CoCreateInstance(CLSID_FilterGraph);
    CHKHR;
    m_pMediaControl = m_pFilterGraph;
    CheckPointer(m_pMediaControl,E_NOINTERFACE);
    m_pMediaSeeking = m_pFilterGraph;
    CheckPointer(m_pMediaSeeking,E_NOINTERFACE);
    m_pMediaEvent = m_pFilterGraph;
    CheckPointer(m_pMediaEvent,E_NOINTERFACE);

	if(::IsWindow(m_hVideoWnd))
	{
		// Set up event notification.
		hr = m_pMediaEvent->SetNotifyWindow((OAHWND)m_hVideoWnd, WM_GRAPH_EVENT, NULL);
	}

    return hr;
}

void CIMVDShowPlayer::TearDownGraph()
{
    IMV_TRACE(_T("CIMVDShowPlayer::TearDownGraph()\n"));

	/*if (m_spDVRPlayer.get() != NULL)
	{
		m_spDVRPlayer->Destory();
		m_spDVRPlayer.reset(0);
	}*/
    // Stop sending event messages
    if (m_pMediaEvent)
    {
        m_pMediaEvent->SetNotifyWindow((OAHWND)NULL, NULL, NULL);
    }

    m_pFilterGraph.Release();
    m_pMediaControl.Release();
    m_pMediaSeeking.Release();
    m_pMediaEvent.Release();
    m_pFileSource.Release();
    m_pHDDemuxer.Release();
    m_pAudDec.Release();
    m_pDirectSound.Release();
    m_pVidDec.Release();
	if (m_pEPID)
	{
		if(m_mvSession)
		{
			m_pEPID->CloseSession();
			m_mvSession = 0;
		}
		//ISonicEPID::DestroyEPID(m_pEPID);
		m_pEPID = 0;
	}
	m_pEVR.Release();
	m_pEVRVideoDisplayControl.Release();

	if(m_hFile)
	{
		CloseHandle(m_hFile);
		m_hFile = 0;
	}

	m_dRate = 1.0f;
    m_videoHeight = 0;
    m_videoWidth = 0;
    m_bProtectedStream = FALSE;
    m_state = PlayerState_Uninitialized;
}

int CIMVDShowPlayer::SplitMode(int pahtLen)
{

    int splitMode = 0;	//1;3;4;6;9;16;25;36
	switch (pahtLen)
	{
	case 1:
		splitMode = 1;
		break;
	case 2:
	case 3:
		splitMode = 3;
		break;
	case 4:
		splitMode = 4;
		break;
	case 5:
	case 6:
		splitMode = 6;
		break;
	case 7:
	case 8:
	case 9:
		splitMode = 9;
		break;
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
		splitMode = 16;
		break;
	case  17:
	case  18:
	case  19:
	case  20:
	case  21:
	case  22:
	case  23:
	case  24:
	case  25:
		splitMode = 25;
		break;
	case  26:
	case  27:
	case  28:
	case  29:
	case  30:
	case  31:
	case  32:
	case  33:
	case  34:
	case  35:
	case  36:
		splitMode = 36;
		break;
	default:
		splitMode = 0;
	}

	return splitMode;
}

HRESULT CIMVDShowPlayer::Open(LPCWSTR moviePath)
{
	HRESULT hr = E_FAIL;
	//do 
	//{
		if(moviePath == NULL || lstrlenW(moviePath) == 0)
		{
			hr = VARIANT_FALSE;
			return hr;
			//break;
		}

		// Create a new filter graph. (This also closes the old one, if any.)
		if (m_hVideoWnd == 0) //当播放报警文件的时候，有时候CDVRMVPlayerCtrl::OnCreate函数不执行(原因不知道，比较头疼)
		{
			MessageBox(NULL, _T("Open file failed"), _T("Error"), MB_OK);
			hr = VARIANT_FALSE;
			return hr;
		}
		hr = InitializeGraph();
		CHKHR;
		//if(FAILED(hr))
			//break;
		IMV_TRACE(_T("CIMVDShowPlayer::InitializeGraph() OK ! \n"));
		
		//moviePath = _T("D:\\vsFiles\\20110831-205305-02.vs;D:\\vsFiles\\20110831-205305-02.vs;D:\\vsFiles\\20110831-205305-02.vs");
		//moviePath = _T("D:\\vsFiles\\有人入侵.vs;D:\\vsFiles\\有人入侵.vs;D:\\vsFiles\\有人入侵.vs");
		CString tempPath = moviePath;
		CString path; 
		int curPos = 0;

		path = tempPath.Tokenize( _T(";"),curPos);

		while (path !="")
		{
			paths.push_back(path);
			path = tempPath.Tokenize(_T( ";"),curPos);
		}; 

		int len = paths.size();

		CDVRSettings::GetInstance()->m_nRenderWndNum = SplitMode(len);

		m_spDVRPlayer->OpenVsFile(paths);
		paths.clear();
		//m_spDVRPlayer->Open(CW2T(moviePath));
		m_state = m_spDVRPlayer->GetPlayState() == CDVRPlayer::eState_Play ? PlayerState_Playing : PlayerState_Uninitialized;
		if (m_state == PlayerState_Playing)
		{
			//m_spDVRPlayer->GetPictureSize(&m_videoWidth, &m_videoHeight);
			//m_spDVRPlayer->RefreshPlay();
			return S_OK;
		}
		return E_FAIL;
	//	// Add the source filter to the graph.
	//	hr = LoadSourceFilter(moviePath);
	//	if(FAILED(hr))
	//		break;
	//	IMV_TRACE(_T("CIMVDShowPlayer LoadSourceFilter OK ! \n"));

	//	// Add the Demuxer filter to the graph and connect it to the Source Filter.
	//	hr = LoadHDDemuxer();
	//	if(FAILED(hr))
	//		break;
	//	IMV_TRACE(_T("CIMVDShowPlayer LoadHDDemuxer OK ! \n"));

	//	// render the Video stream
	//	hr = LoadVideoDecoder();
	//	if(FAILED(hr))
	//		break;
	//	IMV_TRACE(_T("CIMVDShowPlayer LoadVideoDecoder OK ! \n"));

	//	// MUST turn off Areo before config EVR; Otherwise OS hange when the video window is astride two monitors  
	//	if(m_bProtectedStream)
	//	{
	//		if(m_bIsIMVPlatform)
	//		{
	//			BOOL bCompositionEnabled = TRUE;
	//			DwmIsCompositionEnabled(&bCompositionEnabled);
	//			if(bCompositionEnabled)
	//				DwmEnableComposition(DWM_EC_DISABLECOMPOSITION);
	//		}
	//		else
	//		{
	//			IMV_TRACE(_T("CIMVDShowPlayer Cannot play protected content on NON-INTEL platform!!!\n"));
	//			break;
	//		}
	//	}

	//	hr = LoadVideoRenderer();
	//	if(FAILED(hr))
	//		break;
	//	IMV_TRACE(_T("CIMVDShowPlayer LoadVideoRenderer OK ! \n"));

	//	// render the Audio stream
	//	hr = LoadAudioDecoder();
	//	if(FAILED(hr))
	//		break;
	//	IMV_TRACE(_T("CIMVDShowPlayer LoadAudioDecoder OK ! \n"));
	//	hr = LoadAudioRenderer();
	//	if(FAILED(hr))
	//		break;
	//	IMV_TRACE(_T("CIMVDShowPlayer LoadAudioRenderer OK ! \n"));

	//	// setup security session
	//	if(m_bProtectedStream)
	//	{
	//		hr = InitializePavp(moviePath);
	//		if(FAILED(hr))
	//			break;
	//		IMV_TRACE(_T("CIMVDShowPlayer InitializePavp OK\n"));
	//	}

	//	m_hFile = CreateFileW(moviePath,
	//		GENERIC_READ,
	//		FILE_SHARE_READ | FILE_SHARE_WRITE,
	//		NULL,
	//		OPEN_EXISTING,
	//		FILE_ATTRIBUTE_NORMAL,
	//		0);

	//	if(!m_hFile)
	//	{
	//		hr = HRESULT_FROM_WIN32(GetLastError());
	//		break;
	//	}

	//	m_state = PlayerState_Stopped;
	//	return S_OK;

	//} while (FALSE);

	//// Open movie failed
	//TearDownGraph();
	//return hr;
}

HRESULT CIMVDShowPlayer::CheckBufferring()
{
	if(m_state == PlayerState_Uninitialized ||
	   m_state == PlayerState_Paused ||
	   m_state == PlayerState_Stopped ||
	   m_state == PlayerState_ScanReverse)
	   return S_FALSE;

	LONGLONG curPos = 0;
	GetCurrentPosition(&curPos);
	LONGLONG availabelPos = 0;
	GetAvailablePosition(&availabelPos);
	LONGLONG durition = 0;
	GetDuration(&durition);

	#if defined(DEBUG) || defined(DEV_RELEASE)
	CHAR debugString[MAX_PATH];
	sprintf_s(debugString,256,"CheckBufferring : cur=%d available=%d during=%d\n",(INT)(curPos/UNITS),(INT)(availabelPos/UNITS) ,(INT)(durition/UNITS));
	OutputDebugStringA(debugString);
	#endif

	if(m_state == PlayerState_Buffering)
	{
		
		if((availabelPos-curPos) > BUFFERING_THRESHOLD_OUT || 
			availabelPos == durition )
		{
			#if defined(DEBUG) || defined(DEV_RELEASE)
			OutputDebugStringA("CheckBufferring: End buffer");
			#endif

			//m_pMediaControl->Run();
			m_spDVRPlayer->Play();
			m_state = PlayerState_Playing;
		}
	}
	else
	{
		if((availabelPos-curPos) < BUFFERING_THRESHOLD_IN && 
			availabelPos != durition )
		{
			if(m_state == PlayerState_ScanForward)
				SetRate(1.0f);

			#if defined(DEBUG) || defined(DEV_RELEASE)
			OutputDebugStringA("CheckBufferring: Begin buffering......");
			#endif

			m_spDVRPlayer->Play();
			//m_pMediaControl->Pause();
			m_state = PlayerState_Buffering;
		}
	}

	return S_OK;
}

HRESULT CIMVDShowPlayer::Play()
{
    if (!CanPlay())
		return VFW_E_WRONG_STATE;

    HRESULT hr = S_OK;

	if(m_dRate != 1.0f)
	{
		hr = SetRate(1.0f);
	}
	else
	{
		 //hr = m_pMediaControl->Run();
		//m_spDVRPlayer->Play();
		m_spDVRPlayer->PlayVsFile();
	}

	if (SUCCEEDED(hr))
	{
		m_state = m_spDVRPlayer->GetPlayState() == CDVRPlayer::eState_Play ? PlayerState_Playing : m_state;
	}

    return hr;
}

HRESULT CIMVDShowPlayer::Pause()
{
    if (!CanPause())
        return VFW_E_WRONG_STATE;

    //HRESULT hr = m_pMediaControl->Pause();
	HRESULT hr = S_OK;
	m_spDVRPlayer->PauseVsFile();
	//m_spDVRPlayer->Pause();
    if (SUCCEEDED(hr))
    {
        m_state = m_spDVRPlayer->GetPlayState() == CDVRPlayer::eState_Pause ? PlayerState_Paused : m_state;
    }
    return hr;
}

HRESULT CIMVDShowPlayer::Stop(BOOL bSeekToBegin /* = TRUE */)
{
    if (!CanStop())
        return VFW_E_WRONG_STATE;

	if(m_dRate != 1.0f)
	{
		SetRate(1.0f,true);
	}

    //HRESULT hr = m_pMediaControl->Stop();
	HRESULT hr = S_OK;
	//m_spDVRPlayer->Stop();
	m_spDVRPlayer->StopVsFile();
    if (SUCCEEDED(hr))
    {
		if(bSeekToBegin)
		{
			LONGLONG currentTime = 0;
			//m_pMediaSeeking->SetPositions(&currentTime, AM_SEEKING_AbsolutePositioning, NULL, 0);
			m_spDVRPlayer->SetPosition(0);
		}

        m_state = m_spDVRPlayer->GetPlayState() == CDVRPlayer::eState_Stop ? PlayerState_Stopped : m_state;
    }
    return hr;
}

HRESULT CIMVDShowPlayer::SetRate(double dSpeed, bool bStop/* =false */)
{
    HRESULT hr = S_OK;

    if (!CanChangeRate())
        return VFW_E_WRONG_STATE;

    if(m_state == PlayerState_Buffering && dSpeed > 0.0f)
        return VFW_E_WRONG_STATE;

    if (dSpeed != m_dRate)
    {
		if((dSpeed-1.0) > 0)
		{
			m_state = PlayerState_ScanForward;
		}
		else
			m_state = PlayerState_ScanReverse;

		m_spDVRPlayer->AdjustSpeed(dSpeed-1.0);
        //CComQIPtr<IParserTrickPlay> pScan(m_pHDDemuxer);
        //CheckPointer(pScan,E_UNEXPECTED);

        //bool bSetPos = false;
        //LONGLONG pos = 0;

        //// workaround for CNMM-401
        //// if change direction in trick play, switch to normal play mode first
        //if( (m_dRate < -1.01f && dSpeed  > 1.01f) ||
        //    (dSpeed  < -1.01f && m_dRate > 1.01f))
        //{
        //    SetRate(1.0);
        //    ::Sleep(100);
        //}

        //m_dRate = dSpeed;
        //// When returning to 1x playback issue a standard seek before changing
        //// the rate to get the pipe line ready for playback.
        //if(dSpeed > 0.99f && dSpeed< 1.01f)
        //{
        //    hr = pScan->GetScanningTime(&pos);
        //    if(SUCCEEDED(hr))
        //    {
        //        bSetPos = true;
        //    }
        //}

        //pScan->SetScanningRate(dSpeed);

        //// should call SetScanningRate() before SetPositions().
        //if((bSetPos) && (!bStop))
        //{
        //    hr = m_pMediaSeeking->SetPositions(&pos, AM_SEEKING_AbsolutePositioning, 0, 0);
        //}

        //// Update m_state after set rate.
        //if( dSpeed>0.99f && dSpeed<1.01f && 
        //    ( m_state==PlayerState_ScanForward || m_state==PlayerState_ScanReverse || m_state==PlayerState_Paused) )
        //{
        //    m_state = PlayerState_Playing;
        //}
        //else if(dSpeed >= 1.01f)
        //{
        //    // from pause to trick play
        //    if(SUCCEEDED(hr) && m_state==PlayerState_Paused)
        //    {
        //        hr = m_pMediaControl->Run();
        //    }

        //    if(SUCCEEDED(hr))
        //    {
        //        m_state = PlayerState_ScanForward;
        //    }
        //}
        //else if(dSpeed < 0.0f)
        //{
        //    // from pause to trick play
        //    if(m_state==PlayerState_Paused)
        //    {
        //        hr = m_pMediaControl->Run();
        //    }

        //    if(SUCCEEDED(hr))
        //    {
        //        m_state = PlayerState_ScanReverse;
        //    }
        //}
        //else
        //{
        //    IMV_TRACE(_T("In SetRate, unknown rate "));
        //}
    }

    return hr;
}

HRESULT CIMVDShowPlayer::GetDuration(LONGLONG *pDuration)
{
    CheckPointer(pDuration,E_POINTER);
    *pDuration = 0;
    if(m_state == PlayerState_Uninitialized)
        return VFW_E_WRONG_STATE;

	m_spDVRPlayer->GetMaxVsFileTime();
	if(!m_spDVRPlayer->m_MaxVSFileTime.empty())
	{
		for (std::map<int, LONG>::iterator it = m_spDVRPlayer->m_MaxVSFileTime.begin(); it != m_spDVRPlayer->m_MaxVSFileTime.end(); ++it)
		{
			*pDuration = it->second;
		}
	}
	//*pDuration = m_spDVRPlayer->GetDuration();
	return S_OK;
    //CheckPointer(m_pMediaSeeking,E_UNEXPECTED);
    //return m_pMediaSeeking->GetDuration(pDuration);
}

HRESULT CIMVDShowPlayer::GetAvailablePosition(LONGLONG *pAvailablePosition)
{
	CheckPointer(pAvailablePosition,E_POINTER);
	*pAvailablePosition = 0;
	if(m_state == PlayerState_Uninitialized)
		return VFW_E_WRONG_STATE;

	LARGE_INTEGER fs;
	if (!GetFileSizeEx(m_hFile, &fs))
		return HRESULT_FROM_WIN32(GetLastError());

	//CComQIPtr<IParserTrickPlay> pScan(m_pHDDemuxer);
	//CheckPointer(pScan,E_UNEXPECTED);
	//return pScan->Size2Duration(fs.QuadPart,pAvailablePosition);

	return E_FAIL;
}

HRESULT CIMVDShowPlayer::GetCurrentPosition(LONGLONG *pCurrentPosition)
{
	CheckPointer(pCurrentPosition,E_INVALIDARG);
	*pCurrentPosition = 0;
	if(m_state == PlayerState_Uninitialized)
		return VFW_E_WRONG_STATE;

	CheckPointer(m_pMediaSeeking,E_UNEXPECTED);
	//HRESULT hr = m_pMediaSeeking->GetCurrentPosition(pCurrentPosition);
	HRESULT hr = S_OK;
	CHKHR;

	m_spDVRPlayer->GetMaxVsFileCurrentTime();

	if(!m_spDVRPlayer->m_CurrentVSFileTime.empty())
	{
		for (std::map<int, LONG>::iterator it = m_spDVRPlayer->m_CurrentVSFileTime.begin(); it != m_spDVRPlayer->m_CurrentVSFileTime.end(); ++it)
		{
			*pCurrentPosition = it->second;
		}
	}

	//*pCurrentPosition = m_spDVRPlayer->GetCurrentPosition();

	// when trick play mods, report native PTS(from video decoder) as current position.
	if(m_state == PlayerState_ScanForward || m_state == PlayerState_ScanReverse)
	{
		//CComQIPtr<IParserTrickPlay> pScan(m_pHDDemuxer);
		//CheckPointer(pScan,E_UNEXPECTED);
		//double dRate = pScan->GetScanningRate();
		//if(dRate<=0.99 || dRate>=1.01)
		//{
		//	hr = pScan->GetScanningTime(pCurrentPosition);
		//}
	}

	return hr;
}

HRESULT CIMVDShowPlayer::SetCurrentPosition(LONGLONG toPosition)
{
    if (!CanSeek())
        return VFW_E_WRONG_STATE;

	if(m_state == PlayerState_Buffering)
	{
		LONGLONG curPos = 0;
		GetCurrentPosition(&curPos);
		if(toPosition >= curPos)
			return VFW_E_WRONG_STATE;
	}

    // switch to normal play mode if seek in trick play.
    // should call SetScanningRate() before SetPositions().
    if(m_state == PlayerState_ScanForward || m_state == PlayerState_ScanReverse)
    {
		//CComQIPtr<IParserTrickPlay> pScan(m_pHDDemuxer);
		//CheckPointer(pScan,E_UNEXPECTED);
		//m_dRate = 1.0f;
		//m_state = PlayerState_Playing;
		//pScan->SetScanningRate(m_dRate);
    }

	LONGLONG availablePos = 0;
	GetAvailablePosition(&availablePos);
	LONGLONG durition = 0;
	GetDuration(&durition);
	if( (toPosition > availablePos) ||
		((availablePos-toPosition < BUFFERING_THRESHOLD_IN) && (availablePos != durition)))
	{
		//toPosition = availablePos - BUFFERING_THRESHOLD_IN;
		if(m_state == PlayerState_Playing )
		{
			// PlayerState_Buffering should only be changed from PlayerState_Playing
			//m_spDVRPlayer->Pause();
			//m_pMediaControl->Pause();
			//m_state = PlayerState_Buffering;
		}
	}

	//CheckPointer(m_pMediaSeeking,E_UNEXPECTED);
 //   return m_pMediaSeeking->SetPositions(&toPosition, AM_SEEKING_AbsolutePositioning, NULL, 0);
	if (m_spDVRPlayer->SetPosition(toPosition*1000))
	{
		m_spDVRPlayer->Play();
	}
	return S_OK;
}

HRESULT CIMVDShowPlayer::SetFilePointer(float toPosition)
{	
	//if(m_spDVRPlayer->SetFilePointer(toPosition))
	if(m_spDVRPlayer->SetVsFilePointer(toPosition))
	{
		return S_OK;
	}
	else
		return S_FALSE;
}

HRESULT CIMVDShowPlayer::HandleGraphEvent(IGraphEventCallback* pGraphEventCallback)
{
    CheckPointer(m_pMediaEvent,E_UNEXPECTED);

    long evCode = 0;
    LONG_PTR param1 = 0, param2 = 0;

    HRESULT hr = S_OK;

    // Get the events from the queue.
    while (SUCCEEDED(m_pMediaEvent->GetEvent(&evCode, &param1, &param2, 0)))
    {
// 	    switch (evCode)
// 	    {
// 	    case EC_COMPLETE:
// 	    case EC_USERABORT:
// 	    case EC_ERRORABORT:
// 
// 		    if (m_dRate != 1.0)
// 		        SetRate(1.0, true);
// 		        
// 		    break;
//	    }
        // Invoke the callback.
        pGraphEventCallback->OnGraphEvent(evCode, param1, param2);

        // Free the event data.
        hr = m_pMediaEvent->FreeEventParams(evCode, param1, param2);
        if (FAILED(hr))
        {
            break;
        }
    }
    return hr;
}

HRESULT CIMVDShowPlayer::LoadSourceFilter(LPCWSTR moviePath)
{
	HRESULT hr = E_FAIL;
//#if USE_LOCALLOAD
// 	WCHAR filterPath[MAX_PATH];
// 	swprintf_s(filterPath,MAX_PATH,L"%s\\%s", m_wszFiltersPath, kSonicMediaVaultSourceFileName);
// 	hr = LocalLoad(CLSID_MediaVaultSource, NULL, filterPath, IID_IBaseFilter, (void**)&m_pFileSource);
//#else
//	hr = m_pFileSource.CoCreateInstance(CLSID_MediaVaultSource);
//#endif
//    CHKHR;
//    
//	CComQIPtr<IFileSourceFilter> pIFileSource = m_pFileSource;
//    if(NULL == pIFileSource)
//        return E_NOINTERFACE;
//
//    hr = pIFileSource->Load(moviePath, NULL);
//    CHKHR;
//
//    hr = m_pFilterGraph->AddFilter(m_pFileSource,L"File Source");
//	CHKHR;
//
//#ifdef MODULE_CHECK //check for unsigned modules
//	DWORD dwProcId = GetProcessId(GetCurrentProcess());	    
//	ModuleCheck *pModChk = new ModuleCheck();
//	hr = pModChk->CheckProcess(dwProcId, ALL_DLLS);
//	delete pModChk;
//
//	if(hr != S_OK)	//race condition bug..? try again and we should work fine
//	{
//		ModuleCheck *pModChk = new ModuleCheck();
//		hr = pModChk->CheckProcess(dwProcId, ALL_DLLS);
//		delete pModChk;
//	}
//
//	if(hr != S_OK)	//if still not OK, we have to fail
//	{
//		#if defined(ANTI_TAMPER)
//
//		static DWORD	dwThreadId = 0;
//		static HANDLE	hThread = NULL;
//
//		hThread = pModChk->CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)XORV3(pfnEnvDlgAndExit, XOR_ENVDLGANDEXIT), NULL, 0, &dwThreadId);
//
//		Sleep(THREAD_SWAP_TIMEOUT);
//
//		//back-up plan - if CreateThread fails (highly unlikely) - we still need to inform
//		//the user that we are going to exit		
//		if(hThread == NULL)
//		{
//
//			WCHAR wMsgBuf[MAX_PATH];
//			MultiByteToWideChar( CP_ACP             //UINT CodePage
//				, MB_PRECOMPOSED                     //DWORD dwFlags
//				, Sonic_StrUncloak(EnvErrorCloaked)  //LPCSTR lpMultiByteStr
//				, -1                                 //int cbMultiByte - null term str
//				, wMsgBuf                            //LPWSTR lpWideCharStr
//				, MAX_PATH                           //int cchWideChar        
//				);
//			MessageBox(HWND_DESKTOP,wMsgBuf,appExeName,MB_ICONERROR | MB_SYSTEMMODAL | MB_TOPMOST | MB_SETFOREGROUND);
//			TerminateProcess((HANDLE)-1,0);
//			_exit(0);
//
//			//backup controlled crash (in case the attacker bypassed the TerminateProcess() and exit()
//			_asm
//			{
//				mov ebx,0
//				push ebx
//				pop esp
//				mov ebp, esp
//				mov eax, ebx
//				jmp eax
//			}
//
//		}
//
//		// Hide the fact that we are waiting forever by Creating an Event
//		// and then never have it signaled. Harder to track on WinDBG
//		HANDLE	hEvent;
//		DWORD	dwEvent; 
//
//		hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
//		dwEvent = WaitForMultipleObjects(1, &hEvent, FALSE, INFINITE );
//
//		//if event failed, we fall down here and sleep forever
//		while(1)
//			Sleep(1000);
//
//		#else
//		char	dbgMsg[MAX_PATH];
//		sprintf_s(dbgMsg,MAX_PATH,"!!!MODULE_CHECK::CheckProcess() returns Error - hr: %0X", hr);
//		OutputDebugStringA(dbgMsg);
//		return E_MODULE_CHECK;
//		#endif //ANTI_TAMPER 	
//	}
//#endif //MODULE_CHECK //check for unsigned modules

    return S_OK;
}

HRESULT CIMVDShowPlayer::LoadHDDemuxer()
{
    HRESULT hr = E_FAIL;
//#if USE_LOCALLOAD
//    WCHAR filterPath[MAX_PATH];
//    swprintf_s(filterPath,MAX_PATH,L"%s\\%s", m_wszFiltersPath, kSonicHDDemuxerFileName);
//    hr = LocalLoad(CLSID_SonicHDDemuxer, NULL, filterPath, IID_IBaseFilter, (void**)&m_pHDDemuxer);
//#else
//    hr = m_pHDDemuxer.CoCreateInstance(CLSID_SonicHDDemuxer);
//#endif
//    CHKHR;
//
//    hr = m_pFilterGraph->AddFilter(m_pHDDemuxer,C_SZ_SONIC_HD_DEMUXER);
//    CHKHR;
//
//    CComPtr<IPin> pPinOut;
//    hr = GetOutputPin(m_pFileSource,&pPinOut);
//    CHKHR;
//
//    CComPtr<IPin> pPinIn;
//    hr = GetInputPin(m_pHDDemuxer,&pPinIn);
//    CHKHR;
//
//    return m_pFilterGraph->ConnectDirect(pPinOut,pPinIn,NULL);
	return E_FAIL;
}

HRESULT CIMVDShowPlayer::LoadAudioDecoder()
{
    HRESULT hr = E_FAIL;
//#if USE_LOCALLOAD
//    WCHAR filterPath[MAX_PATH];
//    swprintf_s(filterPath,MAX_PATH,L"%s\\%s", m_wszFiltersPath, kSonicCinemasterAudioFileName);
//    hr = LocalLoad(CLSID_SonicAudioDecoder, NULL, filterPath, IID_IBaseFilter, (void**)&m_pAudDec);
//#else
//    hr = m_pAudDec.CoCreateInstance(CLSID_SonicAudioDecoder);
//#endif
//    CHKHR;
//    hr = m_pFilterGraph->AddFilter(m_pAudDec,C_SZ_SONIC_AUDIO_DECODER);
//    CHKHR;
//
//    // Enable AAC decoding
//    CComQIPtr<ICodecAPI> pAudCodecAPI(m_pAudDec);
//    CheckPointer(pAudCodecAPI,E_UNEXPECTED);
//    VARIANT var;
//    var.intVal = 8;
//    hr = pAudCodecAPI->SetValue(&CODECAPI_SONIC_AUDIO_AAC, &var);
//    CHKHR;
//
//    CComPtr<IPin> pPinOut;
//    hr = GetOutputPin(m_pHDDemuxer,&pPinOut);
//    CHKHR;
//    CComPtr<IPin> pPinIn;
//    hr = GetInputPin(m_pAudDec,&pPinIn);
//    CHKHR;
//    hr = m_pFilterGraph->ConnectDirect(pPinOut,pPinIn,NULL);
//    CHKHR;
//
//    AM_MEDIA_TYPE mt;
//    hr = pPinOut->ConnectionMediaType(&mt);
//    CHKHR;
//    if(mt.subtype != MEDIASUBTYPE_MEDIA_VAULT_AAC && mt.subtype != MEDIASUBTYPE_AAC)
//        hr = E_FAIL;
//    _FreeMediaType(mt);
    return hr;
}

HRESULT CIMVDShowPlayer::LoadAudioRenderer()
{
    HRESULT hr = m_pDirectSound.CoCreateInstance(CLSID_DSoundRender);
    CHKHR;
    hr = m_pFilterGraph->AddFilter(m_pDirectSound,L"DirectSound Device");
    CHKHR;

    CComPtr<IPin> pPinOut;
    hr = GetOutputPin(m_pAudDec,&pPinOut);
    CHKHR;
    CComPtr<IPin> pPinIn;
    hr = GetInputPin(m_pDirectSound,&pPinIn);
    CHKHR;
    return m_pFilterGraph->ConnectDirect(pPinOut,pPinIn,NULL);
}

HRESULT CIMVDShowPlayer::LoadVideoDecoder()
{
    HRESULT hr = E_FAIL;
//#if USE_LOCALLOAD
//    WCHAR filterPath[MAX_PATH];
//    swprintf_s(filterPath,MAX_PATH, L"%s\\%s", m_wszFiltersPath, kSonicCinemasterVideoFileName);
//    hr = LocalLoad(CLSID_SonicVideoDecoder, NULL, filterPath, IID_IBaseFilter, (void**)&m_pVidDec);
//#else
//    hr = m_pVidDec.CoCreateInstance(CLSID_SonicVideoDecoder);
//#endif
//    CHKHR;
//    hr = m_pFilterGraph->AddFilter(m_pVidDec,C_SZ_SONIC_VIDEO_DECODER);
//    CHKHR;
//
//    // Enable AVC decoding
//    CComQIPtr<ICodecAPI> pVidCodecAPI(m_pVidDec);
//    CheckPointer(pVidCodecAPI,E_UNEXPECTED);
//    VARIANT var;
//    var.intVal = 1;
//    hr = pVidCodecAPI->SetValue(&CODECAPI_SONIC_HDVIDEO_AVC, &var);
//    CHKHR;
//
//    CComPtr<IPin> pPinOut;
//    hr = GetOutputPin(m_pHDDemuxer,&pPinOut,1);
//    CHKHR;
//    CComPtr<IPin> pPinIn;
//    hr = GetInputPin(m_pVidDec,&pPinIn);
//    CHKHR;
//    hr = m_pFilterGraph->ConnectDirect(pPinOut,pPinIn,NULL);
//    CHKHR;
//
//    AM_MEDIA_TYPE mt;
//    hr = pPinOut->ConnectionMediaType(&mt);
//    CHKHR;
//    if(mt.subtype == MEDIASUBTYPE_MEDIA_VAULT_AVC1)
//        m_bProtectedStream = TRUE;
//    else if(mt.subtype == MEDIASUBTYPE_AVC1)
//        m_bProtectedStream = FALSE;
//    else
//        hr = E_FAIL;
//
//    if(SUCCEEDED(hr))
//    {
//        if (mt.formattype == FORMAT_MPEG2Video)
//        {
//            MPEG2VIDEOINFO* pMPVI = (MPEG2VIDEOINFO*)mt.pbFormat;
//            m_videoHeight = pMPVI->hdr.bmiHeader.biHeight;
//            m_videoWidth = pMPVI->hdr.bmiHeader.biWidth;
//        }
//    }
//
//    _FreeMediaType(mt);
    return hr;
}

HRESULT CIMVDShowPlayer::LoadVideoRenderer()
{
    // added EVR filter
    HRESULT hr = m_pEVR.CoCreateInstance(CLSID_EnhancedVideoRenderer);
    CHKHR;
    hr = m_pFilterGraph->AddFilter(m_pEVR,L"Enhanced Video Renderer");
    CHKHR;

	if(m_bProtectedStream)
	{
		// use custom presenter 
		CComPtr<IMFVideoPresenter> pPresenter;
		//hr = EVRCustomPresenter::CreateInstance(IID_IMFVideoPresenter,(void**)&pPresenter);
		CHKHR;
		CComQIPtr<IMFVideoRenderer> pMFVR(m_pEVR);
		CheckPointer(pMFVR,E_UNEXPECTED);
		hr = pMFVR->InitializeRenderer(NULL,pPresenter);
		CHKHR;
	}

    //Config EVR
    CComQIPtr<IMFGetService> pMFGetService(m_pEVR);
    CheckPointer(pMFGetService,E_NOINTERFACE);
    hr = pMFGetService->GetService(MR_VIDEO_RENDER_SERVICE,IID_IMFVideoDisplayControl,(LPVOID*)&m_pEVRVideoDisplayControl);
    CHKHR;

	if(::IsWindow(m_hVideoWnd))
	{
		hr = m_pEVRVideoDisplayControl->SetVideoWindow(m_hVideoWnd);
		CHKHR;

		RECT rc;
		if(GetClientRect(m_hVideoWnd,&rc))
			hr = m_pEVRVideoDisplayControl->SetVideoPosition(NULL,&rc);
	}

    hr = m_pEVRVideoDisplayControl->SetAspectRatioMode(MFVideoARMode_PreservePicture);
    hr = m_pEVRVideoDisplayControl->SetBorderColor(RGB(0,0,0));
    hr = m_pEVRVideoDisplayControl->SetRenderingPrefs(MFVideoRenderPrefs_DoNotClipToDevice);

    //Connect EVR with Video decoder
    CComPtr<IPin> pPinOut;
    hr = GetOutputPin(m_pVidDec,&pPinOut);
    CHKHR;
    CComPtr<IPin> pPinIn;
    hr = GetInputPin(m_pEVR,&pPinIn);
    CHKHR;	
    hr = m_pFilterGraph->ConnectDirect(pPinOut,pPinIn,NULL);
    return hr;
}

HRESULT CIMVDShowPlayer::HandleDeviceChanged()
{
	if(m_state == PlayerState_Uninitialized)
		return E_UNEXPECTED;

	if(m_state != PlayerState_Stopped)
	{
		if(m_dRate != 1.0f)
			SetRate(1.0f);
		m_pMediaControl->Stop();
	}

	HRESULT hr = ReInitSession();
	if(FAILED(hr))
	{
		m_state = PlayerState_Stopped;
		return hr;
	}

	RECT rc;
	if(GetClientRect(m_hVideoWnd,&rc))
		m_pEVRVideoDisplayControl->SetVideoPosition(NULL,&rc);

	// state recovery
	switch (m_state)
	{
	case PlayerState_Paused:
	case PlayerState_Buffering:
		m_pMediaControl->Pause();
		break;
	case PlayerState_Playing:
		m_pMediaControl->Run();	
		break;
	}

	return S_OK;
}

HRESULT CIMVDShowPlayer::InitializePavp(LPCWSTR moviePath /* = NULL */)
{
    CComQIPtr<IMFGetService> pMFGetService(m_pEVR);
    CheckPointer(pMFGetService,E_UNEXPECTED);
    CComPtr<IDirect3DDeviceManager9> pDeviceManager;
    HRESULT hr = pMFGetService->GetService(MR_VIDEO_ACCELERATION_SERVICE,IID_IDirect3DDeviceManager9,(LPVOID*)&pDeviceManager);
    CHKHR;

 //   if (!m_pEPID)
 //   {
 //       // Create a MV EPID object
 //       m_pEPID = ISonicEPID::CreateEPID(ISonicEPID::eMediaVault);
	//	if(!m_pEPID)
	//		return E_OUTOFMEMORY;

	//	// Setup a secure session
	//	typedef struct
	//	{
	//		MV_StreamParams_t* params;
	//		ISonicMVClient* pAPI;
	//	} mv_init;

	//	//MV_KeyBlob_t keyBlob;
	//	MV_StreamParams_t streamParams = {0};
	//	streamParams.pKeyBlob = &m_keyBlob;
	//	streamParams.noEncryptAudio = !m_bProtectAudio;
	//	streamParams.streamIdAudio = m_bProtectAudio ? streamParams.streamIdVideo : 1;

	//	mv_init mi;
	//	mi.params = &streamParams;
	//	mi.pAPI = dynamic_cast<ISonicMVClient*>(m_pMvAPI);   
	//	m_pEPID->Init((void*)&mi, sizeof(mi));
 //   }

	//// without moviePath param means re-init session for the current opened movie
	//if(NULL == moviePath)
	//{
	//	SInt32 epid_ret = m_pEPID->PerformKeyExchange(pDeviceManager.p, ISonicEPID::eSessionD3DManager);
	//	if (epid_ret == ISonicEPID::eEPID_OK)
	//	{
	//		m_mvSession = m_pEPID->GetSessionID();
	//		return S_OK;
	//	}
	//	else
	//	{
	//		#if defined(DEBUG) || defined(DEV_RELEASE)
	//		char debugString[256];
	//		sprintf_s(debugString,256,"MV_InitStream err = %d\n",epid_ret);
	//		OutputDebugStringA(debugString);
	//		#endif

	//		return E_FAIL;
	//	}
	//}

	//CHAR szfileName[MAX_PATH];
	//WideCharToMultiByte(CP_UTF8,0, PathFindFileNameW(moviePath), -1, szfileName, MAX_PATH,NULL,NULL);

	//if(!CIMVLicenseDB::GetTitleValidLicense(szfileName,m_keyBlob,m_pEPID,pDeviceManager.p))
	//	return E_NO_VALID_KEY;

	//SInt32 epid_ret = m_pEPID->PerformKeyExchange(pDeviceManager.p, ISonicEPID::eSessionD3DManager);
	//if (epid_ret != ISonicEPID::eEPID_OK)
	//{
	//	#if defined(DEBUG) || defined(DEV_RELEASE)
	//	char debugString[256];
	//	sprintf_s(debugString,256,"MV_InitStream err = %d\n",epid_ret);
	//	OutputDebugStringA(debugString);
	//	#endif

	//	return E_COPP_OUTPUT_PROTECTION_FAILURE;
	//}

	//size_t key_size = 0;
	//MV_StreamKey_t streamKey;
	//m_pEPID->GetStreamKey(ISonicEPID::eAudioPath, 1, streamKey, sizeof(streamKey), key_size);
	//m_mvSession = m_pEPID->GetSessionID();

 //   // Pass this key to the audio decoder
 //   CComQIPtr<ICodecAPI> pAudCodecAPI(m_pAudDec);
 //   CheckPointer(pAudCodecAPI,E_UNEXPECTED);
 //   // AES-ECB encrypt the audio key using the audio decoder's private AES GUID &= itself
 //   char chAesEcbKey [100] = {0};
 //   char chTmpSubKey[256] = {0};
 //   StringFromGUID2(CODECAPI_SONIC_AUDIO_AES_KEY, (LPOLESTR)&chTmpSubKey,128);
 //   WideCharToMultiByte(CP_ACP,0,(LPCWSTR)chTmpSubKey,-1,chAesEcbKey,sizeof(chAesEcbKey),NULL,NULL);
 //   // Use the first 16 bytes of the GUID for our AES-ECB key but tweak it a bit
 //   for (int i=0; i<16; ++i)
 //       chAesEcbKey[i] = ~chAesEcbKey[i];
 //   // Encrypt the audio key
 //   TCAES128 tcenc;
 //   tcenc.EncryptSessionKey((UInt8*)chAesEcbKey, 16, streamKey, 16, (UInt8*)chAesEcbKey);
 //   
 //   // Blast the in-the-clear key
 //   SecureZeroMemory(streamKey, sizeof(streamKey));
 //   
 //   // Pass the encrypted key to the audio decoder
 //   VARIANT var;
 //   InitVariantFromBuffer(chAesEcbKey, 16, &var);
 //   hr = pAudCodecAPI->SetValue(&CODECAPI_SONIC_AUDIO_AES_KEY, &var);
 //   VariantClear(&var);
    return hr;
}

HRESULT CIMVDShowPlayer::SetVolume(LONG lVol)
{
    if(m_state == PlayerState_Uninitialized)
        return VFW_E_WRONG_STATE;

    if (lVol > 100)
        return E_INVALIDARG;

    CComQIPtr<IBasicAudio> pBasicAudio(m_pFilterGraph);
    CheckPointer(pBasicAudio,E_UNEXPECTED);

    long lVolume = -10000;
    lVolume = long( -10000.0f + logf(1.0f + float(lVol) * 5.0f) / log(501.0f) * 10000.0f );
    HRESULT hr = pBasicAudio->put_Volume(lVolume);

    return hr;
}

HRESULT CIMVDShowPlayer::GetVolume(LONG* plVol)
{
    if(m_state == PlayerState_Uninitialized)
        return VFW_E_WRONG_STATE;

    CheckPointer(plVol,E_POINTER);
    CComQIPtr<IBasicAudio> pBasicAudio(m_pFilterGraph);
    CheckPointer(pBasicAudio,E_UNEXPECTED);
    long lVolume;
    HRESULT hr = pBasicAudio->get_Volume(&lVolume); CHKHR;

    const float Y = (1.0f + float(lVolume) / 10000.0f) * logf(501.0f);
    const float fRes = ( powf(2.71828182845904523536, Y) - 1.0f ) / 5.0f;
    if (ceilf(fRes) - fRes > 0.5f)
        *plVol = long(floorf(fRes));
    else
        *plVol = long(ceilf(fRes));

    return S_OK;
}

HRESULT CIMVDShowPlayer::GetFreePin(IBaseFilter* pFilter,IPin** ppPin,PIN_DIRECTION PinDir)
{
    CheckPointer(pFilter,E_INVALIDARG);
    CheckPointer(ppPin,E_INVALIDARG);

    IPin *pPin = NULL;
    CComPtr<IEnumPins> pEnum;
    HRESULT hr = pFilter->EnumPins(&pEnum); CHKHR;
    pEnum->Reset();
    while(pEnum->Next(1, &pPin, 0) == S_OK)
    {
        PIN_DIRECTION PinDirThis;
        pPin->QueryDirection(&PinDirThis);
        if (PinDir == PinDirThis)
        {
            CComPtr<IPin> connectedTo;
            pPin->ConnectedTo(&connectedTo);
            if(connectedTo == NULL)
            {
                // Found a match. Return the IPin pointer to the caller.
                *ppPin = pPin;
                return S_OK;
            }
        }
        // Release the pin for the next time through the loop.
        pPin->Release();
    }
    // No more pins. We did not find a match.
    return E_FAIL;   
}

HRESULT CIMVDShowPlayer::GetInputPin(IBaseFilter* pFilter,IPin** ppPin,UINT PinIndex /* = 0 */)
{
    CheckPointer(pFilter,E_INVALIDARG);
    CheckPointer(ppPin,E_INVALIDARG);

    IPin *pPin = NULL;
    CComPtr<IEnumPins> pEnum;
    HRESULT hr = pFilter->EnumPins(&pEnum); CHKHR;
    pEnum->Reset();
    while(pEnum->Next(1, &pPin, 0) == S_OK)
    {
        PIN_DIRECTION PinDirThis;
        pPin->QueryDirection(&PinDirThis);
        if (PINDIR_INPUT == PinDirThis)
        {
            if ( 0 == PinIndex)
            {
                // Found a match. Return the IPin pointer to the caller.
                *ppPin = pPin;
                return S_OK;
            }

            --PinIndex;
        }
        // Release the pin for the next time through the loop.
        pPin->Release();
    }
    // No more pins. We did not find a match.
    return E_FAIL;   
}

HRESULT CIMVDShowPlayer::GetOutputPin(IBaseFilter* pFilter,IPin** ppPin,UINT PinIndex /* = 0 */)
{
    CheckPointer(pFilter,E_INVALIDARG);
    CheckPointer(ppPin,E_INVALIDARG);

    IPin *pPin = NULL;
    CComPtr<IEnumPins> pEnum;
    HRESULT hr = pFilter->EnumPins(&pEnum); CHKHR;
    pEnum->Reset();
    while(pEnum->Next(1, &pPin, 0) == S_OK)
    {
        PIN_DIRECTION PinDirThis;
        pPin->QueryDirection(&PinDirThis);
        if (PINDIR_OUTPUT == PinDirThis)
        {
            if ( 0 == PinIndex)
            {
                // Found a match. Return the IPin pointer to the caller.
                *ppPin = pPin;
                return S_OK;
            }

            --PinIndex;
        }
        // Release the pin for the next time through the loop.
        pPin->Release();
    }
    // No more pins. We did not find a match.
    return E_FAIL;   
}

VOID CIMVDShowPlayer::SetWindow(HWND hWnd) 
{
	if(::IsWindow(hWnd) && m_hVideoWnd != hWnd)
	{
		HRESULT hr = S_OK;
		if(m_pMediaEvent)
		{
			// Set up event notification.
			hr = m_pMediaEvent->SetNotifyWindow((OAHWND)hWnd, WM_GRAPH_EVENT, NULL);
		}

		if(m_pEVRVideoDisplayControl)
		{
			hr = m_pEVRVideoDisplayControl->SetVideoWindow(hWnd);
			RECT rcVideo;
			GetClientRect(hWnd, &rcVideo);
			UpdateVideoWindow(&rcVideo);
		}

		if(SUCCEEDED(hr))
		{
			m_hVideoWnd = hWnd;
			if (m_spDVRPlayer.get() == NULL)
				m_spDVRPlayer.reset(new CDVRPlayer);
			m_spDVRPlayer->Init(m_hVideoWnd, m_hVideoWnd);
		}
	}
}

VOID CIMVDShowPlayer::RepaintVideo()
{
	if (m_spDVRPlayer.get())
	{
		m_spDVRPlayer->RefreshPlay();
	}
}

VOID CIMVDShowPlayer::UpdateVideoWindow(const LPRECT prc, int nFullScreenState)
{
	if (m_spDVRPlayer.get())
	{
		if (prc != NULL && prc->right - prc->left > 0 && prc->bottom - prc->top > 0)
		{
			CDVRSettings::GetInstance()->m_nRenderWidth = prc->right - prc->left;
			CDVRSettings::GetInstance()->m_nRenderHeight = prc->bottom - prc->top;
		}
		if (nFullScreenState == 0)
		{
			m_spDVRPlayer->ResizeMonitorWindow(false);
		}
		else if (nFullScreenState == 1)
		{
			m_spDVRPlayer->ResizeMonitorWindow(true);
		}
		m_spDVRPlayer->RefreshPlay();
	}
}

HRESULT CIMVDShowPlayer::OpenFile(BSTR bstrMediaFile)
{
	m_spDVRPlayer->Open(bstrMediaFile);
	return S_OK;
}
HRESULT CIMVDShowPlayer::Login(BSTR bstrUsername, BSTR bstrPassword, BSTR bstrIP, LONG lPort)
{
	if (m_spDVRPlayer->Login(_bstr_t(bstrUsername), _bstr_t(bstrPassword), _bstr_t(bstrIP), lPort))
	{
		return S_OK;
	}
	return S_FALSE;
}
HRESULT CIMVDShowPlayer::Logout()
{
	m_spDVRPlayer->Logout();
	return S_OK;
}
HRESULT CIMVDShowPlayer::SetMediaServer(BSTR bstrMediaServerIP, LONG lPort)
{
	CDVRSettings::GetInstance()->m_csMediaServerIP = bstrMediaServerIP;
	CDVRSettings::GetInstance()->m_lPort = lPort;
	return S_OK;
}
HRESULT CIMVDShowPlayer::GetMediaServer(BSTR* bstrMediaServerIP, LONG* lPort)
{
	*bstrMediaServerIP = CDVRSettings::GetInstance()->m_csMediaServerIP.AllocSysString();
	*lPort = CDVRSettings::GetInstance()->m_lPort;
	return S_OK;
}
HRESULT CIMVDShowPlayer::StartMonitor(LONG lWndNum)
{
	CDVRSettings::GetInstance()->m_nRenderWndNum = lWndNum;
	if (m_spDVRPlayer->StartMonitor())
		return S_OK;

	return S_FALSE;
}
HRESULT CIMVDShowPlayer::StopMonitor()
{
	m_spDVRPlayer->StopMonitor();
	return S_OK;
}
HRESULT CIMVDShowPlayer::CapPic(BSTR* pbstrCapFolder)
{
	CString csPicPath = m_spDVRPlayer->Cappic();
	if (!csPicPath.IsEmpty())
	{
		*pbstrCapFolder = csPicPath.AllocSysString();
		return S_OK;
	}
	return S_FALSE;
}
HRESULT CIMVDShowPlayer::GetCapturePath(BSTR* bstrCapFolder)
{
	*bstrCapFolder = CDVRSettings::GetInstance()->m_csPicCapturePath.AllocSysString();
	return S_OK;
}
HRESULT CIMVDShowPlayer::SetCapturePath(BSTR bstrCapFolder)
{
	CDVRSettings::GetInstance()->m_csPicCapturePath = bstrCapFolder;
	return S_OK;
}
HRESULT CIMVDShowPlayer::ShowMetaData(VARIANT_BOOL bShow)
{
	CDVRSettings::GetInstance()->m_bDrawMetaData = (bShow == VARIANT_TRUE);
	return S_OK;
}
HRESULT CIMVDShowPlayer::SelectMonitorWnd()
{
	POINT pos;
	GetCursorPos(&pos);

	POINT ptClient;
	ptClient.x = pos.x;
	ptClient.y = pos.y;
	ScreenToClient(m_hVideoWnd, &ptClient);
	m_spDVRPlayer->SelectMonitorWnd(&ptClient);
	return S_OK;
}
HRESULT CIMVDShowPlayer::SetWndChannel(LONG lWndIndex, LONG lChannelIndex)
{
	m_spDVRPlayer->SetWndChannel(lWndIndex, lChannelIndex);
	return S_OK;
}

HRESULT CIMVDShowPlayer::AddFileToPlayList(BSTR bstrFile)
{
	m_spDVRPlayer->AddToPlayList(bstrFile);
	return S_OK;
}

HRESULT CIMVDShowPlayer::ClosePlayList()
{
	m_spDVRPlayer->ClosePlayList();
	return S_OK;
}

HRESULT CIMVDShowPlayer::PlayNextFile()
{
	m_spDVRPlayer->PlayNextFile();
	return S_OK;
}

HRESULT CIMVDShowPlayer::StartPlayback(LONGLONG lStartTime, LONGLONG lEndTime, LONG lChannel)
{
	CTime st(lStartTime);
	CTime et(lEndTime);
	SYSTEM_VIDEO_FILE svf;
	svf.channel = lChannel;

	svf.start_date.year = st.GetYear();
	svf.start_date.month = st.GetMonth();
	svf.start_date.date = st.GetDay();
	svf.start_time.hour = st.GetHour();
	svf.start_time.minute = st.GetMinute();
	svf.start_time.second = st.GetSecond();

	svf.end_date.year = et.GetYear();
	svf.end_date.month = et.GetMonth();
	svf.end_date.date = et.GetDay();
	svf.end_time.hour = et.GetHour();
	svf.end_time.minute = et.GetMinute();
	svf.end_time.second = et.GetSecond();
	return m_spDVRPlayer->StartPlayback(svf) ? S_OK : S_FALSE;
}

HRESULT CIMVDShowPlayer::StopPlayback()
{
	m_spDVRPlayer->StopPlayback();
	return S_OK;
}