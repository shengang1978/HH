// DVRMVPlayerCtrl.cpp : Implementation of CDVRMVPlayerCtrl
#include "stdafx.h"
#include "DVRMVPlayerCtrl.h"
#include "OPErrorTypes.h"
#include <assert.h>
#include "dvr/TraceLog.h"


// CDVRMVPlayerCtrl

#define ID_ELAPSED_TIMER (2000)
#define ID_CURSOR_HIDE_TIMER (2001)

LRESULT CDVRMVPlayerCtrl::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	assert(false);
	m_bWindowOnly = TRUE;
    m_IMVPlayer.SetWindow(m_hWnd);

    #undef SubclassWindow
	HWND hRootWnd = GetAncestor(m_hWnd,GA_ROOT);
	if(::IsWindow(hRootWnd))
		m_msgSniffer.SubclassWindow(hRootWnd);

	if(m_IMVPlayer.State() == PlayerState_Playing)
		SetTimer(ID_ELAPSED_TIMER, 500, NULL);

    return 0;
}

LRESULT CDVRMVPlayerCtrl::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_bFullScreen = VARIANT_FALSE;
	m_hWnd = NULL;
	m_IMVPlayer.TearDownGraph();
	m_msgSniffer.UnsubclassWindow(TRUE);
    return 0;
}

LRESULT CDVRMVPlayerCtrl::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    PAINTSTRUCT ps;
    HDC hdc;

    hdc = BeginPaint(&ps);

    if (m_IMVPlayer.State() != PlayerState_Uninitialized && m_IMVPlayer.State() != PlayerState_Stopped && !m_bDisplayChaning)
    {
        m_IMVPlayer.RepaintVideo();
    }
    else
    {
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(::GetStockObject(BLACK_BRUSH)));
    }

    EndPaint(&ps);

    return 0;
}

LRESULT CDVRMVPlayerCtrl::OnWindowPosChanging(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(m_bParentChanging)
	{
		WINDOWPOS* pWindowPos = (WINDOWPOS*)lParam;
		if(pWindowPos)
		{
			pWindowPos->flags |= SWP_NOSIZE | SWP_NOMOVE;
			if(m_bFullScreen)
				pWindowPos->flags |= SWP_NOREDRAW;

		}
	}
	 return 0;
}

LRESULT CDVRMVPlayerCtrl::OnMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	if(!m_bDisplayChaning)
	{
		if(m_IMVPlayer.State() == PlayerState_Paused || m_IMVPlayer.State() == PlayerState_Buffering)
			m_IMVPlayer.RepaintVideo();
	}

	bHandled = FALSE;
	return 0;
}

LRESULT CDVRMVPlayerCtrl::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if(!m_bDisplayChaning)
	{
		RECT rcClient, rcWindow;
		GetClientRect(&rcClient);
		GetWindowRect(&rcWindow);
		m_IMVPlayer.UpdateVideoWindow(&rcClient);
	}

	return 0;
}

LRESULT CDVRMVPlayerCtrl::OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    return 0;
}

void CDVRMVPlayerCtrl::OnGraphEvent(long eventCode, LONG_PTR param1, LONG_PTR param2)
{
    switch (eventCode)
    {
    case EC_COMPLETE:
    case EC_USERABORT:
		StopInternal(TRUE,TRUE);
        break;
    case EC_ERRORABORT:
        Error(_T("Playback error."));
		
		#if defined(DEBUG) || defined(DEV_RELEASE)
		{
			char debugString[256];
			sprintf_s(debugString,256,"EC_ERRORABORT hr1 = 0x%08x hr2 = 0x%08x\n",param1,param2);
			OutputDebugStringA(debugString);
		}
		#endif

		if(param1 == E_COPP_OUTPUT_PROTECTION_FAILURE)
		{
			// ERROR_GRAPHICS_OPM_VIDEO_OUTPUT_NO_LONGER_EXISTS:
			//The operating system asynchronously destroyed this OPM video output because the operating system's state changed. 
			//This error typically occurs because the monitor PDO associated with this video output was removed,
			//the monitor PDO associated with this video output was stopped, 
			//the video output's session became a non-console session or the video output's desktop became an inactive desktop.

			// For such a case, we simply ignore the opm error and wait display change to re-create opm session.
			if(param2 != ERROR_GRAPHICS_OPM_VIDEO_OUTPUT_NO_LONGER_EXISTS)
			{
				StopInternal(FALSE,TRUE);
				//m_IMVPlayer.CloseSession();
				m_bNeedReInitMVSession = TRUE;
				Fire_HDCPError();
			}
		}
		else
		{
			StopInternal(TRUE,TRUE);
		}

		break;
    }
}

