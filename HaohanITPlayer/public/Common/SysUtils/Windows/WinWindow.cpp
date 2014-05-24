//-----------------------------------------------------------------------------
// WinWindow.cpp
// Copyright (c) 2002 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914]

#if defined _WIN32
#include "WinWindow.h"


// _____________________________________________________________


//____ WinWindow() __________________________________________________________________________
//
//	Description: Constructor
//
//  Parameters: N/A
//  Returns: N/A
//
WinWindow::WinWindow(HWND hWnd) : 
	BaseWindow()
,	m_hWnd(hWnd)
{
	mBoundsRect.Set(0,0,0,0);
}

WinWindow::WinWindow(WinWindow const &rhs) :
	BaseWindow(rhs), m_hWnd(rhs.m_hWnd)
{
}


WinWindow& WinWindow::operator=(WinWindow const &rhs)
{
	if (this != &rhs) 
	{
		BaseWindow::operator= (rhs);
		m_hWnd = rhs.m_hWnd;
		mBoundsRect = rhs.mBoundsRect;
	}
	return *this;
}

//____ ~WinWindow() __________________________________________________________________________
//
//	Description: dtor
//
//  Parameters: N/A
//  Returns: N/A
//
WinWindow::~WinWindow()
{
}

PiRect const * WinWindow::GetBounds() const
{
	if (mBoundsRect.IsEmpty())
	{
		RECT r;
		::GetClientRect(m_hWnd, &r);
		mBoundsRect.Set(r.left, r.top, r.right, r.bottom);
	}
	return(&mBoundsRect);
}
#endif

