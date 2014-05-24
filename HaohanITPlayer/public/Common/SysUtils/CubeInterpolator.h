//-----------------------------------------------------------------------------
// CubeInterpolator.h
// Copyright (c) 2002 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#ifndef CUBE_INTERPPOLATOR
#define CUBE_INTERPPOLATOR

class CubeInterpolator
{
	double a, b, c, d, startX, startY, startM, endX, endY, endM;

public:
	CubeInterpolator();

	void ComputePolynomial(double x1, double y1, double m1, double x2, double y2, double m2);
	long InRange(double x) const; // return -1 if before range, 0 if in range, and 1 post range
	double EvaluatePolynomial(double x);
};

#endif
