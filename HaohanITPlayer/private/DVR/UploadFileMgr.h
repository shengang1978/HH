// UploadFileMgr.h: interface for the CUploadFileMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UPLOADFILEMGR_H__D22E8AC3_154A_4DE8_8DCB_41AE907E778A__INCLUDED_)
#define AFX_UPLOADFILEMGR_H__D22E8AC3_154A_4DE8_8DCB_41AE907E778A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "UploadFile.h"

#include "Sync_Locks.h"
#include "Utilities.h"

#define MAX_UPLOADFILE_NUM	36

class CUploadFileMgr  
{
public:
	CUploadFileMgr();
	virtual ~CUploadFileMgr();

	INT UploadCfgFile(int userID, LPCTSTR dvrIP, 
		int dvrPort, int fileType, int channel, LPCTSTR imageName );

private:
	int	GetIdleIndex();
private:
	std::vector<CUploadFile*> m_UploadFile;
	
	CMutex m_csLock;
	
	int m_status;
};

extern CUploadFileMgr g_UploadFileMgr;

#endif // !defined(AFX_UPLOADFILEMGR_H__D22E8AC3_154A_4DE8_8DCB_41AE907E778A__INCLUDED_)
