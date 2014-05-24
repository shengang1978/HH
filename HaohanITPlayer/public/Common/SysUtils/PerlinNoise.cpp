//-----------------------------------------------------------------------------
// PerlinNoise.cpp
// Copyright (c) 2002 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

/* Coherent noise function over 1, 2 or 3 dimensions */
/* (copyright Ken Perlin) */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>	// random seed
#include "PerlinNoise.h"

PerlinNoise::PerlinNoise(UInt32 seed)
	: mIsInitialized(false)
{
	Initialize(seed);
}

void PerlinNoise::Normalize2(double v[2])
{
	double s;

	s = sqrt(v[0] * v[0] + v[1] * v[1]);
	v[0] = v[0] / s;
	v[1] = v[1] / s;
}

void PerlinNoise::Normalize3(double v[3])
{
	double s;

	s = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] = v[0] / s;
	v[1] = v[1] / s;
	v[2] = v[2] / s;
}

void PerlinNoise::Initialize(UInt32 seed)
{
	SInt32 i, j, k;

	srand((seed == 0) ? static_cast<UInt32>(time(0)) : seed);

	// Initialize permutation and gradients tables
	for (i = 0 ; i < kPerlinNoiseWidth ; i++)
	{
		mPermutations[i] = i;
		m1DGradient[i] = static_cast<double>((rand() % (kPerlinNoiseWidth + kPerlinNoiseWidth)) - kPerlinNoiseWidth) / kPerlinNoiseWidth;

		for (j = 0 ; j < 2 ; j++)
			m2DGradient[i][j] = static_cast<double>((rand() % (kPerlinNoiseWidth + kPerlinNoiseWidth)) - kPerlinNoiseWidth) / kPerlinNoiseWidth;
		Normalize2(m2DGradient[i]);

		for (j = 0 ; j < 3 ; j++)
			m3DGradient[i][j] = static_cast<double>((rand() % (kPerlinNoiseWidth + kPerlinNoiseWidth)) - kPerlinNoiseWidth) / kPerlinNoiseWidth;
		Normalize3(m3DGradient[i]);
	}

	// Scramble permutations table
// **CodeWizzard** - Possible Violation:  User Defined item 708  - Do not use operator ++ or -- in the conditional expression of if, while, or switch
	while (--i)
	{
		k = mPermutations[i];
		mPermutations[i] = mPermutations[j = rand() % kPerlinNoiseWidth];
		mPermutations[j] = k;
	}

	// Duplicate permutations and gradients table (copies 1st half into 2nd half of the array(s))
	for (i = 0 ; i < kPerlinNoiseWidth + 2 ; i++)
	{
		mPermutations[kPerlinNoiseWidth + i] = mPermutations[i];
		m1DGradient[kPerlinNoiseWidth + i] = m1DGradient[i];
		for (j = 0 ; j < 2 ; j++)
			m2DGradient[kPerlinNoiseWidth + i][j] = m2DGradient[i][j];
		for (j = 0 ; j < 3 ; j++)
			m3DGradient[kPerlinNoiseWidth + i][j] = m3DGradient[i][j];
	}

	mIsInitialized = true;
}

double PerlinNoise::Noise1(double arg)
{
	SInt32 bx0, bx1;
	double rx0, rx1, sx, t, u, v, vec[1];

	vec[0] = arg;

	setup(0, vec, bx0, bx1, rx0, rx1, t);

	sx = s_curve(rx0);
	u = rx0 * m1DGradient[ mPermutations[ bx0 ] ];
	v = rx1 * m1DGradient[ mPermutations[ bx1 ] ];

	return(lerp(sx, u, v));
}

double PerlinNoise::Noise2(double vec[2])
{
   SInt32 bx0, bx1, by0, by1, b00, b10, b01, b11;
   double rx0, rx1, ry0, ry1, *q, sx, sy, a, b, t, u, v;
   SInt32 i, j;

   setup(0, vec, bx0, bx1, rx0, rx1, t);
   setup(1, vec, by0, by1, ry0, ry1, t);

   i = mPermutations[ bx0 ];
   j = mPermutations[ bx1 ];

   b00 = mPermutations[ i + by0 ];
   b10 = mPermutations[ j + by0 ];
   b01 = mPermutations[ i + by1 ];
   b11 = mPermutations[ j + by1 ];

   sx = s_curve(rx0);
   sy = s_curve(ry0);

   q = m2DGradient[ b00 ] ; u = at2(q, rx0, ry0);
   q = m2DGradient[ b10 ] ; v = at2(q, rx1, ry0);
   a = lerp(sx, u, v);

   q = m2DGradient[ b01 ] ; u = at2(q, rx0, ry1);
   q = m2DGradient[ b11 ] ; v = at2(q, rx1, ry1);
   b = lerp(sx, u, v);

   return lerp(sy, a, b);
}

