///////////////////////////////////////////////////////////////////////////////
// SystemException.h
// Copyright (c) 2004, Haohanit.  All rights reserved.
///////////////////////////////////////////////////////////////////////////////

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#ifndef __SystemException_h__
#define __SystemException_h__

#include "SonicException.h"

class SystemException
{

public:

	class Errno : public SonicException
	{

	public:

		Errno(SInt32 error);
		virtual ~Errno() throw();
		virtual SonicException * Copy() const;
		SInt32 GetError() const;
		virtual const Errno & Note(const std::string & note) const;
		virtual void Throw() const;

		static const SonicText text;

	private:

		SInt32 m_error;
	};

#if defined _WIN32

	class Win32 : public SonicException
	{

	public:

		Win32(DWORD error);
		virtual ~Win32() throw();
		virtual SonicException * Copy() const;
		DWORD GetError() const;
		virtual const Win32 & Note(const std::string & note) const;
		virtual void Throw() const;

		static const SonicText text;

	private:

		DWORD m_error;
	};

	class HRESULT : public SonicException
	{

	public:

		HRESULT(::HRESULT error);
		virtual ~HRESULT() throw();
		virtual SonicException * Copy() const;
		::HRESULT GetError() const;
		virtual const HRESULT & Note(const std::string & note) const;
		virtual void Throw() const;

		static const SonicText text;

	private:

		::HRESULT m_error;
	};

	#if defined _INC_MMSYSTEM

	class MMRESULT : public SonicException
	{

	public:

		MMRESULT(::MMRESULT error);
		virtual ~MMRESULT() throw();
		virtual SonicException * Copy() const;
		::MMRESULT GetError() const;
		virtual const MMRESULT & Note(const std::string & note) const;
		virtual void Throw() const;

		static const SonicText text;

	private:

		::MMRESULT m_error;
	};

	#endif // _INC_MMSYSTEM

	#if defined _GDIPLUSTYPES_H

	class Gdiplus : public SonicException
	{

	public:

		Gdiplus(::Gdiplus::Status);
		virtual ~Gdiplus() throw();
		virtual SonicException * Copy() const;
		::Gdiplus::Status GetError() const;
		virtual const Gdiplus & Note(const std::string & note) const;
		virtual void Throw() const;

		static const SonicText text;

	private:

		::Gdiplus::Status m_error;
	};

	#endif // _GDIPLUSTYPES_H

	#if defined __MACTYPES__

	class OSErr : public SonicException
	{

	public:

		OSErr(::OSErr error);
		virtual ~OSErr() throw();
		virtual SonicException * Copy() const;
		::OSErr GetError() const;
		virtual const OSErr & Note(const std::string & note) const;
		virtual void Throw() const;

		static const SonicText text;

	private:

		::OSErr m_error;
	};

	#endif // __MACTYPES__

#endif // _WIN32

};

#endif //__SystemException_h__
