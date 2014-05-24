//-----------------------------------------------------------------------------
// ColorUtilities.cpp
// Copyright (c) 2001 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------
/*===============================================================
	ColorUtilities.cpp							03/07/2001
												Bradley Rowe
												Haohanit
		Purpose:
			Color conversion utilities.
===============================================================*/

//SR FS: Reviewed [JAW 20040911]
//SR FS: Reviewed [wwt 20040915] 

#include "ColorUtilities.h"
#include <cassert>
#include <algorithm>



void
ColorUtilities::RGBtoHSL(
	UInt16		red,
	UInt16		green,
	UInt16		blue,
	double&		H,			// OUT:
	double&		S,
	double&		L)
{

	//	check parameters
	//	Note: 	the comparison of the unsigned integer >= 0 will always be true
	//			and gcc always warns about unneeded comparisons.

#ifdef	__unix__

	assert (red <= 255);
	assert (green <= 255);
	assert (blue <= 255);

#else	// __unix__

	assert(red >= 0 && red <= 255) ;
	assert(green >=0 && green <=255);
	assert(blue >=0 && blue <=255);

#endif

	double		delta;
	double		r = static_cast<double>(red)/255;
	double		g = static_cast<double>(green)/255;
	double		b = static_cast<double>(blue)/255;
	double		cmax = std::max(r,std::max(g,b));
	double		cmin = std::min(r,std::min(g,b));


	L=(cmax+cmin)/2.0;
// **CodeWizzard** - Severe Violation:  User Defined item 208  - Don't check floats for equality; check for greater than or less than
	if (cmax == cmin)
	{
		S = 0;
		H = 0; // it's really undefined
	}
	else
	{
		if (L < 0.5)
		{
			S = (cmax-cmin) / (cmax+cmin);
		}
		else
		{
			S = (cmax-cmin) / (2.0-cmax-cmin);
		}
		delta = cmax - cmin;
// **CodeWizzard** - Severe Violation:  User Defined item 208  - Don't check floats for equality; check for greater than or less than
		if (r == cmax)
		{
			H = (g - b) / delta;
		}
// **CodeWizzard** - Severe Violation:  User Defined item 208  - Don't check floats for equality; check for greater than or less than
		else if (g == cmax)
		{
			H = 2.0 +(b-r)/delta;
		}
		else
		{
			H = 4.0 + (r - g) / delta;
		}
		H /= 6.0;
		if (H < 0.0)
		{
			H += 1;
		}
	}
}

/*******************************************************************************************************************************************
	From:	http://www.r2m.com/win-developer-FAQ/graphics/8.html
*******************************************************************************************************************************************/
double
ColorUtilities::HuetoRGB(
	double		m1,
	double		m2,
	double		h)
{
	if (h < 0)
	{
		h += 1.0;
	}
	if (h > 1)
	{
		h -= 1.0;
	}
	if ((6.0 * h) < 1)
	{
		return(m1 + (m2 - m1) * h * 6.0);
	}
	if ((2.0 * h) < 1)
	{
		return m2;
	}
	if ((3.0 * h) < 2.0)
	{
		return(m1 + (m2 - m1) * ((2.0 / 3.0) -h) * 6.0);
	}
	return m1;
}

/*******************************************************************************************************************************************
	From:	http://www.r2m.com/win-developer-FAQ/graphics/8.html
*******************************************************************************************************************************************/
void
ColorUtilities::HSLtoRGB(
	double		H,			// 0.0 to 1.0
	double		S,			// 0.0 to 1.0
	double		L,			// 0.0 to 1.0
	UInt8&		red,		// OUT:
	UInt8&		green,
	UInt8&		blue)
{
	//	check parameters
	assert(H >= 0.0 && H <= 1.0);
	assert(S >= 0.0 && S <= 1.0);
	assert(L >= 0.0 && L <= 1.0);

	double		r,g,b;
	double		m1, m2;

// **CodeWizzard** - Severe Violation:  User Defined item 208  - Don't check floats for equality; check for greater than or less than
	if (S == 0)
	{
		r = g = b = L;
	}
	else
	{
		if (L <= 0.5)
		{
			m2 = L * (1.0 + S);
		}
		else
		{
			m2 = L + S - L * S;
		}
		m1 = 2.0 * L - m2;
		r = HuetoRGB(m1, m2, H + 1.0 / 3.0);
		g = HuetoRGB(m1, m2, H);
		b = HuetoRGB(m1, m2, H - 1.0 / 3.0);
	}
	red   = static_cast<UInt8>(r * 255);
	green = static_cast<UInt8>(g * 255);
	blue  = static_cast<UInt8>(b * 255);
}
