///////////////////////////////////////////////////////////////////////////////
// SonicText.cpp
// Copyright (c) 2004, Haohanit.  All rights reserved.
///////////////////////////////////////////////////////////////////////////////

//SR FS Needs Review [JAW  20040912] - Check addressing of text list elements in constructor, use of formatted text
//SR FS Reviewed [wwt 20040914] - i think the 2nd construct wont cause invalid memory access
//SR FS: Reviewed [JAW  20040912]
//SR FS Needs Review [wwt 20040914] - i think the 2nd construct wont cause invalid memory access

#include "SonicText.h"
#include "safe_ptr.h"
#include "UnicodeUtilities.h"
#include <fstream>
#include <sstream>
#include "sonic_crt.h"

#ifdef __unix__
#include <sys/time.h>
#endif

#ifdef _MSC_VER

// BUG FIX: in fully-optimized release builds, the compiler attempts to link
// the following statically constructed object at the head of the list BEFORE
// invoking the actual constructor!  DO NOT remove this without first testing!
#pragma optimize("s", on) // turns off auto-inlining

#endif

SonicText * SonicText::m_list = 0;
SonicText::LanguageImpl SonicText::m_local;
SonicText::LanguageImpl * SonicText::m_impl = &SonicText::m_local;

static SonicText zero(0, ""); // zero is pre-defined and reserved

SonicText::SonicText()
{
	m_number = 0;
	m_text = "";
	m_next = this; // not a list entry
}

SonicText::SonicText(
	SInt32 number,
	const char * text)
{
	m_number = number;
	m_text = text;

	// treat list head as first entry to simplify list insertion logic
	SonicText * list = m_list;
	SonicText * prev = reinterpret_cast<SonicText *>(reinterpret_cast<UInt8 *>(&m_list) - offsetof(SonicText, m_next));
	for (;; prev = list, list = list->m_next)
		if (
			list == 0
			|| list->m_number > m_number)
		{
			m_next = list;
			prev->m_next = this;
			break;
		}
		else
			assert(list->m_number != m_number);
}

SonicText::SonicText(
	const SonicText & text)
:	m_number(text.m_number),
	m_text(text.m_text)
{
	m_number = text.m_number;
	m_text = text.m_text;
	m_next = this; // not a list entry
}

SonicText & SonicText::operator =(
	const SonicText & text)
{
	assert(m_next == this); // cannot assign to list entry
	m_number = text.m_number;
	m_text = text.m_text;
	m_next = this;
	return *this;
}

SonicText::~SonicText()
{
	if (m_next != this)
	{
		SonicText * list = reinterpret_cast<SonicText *>(reinterpret_cast<UInt8 *>(&m_list) - offsetof(SonicText, m_next));
		for (; list->m_next != this; list = list->m_next);
		list->m_next = m_next;
	}
}

void SonicText::Dump(
	std::basic_ostream<char> & out)
{
	std::ios_base::fmtflags flags = out.flags(std::ios_base::dec | std::ios_base::right);

	const LanguageInfo * info = GetLanguageInfo(GetLanguage());
	out << "\xef\xbb\xbf";  // UTF-8 BOM
	out << "SonicText::Dump() - " << info->code << " - " << info->name << std::endl;
		
	for (SonicText * list = m_list; list; list = list->m_next)
	{
		out.width(11);
		unicode::string8 text8 = m_impl->GetText(list->m_number);
		const char * text = reinterpret_cast<const char *>(text8.c_str());
		out	<< static_cast<int>(list->m_number) << ": " << (*text ? text : "<NOT FOUND>") << std::endl;
	}

	out.flags(flags);
}

const SonicText * SonicText::Find(
	SInt32 number)
{
	for (SonicText * list = m_list; list; list = list->m_next)
		if (list->m_number >= number)
			if (list->m_number == number)
				return list;
			else
				break;

	return 0;
}

SonicText::LanguageImpl * SonicText::GetLanguageImpl()
{
	return m_impl;
}

SonicText::Language SonicText::GetLanguage()
{
	return m_impl->GetLanguage();
}

const SonicText::LanguageInfo * SonicText::GetLanguageInfo(
	Language language)
{
	return m_impl->GetLanguageInfo(language);
}

SInt32 SonicText::GetNumber() const throw ()
{
	return m_number;
}

unicode::string8 SonicText::GetText() const
{
	unicode::string8 text = m_impl->GetText(m_number);
	return
		text.size()
			? text
			: unicode::to_string8(m_text);
}

unicode::string8 SonicText::GetText(
	const Args & args) const
{
	unicode::string8 text = SonicText::GetText();
	unicode::string8::size_type off = 0;

	for (size_t i = 0; i < args.size(); i++)
	{
		char tokenbuf[16];
		sonic::snprintf_safe(tokenbuf, sizeof(tokenbuf), "%%%d", i);
		unicode::string8 token = unicode::to_string8(tokenbuf);
		for (
			unicode::string8::size_type pos = text.find(token, off);
			pos != unicode::string8::npos;
			pos = text.find(token, off))
		{
			text.replace(pos, token.size(), args[i]);
			off = pos + args[i].size();
		}
	}

	return text;
}

void SonicText::SetLanguage(
	Language language,
	const unicode::string8 & folder)
{
	m_impl->SetLanguage(language, folder);
}

void SonicText::SetLanguageImpl(
	SonicText::LanguageImpl * impl)
{
	m_impl = impl ? impl : &m_local;
}

