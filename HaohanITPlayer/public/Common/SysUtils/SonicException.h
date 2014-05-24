///////////////////////////////////////////////////////////////////////////////
// SonicException.h
// Copyright (c) 2004, Haohanit.  All rights reserved.
///////////////////////////////////////////////////////////////////////////////

//SR FS: Reviewed [wwt 20040914] 
//SR FS: Reviewed [DDT 20040928] Second pass.

#ifndef __SonicException_h__
#define __SonicException_h__

#include "SonicText.h"
#include <exception>

class SonicException : public std::exception
{

public:

	SonicException(const SonicText & text);
	SonicException(const char * file, int line);
	SonicException(const std::exception * exception) throw ();
	SonicException(const SonicException & exception);
	SonicException & operator =(const SonicException & exception);
	virtual ~SonicException() throw();
	virtual SonicException * Copy() const;
	virtual const std::string & GetNote() const;
	virtual SInt32 GetNumber() const throw ();
	virtual const unicode::string8 & GetText() const;
	virtual const SonicException & Note(const std::string & note) const;
	virtual void Throw() const;
	virtual const char * what() const throw();

	inline SonicException * copy() const { return Copy(); } // for copy_ptr

	static const SonicText allocError; // std::bad_alloc
	static const SonicText debugError; // DebugException()
	static const SonicText runtimeError; // std::exception
	static const SonicText unknownError; // Get() failure

protected:

	SInt32 m_number;
	unicode::string8 m_text;
	mutable std::string m_note;
	mutable std::string m_what;

private:

	// DO NOT ADD NEW CLASSES HERE!
	friend class dvd_audio_stream;
	friend class ESDataSource;
	friend class MenuCoreMacrosException;
	friend class MyDVDASException;
	friend class PluginCoder_Exception;
	friend class PrimoException;
	friend class rt_stream;
	friend class SlideshowMacrosException;
	friend class SonicLibraryAssetActionsException;
	friend class VidEditMacrosException;
	static SonicException Get(SonicError error);
};

#define DebugException() SonicException(__FILE__, __LINE__)

#endif //__SonicException_h__
