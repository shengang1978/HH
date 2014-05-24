//-----------------------------------------------------------------------------
// BaseWindow.h
// Copyright (c) 2002 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW 20040911]
//SR FS: Reviewed [wwt 20040915] 

#ifndef BaseWindow_H
#define BaseWindow_H

class PiRect;

//_____________________________________________________________________________

class BaseWindow
{
public:
	BaseWindow();
	virtual ~BaseWindow();

	virtual PiRect const * GetBounds() const = 0;

	// assign & copy constructor. Be sure to properly use!
	BaseWindow(BaseWindow const &rhs);
	BaseWindow& operator=(BaseWindow const &rhs);

};



#endif	//BaseWindow_H