std::map<SInt32, unicode::string8> SonicText::LanguageImpl::m_map;
SonicText::Language SonicText::LanguageImpl::m_language = SonicText::Language_Default;
SonicText::LanguageInfo SonicText::LanguageImpl::m_languageInfo[Language_Total] =
	{
		{"CHS", "Chinese (Simplified)"},
		{"CHT", "Chinese (Traditional)"},
		{"DAN", "Danish"},
		{"NLD", "Dutch"},
		{"ENU", "English"},
		{"FIN", "Finnish"},
		{"FRA", "French"},
		{"DEU", "German"},
		{"ITA", "Italian"},
		{"JPN", "Japanese"},
		{"KOR", "Korean"},
		{"NOR", "Norweigian"},
		{"PTG", "Portuguese"},
		{"PTB", "Portuguese (Brazil)"},
		{"ESN", "Spanish"},
		{"SVE", "Swedish"},
		{"ENU", "Default (Embedded English)"}
	};

SonicText::LanguageImpl::LanguageImpl()
{
} 

SonicText::Language SonicText::LanguageImpl::GetLanguage() const
{
	return m_language;
}

const SonicText::LanguageInfo * SonicText::LanguageImpl::GetLanguageInfo(
	Language language) const
{
	assert(language >= 0 && language < Language_Total);
	return &m_languageInfo[language];
}

unicode::string8 SonicText::LanguageImpl::GetText(
	SInt32 number) const
{
	return m_map[number];
}

void SonicText::LanguageImpl::SetLanguage(
	SonicText::Language language,
	const unicode::string8 & folder)
{
	assert(language >= 0 && language < Language_Total);
	if (language == m_language)
		return;

	m_map.clear();
	m_language = Language_Default;
	if (language == Language_Default)
		return;

#ifdef _WIN32

	std::wstring path =
		unicode::to_wstring(folder)
		+ unicode::to_wstring("\\")
		+ unicode::to_wstring(m_languageInfo[language].code)
		+ unicode::to_wstring(".txt");

	// NOTE: wfopen() cannot be supported on Win9x systems since we are using
	// the .DLL version of the CRT, which isn't compiled to use the MS Layer
	// for Unicode (unicows.dll)

	FILE * file =
		(GetVersion() & 0x80000000)
			? sonic::fopen(unicode::to_string(path).c_str(), "rb")
			: sonic::wfopen(path.c_str(), L"rb");

#else

	std::string path =
		unicode::to_string(folder)
		+ std::string("/")
		+ std::string(m_languageInfo[language].code)
		+ std::string(".txt");

	FILE * file = fopen(path.c_str(), "rb");

#endif

	if (file == 0) return;

#if !defined(__bcm__)
	// load the file and determine UTF format
	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	safe_ptr_array<unicode::char8> data = new unicode::char8[size + 1]; 
	unicode::char8 * p = data.get();
	fseek(file, 0, SEEK_SET);
	fread(p, sizeof(unicode::char8), size, file);
	fclose(file);

	if (
		size >= 3 // size of UTF-8 signature
		&& memcmp(p, "\xef\xbb\xbf", 3) == 0) // UTF-8?
	{
		p += 3;
		size -= 3;
	}
	else
	{
		assert(size >= 2); // UTF-16 BOM required
		if (memcmp(p, "\xfe\xff", 2) == 0)
			// convert UTF-16BE to UTF-16LE
			for (size_t i = 0; i < size - 1; i += 2)
				std::swap(p[i], p[i + 1]);

		// convert UTF-16LE to UTF-8
		assert(memcmp(p, "\xff\xfe", 2) == 0); // UTF-16LE required
		unicode::string16 utf16 = unicode::string16(reinterpret_cast<unicode::char16 *>(p + 2), (size - 1) / 2);
		unicode::string8 utf8 = unicode::to_string8(utf16);

		// copy UTF-8 to data buffer
		data.reset();
		size = utf8.size();
		data = new unicode::char8[size + 1];
		p = data.get();
		memcpy(p, utf8.c_str(), size);
	}

	// extract text from data buffer
	unicode::char8 * eod = p + size;
	while (p < eod)
	{
		while ((*p == 0x0a || *p == 0x0d) && p < eod) p++; // find start of line
		if (p < eod)
		{
			unicode::char8 * start = p; // save start of line
			while ((*p != 0x0a && *p != 0x0d) && p < eod) p++; // find end of line
			*p++ = 0; // mark end of line

			std::basic_istringstream<unicode::char8> line(start);
			if (
				line.str().empty() // empty line?
				|| line.peek() == ';' // comment?
				|| line.peek() == '#') // comment?
				continue; // ignore line

			// extract text number
			SInt32 number = 0;
			line.unsetf(std::ios_base::skipws);
			line >> number;

			if (
				number == 0
				&& line.peek() == 'x')
			{
				line.unget();
				line.setf(std::ios_base::hex, std::ios_base::basefield);
				line >> number;
			}

			if (number == 0)
			{
				assert(false); // invalid line
				continue;
			}

			// skip whitespace trailing text number
			unicode::char8 c;
			while (line.get(c))
				if (!isspace(c))
				{
					line.putback(c);
					break;
				}

			// extract remainder of line
			unicode::string8 text;
			getline(line, text);

			// trim trailing whitespace
			while (
				text.size() > 0
				&& isspace(*text.rbegin()))
				text.resize(text.size() - 1);

			assert(m_map[number].empty()); // duplicate number - please fix!
			m_map[number] = text;
		}
	}

	m_language = language;
#endif
}
