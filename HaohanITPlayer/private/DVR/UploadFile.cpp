// UploadFile.cpp: implementation of the CUploadFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UploadFile.h"
#include "Process.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUploadFile::CUploadFile()
{
	m_nIndex = -1;
	m_nUserID = -1;
	m_skSocket = INVALID_SOCKET;
	memset( m_fileName, 0x00, sizeof(m_fileName) );
	m_fileLen = 0;
	m_bCancel = FALSE;
	m_upgradePos = 0;
	m_state = 0;
	m_channel = -1;
}

CUploadFile::~CUploadFile()
{

}

int CUploadFile::UploadCfgFile( int index, int userID, LPCTSTR dvrIP, 
							  int dvrPort,int cfgType, int channel, LPCTSTR imageName )
{
	m_nIndex = index;
	m_nUserID = userID;
	m_cfgType = cfgType;
	_tcscpy( m_fileName, imageName );
	m_channel = channel;

	FILE *fin = _tfopen(imageName, _T("r+b"));
	if( fin == NULL )
	{
		return HHV_ERROR_OPENFILE;
	}
	fclose( fin );
	fin = NULL;
	
	SOCKET s = CreateSocket( 0, 30 );
	if( s == INVALID_SOCKET )
		return HHV_ERROR_CREATESOCKET;
	
	int ret = Connect( s, CT2A(dvrIP),dvrPort ,30 );
	if( ret == SOCKET_ERROR )
	{
		closesocket( s );
		return HHV_ERROR_CONNECT;
	}
	
	U32 fileLen = CCommClass::PowerGetFileSize(imageName);
	ret = UpdateImageBeginCmd( s, userID, cfgType, m_channel, fileLen );
	if( ret < 0 )
	{
		closesocket( s );
		return ret;
	}
	m_skSocket = s;
	m_fileLen = fileLen;
	
	UINT dwThreadID = 0;
	HANDLE	hThread = (HANDLE)_beginthreadex( NULL, 0,  TD_UpgradeRoute, this, 0, &dwThreadID );
	CloseHandle(hThread);
	return index;
}