double PerlinNoise::Noise3(double vec[3])
{
   SInt32 bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
   double rx0, rx1, ry0, ry1, rz0, rz1, *q, sy, sz, a, b, c, d, t, u, v;
   SInt32 i, j;

   setup(0, vec, bx0, bx1, rx0, rx1, t);
   setup(1, vec, by0, by1, ry0, ry1, t);
   setup(2, vec, bz0, bz1, rz0, rz1, t);

   i = mPermutations[ bx0 ];
   j = mPermutations[ bx1 ];

   b00 = mPermutations[ i + by0 ];
   b10 = mPermutations[ j + by0 ];
   b01 = mPermutations[ i + by1 ];
   b11 = mPermutations[ j + by1 ];

   t  = s_curve(rx0);
   sy = s_curve(ry0);
   sz = s_curve(rz0);

   q = m3DGradient[ b00 + bz0 ] ; u = at3(q, rx0, ry0, rz0);
   q = m3DGradient[ b10 + bz0 ] ; v = at3(q, rx1, ry0, rz0);
   a = lerp(t, u, v);

   q = m3DGradient[ b01 + bz0 ] ; u = at3(q, rx0, ry1, rz0);
   q = m3DGradient[ b11 + bz0 ] ; v = at3(q, rx1, ry1, rz0);
   b = lerp(t, u, v);

   c = lerp(sy, a, b);

   q = m3DGradient[ b00 + bz1 ] ; u = at3(q, rx0, ry0, rz1);
   q = m3DGradient[ b10 + bz1 ] ; v = at3(q, rx1, ry0, rz1);
   a = lerp(t, u, v);

   q = m3DGradient[ b01 + bz1 ] ; u = at3(q, rx0, ry1, rz1);
   q = m3DGradient[ b11 + bz1 ] ; v = at3(q, rx1, ry1, rz1);
   b = lerp(t, u, v);

   d = lerp(sy, a, b);

   return lerp(sz, c, d);
}

/* --- My harmonic summing functions - PDB --------------------------*/

/*
   In what follows "alpha" is the weight when the sum is formed.
   Typically it is 2, As this approaches 1 the function is noisier.
   "beta" is the harmonic scaling/spacing, typically 2.
*/


double PerlinNoise::PerlinNoise1D(double x,double alpha,double beta,SInt32 n, bool sumAbsolutes)
{
   SInt32 i;
   double val,sum = 0;
   double p,scale = 1;

   p = x; // persistance value
   for (i=0;i<n;i++) {
      val = Noise1(p);
	  if (sumAbsolutes)
		  val = fabs(val);
      sum += val / scale;
      scale *= alpha;	// alpha is used to scale the amplitude of each iteration (e.g. sum = 
      p *= beta; // beta is used to scale the frequency of each iteration
   }
   return(sum);
}

double PerlinNoise::PerlinNoise2D(double x,double y,double alpha,double beta, SInt32 n, bool sumAbsolutes)
{
   SInt32 i;
   double val,sum = 0;
   double p[2],scale = 1;

   p[0] = x;
   p[1] = y;
   for (i=0;i<n;i++) {
      val = Noise2(p);
	  if (sumAbsolutes)
		  val = fabs(val);
      sum += val / scale;
      scale *= alpha;
      p[0] *= beta;
      p[1] *= beta;
   }
   return(sum);
}

double PerlinNoise::PerlinNoise3D(double x,double y,double z,double alpha,double beta, SInt32 n, bool sumAbsolutes)
{
   SInt32 i;
   double val,sum = 0;
   double p[3],scale = 1;

   p[0] = x;
   p[1] = y;
   p[2] = z;
   for (i=0;i<n;i++) {
      val = Noise3(p);
	  if (sumAbsolutes)
		  val = fabs(val);
      sum += val / scale;
      scale *= alpha;
      p[0] *= beta;
      p[1] *= beta;
      p[2] *= beta;
   }
   return(sum);
}

