//-----------------------------------------------------------------------------
// CubeInterpolator.cpp
// Copyright (c) 2002 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#include "CubeInterpolator.h"

CubeInterpolator::CubeInterpolator()
	: a(0.0), b(0.0), c(1.0), d(0.0), startX(0.0), endX(0.0)
{
}

void CubeInterpolator::ComputePolynomial(double x1, double y1, double m1, double x2, double y2, double m2)
{
	startX = x1;
	startY = y1;
	startM = m1;
	endX = x2;
	endY = y2;
	endM = m2;
// **CodeWizzard** - Severe Violation:  User Defined item 208  - Don't check floats for equality; check for greater than or less than
	if (x1 == x2)
	{
		// Try to do something reasonable with the divide by zero case.
		a = 0.0;
		b = 0.0;
		c = m1;
		d = y1-x1*m1;
		return;
	}
	double delta = x2 - x1;
	double delta_squared = delta*delta;
	double delta_cubed = delta*delta_squared;
	double delta_y = y2 - y1;

	a = (m1+m2)/delta_squared - (2.0*delta_y)/delta_cubed;
	b = 3.0*delta_y/delta_squared - (2.0*m1+m2)/delta;
	c = m1;
	d = y1;
}

long CubeInterpolator::InRange(double x) const
{
	if (startX < endX)
	{
		if (x < startX)
			return -1;
		if (x > endX)
			return 1;
		return 0;
	}
	if (x > startX)
		return -1;
	if (x < endX)
		return 1;
	return 0;
}

double CubeInterpolator::EvaluatePolynomial(double x)
{
	double y = 0.0;
	double dx = x-startX;
	switch (InRange(x))
	{
	case -1:
		y = startY + dx*startM;		// If we are out of range, go linear.
		break;
	case 1:
		y = endY + (x-endX)*endM;		// If we are out of range, go linear.
		break;
	default:
		y =  d + dx*(c + dx*(b + dx*a));
		break;
	}
	return y;
}
