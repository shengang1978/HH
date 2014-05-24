//-----------------------------------------------------------------------------
// secure_buffer.h
// Copyright (c) 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

#ifndef __secure_buffer_h__
#define __secure_buffer_h__

#define SECURE_BUFFER_CHECK(buffer_start, buffer_size, access_start, access_size) \
	if( \
		(access_start) < (buffer_start) \
		|| (char *)(access_start) + (access_size) >= (char *)(buffer_start) + (buffer_size)) \
		throw DebugException();

#endif // __secure_buffer_h__
