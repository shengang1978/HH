//-----------------------------------------------------------------------------
// ATLHeaders.h
// Copyright (c) 2003 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

#ifndef ATLHeaders_H
#define ATLHeaders_H

#ifdef _WIN32

// Turn off warning that occur in directshow headers
//_atl_all_warnings

#pragma warning(push)
#pragma warning(disable:4265;disable:4505)
#include <atlbase.h>
#pragma warning(pop)


#endif

#endif
