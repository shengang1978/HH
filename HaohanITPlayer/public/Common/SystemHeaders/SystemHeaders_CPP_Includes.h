//-----------------------------------------------------------------------------
// SytemHeaders_CPP_Includes.h
// Copyright (c) 2003 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

#ifndef __SytemHeaders_CPP_Includes__
#define __SytemHeaders_CPP_Includes__

#ifdef __cplusplus

#pragma warning(push)
#pragma warning(disable:4625)		// copy constructor could not be generated because a base class copy constructor is inaccessible
#pragma warning(disable:4702)		// unreachable code warnings in VC7 STL xtree/list implementation
#pragma warning(disable:4626)		// assignment operator could not be generated because a base class assignment operator is inaccessible
#pragma warning(disable:4100)		// Unreferenced formal parameter

// Standard C++ headers
#include <iosfwd>
#include <istream>
#include <ostream>
#include <sstream>
#include <cstddef>

// Support
#include <locale>
#include <exception>
#include <stdexcept>

// Strings
#include <string>

// Containers
#include <list>
#include <map>
#include <set>
#include <vector>
#include <stack>

#pragma warning(pop)

#endif

#endif //__SytemHeaders_CPP_Includes__
