//-----------------------------------------------------------------------------
// WinWindow.h
// Copyright (c) 2002 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------
//SR FS: Reviewed [wwt 20040914] 
//SR FS: Reviewed [DDT 20040928] Second pass.
//SR FS: Reviewed [JAW 20040928]

#ifndef WinWindow_H
#define WinWindow_H

#if defined (_WIN32)

#include "BaseWindow.h"
#include "PiRect.h"

//_____________________________________________________________________________

class WinWindow : public BaseWindow
{
public:
	explicit WinWindow(HWND hWnd = 0);
	virtual ~WinWindow();

	virtual PiRect const * GetBounds() const;

	// assign & copy constructor. Be sure to properly use!
	WinWindow(WinWindow const &rhs);
	WinWindow& operator=(WinWindow const &rhs);

	HWND GetWindow() const {return m_hWnd;}
protected:

private:
	HWND				m_hWnd;
	mutable PiRect		mBoundsRect;

};

#endif	//(_WIN32)

#endif	//WinWindow_H
