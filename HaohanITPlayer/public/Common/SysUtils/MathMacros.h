//-----------------------------------------------------------------------------
// MathMacros.h
// Copyright (c) 1999 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#ifndef __MATHMACROS__
#define __MATHMACROS__

#include <cmath>

//common math macros since VC++ doesn't have them.
#ifndef ABS
#define	ABS(x)	((x) >= 0 ? (x) : (-(x)))
#endif

#ifndef MIN
#define MIN(a,b) ((a) > (b) ? (b) : (a))
#endif

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifndef ROUND
#define ROUND(a) (floor((a) + .5))
#endif

#ifndef RANDOMRANGE
#define	RANDOMRANGE(min, max)	(((max) > (min)) ? (rand() % ((max)-(min))) + (min) : (max))
#endif

#ifndef RANDOMPERCENT
#define	RANDOMPERCENT()	(static_cast<double>(rand()) / static_cast<double>(RAND_MAX))
#endif

#endif

