#include "stdafx.h"
#include "PlayMp4H_fFunDef.h"
#include "Utilities.h"
#include "DvrSDKErr.h"

fPlayM4_Play                 HK_PlayM4_Play = NULL;
fPlayM4_OpenStream           HK_PlayM4_OpenStream = NULL;
fPlayM4_GetLastError         HK_PlayM4_GetLastError = NULL;
fPlayM4_CloseStream          HK_PlayM4_CloseStream = NULL;
fPlayM4_Stop                 HK_PlayM4_Stop = NULL;
fPlayM4_InputData            HK_PlayM4_InputData = NULL;
fPlayM4_OpenFile             HK_PlayM4_OpenFile = NULL;
fPlayM4_CloseFile            HK_PlayM4_CloseFile = NULL;
fPlayM4_Pause                HK_PlayM4_Pause = NULL;
fPlayM4_OneByOne             HK_PlayM4_OneByOne = NULL;
fPlayM4_SetPlayPos           HK_PlayM4_SetPlayPos = NULL;
fPlayM4_PlaySound            HK_PlayM4_PlaySound = NULL;
fPlayM4_StopSound            HK_PlayM4_StopSound = NULL;
fPlayM4_Fast                 HK_PlayM4_Fast = NULL;
fPlayM4_Slow                 HK_PlayM4_Slow = NULL;
fPlayM4_GetPlayedTime        HK_PlayM4_GetPlayedTime = NULL;
fPlayM4_GetPlayPos           HK_PlayM4_GetPlayPos = NULL;
fPlayM4_RigisterDrawFun      HK_PlayM4_RigisterDrawFun = NULL;
fPlayM4_GetBMP				 HK_PlayM4_GetBMP = NULL;
fPlayM4_SetStreamOpenMode	 HK_PlayM4_SetStreamOpenMode = NULL;

fPlayM4_SetDecCBStream		 HK_PlayM4_SetDecCBStream = NULL;
fPlayM4_SetDecCallBackMend	 HK_PlayM4_SetDecCallBackMend = NULL;

fPlayM4_SetDisplayCallBack	 HK_PlayM4_SetDisplayCallBack = NULL;
fPLayM4_ConvertToBmpFile	 HK_PLayM4_ConvertToBmpFile = NULL;

fPlayM4_GetFileHeadLength	 HK_PlayM4_GetFileHeadLength = NULL;
fPlayM4_GetSourceBufferRemain	HK_PlayM4_GetSourceBufferRemain = NULL;

