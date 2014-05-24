//-----------------------------------------------------------------------------
// PerlinNoise.h
// Copyright (c) 2002 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#ifndef PERLIN_NOISE
#define PERLIN_NOISE

#include "CommonTypes.h"
#include "safe_ptr.h"

class PerlinNoiseData
// **CodeWizzard** - Violation:  Effective C++ item 20  - Avoid data members in the public interfac
// **CodeWizzard** - Violation:  Universal Coding Standards item 16  - Do not specify public data member
{
public:
	double mXScalar;
	double mYScalar;
	double mAlpha;
	double mBeta;
	UInt32 mNumHarmonics;

	bool mbTiled;
	bool mbSumAbsolutes;
	bool mbPhaseShift;

	PerlinNoiseData()
		: mXScalar(1.0), mYScalar(1.0), mAlpha(2.0), mBeta(2.0), mNumHarmonics(6), mbTiled(false), mbSumAbsolutes(false), mbPhaseShift(false)
	{

	}

	PerlinNoiseData(double xScalar, double yScalar, double alpha, double beta, UInt32 numHarmonics,
		bool tiled = false, bool sumAbsolutes = false, bool phaseShift = false)
		: mXScalar(xScalar), mYScalar(yScalar), mAlpha(alpha), mBeta(beta), mNumHarmonics(numHarmonics),
		mbTiled(tiled), mbSumAbsolutes(sumAbsolutes), mbPhaseShift(phaseShift)
	{

	}
};

class PerlinNoise
{
private:
	// Private data
	#define kPerlinNoiseWidth 0x100
	#define kPerlinNoiseMask 0xff
	#define N 0x1000
	#define NP 12   /* 2^N */
	#define NM 0xfff

	bool mIsInitialized;

	SInt32 mPermutations[kPerlinNoiseWidth + kPerlinNoiseWidth + 2];
	double m3DGradient[kPerlinNoiseWidth + kPerlinNoiseWidth + 2][3];
	double m2DGradient[kPerlinNoiseWidth + kPerlinNoiseWidth + 2][2];
	double m1DGradient[kPerlinNoiseWidth + kPerlinNoiseWidth + 2];

	safe_ptr_array<UInt8> mpNoiseMap;

	// Private functions
	inline double s_curve(double t)						{ return (t * t * (3. - 2. * t)); }
	inline double lerp(double t, double a, double b)	{ return (a + t * (b - a)); }

	inline void setup(const size_t i, const double *vec, SInt32& b0, SInt32& b1, double& r0, double& r1, double& t)
	{
		t = vec[i] + N;
		b0 = static_cast<SInt32>(t) & kPerlinNoiseMask;
		b1 = (b0 + 1) & kPerlinNoiseMask;
		r0 = t - static_cast<SInt32>(t);
		r1 = r0 - 1.;
	}

	inline double at2(double *q, const double rx, const double ry) { return (rx * q[0] + ry * q[1]); }
	inline double at3(double *q, const double rx, const double ry, const double rz) { return (rx * q[0] + ry * q[1] + rz * q[2]); }

	void Normalize2(double *);
	void Normalize3(double *);

	double Noise1(double arg);
	double Noise2(double *);
	double Noise3(double *);

	double PerlinNoise1D(double x, double alpha, double beta, SInt32 n, bool sumAbsolutes);
	double PerlinNoise2D(double x, double y, double alpha, double beta, SInt32 n, bool sumAbsolutes);
	double PerlinNoise3D(double x, double y, double z, double alpha, double beta, SInt32 n, bool sumAbsolutes);

public:
// **CodeWizzard** - Informational:  More Effective C++ item 5  - Be wary of user-defined conversion function
	PerlinNoise(UInt32 seed = 0);
	void Initialize(UInt32 seed = 0);

	inline UInt32 GetNoiseMapWidth() { return(kPerlinNoiseWidth); }
	inline UInt32 GetNoiseMapHeight() { return(kPerlinNoiseWidth); }

	inline double Get1DNoise(UInt32 arg)
	{
		arg = arg % GetNoiseMapWidth();
		double value = 0.0;
		if (mpNoiseMap.get())
		{
			value = static_cast<double>(mpNoiseMap[arg] - 128) / 128.0;
		}
		return (value);
	}
	void Create1DNoiseMap(const PerlinNoiseData& params);

	inline double Get2DNoise(UInt32 x, UInt32 y)
	{
		y += x / GetNoiseMapWidth();	// wrap around to next row
		x = x % GetNoiseMapWidth();
		y = y % GetNoiseMapHeight();
		double value = 0.0;
		if (mpNoiseMap.get())
		{
			value = static_cast<double>(mpNoiseMap[(y * GetNoiseMapWidth()) + x] - 128) / 128.0;
		}
		return (value);
	}
	void Create2DNoiseMap(const PerlinNoiseData& params);
};

#endif
