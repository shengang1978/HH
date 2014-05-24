// UserLogin.h: interface for the CLoginDvrMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_USERLOGIN_H__B480E331_A56A_4B89_8C20_128E6FA6BDB8__INCLUDED_)
#define AFX_USERLOGIN_H__B480E331_A56A_4B89_8C20_128E6FA6BDB8__INCLUDED_

#pragma warning(disable:4786)

#include "LoginDvr.h"

#define STATUS_LOGINED		1
#define STATUS_UNLOGINED	0

#define	MAX_LOGIN_DVR		128

class CLoginDvr;
#define		STOP_STATUS	0
#define		RUN_STATUS	1
class CLoginDvrMgr
{
public:
	CLoginDvrMgr();
	virtual ~CLoginDvrMgr();
	
	int		MakeKeyFrame(LONG lUserID, LONG lChannel);	

	void	Startup();
	void	Clearup();

	int		Login(LPCTSTR userName,  LPCTSTR pwd, LPCTSTR addr, int port);
	int		Logout( int index );

	int	 SetVideoEffect(int userID, int channel, int bright,int contrast, int saturation, int hueValue);
	int	 GetVideoEffect(int userID, int channel, int &bright, int &contrast, 
		int &saturation, int &hueValue);

	int ReStartDVR( int loginID, int restartType, int channel = 0 );

	int StartDVRRecord( int userID, int channel );
	int StopDVRRecord( int userID, int channel );
	
	INT UploadCfgFile(int userID, int cfgType, int channel, LPCTSTR imageName );
	
private:
	int		CheckUserID(int handle);
	int		GetFreeIndex();	
	CLoginDvr* GetDvrLoginInfo(LPCTSTR addr, int port);
	
public:
	std::vector<CLoginDvr*>		m_loginInfo;

private:
	bool			m_bExit;
	HANDLE			m_hEventThreadExit;
	HANDLE			m_hCreateWindowEvent;

	HANDLE			m_hHeattbeat;
	HANDLE			m_hCheckOnline;
	
//	static			UINT WINAPI HeartBeatThread( void * dat);
	static			UINT WINAPI CheckOnlineThread( void * dat);
//
//	int				HeartBeatLoop();
//	int				OnlineHeartBeat( int index );
	int				CheckOnlineStatusLoop( );



	int				m_status;	
	CCritSec		m_csLock;
	
private:
	void			NotifyLogoutMessage(int index, char* dvrIP);
};

extern CLoginDvrMgr	g_LoginDvrMgr;

/***********************************************************************************************/
#endif // !defined(AFX_USERLOGIN_H__B480E331_A56A_4B89_8C20_128E6FA6BDB8__INCLUDED_)
