//-----------------------------------------------------------------------------
// pconfig.cpp
// Copyright (c) 2003 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS Needs Review [JAW  20040912] - Check buffers used for encrypt/decrypt, and file access for pconfig
//SR FS Reviewed [wwt 20040914]
//SR FS: Reviewed [JAW  20040912]
//SR FS Needs Review [wwt 20040914] - the encrypt/decrypt doesnt access data outside of its boundary. m_end_key's size is KEY_SIZE + 1.
//SR FS Needs Review [wwt 20040914] - RWPConfig::get_value is not a risky routine. it uses STL calls to avoid invalid memory access
//SR FS Reviewed [JAW 20040924] - Added comments for risky routines (non-STL get_value)

#if defined _WIN32
#include "pconfig.h"
#include "sonic_crt.h"


#pragma message(__FILE__" turn off depricated warning for this.. I don't want to touch it - hvh")
#pragma warning(push)
#pragma warning(disable:4996)

//
// NOTE: Pconfig should only be decrypted once for multiple reads.
//

//===========================================================================
// FUNCTION  : open()
//---------------------------------------------------------------------------
// PURPOSE   : decrypt a file to a new temp file, re-set data structure
//             contents if a new file is specified.
// ARGUMENTS : szFileName - new PConfig to decrypt.
// RETURNS   : boolean true if data structure is (re-)filled.
//===========================================================================
const bool RWPConfig::open(const char * szFileName)
{
    if (!file_exists(szFileName)) {
        m_bSourceMissing = true;
        return(false);
    }

    if (m_bOpen) close();

    m_szPConfigName = szFileName;
	    
	FILE * infile =	sonic::fopen(m_szPConfigName.c_str(), "rb");
    if (infile == NULL) return(false);
    m_bSourceMissing = false;


    //
    // Allocate a buffer and decrypt the file into it.
    //
    size_t stFileSize = file_size(m_szPConfigName.c_str());
    if (stFileSize > 32768L) {
        m_bSourceTooBig = true;
        return(false);
    }

    char * szDCBuf = (char *)::calloc(stFileSize + 1, sizeof(char));
    if (szDCBuf == NULL) {
        m_bSourceTooBig = true;
        return(false);
    }

    //
    // This next block performs the decryption
    //
    char   input_buffer[BUF_SIZE];
    char   output_buffer[BUF_SIZE];
    char * pCurPos = szDCBuf;
    size_t ib_count = 0;

    while ((ib_count = ::fread(input_buffer, ONE_BYTE, BUF_SIZE, infile)) != 0)
    {
        if ((pCurPos + ib_count) > (szDCBuf + stFileSize)) break;  // Let's not clobber memory.

        for (size_t ib_index = 0; ib_index < ib_count; ib_index++)
            output_buffer[ib_index] = decrypt(input_buffer[ib_index],
                                               reverse((unsigned char)ib_index));

        ::memmove(pCurPos, output_buffer, ib_count);
        pCurPos += ib_count;
    }
    ::fclose(infile);

    //
    // Parse through the de-crypted buffer and fill the PConfig data structure.
    //
    char * pBufEnd = pCurPos;
    pCurPos = szDCBuf;

    //
    // Replace all of the newlines (cr/lf pairs) with nulls (two each).
    //                            0x0D/0x0A = 10/13
    // Doing this now makes it easier to parse through the list later.
    // Two passes are easier than one.
    //
    while (pCurPos < pBufEnd)
    {
        while ((pCurPos < (pBufEnd - 1)) &&  // just past the last 0x0D.
               ((*pCurPos) != 13))
            pCurPos++;

        if (13 == (*pCurPos)) {
            *pCurPos = '\0';  // 0x0D
            pCurPos++;
            *pCurPos = '\0';  // 0x0A
            pCurPos++;
        }
        else pCurPos++;
    }

    pCurPos = szDCBuf;
    std::string strCurrentSection;
    std::string strCurrentLine;

    //
    // Fill up the data structure
    //
    while (pCurPos < pBufEnd)
    {
        strCurrentLine = pCurPos;
        trim_spaces(strCurrentLine);

        if ("" == strCurrentLine)
        {	//
            // Whitespace only.  Denotes end of section.
            // Comments can be part of a section.
            //
            strCurrentSection.erase();
        }

        //
        // Comment character = ';' - just like an INI file.
        // Note that all comments are lost during writing...
        //
        strCurrentLine = strCurrentLine.substr(0, strCurrentLine.find(';'));
        trim_spaces(strCurrentLine);

        if ("" == strCurrentLine)
        {	//
            // Comment line or whitespace only
            // move on to the next line
            //
            pCurPos += (strlen(pCurPos) + 2);  // two nulls after each string.
        }

        else
        {	//
            // Look for a section title
            //
            std::string::size_type nLBracketPos, nRBracketPos;

            if (0 == (nLBracketPos = strCurrentLine.find('[')))
            {	//
                // Probably have a section title here.
                // Let's make sure before changing the current section.
                //
                if (std::string::npos != (nRBracketPos = strCurrentLine.find(']')))
                {	//
                    // Yep, found a section title.
                    // Note that the line
                    // [bob]spam
                    // will equate to the section title "bob" and
                    // bob[spam]
                    // will equate as the lhs "bob[spam]" with an empty rhs
                    //
                    strCurrentSection = strCurrentLine.substr(nLBracketPos + 1,
                                                               nRBracketPos - 1 - nLBracketPos);

                    trim_spaces(strCurrentSection);

                    //
                    // assign a blank SectionData structure to this section title
                    //
                    m_PConfig[strCurrentSection];

                    //
                    // Move on to the next line
                    //
                    pCurPos += (strlen(pCurPos) + 2);  // two nulls after each string.
                } // end if really a section title
            }

            else
            {
                if (!strCurrentSection.empty())
                {	//
                    // This is section data.
                    //
                    std::string::size_type nEqualsPos = strCurrentLine.find('=');

                    std::string strLHS, strRHS;

                    strLHS = strCurrentLine.substr(0, nEqualsPos);
                    strRHS = "";

                    if (nEqualsPos != std::string::npos)
                        strRHS = strCurrentLine.substr(nEqualsPos + 1);  // the rest after the equals sign.

                    trim_spaces(strLHS);
                    trim_spaces(strRHS);

                    m_PConfig[strCurrentSection][strLHS] = strRHS;
                }// end if past the first section title.

                //
                // Move on to the next line
                //
                pCurPos += (strlen(pCurPos) + 2);  // two nulls after each string.
            } // end if maybe a section title
        } // end if a blank line
    }// end while in the buffer

    //
    // Finish up and set global boolean.
    //
    ::free(szDCBuf);

    m_bOpen     = true;
    m_bModified = false;

    if (file_readonly(m_szPConfigName.c_str())) {
        m_bWritable = false;
        m_bSourceReadOnly = true;
    }
    else m_bSourceReadOnly = false;

    return(true);
}


