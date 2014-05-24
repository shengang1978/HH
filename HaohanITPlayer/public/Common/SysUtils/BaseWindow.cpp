//-----------------------------------------------------------------------------
// BaseWindow.cpp
// Copyright (c) 2002 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW 20040911]
//SR FS: Reviewed [wwt 20040915] 

#include "BaseWindow.h"


//____ BaseWindow() __________________________________________________________________________
//
//	Description: Constructor
//
//  Parameters: N/A
//  Returns: N/A
//
BaseWindow::BaseWindow()
{

}


BaseWindow::BaseWindow(BaseWindow const & /*rhs*/)
{

}


BaseWindow& BaseWindow::operator=(BaseWindow const &rhs)
{
	if (this != &rhs) 
	{
		//do any real work here
	}
	return *this;
}

//____ ~BaseWindow() __________________________________________________________________________
//
//	Description: dtor
//
//  Parameters: N/A
//  Returns: N/A
//
BaseWindow::~BaseWindow()
{

}