STDMETHODIMP CDVRMVPlayerCtrl::get_URL(BSTR* pbstrURL)
{
    IMV_TRACE(_T("CDVRMVPlayerCtrl::get_URL()\n"));

    // make a copy of m_bstrURL pointed to by pstrURL
    *pbstrURL = m_bstrURL.Copy();
    return S_OK;
}

STDMETHODIMP CDVRMVPlayerCtrl::put_URL(BSTR bstrURL)
{
    IMV_TRACE(_T("CDVRMVPlayerCtrl::put_URL()\n"));

	if (bstrURL == NULL)
	{
		return S_OK;
	}
	m_bNeedReInitMVSession = FALSE;
	HRESULT hr = m_IMVPlayer.Open(bstrURL);
	if(SUCCEEDED(hr))
	{
		put_mute(m_bMute);
		m_bstrURL = bstrURL;
		m_bstrStatus = _T("Open MP4 file succeeded!");
		return S_OK;
	}
	else
	{
		m_bstrURL.Empty();

		if(hr == E_COPP_OUTPUT_PROTECTION_FAILURE)
		{
			m_bstrStatus = _T("HDCP cannot be enabled");
			hr = IMV_FAIL_HDCP;
		}
		else if(hr == E_NO_VALID_KEY)
		{
			m_bstrStatus = _T("There is no license with the file");
			hr = IMV_FAIL_LICENSE;
		}
		else
		{
			m_bstrStatus = _T("This is an invalide MP4 file");
			hr = IMV_FAIL_INVALIDATE_FILE;
		}

		Error(m_bstrStatus);
		return hr;
	}
}

STDMETHODIMP CDVRMVPlayerCtrl::play(void)
{
    IMV_TRACE(_T("CDVRMVPlayerCtrl::play()\n"));

	if(m_IMVPlayer.State() == PlayerState_Stopped)
	{
		if(m_bNeedReInitMVSession)
		{
			if(FAILED(m_IMVPlayer.ReInitSession())) 
				return IMV_FAIL_HDCP;

			m_bNeedReInitMVSession = FALSE;
		}

		/*if(IsWindow())
			SetTimer(ID_ELAPSED_TIMER, 500, NULL);*/
	}

	return m_IMVPlayer.Play(); 
}

STDMETHODIMP CDVRMVPlayerCtrl::pause(void)
{
    IMV_TRACE(_T("CDVRMVPlayerCtrl::pause()\n"));

    return m_IMVPlayer.Pause();
}

STDMETHODIMP CDVRMVPlayerCtrl::stop(void)
{
    IMV_TRACE(_T("CDVRMVPlayerCtrl::stop()\n"));

	return StopInternal();
}

HRESULT CDVRMVPlayerCtrl::StopInternal(BOOL bResetTime /* = TRUE */, BOOL bResetSize /* = FALSE */)
{
	if(m_bFullScreen)
	{
		m_bFullScreen = VARIANT_FALSE;
		if(IsWindow())
		{
			HWND hwndParent;
			m_spInPlaceSite->GetWindow(&hwndParent);
			ModifyStyle(WS_POPUP,WS_CHILD);
			m_bParentChanging = TRUE;
			SetParent(hwndParent);
			m_bParentChanging = FALSE;
		}
	}

	if(IsWindow())
	{
		KillTimer(ID_ELAPSED_TIMER);	
	}

	HRESULT hr = m_IMVPlayer.Stop(bResetTime);

	if(bResetSize)
	{
		RECT rcPos = {0}, rcClip = {0};
		IOleInPlaceObject_SetObjectRects(&rcPos,&rcClip);
	}

	return hr;
}

STDMETHODIMP CDVRMVPlayerCtrl::get_duration(DOUBLE* pDuration)
{
    IMV_TRACE(_T("CDVRMVPlayerCtrl::get_duration()\n"));

	CheckPointer(pDuration,E_POINTER);
	*pDuration = 0;
	LONGLONG Duration = 0;
	HRESULT hr = m_IMVPlayer.GetDuration(&Duration);
	CHKHR;
	*pDuration = (double)Duration / UNITS;
	return S_OK;
}