//===========================================================================
// FUNCTION  : close()
//---------------------------------------------------------------------------
// PURPOSE   : write out any changes and clear out the data structure.
// ARGUMENTS : none.
// RETURNS   : boolean true if data structure is cleared out.
//===========================================================================
const bool RWPConfig::close()
{
    if (m_bModified && m_bWritable) {
		bool bEncryptable = true;
        //
        // If the file has changed, re-encrypt the new data and
        // overwrite the old PConfig.
        //
        //FILE * outfile = ::fopen(m_szPConfigName.c_str(), "wb");
		FILE * outfile = sonic::fopen(m_szPConfigName.c_str(), "rb");
        if (outfile == NULL) bEncryptable = false;

		                                       // account for ending null in algorithm.
		size_t fileBufferSize = get_buffersize("") + 1;
		char * pFileBuffer = (char *)::calloc(fileBufferSize, sizeof(char));
		if (NULL == pFileBuffer) bEncryptable = false;

		if (bEncryptable) 
		{	//
			// Assemble a decrypted version of the file in memory.
			// Note that all comments were lost during reading...
			//
			char * pBufferIndex = pFileBuffer;

			for (INIFile::const_iterator PCGit = m_PConfig.begin();
				 PCGit != m_PConfig.end();
				 PCGit++)
			{	//
				// '[' section title string ']' '\r\n'
				//
				sonic::strncpy_safe(pBufferIndex, "[", fileBufferSize - (pBufferIndex-pFileBuffer));
				pBufferIndex++;
				sonic::strncpy_safe(pBufferIndex, (PCGit->first).c_str(), fileBufferSize - (pBufferIndex-pFileBuffer));
				pBufferIndex += (PCGit->first).length();

				sonic::strncpy_safe(pBufferIndex, "]\r\n", fileBufferSize - (pBufferIndex-pFileBuffer));
				pBufferIndex += 3;

				for (SectionData::const_iterator SDit = (PCGit->second).begin();
					 SDit != (PCGit->second).end();
					 SDit++)
				{	//
					// LHS string '=' RHS string '\r\n'
					//
					sonic::strncpy_safe(pBufferIndex, (SDit->first).c_str(), fileBufferSize - (pBufferIndex-pFileBuffer));
					pBufferIndex += (SDit->first).length();

					sonic::strncpy_safe(pBufferIndex, "=", fileBufferSize - (pBufferIndex-pFileBuffer));
					pBufferIndex++;

					sonic::strncpy_safe(pBufferIndex, (SDit->second).c_str(), fileBufferSize - (pBufferIndex-pFileBuffer));
					pBufferIndex += (SDit->second).length();

					sonic::strncpy_safe(pBufferIndex, "\r\n", fileBufferSize - (pBufferIndex-pFileBuffer));
					pBufferIndex += 2;
				}
			}

			//
			// This next block performs the encryption
			//
			char * pBufferEnd = pBufferIndex;
			pBufferIndex = pFileBuffer;

			while (pBufferIndex < pBufferEnd)
			{
				char output_buffer[BUF_SIZE];
				ptrdiff_t  ib_count = 0;

				if ((pBufferEnd - pBufferIndex) >= BUF_SIZE)
					ib_count = BUF_SIZE;

				else
					ib_count = pBufferEnd - pBufferIndex;


				for (ptrdiff_t ib_index=0; ib_index < ib_count; ib_index++)
					output_buffer[ib_index] = encrypt(*(pBufferIndex++),
						                               reverse((unsigned char)ib_index));

				::fwrite(output_buffer, ONE_BYTE, ib_count, outfile);
			}

		}

		::free(pFileBuffer);
        ::fclose(outfile);
        m_bModified = false;
    }

    //
    // Clean up and set global booleans.
    //
    m_PConfig.clear();

    m_szPConfigName   = "";
    m_bSourceReadOnly = false;
    m_bSourceMissing  = true;
	m_bSourceTooBig   = false;
    m_bOpen           = false;

    return(true);
}


