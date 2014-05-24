#pragma once
#include "StreamParser.h"
#include <WinInet.h>


class CFileStreamParser : public CStreamParser
{
public:
	CFileStreamParser(void);
	virtual ~CFileStreamParser(void);

	virtual BOOL OpenFile(LPCTSTR szFile);
	virtual void CloseFile();

	virtual BOOL SetFilePointer(float pointer);//设置文件指针(拖拽进度条)

	virtual DWORD GetFileSize();

	U32 GetLastFrameTime()
	{
		return m_nLastFrameTime;
	}
	U32 GetFirstFrameTime()
	{
		return m_nFirstFrameTime;
	}
public:
	virtual int	ReadCacheData( BYTE* buf, int size );

protected:
	HANDLE	m_hFileStream;
	TCHAR	m_szFilename[MAX_PATH];

	U32		m_nFirstFrameTime;
	U32		m_nLastFrameTime;

	DWORD	dwFileSize;
};

class CHttpStreamParser : public CFileStreamParser
{
public:
	CHttpStreamParser();
	virtual ~CHttpStreamParser();

	virtual BOOL OpenFile(LPCTSTR szURL);
	BOOL IsOpen(){return m_hInternetOpen != NULL && m_hInternetOpenUrl != NULL;}
	virtual void CloseFile();

	virtual DWORD GetFileSize();
public:
	virtual int ReadCacheData(BYTE* buf, int size);

protected:
	HINTERNET	m_hInternetOpen;
	HINTERNET	m_hInternetOpenUrl;
};

class CStreamParseFactory
{
public:
	static CFileStreamParser* OpenFile(LPCTSTR szFile);
};