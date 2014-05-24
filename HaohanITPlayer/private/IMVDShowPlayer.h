#pragma once

#include "DShow.h"
#include "Evr.h"
#include "IDVRMVEPID.h"
#if 0
	#define IMV_TRACE(str) OutputDebugString(str)
#else  
	#define IMV_TRACE(str)
#endif 
class CDVRPlayer;

enum PlayerState
{
	PlayerState_Uninitialized = 0,
	PlayerState_Stopped,
	PlayerState_Paused,
	PlayerState_Playing,
	PlayerState_ScanForward,
	PlayerState_ScanReverse,
	PlayerState_Buffering
};

const UINT WM_GRAPH_EVENT = WM_APP + 1;

const LONGLONG MILLISECONDS = (1000);            // 10 ^ 3
const LONGLONG NANOSECONDS = (1000000000);       // 10 ^ 9
const LONGLONG UNITS = 1;//(NANOSECONDS / 100);      // 10 ^ 7
const LONGLONG BUFFERING_THRESHOLD_IN = (30*UNITS); // the threshold to begin buffering(default is 30s less to available time)
const LONGLONG BUFFERING_THRESHOLD_OUT = (60*UNITS); // the threshold to end buffering(default is 60s large to available time)

#define E_NO_VALID_KEY			0xA0040B00
#define E_MODULE_CHECK			0xA0040B01

class IGraphEventCallback
{
public:
	virtual void OnGraphEvent(long eventCode, LONG_PTR param1, LONG_PTR param2) = 0;
};

class CIMVDShowPlayer
{
public:
	CIMVDShowPlayer();
	~CIMVDShowPlayer(void);

public:	
	inline PlayerState State() const { return m_state; }
	void    TearDownGraph();
	int SplitMode(int pahtLen);
	HRESULT Open(LPCWSTR moviePath);
	HRESULT Play();
	HRESULT Pause();
	HRESULT Stop(BOOL bSeekToBegin = TRUE);
	HRESULT SetRate(double dSpeed, bool bError=false);
	HRESULT GetDuration(LONGLONG *pDuration);
	HRESULT GetCurrentPosition(LONGLONG *pCurrentPosition);
	HRESULT SetCurrentPosition(LONGLONG toPosition);
	HRESULT GetAvailablePosition(LONGLONG *pAvailablePosition);
	HRESULT SetVolume(LONG lVol);
	HRESULT GetVolume(LONG* plVol);
	HRESULT CheckBufferring();
	HRESULT HandleDeviceChanged();
	HRESULT SetFilePointer(float toPosition);

	inline HRESULT ReInitSession()
	{
		CloseSession();

		m_pFilterGraph->RemoveFilter(m_pEVR);
		m_pEVR.Release();
		m_pEVRVideoDisplayControl.Release();
		HRESULT hr = LoadVideoRenderer(); CHKHR;
		hr = InitializePavp();
		return hr;
	}

	inline VOID CloseSession()
	{
		if (m_mvSession)
		{
			SInt32 epid_ret = m_pEPID->CloseSession();
			
			#if defined(DEBUG) || defined(DEV_RELEASE)
			if(epid_ret != IDVRMVEPID::eEPID_OK)
			{
				char debugString[256];
				sprintf_s(debugString,256,"MV_CloseSession err = %d\n",epid_ret);
				OutputDebugStringA(debugString);
			}
			#endif

			m_mvSession = 0;
		}
	}

	inline BOOL CanPlay()
	{
		if (m_state == PlayerState_Uninitialized ||
			m_state == PlayerState_Playing ||
			m_state == PlayerState_Buffering)
			return FALSE;
		else
			return TRUE;
	}

	inline BOOL CanPause()
	{
		if(m_state == PlayerState_Uninitialized ||
			m_state == PlayerState_Stopped ||
			m_state == PlayerState_Paused)
			return FALSE;
		else
			return TRUE;
	}

	inline BOOL CanStop()
	{
		if(m_state == PlayerState_Uninitialized || 
			m_state == PlayerState_Stopped)
			return FALSE;
		else
			return TRUE;
	}

	inline BOOL CanScanForward()
	{
		if(m_state == PlayerState_Uninitialized || 
			m_state == PlayerState_ScanForward)
			return FALSE;
		else
			return TRUE;
	}

	inline BOOL CanScanReverse()
	{
		if(m_state == PlayerState_Uninitialized || 
			m_state == PlayerState_ScanReverse)
			return FALSE;
		else
			return TRUE;
	}

	inline BOOL CanChangeRate()
	{
		if (m_state == PlayerState_Uninitialized ||
			m_state == PlayerState_Stopped)
			return FALSE;
		else
			return TRUE;
	}

	inline BOOL CanFF()
	{
		return CanChangeRate() && m_state != PlayerState_Buffering;
	}

	inline BOOL CanFR()
	{
		return CanChangeRate();
	}

	inline BOOL CanSeek()
	{
		if (m_state == PlayerState_Uninitialized)
			return FALSE;
		else
			return TRUE;
	}

	VOID SetWindow(HWND hWnd); 
	//{
	//	if(::IsWindow(hWnd) && m_hVideoWnd != hWnd)
	//	{
	//		HRESULT hr = S_OK;
	//		if(m_pMediaEvent)
	//		{
	//			// Set up event notification.
	//			hr = m_pMediaEvent->SetNotifyWindow((OAHWND)hWnd, WM_GRAPH_EVENT, NULL);
	//		}