//===========================================================================
// FUNCTION  : set_writable()
//---------------------------------------------------------------------------
// PURPOSE   : enable/disable PConfig writing features.  If the source is
//             read-only, this function will return false.
// ARGUMENTS : bWriteIt - writable or not.
// RETURNS   : boolean true if writablity affected.
//===========================================================================
const bool RWPConfig::set_writable(bool bWriteIt)  // r/w or read-only
{
    if (bWriteIt) {
        if (!file_readonly(m_szPConfigName.c_str()))  { // if I can write, do.
            m_bWritable = true;
            return(true);
        }

        else {
            m_bSourceReadOnly = true;
            m_bWritable = false;
            return(false);  // can't set writable.
        }
    }

    else {
        m_bWritable = false;
        return(true);      // can always set read-only.
    }
}


//===========================================================================
// FUNCTION  : get_section_names()
//---------------------------------------------------------------------------
// PURPOSE   : Generate a list of section names to parse through.
//             This function mimics the Windows API function
//             GetPrivateProfileSectionNames.  Please read the docs for that.
// ARGUMENTS : szBuffer  - memory buffer to fill with the section names.
//                         If szBuffer == NULL, returns the number of bytes
//                         required to hold the section names in the API's 
//                         format.
//             dwBufSize - size of szBuffer in bytes.
// RETURNS   : DWORD number of bytes copied to buffer or dwBufSize - 2.
//===========================================================================
const size_t RWPConfig::get_section_names(__out_bcount_z(dwBufSize) char * szBuffer, size_t dwBufSize) const
{	//
	// First see if there is enough memory supplied for the list of section names.
	//
	size_t dwNamesSize = get_buffersize("[]");

	if ((dwNamesSize > dwBufSize) ||               // specified buffer too small
		(0 == dwNamesSize))                         // nothing to copy
		return(dwBufSize - 2);                   // act like the GetPrivateProfileSectionNames API

	if (NULL == szBuffer)  // here's an extra feature for determining the required buffer size
		return(dwNamesSize);

	//
	// Fill up the buffer with the section names, formatted just like
	// the GetPrivateProfileSectionNames API would.
	//
	char * pBufferIndex = szBuffer;

	for (INIFile::const_iterator PCGit = m_PConfig.begin();
		 PCGit != m_PConfig.end();
		 PCGit++)
	{	//
        // section title string '\0'
		//
		sonic::strncpy_safe(pBufferIndex, (PCGit->first).c_str(), dwBufSize - (pBufferIndex - szBuffer));
		pBufferIndex += (PCGit->first).length() + 1;
	}

	//
	//  Add the final null.
	//
	*pBufferIndex = '\0';

	return(dwNamesSize);  // Just like API.
}


//===========================================================================
// FUNCTION  : get_first_section_name()
//---------------------------------------------------------------------------
// PURPOSE   : Return a reference to the first section name, a string.
// ARGUMENTS : None
// RETURNS   : The first section name in the list or a null string if the
//             list is empty.
//===========================================================================
const std::string& RWPConfig::get_first_section_name()
{
	if (m_PConfig.empty()) return(m_strNULL);

	m_iCurSection = 0;
	return(m_PConfig.begin()->first);  // INIFile iterators point to pair<string, SectionData>
}


//===========================================================================
// FUNCTION  : get_next_section_name()
//---------------------------------------------------------------------------
// PURPOSE   : Return a reference to the next section name, a string.
// ARGUMENTS : None
// RETURNS   : The next section name in the list or a null string if the
//             list is empty or at the end of the list.
//===========================================================================
const std::string& RWPConfig::get_next_section_name()
{
	if (m_PConfig.empty() || (m_iCurSection == m_PConfig.size()))  // if already at the end
		return(m_strNULL);

	m_iCurSection++;

	if (m_iCurSection == m_PConfig.size()) // if incremented into the end pointer.
		return(m_strNULL);                 // (just past the end of the list)
	else
	{
		INIFile::iterator itNextSec = m_PConfig.begin();
		for (size_t iIndex = 0; iIndex < m_iCurSection; iIndex++)
			itNextSec++;

		return(itNextSec->first);
	}
}


//===========================================================================
// FUNCTION  : get_previous_section_name()
//---------------------------------------------------------------------------
// PURPOSE   : Return a reference to the previous section name, a string.
// ARGUMENTS : None
// RETURNS   : The previous section name in the list or a null string if the
//             list is empty or already at the beginning of the list.
//===========================================================================
const std::string& RWPConfig::get_previous_section_name()
{
	if (m_PConfig.empty() || (m_iCurSection == 0)) // already at the beginning.
		return(m_strNULL);

	m_iCurSection--;

	INIFile::iterator itPrevSec = m_PConfig.begin();
	for (size_t iIndex = 0; iIndex < m_iCurSection; iIndex++)
		itPrevSec++;

	return(itPrevSec->first);
}


//===========================================================================
// FUNCTION  : get_last_section_name()
//---------------------------------------------------------------------------
// PURPOSE   : Return a reference to the last section name, a string.
// ARGUMENTS : None
// RETURNS   : The last section name in the list or a null string if the
//             list is empty.
//===========================================================================
const std::string& RWPConfig::get_last_section_name()
{
	if (m_PConfig.empty()) return(m_strNULL);

	m_iCurSection = m_PConfig.size() - 1;  // last element in list

	return(m_PConfig.rbegin()->first);
}


//===========================================================================
// FUNCTION  : get_section_names()
//---------------------------------------------------------------------------
// PURPOSE   : Return a set of strings containing the section names.
// ARGUMENTS : None
// RETURNS   : set<string>& of local static.
//===========================================================================
const StringSet& RWPConfig::get_section_names() const
{
	static StringSet ssSections;
	ssSections.clear();

	if (m_PConfig.empty()) return(ssSections);

	for (INIFile::const_iterator itSec = m_PConfig.begin(); itSec != m_PConfig.end(); itSec++)
		ssSections.insert(itSec->first);

	return(ssSections);
}


