#pragma once

#include "EVRPresenter.h"
#include "IntelAuxDevice.h"
#include "ref_ptr.h"

class IntelSCDD3DPresentEngine : public D3DPresentEngine
{
public:

    IntelSCDD3DPresentEngine(HRESULT& hr);
    ~IntelSCDD3DPresentEngine();
    
private:

    HRESULT PresentSwapChain(IDirect3DSwapChain9* pSwapChain, IDirect3DSurface9* pSurface);
    HRESULT CreateD3DDevice();
    
    HRESULT CreateSCD();
    void    ReleaseSCD();
	HRESULT UpdateOverlay();
    
private:
    ref_ptr<IntelScdService>    m_scd;
    
    RECT                        m_rcLastWinPos;
	RECT                        m_rcLastSurface;
};