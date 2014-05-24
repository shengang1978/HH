//-----------------------------------------------------------------------------
// ScreenSaverCtl.h
// Copyright (c) 2002 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------
// ScreenSaverCtl.h: interface for the CScreenSaverCtl class.
//
//////////////////////////////////////////////////////////////////////

//SR FS: Reviewed [wwt 20040914] 
//SR FS: Reviewed [DDT 20040928] Second pass.
//SR FS: Reviewed [JAW 20040928] 

#if defined (_WIN32)

#if !defined(AFX_SCREENSAVERCTL_H__B1C08EC2_83BF_4AE8_9DFB_2EA9D65E0CD7__INCLUDED_)
#define AFX_SCREENSAVERCTL_H__B1C08EC2_83BF_4AE8_9DFB_2EA9D65E0CD7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CScreenSaverCtl  
{
public:
	CScreenSaverCtl();
	virtual ~CScreenSaverCtl();

	void	enable();
	void	disable();

	bool	is_enabled();
};

#endif // !defined(AFX_SCREENSAVERCTL_H__B1C08EC2_83BF_4AE8_9DFB_2EA9D65E0CD7__INCLUDED_)

#endif