	//		if(m_pEVRVideoDisplayControl)
	//		{
	//			hr = m_pEVRVideoDisplayControl->SetVideoWindow(hWnd);
	//			RECT rcVideo;
	//			GetClientRect(hWnd, &rcVideo);
	//			UpdateVideoWindow(&rcVideo);
	//		}

	//		if(SUCCEEDED(hr))
	//			m_hVideoWnd = hWnd;
	//	}
	//}

	//Repaints the current video frame. Call this method whenever the application receives a WM_PAINT message.
	 VOID RepaintVideo();
	//{
	//	if(m_pEVRVideoDisplayControl)
	//		m_pEVRVideoDisplayControl->RepaintVideo();
	//}

	 VOID UpdateVideoWindow(const LPRECT prc, int nFullScreenState = -1);
	//{
	//	if(m_pEVRVideoDisplayControl && prc)
	//		m_pEVRVideoDisplayControl->SetVideoPosition(NULL,prc);
	//}

	inline HRESULT GetVideoImageHeight(long* pHeight)
	{
		CheckPointer(pHeight,E_POINTER);
		*pHeight = 0;
		if(m_state == PlayerState_Uninitialized)
			return E_FAIL;
		else
		{
			*pHeight = m_videoHeight;
			return S_OK;
		}
	}

	inline HRESULT GetVideoImageWidth(long* pWidth)
	{
		CheckPointer(pWidth,E_POINTER);
		*pWidth = 0;
		if(m_state == PlayerState_Uninitialized)
			return E_FAIL;
		else
		{
			*pWidth = m_videoWidth;
			return S_OK;
		}
	}

	HRESULT HandleGraphEvent(IGraphEventCallback* pGraphEventCallback);

	HRESULT OpenFile(BSTR bstrMediaFile);
	HRESULT Login(BSTR bstrUsername, BSTR bstrPassword, BSTR bstrIP, LONG lPort);
	HRESULT Logout();
	HRESULT SetMediaServer(BSTR bstrMediaServerIP, LONG lPort);
	HRESULT GetMediaServer(BSTR* bstrMediaServerIP, LONG* lPort);
	HRESULT StartMonitor(LONG lWndNum);
	HRESULT StopMonitor();
	HRESULT CapPic(BSTR* pbstrCapFolder);
	HRESULT GetCapturePath(BSTR* bstrCapFolder);
	HRESULT SetCapturePath(BSTR bstrCapFolder);
	HRESULT ShowMetaData(VARIANT_BOOL bShow);
	HRESULT SetWndChannel(LONG lWndIndex, LONG lChannelIndex);
	HRESULT AddFileToPlayList(BSTR bstrFile);
	HRESULT ClosePlayList();
	HRESULT PlayNextFile();
	HRESULT SelectMonitorWnd();
	HRESULT StartPlayback(LONGLONG lStartTime, LONGLONG lEndTime, LONG lChannel);
	HRESULT StopPlayback();
private:
	HRESULT InitializeGraph();
	HRESULT	LoadSourceFilter(LPCWSTR moviePath);
	HRESULT LoadHDDemuxer();
	HRESULT LoadAudioDecoder();
	HRESULT LoadAudioRenderer();
	HRESULT LoadVideoDecoder();
	HRESULT LoadVideoRenderer();
	HRESULT InitializePavp(LPCWSTR moviePath = NULL);
	HRESULT GetFreePin(IBaseFilter* pFilter,IPin** ppPin,PIN_DIRECTION PinDir);
	HRESULT GetInputPin(IBaseFilter* pFilter,IPin** ppPin,UINT PinIndex = 0);
	HRESULT GetOutputPin(IBaseFilter* pFilter,IPin** ppPin,UINT PinIndex = 0);

private:
	CComPtr<IFilterGraph>  m_pFilterGraph;
	CComPtr<IMediaSeeking> m_pMediaSeeking;
	CComPtr<IMediaControl> m_pMediaControl;
	CComPtr<IMediaEventEx> m_pMediaEvent;
	CComPtr<IBaseFilter> m_pFileSource;
	CComPtr<IBaseFilter> m_pHDDemuxer;
	CComPtr<IBaseFilter> m_pVidDec;
	CComPtr<IBaseFilter> m_pAudDec;
	CComPtr<IBaseFilter> m_pEVR;
	CComPtr<IMFVideoDisplayControl> m_pEVRVideoDisplayControl;
	CComPtr<IBaseFilter> m_pDirectSound;

	HWND m_hVideoWnd; // Video window. This window also receives graph events.
	PlayerState m_state;
	WCHAR m_wszFiltersPath[MAX_PATH];
	LONG  m_videoWidth;
	LONG  m_videoHeight;
	double m_dRate;

	// PAVP/MV attributes
	BOOL m_bIsIMVPlatform;
	BOOL m_bProtectAudio; 
	BOOL m_bProtectedStream;
	UINT32 m_mvSession;
	IDVRMVEPID* m_pEPID;
//	ISonicMVClient* m_pMvAPI;

//	MV_KeyBlob_t    m_keyBlob;
	HANDLE  m_hFile;

//	UINT32 m_seed[MV_PRNG_SEED_BITS_MIN/32];

private:
	std::auto_ptr<CDVRPlayer>	m_spDVRPlayer;
	std::vector<CString> paths;
};
