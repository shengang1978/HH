//-----------------------------------------------------------------------------
// SonicMessage.h
// Copyright (c) 2001 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#ifndef __SonicMessage_h__
#define __SonicMessage_h__

#include "SonicText.h"

class SonicException;

#pragma warning(push)
#pragma warning(disable:4266)		// no override available for virtual member function from base class; function is hidden

class SonicMessage : public SonicText
{

public:

	SonicMessage();
	SonicMessage(const SonicText & text);
	SonicMessage(const SonicException & exception);
	unicode::string8 GetText() const;

private:

	unicode::string8 m_message;
};

#pragma warning(pop)

#endif // __SonicMessage_h__