//===========================================================================
// FUNCTION  : get_first_value_name()
//---------------------------------------------------------------------------
// PURPOSE   : Return a reference to the first value of the current (or
//             specified) section.  If there is no current section, 
//             return a reference to m_strNULL.  Section name is case
//             sensitive.
// ARGUMENTS : Optional section to iterate through.
// RETURNS   : The first value name in the section or a null string if the
//             list is empty.
//===========================================================================
const std::string& RWPConfig::get_first_value_name(const std::string& strSection)
{
	if (((strSection == "") && m_strCurValueSection.empty()) ||  // no specified section
		m_PConfig.empty()                                  )   // empty list
		return(m_strNULL);

	if ((strSection != "") && (strSection != m_strCurValueSection))
	{   //
		// Look for the specified section in the PConfig structure.
		// If the section doesn't exist, return strNULL.  Otherwise, 
		// set the current section to this section and continue.
		//
		if (m_PConfig.find(strSection) == m_PConfig.end())
			return(m_strNULL);

		m_strCurValueSection = strSection;
	}

	m_iCurValuePair = 0;
	const SectionData& sdCurSec = get_section(m_strCurValueSection);

	if (sdCurSec.empty()) return(m_strNULL);  // empty section.
	else                   return((sdCurSec.begin())->first);
}


//===========================================================================
// FUNCTION  : get_next_value_name()
//---------------------------------------------------------------------------
// PURPOSE   : Return a reference to the next value of the current (or
//             specified) section.  If there is no current section, 
//             return a reference to m_strNULL.  Section name is case
//             sensitive.
// ARGUMENTS : Optional section to iterate through.
// RETURNS   : The next value name in the section or a null string if the
//             list is empty.  If a new section is specified, return the 
//             first value in the new section.
//===========================================================================
const std::string& RWPConfig::get_next_value_name(const std::string& strSection)
{
	if (((strSection == "") && m_strCurValueSection.empty()) ||  // no specified section
		m_PConfig.empty()                                  )   // empty list
		return(m_strNULL);

	if ((strSection != "") && (strSection != m_strCurValueSection))
	{   //
		// Look for the specified section in the PConfig structure.
		// If the section doesn't exist, return strNULL.  Otherwise, 
		// set the current section to this section and continue.
		//
		if (m_PConfig.find(strSection) == m_PConfig.end())
			return(m_strNULL);

		return(get_first_value_name(strSection));   // new section name, next == first.
	}

	const SectionData& sdCurSec = get_section(m_strCurValueSection);

	if (sdCurSec.empty()) return(m_strNULL); // empty section.

	if (m_iCurValuePair == sdCurSec.size()) // if at the end pointer.
		return(m_strNULL);                  // (at the end of the list, no next value)

	m_iCurValuePair++;

	if (m_iCurValuePair == sdCurSec.size()) // if incremented into the end pointer.
		return(m_strNULL);                  // (just past the end of the list)
	else
	{
		SectionData::const_iterator itNextVal = sdCurSec.begin();
		for (size_t iIndex = 0; iIndex < m_iCurValuePair; iIndex++)
			itNextVal++;

		return(itNextVal->first);
	}
}


//===========================================================================
// FUNCTION  : get_previous_value_name()
//---------------------------------------------------------------------------
// PURPOSE   : Return a reference to the previous value of the current (or
//             specified) section.  If there is no current section, 
//             return a reference to m_strNULL.  Section name is case
//             sensitive.
// ARGUMENTS : Optional section to iterate through.
// RETURNS   : The previous value name in the section or a null string if the
//             list is empty.  If a new section is specified, return the 
//             last value in the new section.
//===========================================================================
const std::string& RWPConfig::get_previous_value_name(const std::string& strSection)
{
	if (((strSection == "") && m_strCurValueSection.empty()) ||  // no specified section
		m_PConfig.empty()                                    ||  // empty list
		(m_iCurValuePair == 0)                             )   // already at beginning
		return(m_strNULL);

	if ((strSection != "") && (strSection != m_strCurValueSection))
	{   //
		// Look for the specified section in the PConfig structure.
		// If the section doesn't exist, return strNULL.  Otherwise, 
		// set the current section to this section and continue.
		//
		if (m_PConfig.find(strSection) == m_PConfig.end())
			return(m_strNULL);

		return(get_last_value_name(strSection));   // new section name, previous == last.
	}

	const SectionData& sdCurSec = get_section(m_strCurValueSection);

	if (sdCurSec.empty()) return(m_strNULL); // empty section.

	if (0 == m_iCurValuePair) return(m_strNULL); // already at beginning.

	m_iCurValuePair--;

	SectionData::const_iterator itPrevVal = sdCurSec.begin();
	for (size_t iIndex = 0; iIndex < m_iCurValuePair; iIndex++)
		itPrevVal++;

	return(itPrevVal->first);
}


