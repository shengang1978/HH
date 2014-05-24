//-----------------------------------------------------------------------------
// DVDLanguageUtils.cpp
// Copyright (c) 1997 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------
// DVDLanguageUtils.cpp

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040915]  MBCS in used and used internally

#include <algorithm>
//#include "const.h"
#include "DVDLanguageUtils.h"
#include "LanguagesCountries.h"
#include "UnicodeUtilities.h"


// the class actually hold the font list
// should never expose to the user.
class CLangListImpl
// **CodeWizzard** - Severe Violation:  Universal Coding Standards item 30  - Do not directly access global data from a constructo
{
public:
	CLangListImpl();
	~CLangListImpl();

	const LanguageList& GetList()	{return m_langList;}

private:
	CLangListImpl(const CLangListImpl& in);
	CLangListImpl& operator = (const CLangListImpl& inOriginal);

	LanguageList  m_langList;

};

static std::string	nilString = "";

///////////////////////////////////////////////////////////////////////////////////////
CLangListImpl::CLangListImpl()
{
	// init the static language size list
	m_langList.clear();
	const LangInternDesc * pLang = &sLanguageDescList[0];

	while (pLang && strlen(pLang->abbr_desc))
	{
		LanguageDesc * pNewDesc = new LanguageDesc;
//		pNewDesc->localeID = pLang->localeID;
		pNewDesc->abbr_desc = pLang->abbr_desc;
		pNewDesc->def_desc = pLang->def_desc;
		pNewDesc->code = ((pNewDesc->abbr_desc[0] << 8) | pNewDesc->abbr_desc[1]);
// **Prefix** - not a problem, STL container (sjs) - dvdlanguageutils.cpp; Line: 48; warning (11): dereferencing NULL pointer 'm_langList->_Myfirst'
		m_langList.push_back(pNewDesc);
		pNewDesc++;
		pLang++;
	}
}

CLangListImpl::~CLangListImpl()
{
	// clear up the memory
    for (LanguageList::iterator it = m_langList.begin(); it != m_langList.end(); ++ it)
    {
        LanguageDesc* pDesc = *it;
        delete pDesc;
    }
    m_langList.clear ();
}



// safe return the static FontList
const LanguageList& DVDLanguageUtils::GetLanguageList()
{
	static CLangListImpl	langList;
	return langList.GetList();
}


UInt32 DVDLanguageUtils::GetLanguageCount()
{
	return static_cast<UInt32>(GetLanguageList().size());
}

const std::string&	DVDLanguageUtils::GetDescByIndex(UInt32 index)
{
	return GetLanguageList().at(index)->def_desc;
}

const std::string&	DVDLanguageUtils::GetAbbrDescByIndex(UInt32 index)
{
	return GetLanguageList().at(index)->abbr_desc;
}
#if 0
UInt32	DVDLanguageUtils::GetLocaleIDByIndex(UInt32 index)
{
	return GetLanguageList().at(index)->localeID;
}

const std::string&		DVDLanguageUtils::GetDescByCode(UInt16 code)
{
	const LanguageList& langList = GetLanguageList();
	LanguageList::const_iterator pFind = std::find_if(langList.begin(), langList.end(), lang_code_equal(code));
	if (pFind == langList.end())
		return nilString;
	else
		return (*pFind)->def_desc;
}

const std::string&		DVDLanguageUtils::GetAbbrDescByCode(UInt16 code)
{
	const LanguageList& langList = GetLanguageList();
	LanguageList::const_iterator pFind = std::find_if(langList.begin(), langList.end(), lang_code_equal(code));
	if (pFind == langList.end())
		return nilString;
	else
		return (*pFind)->abbr_desc;
}

UInt32	DVDLanguageUtils::GetLocaleIDByCode(UInt16 code)
{
	const LanguageList& langList = GetLanguageList();
	LanguageList::const_iterator pFind = std::find_if(langList.begin(), langList.end(), lang_code_equal(code));
	if (pFind == langList.end())
		return 1252;	//GetACP();
	else
		return (*pFind)->localeID;
}
#endif

UInt32	DVDLanguageUtils::GetCodePageByLocaleID(UInt32 localeID)
{
#ifdef _WIN32
	char szCP[7];
	if (localeID == 0)
		localeID = GetUserDefaultLCID();
	GetLocaleInfoA(localeID, LOCALE_IDEFAULTANSICODEPAGE, szCP, 7);
	return ::atoi(szCP);
#else
	localeID;
	return 0;
#endif
}

UInt32	DVDLanguageUtils::GetCodePageByLangID(UInt16 langID)
{
#ifdef _WIN32
	if (langID == 0)
		langID = GetUserDefaultLangID();
	UInt32 localeID = MAKELCID(langID, SORT_DEFAULT);
	return GetCodePageByLocaleID(localeID);
#else
	langID;
	return 0;
#endif
}