void PerlinNoise::Create1DNoiseMap(const PerlinNoiseData& params)
{
	SInt32 width = kPerlinNoiseWidth; // 256 byte noise map

	mpNoiseMap = new UInt8[width];
	UInt8 *pSrc = mpNoiseMap.get();

	double noise;
	double minNoise = 1.0;
	double maxNoise = -1.0;
	double arg;

    for (SInt32 x=0; x < width; ++x)
    {
		SInt32 value = 128;

		arg = (static_cast<double>(x) * params.mXScalar) / static_cast<double>(width);
		noise = PerlinNoise1D(arg, params.mAlpha, params.mBeta, params.mNumHarmonics, params.mbSumAbsolutes);

		if (params.mbTiled)
		{
			// PerlinNoise1D(x) * (width - x)
			noise *= (width - x);

			// + PerlinNoise1D(x-width) * (x)
			arg = (static_cast<double>(x-width) * params.mXScalar) / static_cast<double>(width);
			double noise2 = PerlinNoise1D(arg, params.mAlpha, params.mBeta, params.mNumHarmonics, params.mbSumAbsolutes);
			noise2 *= x;
			noise += noise2;

			noise /= width;
		}

		if (params.mbPhaseShift)
			noise = sin(x + noise);

		if (noise < minNoise)
			minNoise = noise;
		if (noise > maxNoise)
			maxNoise = noise;
// **CodeWizzard** - Severe Violation:  User Defined item 208  - Don't check floats for equality; check for greater than or less than
		if (noise != 0.0)
		{
			value += static_cast<SInt32>(noise * 128.0);
			if (value < 0)
				value = 0;
			if (value > 255)
				value = 255;
		}
		*pSrc = static_cast<UInt8>(value);
		pSrc++;
    }
}

void PerlinNoise::Create2DNoiseMap(const PerlinNoiseData& params)
{
	SInt32 width, height;
	width = height = kPerlinNoiseWidth; // 256 x 256 noise map

	mpNoiseMap = new UInt8[width * height];
	UInt8 *pSrc = mpNoiseMap.get();

	double noise;
	double minNoise = 1.0;
	double maxNoise = -1.0;
	double xArg, yArg;

    for (SInt32 y=0; y < height; ++y)
    {
        for (SInt32 x=0; x < width; ++x)
        {
			SInt32 value = 128;

			xArg = (static_cast<double>(x) * params.mXScalar) / static_cast<double>(width);
			yArg = (static_cast<double>(y) * params.mYScalar) / static_cast<double>(height);
			noise = PerlinNoise2D(xArg, yArg, params.mAlpha, params.mBeta, params.mNumHarmonics, params.mbSumAbsolutes);

			if (params.mbTiled)
			{
				double noise2;

				// PerlinNoise2D(x, y) * (width - x) * (height - y)
				noise *= ((width - x) * (height - y));

				// + PerlinNoise2D(x - width, y) * (x) * (height - y)
				xArg = (static_cast<double>(x-width) * params.mXScalar) / static_cast<double>(width);
				yArg = (static_cast<double>(y) * params.mYScalar) / static_cast<double>(height);
				noise2 = PerlinNoise2D(xArg, yArg, params.mAlpha, params.mBeta, params.mNumHarmonics, params.mbSumAbsolutes);
				noise2 *= (x * (height - y));
				noise += noise2;

				// + PerlinNoise2D(x - width, y - height) * (x) * (y)
				xArg = (static_cast<double>(x-width) * params.mXScalar) / static_cast<double>(width);
				yArg = (static_cast<double>(y-height) * params.mYScalar) / static_cast<double>(height);
				noise2 = PerlinNoise2D(xArg, yArg, params.mAlpha, params.mBeta, params.mNumHarmonics, params.mbSumAbsolutes);
				noise2 *= (x * y);
				noise += noise2;

				// + PerlinNoise2D(x, y - height) * (width - x) * (y)
				xArg = (static_cast<double>(x) * params.mXScalar) / static_cast<double>(width);
				yArg = (static_cast<double>(y-height) * params.mYScalar) / static_cast<double>(height);
				noise2 = PerlinNoise2D(xArg, yArg, params.mAlpha, params.mBeta, params.mNumHarmonics, params.mbSumAbsolutes);
				noise2 *= ((width - x) * y);
				noise += noise2;

				noise /= (width * height);
			}

			if (params.mbPhaseShift)
				noise = sin(x + noise);

			if (noise < minNoise)
				minNoise = noise;
			if (noise > maxNoise)
				maxNoise = noise;
// **CodeWizzard** - Severe Violation:  User Defined item 208  - Don't check floats for equality; check for greater than or less than
			if (noise != 0.0)
			{
				value += static_cast<SInt32>(noise * 128.0);
				if (value < 0)
					value = 0;
				if (value > 255)
					value = 255;
			}
			*pSrc = static_cast<UInt8>(value);
			pSrc++;
        }
    }
}
