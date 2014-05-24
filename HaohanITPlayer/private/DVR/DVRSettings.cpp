#include "StdAfx.h"
#include "DVRSettings.h"
#import "msxml6.dll"
#include "./CommClass.h"
#include "atlcomtime.h"
//
//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif

using namespace std;

#define ERROR_RETURN(x) \
	{\
	HRESULT __hhrr = (x);\
	if (FAILED(__hhrr))\
	{\
		CString csMsg;\
		csMsg.Format(_T("ÔËÐÐ%d = %s³ö´í"), __hhrr, CA2T(#x));\
		::MessageBox(NULL, csMsg, _T("´íÎó"), MB_OK);\
		return false;\
	}\
}

CCritSec CDVRSettings::g_InstanceLock;
std::auto_ptr<CDVRSettings> CDVRSettings::m_Instance;

CDVRSettings::CDVRSettings(void)
	: m_lPort(-1)
	, m_bHighPictureQuality(FALSE)
	, m_eCapturePicType(eJPEG)
	, m_nRenderWndNum(1)
	, m_nRenderWidth(352)
	, m_nRenderHeight(288)
	, m_bDrawMetaData(true)
{
	::CoInitialize(NULL);
	Load();
}

CDVRSettings *CDVRSettings::GetInstance()  
{  
	if (m_Instance.get() == NULL)
	{
		// For multi-thread safe
		g_InstanceLock.Lock();
		if (m_Instance.get() == NULL)
		{
			m_Instance.reset(new CDVRSettings());
		}
		g_InstanceLock.Unlock();
	}
    return m_Instance.get();  
} 

CDVRSettings::~CDVRSettings(void)
{
	//Save();
	::CoUninitialize();
}

//auto_ptr<CDVRSettings> CDVRSettings::m_Instance;

bool CDVRSettings::Save(LPCTSTR szXmlPath)
{
	CComVariant bstrPath;
	if (szXmlPath != NULL)
	{
		bstrPath = szXmlPath;
	}
	else
	{
		TCHAR path[_MAX_PATH] = {0X00};
		CCommClass::ExtractFilePath( path );
		_tcscat(path, _T("\\DVRSettings.xml"));
		bstrPath = path;
	}

	try
	{
		MSXML2::IXMLDOMDocumentPtr spDoc;
		MSXML2::IXMLDOMNodePtr spRoot;

		HRESULT hr = spDoc.CreateInstance(__uuidof(MSXML2::DOMDocument60));
		//if (FAILED(hr))
		//{
		//	hr = spDoc.CreateInstance(__uuidof(MSXML2::DOMDocument60));
		//}
		CComVariant	nodeType = NODE_ELEMENT;

		spRoot = spDoc->createNode(nodeType, _T("DVRSettings"), _T(""));
		spDoc->appendChild(spRoot);

		// ´æ´¢MediaServerIP
		MSXML2::IXMLDOMNodePtr spElm = spDoc->createElement(_T("MediaServerIP"));
		spElm->Puttext((LPCTSTR)m_csMediaServerIP);
		spRoot->appendChild(spElm);

		// ´æ´¢MediaServerPort
		spElm = spDoc->createElement(_T("MediaServerPort"));
		CString csPort;
		csPort.Format(_T("%d"), m_lPort);
		spElm->Puttext((LPCTSTR)csPort);
		spRoot->appendChild(spElm);

		// ´æ´¢MediaLoginUsername
		spElm = spDoc->createElement(_T("MediaLoginUsername"));
		spElm->Puttext((LPCTSTR)m_csUsername);
		spRoot->appendChild(spElm);

		// ´æ´¢MediaLoginPassword
		spElm = spDoc->createElement(_T("MediaLoginPassword"));
		spElm->Puttext((LPCTSTR)m_csPassword);
		spRoot->appendChild(spElm);

		// ´æ´¢MediaRenderStartTime
		spElm = spDoc->createElement(_T("MediaRenderStartTime"));
		CString csStartTime;
		m_StartTime = CTime::GetCurrentTime();
		csStartTime = m_StartTime.Format(_T("%Y-%m-%d %H:%M:%S"));
		spElm->Puttext((LPCTSTR)csStartTime);
		spRoot->appendChild(spElm);

		// ´æ´¢MediaRenderEndTime
		spElm = spDoc->createElement(_T("MediaRenderEndTime"));
		CString csEndTime;
		m_EndTime = CTime::GetCurrentTime();
		csEndTime = m_EndTime.Format(_T("%Y-%m-%d %H:%M:%S"));
		spElm->Puttext((LPCTSTR)csEndTime);
		spRoot->appendChild(spElm);

		// ´æ´¢RenderWidth
		spElm = spDoc->createElement(_T("RenderWidth"));
		CString csWidth;
		csWidth.Format(_T("%d"), m_nRenderWidth);
		spElm->Puttext((LPCTSTR)csWidth);
		spRoot->appendChild(spElm);

		// ´æ´¢RenderHeight
		spElm = spDoc->createElement(_T("RenderHeight"));
		CString csHeight;
		csHeight.Format(_T("%d"), m_nRenderHeight);
		spElm->Puttext((LPCTSTR)csHeight);
		spRoot->appendChild(spElm);

		// ´æ´¢RenderWndNum
		spElm = spDoc->createElement(_T("RenderWndNum"));
		CString csWndNum;
		csWndNum.Format(_T("%d"), m_nRenderWndNum);
		spElm->Puttext((LPCTSTR)csWndNum);
		spRoot->appendChild(spElm);

		// ²»ÓÃ´æ´¢ReaderEnableChannel£¬ÒÑ¾­É¾³ý¸Ãmap
		
		// ´æ´¢PlayerHighPictureQuality
		spElm = spDoc->createElement(_T("PlayerHighPictureQuality"));
		CString csHPicQuality;
		csHPicQuality.Format(_T("%d"), m_bHighPictureQuality);
		spElm->Puttext((LPCTSTR)csHPicQuality);
		spRoot->appendChild(spElm);

		// ´æ´¢PlayerCapturePicType
		spElm = spDoc->createElement(_T("PlayerCapturePicType"));
		CString csCapturePicType;
		csCapturePicType.Format(_T("%d"), m_eCapturePicType);
		spElm->Puttext((LPCTSTR)csCapturePicType);
		spRoot->appendChild(spElm);

		// ´æ´¢PlayersPicCapturePath
		spElm = spDoc->createElement(_T("PlayersPicCapturePath"));
		spElm->Puttext((LPCTSTR)m_csPicCapturePath);
		spRoot->appendChild(spElm);

		ERROR_RETURN(spDoc->save(bstrPath));
	}
	catch (...)
	{
		return false;
	}
	return true;
}

bool CDVRSettings::Load(LPCTSTR szXmlPath)
{
	CComVariant bstrPath;
	if (szXmlPath != NULL)
	{
		bstrPath = szXmlPath;
	}
	else
	{
		TCHAR path[_MAX_PATH] = {0X00};
		CCommClass::ExtractFilePath( path );
		_tcscat(path, _T("\\DVRSettings.xml"));
		bstrPath = path;
	}

	try
	{
		MSXML2::IXMLDOMDocumentPtr spDoc;
		MSXML2::IXMLDOMNodePtr spRoot;

		HRESULT hr = spDoc.CreateInstance(__uuidof(MSXML2::DOMDocument60));
		//if (FAILED(hr))
		//{
		//	hr = spDoc.CreateInstance(__uuidof(MSXML2::DOMDocument60));
		//}
		VARIANT_BOOL bIsSuccess = spDoc->load(bstrPath);
		if (bIsSuccess == VARIANT_FALSE)
		{
			CString csMsg;
			csMsg.Format(_T("¶ÁÈ¡ÎÄ¼þ%sÊ§°Ü£¡"), bstrPath.bstrVal);
			TRACE((LPCTSTR)csMsg);
			//AfxMessageBox(csMsg);
			return false;
		}

		spRoot = spDoc->documentElement;
		for (long index = 0; index <spRoot->childNodes->Getlength(); ++index)
		{
			if (spRoot->childNodes->Getitem(index)->GetnodeName() == _bstr_t(_T("MediaServerIP")))
			{
				m_csMediaServerIP = spRoot->childNodes->Getitem(index)->Gettext().GetBSTR();
			}
			else if (spRoot->childNodes->Getitem(index)->GetnodeName() == _bstr_t(_T("MediaServerPort")))
			{
				m_lPort = _ttoi(spRoot->childNodes->Getitem(index)->Gettext());
			}
			else if (spRoot->childNodes->Getitem(index)->GetnodeName() == _bstr_t(_T("MediaLoginUsername")))
			{
				m_csUsername = spRoot->childNodes->Getitem(index)->Gettext().GetBSTR();
			}
			else if (spRoot->childNodes->Getitem(index)->GetnodeName() == _bstr_t(_T("MediaLoginPassword")))
			{
				m_csPassword = spRoot->childNodes->Getitem(index)->Gettext().GetBSTR();
			}
			else if (spRoot->childNodes->Getitem(index)->GetnodeName() == _bstr_t(_T("MediaRenderStartTime")))
			{
				CString cStr =  spRoot->childNodes->Getitem(index)->Gettext().GetBSTR();
				CComVariant vtime(cStr);
				vtime.ChangeType(VT_DATE);
				ATL::COleDateTime time4 = vtime;
				SYSTEMTIME systime;
				VariantTimeToSystemTime(time4, &systime);
				CTime startTm(systime);//CString ===> CTime
				m_StartTime = startTm;
			}
			else if (spRoot->childNodes->Getitem(index)->GetnodeName() == _bstr_t(_T("MediaRenderEndTime")))
			{
				CString cStr =  spRoot->childNodes->Getitem(index)->Gettext().GetBSTR();
				CComVariant vtime(cStr);
				vtime.ChangeType(VT_DATE);
				COleDateTime time4 = vtime;
				SYSTEMTIME systime;
				VariantTimeToSystemTime(time4, &systime);
				CTime endTm(systime);
				m_EndTime = endTm;
			}
			else if (spRoot->childNodes->Getitem(index)->GetnodeName() == _bstr_t(_T("RenderWidth")))
			{
				m_nRenderWidth =  _ttoi(spRoot->childNodes->Getitem(index)->Gettext());
			}
			else if (spRoot->childNodes->Getitem(index)->GetnodeName() == _bstr_t(_T("RenderHeight")))
			{
				m_nRenderHeight =  _ttoi(spRoot->childNodes->Getitem(index)->Gettext());
			}
			else if (spRoot->childNodes->Getitem(index)->GetnodeName() == _bstr_t(_T("RenderWndNum")))
			{
				m_nRenderWndNum = _ttoi(spRoot->childNodes->Getitem(index)->Gettext());
			}
			else if (spRoot->childNodes->Getitem(index)->GetnodeName() == _bstr_t(_T("PlayerHighPictureQuality")))
			{
				m_bHighPictureQuality = _ttoi(spRoot->childNodes->Getitem(index)->Gettext().GetBSTR());
			}
			else if (spRoot->childNodes->Getitem(index)->GetnodeName() == _bstr_t(_T("PlayerCapturePicType")))
			{
				int capturePicType;
				capturePicType = _ttoi(spRoot->childNodes->Getitem(index)->Gettext());
				m_eCapturePicType = eCapPicType(capturePicType);
			}
			else if (spRoot->childNodes->Getitem(index)->GetnodeName() == _bstr_t(_T("PlayersPicCapturePath")))
			{
				m_csPicCapturePath = spRoot->childNodes->Getitem(index)->Gettext().GetBSTR();
			}
		}
	}
	catch (...)
	{
		return false;
	}

	return true;
}