//===========================================================================
// FUNCTION  : get_last_value_name()
//---------------------------------------------------------------------------
// PURPOSE   : Return a reference to the last value of the current (or
//             specified) section.  If there is no current section, 
//             return a reference to m_strNULL.  Section name is case
//             sensitive.
// ARGUMENTS : Optional section to iterate through.
// RETURNS   : The last value name in the section or a null string if the
//             list is empty.
//===========================================================================
const std::string& RWPConfig::get_last_value_name(const std::string& strSection)
{
	if (((strSection == "") && m_strCurValueSection.empty()) ||  // no specified section
		m_PConfig.empty()                                  )   // empty list.
		return(m_strNULL);

	if ((strSection != "") && (strSection != m_strCurValueSection))
	{   //
		// Look for the specified section in the PConfig structure.
		// If the section doesn't exist, return strNULL.  Otherwise, 
		// set the current section to this section and continue.
		//
		if (m_PConfig.find(strSection) == m_PConfig.end())
			return(m_strNULL);

		m_strCurValueSection = strSection;
	}

	const SectionData& sdCurSec = get_section(m_strCurValueSection);

	if (sdCurSec.empty())
	{
		m_iCurValuePair = 0;
		return(m_strNULL); // empty section.
	}

	else                   
	{
		m_iCurValuePair = sdCurSec.size() - 1; // last element in list
		return((sdCurSec.rbegin())->first);
	}
}


//===========================================================================
// FUNCTION  : get_value_names()
//---------------------------------------------------------------------------
// PURPOSE   : Return a set of strings containing the value names in a
//             particular section.
// ARGUMENTS : const string& the section name to parse.
// RETURNS   : set<string>& of local static.
//===========================================================================
const StringSet& RWPConfig::get_value_names(const std::string& strSection) const
{
	static StringSet ssValues;
	ssValues.clear();

	if (m_PConfig.empty() || strSection.empty()) return(ssValues);

	//
	// Get the specified section
	//
	const SectionData& sdSec = get_section(strSection);

	//
	// If the section isn't empty, add the value names to the list.
	//
	if (!sdSec.empty()) {
		for (SectionData::const_iterator itVal = sdSec.begin(); itVal != sdSec.end(); itVal++) {
			ssValues.insert(itVal->first);
		}
	}

	return(ssValues);
}


//===========================================================================
// FUNCTION  : section_exists()
//---------------------------------------------------------------------------
// PURPOSE   : Check to see if a section exists in the PConfig structure.
// ARGUMENTS : strSection -- Name of the section to look for.
// RETURNS   : Boolean true if the section is in the PConfig data structure.
//===========================================================================
const bool RWPConfig::section_exists(const std::string& strSection) const
{
	return(m_PConfig.find(strSection) != m_PConfig.end());
}


//===========================================================================
// FUNCTION  : get_section()
//---------------------------------------------------------------------------
// PURPOSE   : Fill a buffer with the section data.  Works just like Windows
//             API GetPrivateProfileSection except the section name is case
//             sensitive.
// ARGUMENTS : szSection -- Name of the section to fill the buffer with.
//             szBuffer  -- A buffer to fill the section with.
//             dwBufSize -- Size of the buffer.
// RETURNS   : Number of bytes copied into the buffer or the number of 
//             bytes required to return the data.
//===========================================================================
const size_t RWPConfig::get_section(const char * szSection,
                                    __out_bcount_z(dwBufSize) char *       szBuffer,
                                    size_t       dwBufSize) const
{
	size_t dwReqSize = get_buffersize(szSection);

	if ((0         == dwBufSize) || 
		(dwReqSize >  dwBufSize) || 
		(0         == dwReqSize)) 
		return(dwReqSize);  // leave buffer untouched.

	INIFile::const_iterator itSec = m_PConfig.find(std::string(szSection));

	if (itSec == m_PConfig.end())  // if section doesn't exist, return 0.
		return(0);

	char *              pIndex = szBuffer;
	const SectionData&  sdSec  = itSec->second;

	for (SectionData::const_iterator itValPair = sdSec.begin(); itValPair != sdSec.end(); itValPair++) {
		sonic::snprintf_safe(pIndex, dwBufSize - (pIndex - szBuffer), "%s=%s", (itValPair->first).c_str(), (itValPair->second).c_str());
		pIndex += (strlen(pIndex) + 1); // skip null character.
	}

	*pIndex = '\0';

	return(dwReqSize);
}


//===========================================================================
// FUNCTION  : get_section()
//---------------------------------------------------------------------------
// PURPOSE   : Overload for working with the native data structures.
//             The section name is case sensitive.
// ARGUMENTS : strSection -- Name of the section to return a reference to.
// RETURNS   : A const reference to the named section or the NULL section
//             if the specified one doesn't exist.
//===========================================================================
const RWPConfig::SectionData& RWPConfig::get_section(const std::string& strSection) const
{
	INIFile::const_iterator itSec = m_PConfig.find(strSection);

	if (itSec != m_PConfig.end()) return(itSec->second);
	else                           return(m_sectionNULL);   // default return value
}


//===========================================================================
// FUNCTION  : write_section()
//---------------------------------------------------------------------------
// PURPOSE   : Overload for working with the native data structures.
//             The section name is case sensitive.
//             Either modify an existing section or add a new section.
// ARGUMENTS : strSection    -- Name of the section to return a reference to.
//             sdSectionData -- Section data to add/replace existing.
// RETURNS   : true if the PConfig is writable.
//===========================================================================
const bool RWPConfig::write_section(const std::string& strSection, const SectionData& sdData)
{
	if (strSection.empty()) return(false);

    if (m_bWritable)
    {
        m_PConfig[strSection] = sdData;
        m_bModified = true;
    }

    return(m_bWritable);
}


