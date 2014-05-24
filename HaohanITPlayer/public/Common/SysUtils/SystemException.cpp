///////////////////////////////////////////////////////////////////////////////
// SystemException.cpp
// Copyright (c) 2004, Haohanit.  All rights reserved.
///////////////////////////////////////////////////////////////////////////////

//SR FS Needs Review [JAW  20040912] - check unusual use of format strings
//SR FS: Reviewed [JAW  20040913] - All formatting handled by SonicText in safe manner
//SR FS: Reviewed [wwt 20040914] 

#ifdef _WIN32

#ifndef STRIP_QUICKTIME
#include "MacTypes.h"
#endif

#include "WinGDIPlusHeaders.h"
#include <dxerr9.h>

#endif

#include "SystemException.h"
#include "SonicTextBase.h"
#include "UnicodeUtilities.h"
#include "sonic_crt.h"

const SonicText SystemException::Errno::text	(SonicTextBase::SonicExceptionText - 10, "operating system error: number = %0 - %1");

SystemException::Errno::Errno(
	SInt32 error)
:	SonicException(text),
	m_error(error)
{
	SonicText::Args args;
	char number[16];
	sonic::snprintf_safe(number, sizeof(number), "%d", error);
	args.push_back(unicode::to_string8(number));
	
#if defined(_WIN32) && _MSC_VER >= 1400

	char buffer[1024] = {0};
	strerror_s(buffer, sizeof(buffer), static_cast<int>(error));
	args.push_back(unicode::to_string8(buffer));

#else

	args.push_back(unicode::to_string8(strerror(static_cast<int>(error))));

#endif

	m_text = text.GetText(args);
}

SystemException::Errno::~Errno() throw()
{
}

SonicException * SystemException::Errno::Copy() const
{
	return new SystemException::Errno(*this);
}

SInt32 SystemException::Errno::GetError() const
{
	return m_error;
}

const SystemException::Errno & SystemException::Errno::Note(const std::string & note) const
{
	SonicException::Note(note);
	return *this;
}

void SystemException::Errno::Throw() const
{
	throw *this;
}

#if defined(_WIN32)

const SonicText SystemException::Win32::text	(SonicTextBase::SonicExceptionText - 11, "operating system error: number = %0 - %1");

SystemException::Win32::Win32(
	DWORD error)
:	SonicException(text),
	m_error(error)
{
	SonicText::Args args;
	char number[16];
	sonic::snprintf_safe(number, sizeof(number), "0x%08lX", (long) error);
	args.push_back(unicode::to_string8(number));

	wchar_t * buffer = NULL;
	FormatMessageW(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		error,
		0,
		reinterpret_cast<wchar_t *>(&buffer),
		0,
		NULL);

	// get error message and remove trailing CR/LF
	unicode::string8 msg = unicode::to_string8(buffer ? buffer : L"n/a");
	for (size_t i = msg.size(); i-- && (msg[i] == 0x0a || msg[i] == 0x0d); msg.erase(i));
	LocalFree(buffer);

	args.push_back(msg);
	m_text = text.GetText(args);
}

SystemException::Win32::~Win32() throw()
{
}

SonicException * SystemException::Win32::Copy() const
{
	return new SystemException::Win32(*this);
}

DWORD SystemException::Win32::GetError() const
{
	return m_error;
}

const SystemException::Win32 & SystemException::Win32::Note(const std::string & note) const
{
	SonicException::Note(note);
	return *this;
}

void SystemException::Win32::Throw() const
{
	throw *this;
}

const SonicText SystemException::Gdiplus::text	(SonicTextBase::SonicExceptionText - 12, "operating system error: Gdiplus::Status = %0");

SystemException::Gdiplus::Gdiplus(
	::Gdiplus::Status error)
:	SonicException(text),
	m_error(error)
{
	SonicText::Args args;
	char number[16];
	sonic::snprintf_safe(number, sizeof(number), "0x%08lX", (long) error);
	args.push_back(unicode::to_string8(number));

	m_text = text.GetText(args);
}

SystemException::Gdiplus::~Gdiplus() throw()
{
}

