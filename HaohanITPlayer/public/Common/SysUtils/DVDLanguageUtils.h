//-----------------------------------------------------------------------------
// DVDLanguageUtils.h
// Copyright (c) 2001 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------
// DVDLanguageUtils.h
//
// Language list utilities functions
// 
// Created by Jason Yang
//
// History:
//			JY	5/24/01		Create
//////////////////////////////////////////////////////////////////////////

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#ifndef __DVDLanguageUtils__h__
#define __DVDLanguageUtils__h__

#include <string>
#include <vector>

struct	LanguageDesc
{
	UInt16			code;				// should be unique
	std::string		abbr_desc;			// 2 charactor code
	std::string		def_desc;
};

typedef std::vector<LanguageDesc*>	LanguageList;

namespace DVDLanguageUtils 
{
	const LanguageList&		GetLanguageList();

	UInt32					GetLanguageCount();

	const std::string&		GetDescByIndex(UInt32 index);
	const std::string&		GetAbbrDescByIndex(UInt32 index);

	// utilities to convert between codepage ->LangID/LocaleID
	UInt32					GetCodePageByLocaleID(UInt32 localeID);
	UInt32					GetCodePageByLangID(UInt16 LangID);
}

class lang_code_equal
{
    UInt16 m_code;
public:
// **CodeWizzard** - Informational:  More Effective C++ item 5  - Be wary of user-defined conversion function
    lang_code_equal(UInt16 code): m_code(code){};
    bool operator()(const LanguageDesc* langDesc)  {return langDesc->code == m_code;};
};

#endif
