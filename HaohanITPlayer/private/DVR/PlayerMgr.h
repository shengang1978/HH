// PlayerMgr.h: interface for the CPlayerMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLAYERMGR_H__448B2740_E614_4457_9F1D_0B156F80B93C__INCLUDED_)
#define AFX_PLAYERMGR_H__448B2740_E614_4457_9F1D_0B156F80B93C__INCLUDED_

#include "Player.h"
#include "Sync_Locks.h"
#include "Utilities.h"

#define MAX_PLAYER	36

#define STOP_STATUS	0
#define RUN_STATUS	1


class CPlayerMgr  
{
public:
	CPlayerMgr();
	virtual ~CPlayerMgr();

	VOID Init(HWND hNotifyWnd);
	VOID Clearup();
	
	INT StartMonitor(HWND hWnd, HHV_CLIENT_INFO* clientInfo);
	INT StopMonitor( int handle );

	INT OpenVSFile(HWND hWnd, LPCTSTR szFile);
	INT CloseVSFile(int handle);

	INT Pause(int handle);
	INT Stop(int handle);
	INT Play(int handle, BOOL bPause);

	INT Fast(int handle);
	INT Slow(int handle);

	BOOL SetVSFilePointer(int handle, float pointer);

	LONG GetMaxVSFileTime(int handle);

	LONG GetMaxVSFileCurrentTime(int handle);

	int  StartPlayBackByTime(HWND hWnd, SYSTEM_VIDEO_FILE* recdFile,
								char* ssIP, int ssPort);
	int StopPlayBackByTime(int realHandle);

	void ResetRenderWindow(LONG lWidth, LONG lHeight);
	void ResizeMonitorWindow(int handle);
private:
	int	GetIdlePlayer();
private:
	CPlayer* m_player[MAX_PLAYER];

	CMutex m_csLock;

	int m_status;
};
//extern CPlayerMgr	g_playerMgr;

#endif // !defined(AFX_PLAYERMGR_H__448B2740_E614_4457_9F1D_0B156F80B93C__INCLUDED_)
