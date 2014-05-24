#include <windows.h>
#include "StdAfx.h"
#include "FileStreamParser.h"

CFileStreamParser* CStreamParseFactory::OpenFile(LPCTSTR szFile)
{
	LPCTSTR szHttpHead = _T("http://");
	const int nHttpHeadLen = _tcslen(szHttpHead);
	CFileStreamParser* p = NULL;
	if (_tcslen(szFile) > nHttpHeadLen && _tcsnicmp(szFile, szHttpHead, nHttpHeadLen) == 0)
	{
		p = new CHttpStreamParser;
	}
	else
	{
		p = new CFileStreamParser;
	}
	if (p->OpenFile(szFile))
	{
		return p;
	}
	delete p;
	return NULL;
}
CFileStreamParser::CFileStreamParser(void)
	: m_hFileStream(INVALID_HANDLE_VALUE)
{
	memset(m_szFilename, 0, sizeof(m_szFilename));
	m_nFirstFrameTime = 0;
	m_nLastFrameTime = 0;
}


CFileStreamParser::~CFileStreamParser(void)
{
	CloseFile();
}

BOOL CFileStreamParser::OpenFile(LPCTSTR szFile)
{
	CloseFile();

	m_hFileStream = ::CreateFile(szFile,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (m_hFileStream != INVALID_HANDLE_VALUE)
	{
		_tcscpy(m_szFilename, szFile);

		std::auto_ptr<BYTE> streamData(new BYTE[ MAX_FRAME_LENGTH ]);
		memset(streamData.get(), 0, MAX_FRAME_LENGTH);
		FRAME_HEADER fh;
		INT nRet = GetOneFrame(streamData.get(), &fh);
		if (nRet <= 0)
		{
			nRet = GetOneFrame(streamData.get(), &fh);
			if ( nRet <= 0)
			{
				m_nFirstFrameTime = 0;
				m_nLastFrameTime = 0;
				return TRUE;
			}
		}
		m_nFirstFrameTime = htonl(fh.FrameTime);
		dwFileSize = GetFileSize();
		DWORD dwMovePos = dwFileSize-100;
		DWORD dwCurPos = 0;
		if (dwMovePos > 0 && INVALID_SET_FILE_POINTER != (dwCurPos = ::SetFilePointer(m_hFileStream, dwMovePos, 0, FILE_BEGIN)))
		{
			nRet = GetOneFrame(streamData.get(), &fh);
			while((nRet <= 0 && (dwMovePos - 100)) > 0 ||
				(nRet > 0 && (fh.FrameRate != 25 || fh.FrameType == 10)))
			{
				dwMovePos -= 100;
				if (INVALID_SET_FILE_POINTER == (dwCurPos = ::SetFilePointer(m_hFileStream, dwMovePos, 0, FILE_BEGIN)))
					break;
				nRet = GetOneFrame(streamData.get(), &fh);
			}
			if (nRet > 0)
			{
				m_nLastFrameTime = htonl(fh.FrameTime);
			}
		}
		::SetFilePointer(m_hFileStream, 0, 0, FILE_BEGIN);
		return TRUE;
	}
	return FALSE;
}

BOOL CFileStreamParser::SetFilePointer(float pointer)
{
	if (m_hFileStream != INVALID_HANDLE_VALUE)
	{
		DWORD dwPointerFileSize = dwFileSize * pointer;

		DWORD dwCurPos = ::SetFilePointer(m_hFileStream, dwPointerFileSize, 0, FILE_BEGIN);

		if (INVALID_SET_FILE_POINTER != dwCurPos)
		{
			return TRUE;			
		}
	}	
	return FALSE;
}

void CFileStreamParser::CloseFile()
{
	if (m_hFileStream != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(m_hFileStream);
		m_hFileStream = INVALID_HANDLE_VALUE;
		memset(m_szFilename, 0, sizeof(m_szFilename));
	}

	m_nFirstFrameTime = 0;
	m_nLastFrameTime = 0;
}

DWORD CFileStreamParser::GetFileSize()
{
	if (m_hFileStream != INVALID_HANDLE_VALUE)
	{
		return ::GetFileSize(m_hFileStream, NULL);
	}
	return 0;
}

int	CFileStreamParser::ReadCacheData( BYTE* buf, int size )
{
	DWORD dwRead = 0;
	if (m_hFileStream != INVALID_HANDLE_VALUE && ::ReadFile(m_hFileStream, buf, size, &dwRead, NULL) && dwRead > 0)
	{
		return dwRead;
	}
	return 0;
}

CHttpStreamParser::CHttpStreamParser()
	: m_hInternetOpen(NULL)
	, m_hInternetOpenUrl(NULL)
{
}
CHttpStreamParser::~CHttpStreamParser()
{
	CloseFile();
}

BOOL CHttpStreamParser::OpenFile(LPCTSTR szURL)
{
	int tryCount = 0;
	const int maxTryCount = 5;
	while(tryCount++ < maxTryCount && (m_hInternetOpen == NULL || m_hInternetOpenUrl == NULL))
	{
		if (tryCount != 1)
		{
			Sleep(100);
		}
		CloseFile();
		m_hInternetOpen = InternetOpen(NULL, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
		if (m_hInternetOpen != NULL)
		{
			m_hInternetOpenUrl = InternetOpenUrl(m_hInternetOpen, szURL, NULL, 0, INTERNET_FLAG_TRANSFER_BINARY | INTERNET_FLAG_PRAGMA_NOCACHE, 0);
		}
	}

	return m_hInternetOpen != NULL && m_hInternetOpenUrl != NULL;
}
void CHttpStreamParser::CloseFile()
{
	if (m_hInternetOpenUrl != NULL)
	{
		::InternetCloseHandle(m_hInternetOpenUrl);
		m_hInternetOpenUrl = NULL;
	}
	if (m_hInternetOpen != NULL)
	{
		::InternetCloseHandle(m_hInternetOpen);
		m_hInternetOpen = NULL;
	}
}

int CHttpStreamParser::ReadCacheData(BYTE* buf, int size)
{
	if (m_hInternetOpenUrl != NULL)
	{
		DWORD dwRead;
		InternetReadFile(m_hInternetOpenUrl, buf, size, &dwRead);
		return dwRead;
	}
	return 0;
}

DWORD CHttpStreamParser::GetFileSize()
{
	if (m_hInternetOpen && m_hInternetOpenUrl)
	{
		DWORD dwContentLength = 0;
		DWORD dwLengthSize = sizeof(dwContentLength);
		if (HttpQueryInfo(m_hInternetOpenUrl, HTTP_QUERY_CONTENT_LENGTH|HTTP_QUERY_FLAG_NUMBER, &dwContentLength, &dwLengthSize, NULL))
		{
			return dwContentLength;
		}
	}
	return 0;
}