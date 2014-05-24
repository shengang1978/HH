#include "IntelSCDD3DPresentEngine.h"
#include "com_ptr.h"

IntelSCDD3DPresentEngine::IntelSCDD3DPresentEngine(HRESULT& hr)
: D3DPresentEngine(hr)
{
	SetRectEmpty(&m_rcLastWinPos);
	SetRectEmpty(&m_rcLastSurface);
}

IntelSCDD3DPresentEngine::~IntelSCDD3DPresentEngine()
{
    ReleaseSCD();
}

HRESULT IntelSCDD3DPresentEngine::PresentSwapChain(IDirect3DSwapChain9* pSwapChain, IDirect3DSurface9* pSurface)
{
    EnterCriticalSection(&m_ObjectLock);
    
    if (m_scd && m_hwnd)
    {
        RECT rcWin = {0};
        GetWindowRect(m_hwnd, &rcWin);

		RECT rcSurface = {0};
		if (pSurface)
		{
			D3DSURFACE_DESC desc;
			memset(&desc, 0, sizeof(desc));
			pSurface->GetDesc(&desc);
			rcSurface.right = desc.Width;
			rcSurface.bottom = desc.Height;
		}

        if (!EqualRect(&m_rcLastWinPos, &rcWin) || !EqualRect(&m_rcLastSurface,&rcSurface))
        {
            m_rcLastWinPos = rcWin;
			m_rcLastSurface = rcSurface;
            UpdateOverlay();
        }
    }

    HRESULT hr = D3DPresentEngine::PresentSwapChain(pSwapChain, pSurface);
    
    LeaveCriticalSection(&m_ObjectLock);
    
    return hr;
}

HRESULT IntelSCDD3DPresentEngine::CreateD3DDevice()
{
    // Hold the lock because we might be discarding an exisiting device.
    EnterCriticalSection(&m_ObjectLock);

    HRESULT hr = D3DPresentEngine::CreateD3DDevice();
    
    if (SUCCEEDED(hr))
    {
		ReleaseSCD();
        // Create SCD
        CreateSCD();
    }
    
    LeaveCriticalSection(&m_ObjectLock);
    
    return hr;
}

HRESULT IntelSCDD3DPresentEngine::CreateSCD()
{
    com_ptr<IDirect3DDeviceManager9> devMgr(m_pDeviceManager);
    devMgr->AddRef();
    
    ref_ptr<IntelAuxiliaryDevice> auxDevice(new IntelAuxiliaryDevice(devMgr));
    
    HRESULT hr = S_OK;
    
    if (auxDevice)
    {
        auxDevice->InitAuxDevice();
        m_scd = new IntelScdService(auxDevice);
        
        if (m_scd)
        {
            hr = m_scd->CreateService();
			if (SUCCEEDED(hr))
			{
				HRESULT hrOut = 0;
				hr = m_scd->BeginAllocProtectedSurface(SCD_MODE_OVERLAY_BASED, &hrOut);
				if (SUCCEEDED(hr) && SUCCEEDED(hrOut))
					hr = m_scd->StopAllocProtectedSurface();
			}

            if (FAILED(hr))
            {
                ReleaseSCD();
            }
        }
    }
    
    return hr;
}

void IntelSCDD3DPresentEngine::ReleaseSCD()
{
	SetRectEmpty(&m_rcLastWinPos);
	SetRectEmpty(&m_rcLastSurface);

    if (m_scd)
	{
        m_scd->DestroyService();
		m_scd = 0; 
	}
}

HRESULT IntelSCDD3DPresentEngine::UpdateOverlay()
{
	HMONITOR hm = MonitorFromWindow(m_hwnd, MONITOR_DEFAULTTONEAREST);
	if(!hm)
		return E_FAIL;

	MONITORINFOEX infoEx;
	ZeroMemory(&infoEx, sizeof(infoEx));
	infoEx.cbSize = sizeof(infoEx);
	if(!GetMonitorInfo(hm, &infoEx))
		return E_FAIL;

	// Used for updating the window position.
	HDC hdcMonitor = CreateDC(NULL, infoEx.szDevice, NULL, NULL);
	if(!hdcMonitor)
		return E_FAIL;

	// rcWindow and rcClient are in screen coordinates.
	// To update an overlay call this and use the rcClient values as is.
	WINDOWINFO winInfo;
	winInfo.cbSize = sizeof(winInfo);
	GetWindowInfo(m_hwnd, &winInfo);

	RECT desktopRect = infoEx.rcMonitor;
	RECT winDestRect = winInfo.rcClient;
	RECT rcIntersect;
	RECT rcSrc = {0};
	RECT rcDest = {0};
	// If the window is on the primary desktop then we have some calcs to do,
	// else just set everything to 0 to keep the overlay hardware from crashing.
	if(IntersectRect(&rcIntersect, &desktopRect, &winDestRect))
	{
		rcSrc = m_rcLastSurface;

		int clientWidth = winDestRect.right - winDestRect.left;
		int clientHeight = winDestRect.bottom - winDestRect.top;

		int leftIn = 0,
			topIn = 0,
			rightIn = 0,
			bottomIn = 0;

		if(winDestRect.left < desktopRect.left)
		{
			leftIn = desktopRect.left - winDestRect.left;
			winDestRect.left = desktopRect.left;
		}

		if(winDestRect.top < desktopRect.top)
		{
			topIn = desktopRect.top - winDestRect.top;
			winDestRect.top = desktopRect.top;
		}

		if(winDestRect.right > desktopRect.right)
		{
			rightIn = winDestRect.right - desktopRect.right;
			winDestRect.right = desktopRect.right;
		}

		if(winDestRect.bottom > desktopRect.bottom)
		{
			bottomIn = winDestRect.bottom - desktopRect.bottom;
			winDestRect.bottom = desktopRect.bottom;
		}

		// << 14 = * 16384
		int clippedLeftPercent = (leftIn << 14) / clientWidth;
		int clippedTopPercent = (topIn << 14) / clientHeight;
		int clippedRightPercent = (rightIn << 14) / clientWidth;
		int clippedBottomPercent = (bottomIn << 14) / clientHeight;

		rcSrc.left = (rcSrc.right * clippedLeftPercent) >> 14;
		rcSrc.top = (rcSrc.bottom * clippedTopPercent) >> 14;
		rcSrc.right -= (rcSrc.right * clippedRightPercent) >> 14;
		rcSrc.bottom -= (rcSrc.bottom * clippedBottomPercent) >> 14;

		rcDest.left = winDestRect.left - desktopRect.left;
		rcDest.top = winDestRect.top - desktopRect.top;
		rcDest.right = winDestRect.right - desktopRect.left;
		rcDest.bottom = winDestRect.bottom - desktopRect.top;
	}


	SCD_SET_WINDOW_POSITION_PARAMS setWinPos;
	setWinPos.rWindowPosition = rcDest;
	setWinPos.rVisibleContent = rcSrc;
	setWinPos.hdcMonitorId = hdcMonitor;

	HRESULT hr = m_scd->SetWindowPosition(&setWinPos);
	DeleteDC(hdcMonitor);

	return hr;
}