///////////////////////////////////////////////////////////////////////////////
// SonicText.h
// Copyright (c) 2004, Haohanit.  All rights reserved.
///////////////////////////////////////////////////////////////////////////////

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#ifndef __SonicText_h__
#define __SonicText_h__

#include "CommonTypes.h"
#include "CommonSTLTypes.h"
#include <map>
#include <ostream>
#include <vector>

class SonicText
{

public:

	typedef std::vector<unicode::string8> Args;

	enum Language
	{
		Language_ChineseSimplified,
		Language_ChineseTraditional,
		Language_Danish,
		Language_Dutch,
		Language_English,
		Language_Finnish,
		Language_French,
		Language_German,
		Language_Italian,
		Language_Japanese,
		Language_Korean,
		Language_Norwegian,
		Language_Portuguese,
		Language_PortugueseBrazil,
		Language_Spanish,
		Language_Swedish,
		Language_Default, // embedded English text
		Language_Total = Language_Default + 1,
	};

	struct LanguageInfo
	{
		char code[4]; // language code (Microsoft 3-character)
		char * name; // language name (in English)
	};

	class LanguageImpl
	{

	public:

		virtual Language GetLanguage() const;	
		virtual const LanguageInfo * GetLanguageInfo(Language language) const;
		virtual unicode::string8 GetText(SInt32 number) const;
		virtual void SetLanguage(Language language, const unicode::string8 & folder);

	private:

		friend class SonicText;
		LanguageImpl();
		LanguageImpl(const LanguageImpl &);
		const LanguageImpl operator =(const LanguageImpl &);

		static Language m_language;
		static LanguageInfo m_languageInfo[Language_Total];
		static std::map<SInt32, unicode::string8> m_map;
	};

	SonicText(SInt32 number, const char * text);
	SonicText(const SonicText & text);
	
	virtual ~SonicText();
	virtual SInt32 GetNumber() const throw ();
	virtual unicode::string8 GetText() const;
	virtual unicode::string8 GetText(const Args & args) const;

	static void Dump(std::basic_ostream<char> & out);
	static const SonicText * Find(SInt32 number);
	static Language GetLanguage();
	static LanguageImpl * GetLanguageImpl();
	static const LanguageInfo * GetLanguageInfo(Language language);
	static void SetLanguage(Language language, const unicode::string8 & folder);
	static void SetLanguageImpl(LanguageImpl * impl = 0);

protected:

	SonicText();
	SonicText & operator =(const SonicText & text);

	SInt32 m_number; // text number
	const char * m_text; // embedded English text
	SonicText * m_next; // next entry in list (this == not in list)

private:

	static SonicText * m_list; // head of list
	static SonicText::LanguageImpl m_local; // local language implementation
	static SonicText::LanguageImpl * m_impl; // current language implementation
};

#pragma warning(push)
#pragma warning(disable:4266)		// no override available for virtual member function from base class; function is hidden

class SonicTextArgs : public SonicText, public SonicText::Args
{

public:

	SonicTextArgs(const SonicText & text) : SonicText(text) {}
	unicode::string8 GetText() const { return SonicText::GetText(static_cast<const SonicText::Args &>(*this)); }
};

#pragma warning(pop)

class DebugText : public SonicText
{

public:

	explicit DebugText(const char * text) { m_text = text; }
};

#endif //__SonicText_h__