//===========================================================================
// FUNCTION  : write_section()
//---------------------------------------------------------------------------
// PURPOSE   : Imitate the WritePrivateProfileSection API.
//             Either modify an existing section or add a new section.
// ARGUMENTS : szSection -- Name of the section to add/replace.
//             szData    -- Section data to add/replace existing.
// RETURNS   : true if the PConfig is writable.
//===========================================================================
const bool RWPConfig::write_section(const char * szSection, const char * szData)
{
	if ((NULL == szSection)  || 
		('\0' == *szSection) ||
		(NULL == szData)     ||
		('\0' == *szData)  )
		return(false);

	if (m_bWritable){
		const char * pData = szData;
		SectionData& sdSec = m_PConfig[std::string(szSection)]; // add the new section.

		while ('\0' != *pData) {
			std::string strCurrentLine = pData;

			std::string::size_type nEqualsPos = strCurrentLine.find('=');

			std::string strLHS(strCurrentLine.substr(0, nEqualsPos));
			std::string strRHS;

			if (nEqualsPos != std::string::npos)
				strRHS = strCurrentLine.substr(nEqualsPos + 1);  // the rest after the equals sign.

			trim_spaces(strLHS);
			trim_spaces(strRHS);

			sdSec[strLHS] = strRHS; // add the value.

			strLHS.erase();
			strRHS.erase();
			pData += (strlen(pData) + 1);  // one null after each string.

            m_bModified = true;
		}
	}
	return(m_bWritable);
}



//===========================================================================
// FUNCTION  : get_value()
//---------------------------------------------------------------------------
// PURPOSE   : Overload for working with the native data structures.
//             The section and value names are case sensitive.
//             Get the specified string data.
// ARGUMENTS : strSection   -- Name of the section to look in.
//             strValueName -- Name of the value to qeury.
// RETURNS   : Value data.
//===========================================================================
const std::string& RWPConfig::get_value(const std::string& strSection, const std::string& strValueName) const
{
	if (m_PConfig.empty()    || 
		strSection.empty()   || 
		strValueName.empty()) return(m_strNULL);

	INIFile::const_iterator itSec = m_PConfig.find(strSection);
	if (itSec == m_PConfig.end()) return(m_strNULL);

	const SectionData& sdSec = itSec->second;
	if (sdSec.empty()) return(m_strNULL);

	SectionData::const_iterator itVal = sdSec.find(strValueName);
	if (itVal == sdSec.end()) return(m_strNULL);
	else                       return(itVal->second);
}


//===========================================================================
// FUNCTION  : get_value()
//---------------------------------------------------------------------------
// PURPOSE   : Mimic the GetPrivateProfileString API.
//             Get the specified string data.
// ARGUMENTS : szSection   -- Name of the section to look in.
//             szValueName -- Name of the value to qeury.
//             szDefault   -- Default to copy into buffer if section/value
//                            doesn't exist.
//             szBuffer    -- Buffer to fill with value data (or default).
//             dwBufSize   -- Size of the buffer in bytes.
// RETURNS   : Number of bytes copied into szBuffer.  See API docs.
//             If the value exists but the buffer is NULL or not big enough,
//             This function will do nothing and return the required size.
// COMMENTS  : Some GetPrivateProfileString functionality is built into 
//             the other function members, like get_section_names().
//===========================================================================

//SR SC: T, I, D [JAW 20040924] - RWPConfig::get_value can cause buffer overflow if szBuffer is too small
//SR DI: DP10, E3, D2 [JAW 20040924] {dread_score: 5} 
//SR MS: M [DDT 20041004] dwDefLen check guarded threats, only ending NULL counting was missing. 
const size_t RWPConfig::get_value(const char * szSection,
                                  const char * szValueName,
                                  const char * szDefault,
                                  __out_bcount_z(dwBufSize) char *       szBuffer,
                                  const size_t dwBufSize) const
{
	if ((NULL == szSection)    || 
		('\0' == *szSection)   ||
		(NULL == szValueName)  ||
		('\0' == *szValueName)) return(0);

	size_t dwDefLen = strlen(szDefault);

	INIFile::const_iterator itSec = m_PConfig.find(std::string(szSection));
	if (itSec == m_PConfig.end())
	{	//
		// Section not found (or pconfig empty)
		//
		if ((NULL != szBuffer) && (dwBufSize > dwDefLen)) 
		{
			sonic::strncpy_safe(szBuffer, szDefault, dwBufSize);
			return(strlen(szBuffer));
		}
		else
			return(0);
	}
	else 
	{	//
		// Found the section, look up the value
		//
		const SectionData& sdSec = itSec->second;
		SectionData::const_iterator itVal = sdSec.find(std::string(szValueName));

		if (itVal == sdSec.end())
		{	//
			// Value not found or section empty
			//
			if ((NULL != szBuffer) && (dwBufSize > dwDefLen)) 
			{
				sonic::strncpy_safe(szBuffer, szDefault, dwBufSize);
				return(strlen(szBuffer));
			}
			else
				return(0);
		}
		else 
		{	//
			// Found the value!
			//
			if ((NULL != szBuffer) && (dwBufSize > (itVal->second).length())) 
			{
				sonic::strncpy_safe(szBuffer, (itVal->second).c_str(), dwBufSize);
				return(strlen(szBuffer));
			}
			else
				return((itVal->second).length());
		} // endif found value
	} // endif found section
}


//===========================================================================
// FUNCTION  : write_value()
//---------------------------------------------------------------------------
// PURPOSE   : Overload for working with the native data structures.
//             Either modify an existing value or add a new section/value.
// ARGUMENTS : strSection   -- Name of the section to modify/add.
//             strValueName -- Name of the value to modify/add.
//             strValueData -- Data to insert/update.
// RETURNS   : true if the PConfig is writable.
//===========================================================================
const bool RWPConfig::write_value(const std::string& strSection, const std::string& strValueName, const std::string& strValueData)
{
	if (strSection.empty() || strValueName.empty()) return(false);

    if (m_bWritable)
    {
        m_PConfig[strSection][strValueName] = strValueData;
        m_bModified = true;
    }

	return(m_bWritable);
}


