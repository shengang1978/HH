//-----------------------------------------------------------------------------
// ColorUtilities.h
// Copyright (c) 2001 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------
/*===============================================================
	ColorUtilities.h							03/07/2001
												Bradley Rowe
												Haohanit
===============================================================*/

//SR FS: Reviewed [JAW 20040911]
//SR FS: Reviewed [wwt 20040914] 

#ifndef _colorutilities_h_
#define _colorutilities_h_


namespace ColorUtilities {

	void	RGBtoHSL(	UInt16	red,
						UInt16	green,
						UInt16	blue,
						double&	H,
						double&	S,
						double&	L);

	double	HuetoRGB(	double	m1,
						double	m2,
						double	h);

	void	HSLtoRGB(	double	H,			// 0.0 to 1.0
						double	S,			// 0.0 to 1.0
						double	L,			// 0.0 to 1.0
						UInt8&	red,		// OUT:
						UInt8&	green,
						UInt8&	blue);

}


#endif	// _colorutilities_h_