SonicException * SystemException::Gdiplus::Copy() const
{
	return new SystemException::Gdiplus(*this);
}

::Gdiplus::Status SystemException::Gdiplus::GetError() const
{
	return m_error;
}

const SystemException::Gdiplus & SystemException::Gdiplus::Note(const std::string & note) const
{
	SonicException::Note(note);
	return *this;
}

void SystemException::Gdiplus::Throw() const
{
	throw *this;
}

const SonicText SystemException::HRESULT::text	(SonicTextBase::SonicExceptionText - 13, "operating system error: HRESULT = %0 - %1");

SystemException::HRESULT::HRESULT(
	::HRESULT error)
:	SonicException(text),
	m_error(error)
{
	SonicText::Args args;
	char number[16];
	sonic::snprintf_safe(number, sizeof(number), "0x%08lX", (long) error);
	args.push_back(unicode::to_string8(number));

	wchar_t * buffer = NULL;
	FormatMessageW(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		error,
		0,
		reinterpret_cast<wchar_t *>(&buffer),
		0,
		NULL);

	// get error message and remove trailing CR/LF
	unicode::string8 msg = unicode::to_string8(buffer ? buffer : DXGetErrorDescription9W(error));
	for (size_t i = msg.size(); i-- && (msg[i] == 0x0a || msg[i] == 0x0d); msg.erase(i));
	LocalFree(buffer);

	args.push_back(msg);
	m_text = text.GetText(args);
}

SystemException::HRESULT::~HRESULT() throw()
{
}

SonicException * SystemException::HRESULT::Copy() const
{
	return new SystemException::HRESULT(*this);
}

::HRESULT SystemException::HRESULT::GetError() const
{
	return m_error;
}

const SystemException::HRESULT & SystemException::HRESULT::Note(const std::string & note) const
{
	SonicException::Note(note);
	return *this;
}

void SystemException::HRESULT::Throw() const
{
	throw *this;
}

#if defined _INC_MMSYSTEM

const SonicText SystemException::MMRESULT::text	(SonicTextBase::SonicExceptionText - 14, "operating system error: MMRESULT = %0");

SystemException::MMRESULT::MMRESULT(
	::MMRESULT error)
:	SonicException(text),
	m_error(error)
{
	SonicText::Args args;
	char number[16];
	sonic::snprintf_safe(number, sizeof(number), "0x%08lX", (long) error);
	args.push_back(unicode::to_string8(number));

	m_text = text.GetText(args);
}

SystemException::MMRESULT::~MMRESULT() throw()
{
}

SonicException * SystemException::MMRESULT::Copy() const
{
	return new SystemException::MMRESULT(*this);
}

::MMRESULT SystemException::MMRESULT::GetError() const
{
	return m_error;
}

const SystemException::MMRESULT & SystemException::MMRESULT::Note(const std::string & note) const
{
	SonicException::Note(note);
	return *this;
}

void SystemException::MMRESULT::Throw() const
{
	throw *this;
}

#endif	// _INC_MMSYSTEM

#ifdef __MACTYPES__

const SonicText SystemException::OSErr::text	(SonicTextBase::SonicExceptionText - 15, "QuickTime system error: number = %0");

SystemException::OSErr::OSErr(
	::OSErr error)
:	SonicException(text),
	m_error(error)
{
	SonicText::Args args;
	char number[16];
	sonic::snprintf_safe(number, sizeof(number), "%ld", (long) error);
	args.push_back(unicode::to_string8(number));

	m_text = text.GetText(args);
}

SystemException::OSErr::~OSErr() throw()
{
}

SonicException * SystemException::OSErr::Copy() const
{
	return new SystemException::OSErr(*this);
}

::OSErr SystemException::OSErr::GetError() const
{
	return m_error;
}

const SystemException::OSErr & SystemException::OSErr::Note(const std::string & note) const
{
	SonicException::Note(note);
	return *this;
}

void SystemException::OSErr::Throw() const
{
	throw *this;
}

#endif // __MACTYPES__

#endif // _WIN32