//===========================================================================
// FUNCTION  : write_value()
//---------------------------------------------------------------------------
// PURPOSE   : Mimic the WritePrivateProfileString API.
//             Either modify an existing value or add a new section/value.
// ARGUMENTS : szSection   -- Name of the section to modify/add.
//             szValueName -- Name of the value to modify/add.
//             szValueData -- Data to insert/update.
// RETURNS   : true if the PConfig is writable.
//===========================================================================
const bool RWPConfig::write_value(const char * szSection, const char * szValueName, const char * szData)
{
	if ((NULL == szSection)    ||
		('\0' == *szSection)   ||
		(NULL == szValueName)  ||
		('\0' == *szValueName)) return(false);

	if (m_bWritable) 
	{
		if ((NULL == szData)) m_PConfig[std::string(szSection)][std::string(szValueName)] = std::string("");
		else                   m_PConfig[std::string(szSection)][std::string(szValueName)] = std::string(szData);
        m_bModified = true;
	}

	return(m_bWritable);
}


//
// Internal Implementation Code  (ie., private functions)
//

//===========================================================================
// FUNCTION  : file_exists()
//---------------------------------------------------------------------------
// PURPOSE   : determine if a file exists.
// ARGUMENTS : szFileName - file to look for.  If full path isn't supplied,
//                          the standard Windows search path is used
//                          (see MSDN docs for FindFirstFile for details).
// RETURNS   : boolean true if file is found.
//===========================================================================
bool RWPConfig::file_exists(const char * szFileName)
{
    WIN32_FIND_DATAA ProdFileData;

    HANDLE hProdFile = ::FindFirstFileA(szFileName, &ProdFileData);
    ::FindClose(hProdFile);

    return(hProdFile != INVALID_HANDLE_VALUE);
}


//===========================================================================
// FUNCTION  : file_size()
//---------------------------------------------------------------------------
// PURPOSE   : determine the size of a file.
// ARGUMENTS : szFileName - file to size up.  If full path isn't supplied,
//                          the standard Windows search path is used
//                          (see MSDN docs for FindFirstFile for details).
// RETURNS   : size of the file (up to the first 4Gb).
//===========================================================================
size_t RWPConfig::file_size(const char * szFileName)
{
	WIN32_FIND_DATAA FileData;

	HANDLE hFile = ::FindFirstFileA(szFileName, &FileData);
	::FindClose(hFile);

	return(FileData.nFileSizeLow);
}