STDMETHODIMP CDVRMVPlayerCtrl::get_durationString(BSTR* pbstrDuration)
{
    IMV_TRACE(_T("CDVRMVPlayerCtrl::get_durationString()\n"));

    CheckPointer(pbstrDuration,E_POINTER);

    CComBSTR bstrDuration(L"00:00");
    LONGLONG Duration;
    HRESULT hr = m_IMVPlayer.GetDuration(&Duration);
    if(SUCCEEDED(hr))
    {
        WCHAR wszDuration[9];
        UINT second = Duration / UNITS;
        UINT hour = second / 3600;
        UINT minute = (second - hour*3600) / 60;
        second = second - hour*3600 - minute*60;
        if(hour)
            swprintf_s(wszDuration, 9,TEXT("%.2d:%.2d:%.2d"), hour,minute,second);
        else
            swprintf_s(wszDuration, 9,TEXT("%.2d:%.2d"), minute,second);

        bstrDuration = wszDuration;
    }

    *pbstrDuration = bstrDuration.Detach();

    return hr;
}

STDMETHODIMP CDVRMVPlayerCtrl::get_currentPosition(DOUBLE* pdCurrentPosition)
{
    IMV_TRACE(_T("CDVRMVPlayerCtrl::get_currentPosition()\n"));
	 
	CheckPointer(pdCurrentPosition,E_POINTER);
	*pdCurrentPosition = 0;
	LONGLONG CurrentPosition = 0;
	HRESULT hr = m_IMVPlayer.GetCurrentPosition(&CurrentPosition);
	CHKHR;
	*pdCurrentPosition = (double)CurrentPosition / UNITS;
	return S_OK;
}

STDMETHODIMP CDVRMVPlayerCtrl::get_currentPositionString(BSTR* pbstrCurrentPosition)
{
    IMV_TRACE(_T("CDVRMVPlayerCtrl::get_currentPositionString()\n"));

    CheckPointer(pbstrCurrentPosition,E_POINTER);

    CComBSTR bstrCurrentPosition(L"00:00");
    LONGLONG CurrentPosition = 0;
    HRESULT hr = m_IMVPlayer.GetCurrentPosition(&CurrentPosition);
    if(SUCCEEDED(hr))
    {
        WCHAR wszCurrentPosition[9];
        UINT second = CurrentPosition / UNITS;
        UINT hour = second / 3600;
        UINT minute = (second - hour*3600) / 60;
        second = second - hour*3600 - minute*60;
        if(hour < 100 && hour > 0)
            swprintf_s(wszCurrentPosition, 9,TEXT("%.2d:%.2d:%.2d"), hour,minute,second);
        else if(hour == 0)
            swprintf_s(wszCurrentPosition, 9,TEXT("%.2d:%.2d"), minute,second);
		if (hour < 100)
		{
			bstrCurrentPosition = wszCurrentPosition;
		}
	}

    *pbstrCurrentPosition = bstrCurrentPosition.Detach();

    return hr;
}

STDMETHODIMP CDVRMVPlayerCtrl::put_currentPosition(DOUBLE dCurrentPosition)
{
    IMV_TRACE(_T("CDVRMVPlayerCtrl::put_currentPosition()\n"));
	LONGLONG CurrentPosition = (LONGLONG)dCurrentPosition * UNITS;
    return m_IMVPlayer.SetCurrentPosition(CurrentPosition);
}

STDMETHODIMP CDVRMVPlayerCtrl::fastForward(void)
{
    IMV_TRACE(_T("CDVRMVPlayerCtrl::fastForward()\n"));
    
	return m_IMVPlayer.SetRate(18.0f);
}

STDMETHODIMP CDVRMVPlayerCtrl::fastReverse(void)
{
    IMV_TRACE(_T("CDVRMVPlayerCtrl::fastReverse()\n"));

    return m_IMVPlayer.SetRate(-18.0f);
}

