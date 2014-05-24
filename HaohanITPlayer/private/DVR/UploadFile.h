// UploadFile.h: interface for the CUploadFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UPLOADFILE_H__9F5085E0_04B7_4D37_B683_87015C3C2CF3__INCLUDED_)
#define AFX_UPLOADFILE_H__9F5085E0_04B7_4D37_B683_87015C3C2CF3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "winsock2.h"
#include "DvrSDKErr.h"
#include "CommClass.h"
#include "CommDef.h"


#define DOWNLOAD_FILE_ONCE	(4 * 1024)
#define UPDATE_BUFLEN_ONCE	1024

#define NET_HHV_UPGRADESUCESS			1000	//升级成功
#define NET_HHV_UPGRADEING			1010	//正在升级

#define NET_HHV_ERROR_UPDATEBEGIN		-3100	//开始更新失败
#define NET_HHV_ERROR_UPDATEDATA		-3200	//更新数据失败
#define NET_HHV_ERROR_UPDATEND		-3300	//结束更新失败

#define NET_HHV_UPGRADENETERROR		-8070	//网络断开，状态未知
#define NET_HHV_UPGRADEFAIL			-8080	//更新数据失败

class CUploadFile  
{
public:
	CUploadFile();
	virtual ~CUploadFile();

	int UploadCfgFile( int index, int userID, LPCTSTR dvrIP, 
		int dvrPort,int fileType, int channel, LPCTSTR imageName );

private:
	int UpdateImageBeginCmd(SOCKET sk, int loginID, int fileType, int channel, int imagLen);
	int UpdateImageDataCmd(SOCKET sk,int loginID, unsigned char* buf, int len);
	int UpdateImageEndCmd( SOCKET sk,int loginID, int checkSum );

	static UINT __stdcall TD_UpgradeRoute( LPVOID pV );
	int UpgradeRoute( );

private:
	int	m_nIndex;
	int m_nUserID;
	SOCKET m_skSocket;
	int	 m_channel;
	TCHAR m_fileName[MAX_PATH];
	UINT m_fileLen;
	BOOL m_bCancel;
	int m_upgradePos;
	int m_state;
	int	m_cfgType;
};

#endif // !defined(AFX_UPLOADFILE_H__9F5085E0_04B7_4D37_B683_87015C3C2CF3__INCLUDED_)
