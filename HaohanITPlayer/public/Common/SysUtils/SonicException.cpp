///////////////////////////////////////////////////////////////////////////////
// SonicException.cpp
// Copyright (c) 2004, Haohanit.  All rights reserved.
///////////////////////////////////////////////////////////////////////////////

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#include "SonicException.h"
#include "SonicTextBase.h"
#include "UnicodeUtilities.h"
#include "sonic_crt.h"

const SonicText SonicException::allocError		(SonicTextBase::SonicExceptionText - 0, "memory allocation error");
const SonicText SonicException::debugError		(SonicTextBase::SonicExceptionText - 1, "internal software error: %0, line %1");
const SonicText SonicException::runtimeError	(SonicTextBase::SonicExceptionText - 2, "runtime error: %0");
const SonicText SonicException::unknownError	(SonicTextBase::SonicExceptionText - 3, "unknown error: %0");

SonicException::SonicException(
	const SonicText & text)
:	m_number(text.GetNumber()),
	m_text(text.GetText())
{
}

SonicException::SonicException(
	const char * file,
	int line)
:	m_number(debugError.GetNumber())
{
	char szLine[16];
	SonicText::Args args;
	sonic::snprintf_safe(szLine, sizeof(szLine), "%d", line);
	args.push_back(unicode::to_string8(file));
	args.push_back(unicode::to_string8(szLine));
	m_text = debugError.GetText(args);
}

SonicException::SonicException(
	const std::exception * exception) throw ()
{
	try
	{
		if (dynamic_cast<const SonicException *>(exception))
			*this = *dynamic_cast<const SonicException *>(exception);
		else if (dynamic_cast<const std::bad_alloc *>(exception))
			*this = SonicException(allocError);
		else
		{
			SonicTextArgs args(runtimeError);
			args.push_back(unicode::to_string8(exception->what()).c_str());
			*this = SonicException(args);
		}
	}
	catch (std::exception &)
	{
		m_number = allocError.GetNumber();
		m_text.clear();
	}
}

SonicException::SonicException(
	const SonicException & exception)
:	m_number(exception.GetNumber()),
	m_text(exception.GetText()),
	m_note(exception.GetNote())
{
}

SonicException & SonicException::operator =(
	const SonicException & exception)
{

#if defined(_MSC_VER) && defined(_DLL)

	// NOTE: MS implementation of std::exception::operator =() has slicing bug!
	
#else

	std::exception::operator =(exception);

#endif

	m_number = exception.GetNumber();
	m_text = exception.GetText();
	m_note = exception.GetNote();
	return *this;
}

SonicException::~SonicException() throw ()
{
}

SonicException * SonicException::Copy() const
{
	return new SonicException(*this);
}

SonicException SonicException::Get(
	SonicError error)
{
	const SonicText * text = SonicText::Find(error);

	if (text) return SonicException(*text);

	assert(false); // we should NEVER get here! figure out why and fix it!

	char szError[16];
	sonic::snprintf_safe(szError, sizeof(szError), "%ld", (long) error);
	SonicTextArgs args(unknownError);
	args.push_back(unicode::to_string8(szError));
	SonicException unknown(args);

	unknown.m_number = error;
	return unknown;
}

const std::string & SonicException::GetNote() const
{
	return m_note;
}

SInt32 SonicException::GetNumber() const throw ()
{
	return m_number;
}

const unicode::string8 & SonicException::GetText() const
{
	return m_text;
}

const SonicException & SonicException::Note(
	const std::string & note) const
{
	m_note = note;
	return *this;
}

void SonicException::Throw() const
{
	throw *this;
}

const char * SonicException::what() const throw ()
{
	try
	{
		m_what = unicode::to_string(m_text);
	}
	catch (std::exception &)
	{
		return "";
	}

	return m_what.c_str();
}