HMODULE ghMP4SDK = NULL;
int  LoadPlayMp4Dll()
{
	TCHAR path[MAX_PATH] = {0x00};
	CUtilities::ExtractAppPath( path );

	_tcscat( path, _T("/PlayCtrl/"));
	
	_tcscat( path, _T("PlayCtrl.dll"));
	
	ghMP4SDK = LoadLibraryEx(path, NULL, LOAD_WITH_ALTERED_SEARCH_PATH );
	
	if( NULL == ghMP4SDK )
	{
		DWORD dwError = GetLastError();	
		
		char buf[128] = {0x00};
		sprintf( buf , "加载动态库HikPlayM4.dll失败 error:%d  Path:%s\r\n" ,dwError, path ) ;
		MessageBoxA( NULL , buf , "Error" , MB_OK ) ;		
		return HHV_ERROR_LOADDLL_SDK;
	}
	
	HK_PlayM4_Play = (fPlayM4_Play)GetProcAddress(ghMP4SDK, "PlayM4_Play");
	HK_PlayM4_OpenStream = (fPlayM4_OpenStream)GetProcAddress(ghMP4SDK, "PlayM4_OpenStream");
    HK_PlayM4_GetLastError = (fPlayM4_GetLastError)GetProcAddress(ghMP4SDK, "PlayM4_GetLastError");
    HK_PlayM4_CloseStream = (fPlayM4_CloseStream)GetProcAddress(ghMP4SDK, "PlayM4_CloseStream");
    HK_PlayM4_Stop = (fPlayM4_Stop)GetProcAddress(ghMP4SDK, "PlayM4_Stop");
	HK_PlayM4_InputData = (fPlayM4_InputData)GetProcAddress(ghMP4SDK, "PlayM4_InputData");
	HK_PlayM4_OpenFile = (fPlayM4_OpenFile)GetProcAddress(ghMP4SDK, "PlayM4_OpenFile");
	HK_PlayM4_CloseFile = (fPlayM4_CloseFile)GetProcAddress(ghMP4SDK, "PlayM4_CloseFile");
	HK_PlayM4_Pause = (fPlayM4_Pause)GetProcAddress(ghMP4SDK, "PlayM4_Pause");
    HK_PlayM4_OneByOne = (fPlayM4_OneByOne)GetProcAddress(ghMP4SDK, "PlayM4_OneByOne");
	HK_PlayM4_SetPlayPos = (fPlayM4_SetPlayPos)GetProcAddress(ghMP4SDK, "PlayM4_SetPlayPos");
    HK_PlayM4_PlaySound = (fPlayM4_PlaySound)GetProcAddress(ghMP4SDK, "PlayM4_PlaySound");
    HK_PlayM4_StopSound = (fPlayM4_StopSound)GetProcAddress(ghMP4SDK, "PlayM4_StopSound");
	HK_PlayM4_Fast = (fPlayM4_Fast)GetProcAddress(ghMP4SDK, "PlayM4_Fast");
	HK_PlayM4_Slow = (fPlayM4_Slow)GetProcAddress(ghMP4SDK, "PlayM4_Slow");
	HK_PlayM4_GetPlayedTime = (fPlayM4_GetPlayedTime)GetProcAddress(ghMP4SDK, "PlayM4_GetPlayedTime");
    HK_PlayM4_GetPlayPos = (fPlayM4_GetPlayPos)GetProcAddress(ghMP4SDK, "PlayM4_GetPlayPos");
    HK_PlayM4_RigisterDrawFun = (fPlayM4_RigisterDrawFun)GetProcAddress(ghMP4SDK, "PlayM4_RigisterDrawFun");
	HK_PlayM4_GetBMP = (fPlayM4_GetBMP)GetProcAddress(ghMP4SDK, "PlayM4_GetBMP");

	HK_PlayM4_SetStreamOpenMode = (fPlayM4_SetStreamOpenMode)GetProcAddress(ghMP4SDK, "PlayM4_SetStreamOpenMode");

	HK_PlayM4_SetDecCBStream = (fPlayM4_SetDecCBStream)GetProcAddress(ghMP4SDK, "PlayM4_SetDecCBStream");
	HK_PlayM4_SetDecCallBackMend = (fPlayM4_SetDecCallBackMend)GetProcAddress(ghMP4SDK, "PlayM4_SetDecCallBackMend");

	HK_PlayM4_SetDisplayCallBack = (fPlayM4_SetDisplayCallBack)GetProcAddress(ghMP4SDK, "PlayM4_SetDisplayCallBack");
	HK_PLayM4_ConvertToBmpFile = (fPLayM4_ConvertToBmpFile)GetProcAddress(ghMP4SDK, "PLayM4_ConvertToBmpFile");

	HK_PlayM4_GetFileHeadLength = (fPlayM4_GetFileHeadLength)GetProcAddress(ghMP4SDK, "PlayM4_GetFileHeadLength");	

	HK_PlayM4_GetSourceBufferRemain = (fPlayM4_GetSourceBufferRemain)GetProcAddress(ghMP4SDK, "PlayM4_GetSourceBufferRemain");	

	if( NULL == HK_PlayM4_Play || 
		NULL == HK_PlayM4_OpenStream || 
		NULL == HK_PlayM4_GetLastError ||
		NULL == HK_PlayM4_CloseStream ||
		NULL == HK_PlayM4_Stop ||
		NULL == HK_PlayM4_InputData ||
		NULL == HK_PlayM4_OpenFile || 
		NULL == HK_PlayM4_CloseFile ||
		NULL == HK_PlayM4_Pause || 
		NULL == HK_PlayM4_OneByOne ||
		NULL == HK_PlayM4_SetPlayPos ||
		NULL == HK_PlayM4_PlaySound ||
		NULL == HK_PlayM4_StopSound ||
		NULL == HK_PlayM4_Fast ||
		NULL == HK_PlayM4_Slow ||
		NULL == HK_PlayM4_GetPlayedTime || 
		NULL == HK_PlayM4_GetPlayPos ||
		NULL == HK_PlayM4_RigisterDrawFun ||
		NULL == HK_PlayM4_GetBMP ||
		NULL == HK_PlayM4_SetDisplayCallBack ||
		NULL == HK_PLayM4_ConvertToBmpFile ||
		NULL == HK_PlayM4_GetSourceBufferRemain)
	{
		FreeLibrary(ghMP4SDK);
		ghMP4SDK = NULL;
		MessageBox( NULL, _T("获取PlayMP4接口函数地址出错"), _T("Error"), MB_OK );
		return HHV_ERROR_LOADDLL_FUNCTION;
	}
	
	return 0;
	
	
}


int FreePlayMp4Dll()
{
	if ( ghMP4SDK != NULL )
	{
		FreeLibrary(ghMP4SDK);
		ghMP4SDK = NULL;
	}
	
	return 0;
}