STDMETHODIMP CDVRMVPlayerCtrl::get_fullScreen(VARIANT_BOOL *pbFullScreen)
{
    IMV_TRACE(_T("CDVRMVPlayerCtrl::get_fullScreen()\n"));

    CheckPointer(pbFullScreen,E_POINTER);
    *pbFullScreen = m_bFullScreen; 
    return S_OK;
}

STDMETHODIMP CDVRMVPlayerCtrl::put_fullScreen(VARIANT_BOOL bFullScreen)
{
    IMV_TRACE(_T("CDVRMVPlayerCtrl::put_fullScreen()\n"));

	if(!IsWindow())
	{
		m_bFullScreen = VARIANT_FALSE;
		return S_OK;
	}

    if(m_bFullScreen == bFullScreen)
        return S_OK;
	/*
	if (bFullScreen && m_IMVPlayer.State() == PlayerState_Uninitialized)
	{
		return S_OK;
	}
	*/
    m_bFullScreen = bFullScreen;
	
    if(m_bFullScreen)
    {
		HMONITOR hMonitor = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
		MONITORINFOEX mi;
		mi.cbSize = sizeof(mi);
		GetMonitorInfo(hMonitor, &mi);

		//SetParent does not modify the WS_CHILD or WS_POPUP window styles of the window whose parent is being changed. 
		//Therefore, if hWndNewParent is NULL, you should also clear the WS_CHILD bit and set the WS_POPUP style after calling SetParent.
		m_bParentChanging = TRUE;
        SetParent(NULL); 
		m_bParentChanging = FALSE;
		ModifyStyle(WS_CHILD,WS_POPUP);
		SetWindowRgn(NULL, TRUE);
		SetWindowPos(
			HWND_TOPMOST, 
			mi.rcMonitor.left, 
			mi.rcMonitor.top, 
			mi.rcMonitor.right-mi.rcMonitor.left, 
			mi.rcMonitor.bottom-mi.rcMonitor.top, 
			SWP_SHOWWINDOW | SWP_NOREDRAW);
		TurnOnCursorHiding();
    }
    else
    {
        // get location in the parent window,
        // as well as some information about the parent
        //
        OLEINPLACEFRAMEINFO frameInfo;
        RECT rcPos, rcClip;
        CComPtr<IOleInPlaceFrame> spInPlaceFrame;
        CComPtr<IOleInPlaceUIWindow> spInPlaceUIWindow;
        frameInfo.cb = sizeof(OLEINPLACEFRAMEINFO);
        HWND hwndParent;
        if (m_spInPlaceSite->GetWindow(&hwndParent) == S_OK)
        {
            m_spInPlaceSite->GetWindowContext(&spInPlaceFrame,
                &spInPlaceUIWindow, &rcPos, &rcClip, &frameInfo);

			//Conversely, if hWndNewParent is not NULL and the window was previously a child of the desktop, 
			//you should clear the WS_POPUP style and set the WS_CHILD style before calling SetParent.
			ModifyStyle(WS_POPUP,WS_CHILD);

			m_bParentChanging = TRUE;
            SetParent(hwndParent);
			m_bParentChanging = FALSE;
           
            SetWindowPos(
                0, 
                rcPos.left, 
                rcPos.top, 
                rcPos.right - rcPos.left, 
                rcPos.bottom - rcPos.top, 
                SWP_SHOWWINDOW | SWP_NOREDRAW);
        }
		TurnOffCursorHiding();
    }
	m_IMVPlayer.UpdateVideoWindow(NULL, m_bFullScreen == VARIANT_TRUE);

    return S_OK;
}

STDMETHODIMP CDVRMVPlayerCtrl::get_isAvailable(BSTR bstrItem, VARIANT_BOOL* pIsAvailable)
{
    IMV_TRACE(_T("CDVRMVPlayerCtrl::get_isAvailable()\n"));

    CheckPointer(bstrItem,E_POINTER);
    CheckPointer(pIsAvailable,E_POINTER);
    CComBSTR bstrTemp(bstrItem);
    bstrTemp.ToLower();
    if(bstrTemp == L"currentposition")
	{
		*pIsAvailable = m_IMVPlayer.CanSeek() ? VARIANT_TRUE : VARIANT_FALSE;
	}
    else if(bstrTemp == L"pause")
    {
		*pIsAvailable = m_IMVPlayer.CanPause() ? VARIANT_TRUE : VARIANT_FALSE;
    }
    else if(bstrTemp == L"play")
    {
		*pIsAvailable = m_IMVPlayer.CanPlay() ? VARIANT_TRUE : VARIANT_FALSE;
    }
    else if(bstrTemp == L"stop")
    {
		*pIsAvailable = m_IMVPlayer.CanStop() ? VARIANT_TRUE : VARIANT_FALSE;
    }
    else if(bstrTemp == L"fastforward")
    {
		*pIsAvailable = m_IMVPlayer.CanFF() ? VARIANT_TRUE : VARIANT_FALSE;
    }
    else if(bstrTemp == L"fastreverse")
    {
		*pIsAvailable = m_IMVPlayer.CanFR() ? VARIANT_TRUE : VARIANT_FALSE;
    }
    else
    {
        *pIsAvailable = VARIANT_FALSE;
    }

    return S_OK;
}