//===========================================================================
// FUNCTION  : dir_exists(char *)
//---------------------------------------------------------------------------
// PURPOSE   : Verify the existence of a directory.
// ARGUMENTS : szDir - Directory Name to check.
// RETURNS   : boolean true if the directory exists.
//===========================================================================
bool RWPConfig::dir_exists(const char * szDir)
{
    WIN32_FIND_DATAA ProdFileData;

    HANDLE hProdFile = ::FindFirstFileA(szDir, &ProdFileData);
    ::FindClose(hProdFile);

    if (INVALID_HANDLE_VALUE == hProdFile)
        return(false);
    else
        return((ProdFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
}


//===========================================================================
// FUNCTION  : file_readonly()
//---------------------------------------------------------------------------
// PURPOSE   : determine if a file is read only.
// ARGUMENTS : szFileName - file to look for.  If full path isn't supplied,
//                          the standard Windows search path is used
//                          (see MSDN docs for FindFirstFile for details).
// RETURNS   : boolean true if file is found.
//===========================================================================
bool RWPConfig::file_readonly(const char * szFile)
{
    UInt32 dwFileAttributes = ::GetFileAttributesA(szFile);

    return((0xFFFFFFFF != dwFileAttributes) &&   // return value on failure.
            ((dwFileAttributes & FILE_ATTRIBUTE_READONLY) != 0));
}


//===========================================================================
// FUNCTION  : trim_spces()
//---------------------------------------------------------------------------
// PURPOSE   : trims the whitespace from both ends of a string. 
//             whitespace = ' ' and '\t'
// ARGUMENTS : strDeSpace - reference to the string that needs trimming
// RETURNS   : nothing.
//===========================================================================
void RWPConfig::trim_spaces(std::string& strDeSpace)
{
	std::string::size_type nFirstNonSpace = strDeSpace.find_first_not_of(" \t");
	std::string::size_type nLastNonSpace  = strDeSpace.find_last_not_of(" \t");

	if ((std::string::npos == nFirstNonSpace) || (std::string::npos == nLastNonSpace)) 
		strDeSpace = "";  // nothing but spaces or the null string.

	else
		strDeSpace = strDeSpace.substr(nFirstNonSpace, 
		                                nLastNonSpace - nFirstNonSpace + 1);
}


//===========================================================================
// FUNCTION  : get_buffersize()
//---------------------------------------------------------------------------
// PURPOSE   : Determine the buffer size required to contain the requested
//             data.  There are three numbers to calculate, the beffer size
//             required to contain all of the data in the PConfig structure,
//             the space required to contain all of the section titles, and
//             the space required to contain the data in a particular
//             section.
// ARGUMENTS : strSectionName - ""   returns the required space for all of
//                                   the data.  (Used for writing the new,
//                                   changed file to disk.)
//                            - "[]" returns the required space for all of
//                                   the section titles.  (Used in the
//                                   get_section function.)
//                            - <section name>  returns the required space
//                                   for the specified section or 0 if the
//                                   section doesn't exist.  (used in the
//                                   get_section_names function.)
// RETURNS   : The size, in bytes of the minimum buffer that could contain
//             the requested data.
//===========================================================================
size_t RWPConfig::get_buffersize(const std::string strSectionName) const
{
	size_t stBufferSize = 0;

	if ((strSectionName != "") &&   // only calculate this section's size
		(strSectionName != "[]"))   // used in get_section.
	{  
		INIFile::const_iterator PCGit = m_PConfig.find(strSectionName);
		if (PCGit == m_PConfig.end()) return(0);  // return 0 if the section doesn't exist.

		const SectionData& CurSection = PCGit->second;  // iterator points to a Section Object.

		if (CurSection.empty()) return(0);

		for (SectionData::const_iterator SDit = CurSection.begin();
		     SDit != CurSection.end();
			 SDit++) 
		{
			stBufferSize += SDit->first.length() + 1 + SDit->second.length() + 1;
			             // LHS string            '='  RHS string             '\0'
		}
		stBufferSize += 1;  // terminating null.
	}


	if (strSectionName == "[]")  // calculate the buffer required to hold
	{                             // all of the section titles.
		for (INIFile::const_iterator PCGit = m_PConfig.begin();
		     PCGit != m_PConfig.end();
			 PCGit++)
		{
			stBufferSize += PCGit->first.length() + 1;
                         // section title string   '\0'
		}

		stBufferSize += 1; // terminating null
	}


	if (strSectionName == "") // calculate the buffer required to hold the
	{                          // entire file (for output to disk).
		for (INIFile::const_iterator PCGit = m_PConfig.begin();
		     PCGit != m_PConfig.end();
			 PCGit++)
		{
			stBufferSize += 1 + PCGit->first.length() + 1 + 2;
			            // '['  section title string   ']' '\r\n'

			for (SectionData::const_iterator SDit = PCGit->second.begin();
			     SDit != PCGit->second.end();
				 SDit++)
			{
				stBufferSize += SDit->first.length() + 1 + SDit->second.length() + 2;
				             // LHS string            '='  RHS string             '\r\n'
			}
		}
	}
	return(stBufferSize);
}


//===========================================================================
// FUNCTION  : encrypt_key()
//---------------------------------------------------------------------------
// PURPOSE   : encrypt the key for use in encrypt and decrypt algorithms.
// ARGUMENTS : none.
// RETURNS   : nothing.
//===========================================================================
void RWPConfig::encrypt_key()
{
    // This block encrypts the encryption key

    const unsigned char start_key[KEY_SIZE+1] = "ABCDEFGH";

    // The main purpose for this is to achieve a greater distribution of
    // values in the encrypted file.

    for (UInt32 yek_index = 0; yek_index < KEY_SIZE; yek_index++)
    {
        m_end_key[yek_index] = 0;  /* initialize it */

        for (UInt32 key_index = 0; key_index < KEY_SIZE; key_index++)
        {
            unsigned char byte = start_key[key_index];

            byte = rotate(byte, ((key_index + yek_index) % KEY_SIZE));

            m_end_key[yek_index] = (byte + m_end_key[yek_index]) % 256;
        }
    }
}


//===========================================================================
// FUNCTION  : encrypt()
//---------------------------------------------------------------------------
// PURPOSE   : encrypt a byte.
// ARGUMENTS : input    - input byte to encrypt.
//             position - current buffer position.
// RETURNS   : encrypted byte.
//===========================================================================
unsigned char RWPConfig::encrypt(unsigned char input, ptrdiff_t position)
{
   size_t temp = input;

   for (size_t index=0; index<KEY_SIZE; index++)
      temp += m_end_key[index]*(position+1);

   return((unsigned char)temp);
}


/*===========================================================================*
 * FUNCTION :  decrypt()
 *===========================================================================*
 * PURPOSE  : decrypt a byte.
 * INPUT    : position - current buffer position
 * RETURNS  : decrypted byte
 *===========================================================================*/
unsigned char RWPConfig::decrypt(unsigned char input, ptrdiff_t position)
{
    size_t temp = input;

    for (ptrdiff_t index=KEY_SIZE-1; index>=0; index--)
        temp -= m_end_key[index]*(position+1);

   return((unsigned char)temp);
}


/*===========================================================================*
 * FUNCTION  : rotate()
 *===========================================================================*
 * ARGUMENTS : byte - shifts byte left & right by "bits" bits and ORs result
 *             bits - number of bits to shift byte.
 * RETURN    : shifted byte
 *===========================================================================*/
unsigned char RWPConfig::rotate(unsigned char byte, int bits)
{
	unsigned char ShiftBits = (unsigned char)bits;

    unsigned char temp1 = byte << ShiftBits;
    unsigned char temp2 = byte >> ShiftBits;

    return temp1 | temp2;
}


/*===========================================================================*
 * FUNCTION : reverse()
 *===========================================================================*
 * ARGUMENTS: byte - bit field to scramble
 * RETURNS  : scrambled byte
 *===========================================================================*/
unsigned char RWPConfig::reverse(unsigned char byte)                // function x
{
    bit_field_t *bf = (bit_field_t *)&byte;

    bit_field_t temp;
    temp.b7 = bf->b1;
    temp.b6 = bf->b3;
    temp.b5 = bf->b5;
    temp.b4 = bf->b7;
    temp.b3 = bf->b0;
    temp.b2 = bf->b2;
    temp.b1 = bf->b4;
    temp.b0 = bf->b6;

    unsigned char *cp = (unsigned char *)&temp;
    return(*cp);
}

#pragma warning(pop)

#endif // not defined _WIN32
// End of file pconfig.cpp
