///////////////////////////////////////////////////////////////////////////////
// SonicMessage.cpp
// Copyright (c) 2004, Haohanit.  All rights reserved.
///////////////////////////////////////////////////////////////////////////////

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#include "SonicMessage.h"
#include "SonicException.h"

SonicMessage::SonicMessage()
{
}

SonicMessage::SonicMessage(
	const SonicText & text)
:	SonicText(text),
	m_message(text.GetText())
{
}

SonicMessage::SonicMessage(
	const SonicException & exception)
:	m_message(exception.GetText())
{
	m_number = exception.GetNumber();
}

unicode::string8 SonicMessage::GetText() const
{
	return m_message;
}