STDMETHODIMP CDVRMVPlayerCtrl::get_imageSourceHeight(LONG* pHeight)
{
    IMV_TRACE(_T("CDVRMVPlayerCtrl::get_imageSourceHeight()\n"));

    return m_IMVPlayer.GetVideoImageHeight(pHeight);
}

STDMETHODIMP CDVRMVPlayerCtrl::get_imageSourceWidth(LONG* pWidth)
{
    IMV_TRACE(_T("CDVRMVPlayerCtrl::get_imageSourceWidth()\n"));

    return m_IMVPlayer.GetVideoImageWidth(pWidth);
}

STDMETHODIMP CDVRMVPlayerCtrl::get_mute(VARIANT_BOOL* pfMute)
{
    IMV_TRACE(_T("CDVRMVPlayerCtrl::get_mute()\n"));

    CheckPointer(pfMute,E_POINTER);
    *pfMute = m_bMute;
    return S_OK;
}

STDMETHODIMP CDVRMVPlayerCtrl::put_mute(VARIANT_BOOL fMute)
{
    IMV_TRACE(_T("CDVRMVPlayerCtrl::put_mute()\n"));

    m_bMute = fMute;
    if(fMute)
        m_IMVPlayer.SetVolume(0);
    else
        m_IMVPlayer.SetVolume(m_lVolume);

    return S_OK;
}

STDMETHODIMP CDVRMVPlayerCtrl::get_volume(LONG* plVolume)
{
    IMV_TRACE(_T("CDVRMVPlayerCtrl::get_volume()\n"));

    CheckPointer(plVolume,E_POINTER);

    *plVolume = m_lVolume;
    return S_OK;
}

STDMETHODIMP CDVRMVPlayerCtrl::put_volume(LONG lVolume)
{
    IMV_TRACE(_T("CDVRMVPlayerCtrl::put_volume()\n"));

    if(lVolume < 0 && lVolume > 100)
        return E_INVALIDARG;

    m_lVolume = lVolume;
    m_IMVPlayer.SetVolume(lVolume);
    return S_OK;
}

STDMETHODIMP CDVRMVPlayerCtrl::get_playState(LONG* plState)
{
    IMV_TRACE(_T("CDVRMVPlayerCtrl::get_playState()\n"));

    CheckPointer(plState,E_POINTER);
    *plState = m_IMVPlayer.State();
    return S_OK;
}

// The status property retrieves a value indicating the status of Player.
// This property is a read-only String.
STDMETHODIMP CDVRMVPlayerCtrl::get_status(BSTR* pbstrStatus)
{
    IMV_TRACE(_T("CDVRMVPlayerCtrl::get_status()\n"));

    CheckPointer(pbstrStatus,E_POINTER);

    // make a copy of m_bstrStatus pointed to by pstrStatus
    *pbstrStatus = m_bstrStatus.Copy();
    return S_OK;
}

STDMETHODIMP CDVRMVPlayerCtrl::get_versionInfo(BSTR* pbstrVersionInfo)
{
    IMV_TRACE(_T("CDVRMVPlayerCtrl::get_versionInfo()\n"));

    CheckPointer(pbstrVersionInfo,E_POINTER);

    // make a copy of m_bstrStatus pointed to by pstrVersion
    *pbstrVersionInfo = m_bStrVersion.Copy();
    return S_OK;
}

