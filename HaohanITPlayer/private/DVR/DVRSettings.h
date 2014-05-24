#pragma once

#include <map>
#include "Sync_Locks.h"
//连接DVR信息
struct HHV_CONN_INFO
{
	HHV_CONN_INFO(){ memset( this, 0x00, sizeof(HHV_CONN_INFO) );	}
	CHAR	ip[48];
	INT		port;
	CHAR	userName[32];	
	CHAR	pwd[32];
};

struct HHV_CLIENT_INFO
{
	HHV_CLIENT_INFO(){ memset( this, 0x00, sizeof(HHV_CLIENT_INFO) );	}
	HHV_CONN_INFO connInfo;
	int channel;//通道号，从0开始	
	int childstream;	//0-主码流；1-次码流
};

class CDVRSettings
{
public:
	static CDVRSettings *GetInstance();  
//连接DVR信息

public:
	bool Save(LPCTSTR szXmlPath = NULL);
	bool Load(LPCTSTR szXmlPath = NULL);

	enum eCapPicType{eJPEG = 0, eBMP};

public:
	//Media Server Settings
	CString		m_csMediaServerIP;			//Media Server IP: e.g. 192.168.123.59
	int			m_lPort;			//Media Server Port: e.g. 2000
	CString		m_csUsername;	//Login Server: Username
	CString		m_csPassword;	// Password

	CTime		m_StartTime;		//Render start time
	CTime		m_EndTime;			//Render end time
	
	//Render Window Settings
	LONG			m_nRenderWidth;
	LONG			m_nRenderHeight;
	int			m_nRenderWndNum;	//Default: 1. 1 , 4, 8, 16

	//Player Settings.
	BOOL		m_bHighPictureQuality;	//Quality of the picture: default is FALSE;
	CString		m_csPicCapturePath;	//Capture Picture folder
	eCapPicType	m_eCapturePicType;			//Capture picture type: default is JPEG

	// Show/Hide MetaData
	bool    	m_bDrawMetaData; //Show/Hide MetaData: default is TRUE;
	
protected:
	CDVRSettings(void);
	~CDVRSettings(void);

	static CCritSec	g_InstanceLock;
	static std::auto_ptr<CDVRSettings> m_Instance;
	friend class std::auto_ptr<CDVRSettings>;
};

	
