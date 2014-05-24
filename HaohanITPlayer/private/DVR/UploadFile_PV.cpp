#include "stdafx.h"
#include "UploadFile.h"
#include "DvrSDKErr.h"
#include "CommClass.h"
#include "CommDef.h"
#include "NetDef.h"
#include "assert.h"

int CUploadFile::UpdateImageBeginCmd(SOCKET sk, int loginID, int fileType, int channel, int imagLen)
{
	REMOTE_UPDATE_BEGIN_REQUEST		msg;
	REMOTE_UPDATE_BEGIN_RESPONSE	rsp;
	msg.header.command				= REMOTE_SYSTEM_UPDATE_REQUEST;
	msg.loginID						= loginID;
	msg.uploadType					= fileType;
	msg.channel						= channel;
	msg.Length						= imagLen;
	msg.h2n();
	
	int ret = SendAndRecv(sk, (char*)&msg, sizeof(msg), (char*)&rsp, sizeof(rsp) );
	if( ret < 0 )
		return ret;	
	rsp.n2h();
	
	if( rsp.header.command != SYSTEM_REQUEST_ACCEPT )
	{		
		return HHV_ERROR_UPDATEBEGIN;
	}
	return 0;
}

int CUploadFile::UpdateImageDataCmd(SOCKET sk,int loginID, unsigned char* buf, int len)
{
	REMOTE_UPDATE_IMAGEDATA_REQUEST			msg;
	REMOTE_UPDATE_IMAGEDATA_RESPONSE		rsp;
	msg.header.command						= REMOTE_SYSTEM_UPDATE_DATA;
	msg.loginID								= loginID;
	msg.length								= len;
	assert( len > 0 );
	memcpy( msg.data, buf, len );
	msg.h2n();
	
	int ret = SendAndRecv(sk, (char*)&msg, sizeof(msg), (char*)&rsp, sizeof(rsp) );
	if( ret < 0 )
	{
		return ret;	
	}
	rsp.n2h();
	
	if( rsp.header.command != SYSTEM_REQUEST_ACCEPT )
		return HHV_ERROR_UPDATEDATA;
	return 0;
}

int CUploadFile::UpdateImageEndCmd( SOCKET sk,int loginID, int checkSum )
{
	REMOTE_UPDATE_END_REQUEST			msg;
	REMOTE_UPDATE_END_RESPONSE			rsp;
	
	msg.header.command					= REMOTE_SYSTEM_UPDATE_OVER;
	msg.loginID							= loginID;
	msg.CheckSum						= checkSum;
	msg.h2n();
	
	int ret = SendAndRecv(sk, (char*)&msg, sizeof(msg), (char*)&rsp, sizeof(rsp) );
	if( ret < 0 )
		return ret;	
	rsp.n2h();
	
	if( rsp.header.command != SYSTEM_REQUEST_ACCEPT )
		return HHV_ERROR_UPDATEND;
	
	return 0;
}

UINT __stdcall CUploadFile::TD_UpgradeRoute( LPVOID pV )
{
	CUploadFile* dvrCfg = (CUploadFile*)pV;
	if( dvrCfg == NULL )
		return 0;
	dvrCfg->UpgradeRoute( );
	return 0;
}
#define UPDATE_ERROR_RETURN(error) {fclose(fp);return error;}
int CUploadFile::UpgradeRoute( )
{
	unsigned char buf[DOWNLOAD_FILE_ONCE + 1];
	int	 ret = 0;
	unsigned __int64  totalrecv = 0;
	unsigned __int64 totalrecv100 = 0;
	unsigned int i = 0, len = 0;
	unsigned int count = 0;	
	unsigned int checksum = 0;
	
	SOCKET	s = m_skSocket;
	FILE *fp = _tfopen(m_fileName, _T("r+b"));
	
	DWORD dwLeft = m_fileLen;
	
	while(!feof(fp) && !m_bCancel) 
	{
		ret = fread(buf, sizeof(char), UPDATE_BUFLEN_ONCE, fp);
		if(ret < UPDATE_BUFLEN_ONCE && ferror(fp))
		{
			UPDATE_ERROR_RETURN(HHV_ERROR_READFILE);
		}
		count += ret;

		ret = UpdateImageDataCmd( m_skSocket, m_nUserID, buf, ret );
		if( ret < 0 )
		{
			return ret;	
		}
		m_upgradePos = (count*100/m_fileLen);
		m_state = NET_HHV_UPGRADEING;	//正在升级
	}
	fclose(fp);	

	if( m_bCancel )
	{// 取消更新操作
		closesocket( s );
		return -1;

	}
	ret = UpdateImageEndCmd(m_skSocket, m_nUserID, checksum );
	if( ret < 0 )
	{
		if(ret == HHV_ERROR_UPDATEND)
		{
			m_state = NET_HHV_ERROR_UPDATEDATA;//升级失败
		}
		else
		{
			m_state = NET_HHV_UPGRADENETERROR;	//网络断开，状态未知
		}
		return ret;
	}
	m_state = NET_HHV_UPGRADEING;	//正在升级

	return 0;
}