STDMETHODIMP CDVRMVPlayerCtrl::close(void)
{
    IMV_TRACE(_T("CDVRMVPlayerCtrl::close()\n"));

    m_IMVPlayer.TearDownGraph();
	m_bNeedReInitMVSession = FALSE;
	m_bstrURL.Empty();

    return S_OK;
}

STDMETHODIMP CDVRMVPlayerCtrl::get_uiMode(BSTR* pbstrMode)
{
    IMV_TRACE(_T("CDVRMVPlayerCtrl::get_uiMode()\n"));

    CheckPointer(pbstrMode,E_POINTER);

    // make a copy of m_bstruiMode pointed to by pstruiMode
    *pbstrMode = m_bstruiMode.Copy();
    return S_OK;
}

STDMETHODIMP CDVRMVPlayerCtrl::put_uiMode(BSTR bstrMode)
{
    IMV_TRACE(_T("CDVRMVPlayerCtrl::put_uiMode()\n"));

    m_bstruiMode = bstrMode;
    return S_OK;
}

STDMETHODIMP CDVRMVPlayerCtrl::get_controls(IDVRMVControls **ppControl)
{
    IMV_TRACE(_T("CDVRMVPlayerCtrl::get_controls()\n"));
	return _InternalQueryInterface(IID_IDVRMVControls,(void**)ppControl);
}

STDMETHODIMP CDVRMVPlayerCtrl::get_settings(IDVRMVSettings** ppSettings)
{
    IMV_TRACE(_T("CDVRMVPlayerCtrl::get_settings()\n"));
	return _InternalQueryInterface(IID_IDVRMVSettings,(void**)ppSettings);
}

STDMETHODIMP CDVRMVPlayerCtrl::get_currentMedia(IDVRMVMedia** ppMedia)
{
    IMV_TRACE(_T("CDVRMVPlayerCtrl::get_currentMedia()\n"));
    return _InternalQueryInterface(IID_IDVRMVMedia,(void**)ppMedia);
}

LRESULT CDVRMVPlayerCtrl::OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	switch (wParam)
	{
	case ID_CURSOR_HIDE_TIMER:
		::ShowCursor(FALSE);
		KillCursorHideTimer();
		break;
	case ID_ELAPSED_TIMER:
		m_IMVPlayer.CheckBufferring();
		break;
	}

	return 0;
}

LRESULT CDVRMVPlayerCtrl::OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if(m_bFullScreen)
	{
		TurnOffCursorHiding();
		TurnOnCursorHiding();
	}
	return 0;
}

LRESULT CDVRMVPlayerCtrl::OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if(m_bFullScreen)
	{
		TurnOffCursorHiding();
		TurnOnCursorHiding();
	}
	else
	{
		m_IMVPlayer.SelectMonitorWnd();
	}

	return 0;
}

LRESULT CDVRMVPlayerCtrl::OnRButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if(m_bFullScreen)
	{
		TurnOffCursorHiding();
		TurnOnCursorHiding();
	}
	return 0;
}

void CDVRMVPlayerCtrl::KillCursorHideTimer()
{
	if (m_idCursorHideTimer)
	{
		// Stop cursor hiding timer.
		KillTimer(ID_CURSOR_HIDE_TIMER);
		m_idCursorHideTimer = 0;
	}
}

void CDVRMVPlayerCtrl::TurnOnCursorHiding()
{
	if (!m_idCursorHideTimer)
		m_idCursorHideTimer = SetTimer(ID_CURSOR_HIDE_TIMER,2000,(TIMERPROC) NULL);
}

void CDVRMVPlayerCtrl::TurnOffCursorHiding()
{
	KillCursorHideTimer();

	// Make sure the cursor showed up.
	CURSORINFO info;
	ZeroMemory(&info, sizeof(CURSORINFO));
	info.cbSize = sizeof(CURSORINFO);
	GetCursorInfo(&info);
	if (0 == info.flags)
		::ShowCursor(TRUE);
}

STDMETHODIMP CDVRMVPlayerCtrl::OpenFile(BSTR bstrMediaFile)
{
	return m_IMVPlayer.Open(bstrMediaFile);
}
STDMETHODIMP CDVRMVPlayerCtrl::Login(BSTR bstrUsername, BSTR bstrPassword, BSTR bstrIP, LONG lPort, VARIANT_BOOL* pRet)
{
	HRESULT hr = m_IMVPlayer.Login(bstrUsername, bstrPassword, bstrIP, lPort);
	*pRet = hr == S_OK ? VARIANT_TRUE : VARIANT_FALSE;
	return hr;
}
STDMETHODIMP CDVRMVPlayerCtrl::Logout()
{
	return m_IMVPlayer.Logout();

}
STDMETHODIMP CDVRMVPlayerCtrl::SetMediaServer(BSTR bstrMediaServerIP, LONG lPort)
{
	return m_IMVPlayer.SetMediaServer(bstrMediaServerIP, lPort);

}
STDMETHODIMP CDVRMVPlayerCtrl::GetMediaServer(BSTR* bstrMediaServerIP, LONG* lPort)
{
	return m_IMVPlayer.GetMediaServer(bstrMediaServerIP, lPort);

}
STDMETHODIMP CDVRMVPlayerCtrl::StartMonitor(LONG lWndNum)
{
	return m_IMVPlayer.StartMonitor(lWndNum);

}
STDMETHODIMP CDVRMVPlayerCtrl::StopMonitor()
{
	return m_IMVPlayer.StopMonitor();

}
STDMETHODIMP CDVRMVPlayerCtrl::CapPic(BSTR* pbstrCapFolder)
{
	return m_IMVPlayer.CapPic(pbstrCapFolder);

}
STDMETHODIMP CDVRMVPlayerCtrl::GetCapturePath(BSTR* bstrCapFolder)
{
	return m_IMVPlayer.GetCapturePath(bstrCapFolder);

}
STDMETHODIMP CDVRMVPlayerCtrl::SetCapturePath(BSTR bstrCapFolder)
{
	return m_IMVPlayer.SetCapturePath(bstrCapFolder);

}
STDMETHODIMP CDVRMVPlayerCtrl::ShowMetaData(VARIANT_BOOL bShow)
{
	return m_IMVPlayer.ShowMetaData(bShow);

}
STDMETHODIMP CDVRMVPlayerCtrl::SetWndChannel(LONG lWndIndex, LONG lChannelIndex)
{
	return m_IMVPlayer.SetWndChannel(lWndIndex, lChannelIndex);
}
STDMETHODIMP CDVRMVPlayerCtrl::GetFullScreen(VARIANT_BOOL *pbFullScreen)
{
	return get_fullScreen(pbFullScreen);
}
STDMETHODIMP CDVRMVPlayerCtrl::SetFullScreen(VARIANT_BOOL bFullScreen)
{
	return put_fullScreen(bFullScreen);
}

STDMETHODIMP CDVRMVPlayerCtrl::AddFileToPlayList(BSTR bstrFile)
{
	return m_IMVPlayer.AddFileToPlayList(bstrFile);
}

STDMETHODIMP CDVRMVPlayerCtrl::ClosePlayList()
{
	return m_IMVPlayer.ClosePlayList();
}
STDMETHODIMP CDVRMVPlayerCtrl::PlayNextFile()
{
	return m_IMVPlayer.PlayNextFile();
}

STDMETHODIMP CDVRMVPlayerCtrl::StartPlayback(LONGLONG lStartTime, LONGLONG lEndTime, LONG lChannel)
{
	return m_IMVPlayer.StartPlayback(lStartTime, lEndTime, lChannel);
}

STDMETHODIMP CDVRMVPlayerCtrl::StopPlayback()
{
	return m_IMVPlayer.StopPlayback();
}

STDMETHODIMP CDVRMVPlayerCtrl::GetDuration(DOUBLE* pDuration)
{
	return get_duration(pDuration);
}

STDMETHODIMP CDVRMVPlayerCtrl::GetCurrentPosition(DOUBLE* pdCurrentPosition)
{
	return get_currentPosition(pdCurrentPosition);
}

STDMETHODIMP CDVRMVPlayerCtrl::GetDurationString(BSTR* pbstrDuration)
{
	return get_durationString(pbstrDuration);
}

STDMETHODIMP CDVRMVPlayerCtrl::GetCurrentPositionString(BSTR* pbstrCurrentPosition)
{
	return get_currentPositionString(pbstrCurrentPosition);
}

STDMETHODIMP CDVRMVPlayerCtrl::SetPosition(float position)
{
	return m_IMVPlayer.SetFilePointer(position);
}