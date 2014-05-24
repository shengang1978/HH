//-----------------------------------------------------------------------------
// pconfig.h
// Copyright (c) 2003 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#ifndef __PCONFIG_H
#define __PCONFIG_H

#pragma warning(disable : 4786 4503)    // loose the typical truncated symbol information warnings

#include "pconfigbase.h"

#include <windows.h>
#include <cstdio>

#include <utility>
#include <map>
#include <string>
#include <set>


#define KEY_SIZE         8
#define BUF_SIZE         256
#define ONE_BYTE         1

#define READ_WRITE       0
#define READ_ONLY        1

typedef std::set<std::string> StringSet;

//**********************************************
//   PCONFIG INFO RETRIEVAL RELATED CODE
//**********************************************
class RWPConfig : public RWPConfigBase
{
private:
	//
	// Make these case insensitive just like a Windows INI file....
	//
	struct case_insensitive_comp : std::binary_function<std::string, std::string, bool> {
		bool operator()(const std::string& _X, const std::string& _Y) const {
			return (::_stricmp(_X.c_str(), _Y.c_str()) < 0); 
		}
	};

public:
	typedef std::pair<std::string, std::string> ValuePair;
	typedef std::map<std::string, std::string, case_insensitive_comp> SectionData;

	typedef std::pair<std::string, SectionData> Section;
	typedef std::map<std::string, SectionData, case_insensitive_comp> INIFile;

    //
    // basic functionality
    //
    const bool open(const char *);                                              // existing PConfig file name. done.
	const bool open(const std::string& strName) { return(open(strName.c_str())); } // existing PConfig file name. done.

    const bool close(); // write out data and clear out object. done.

    //
    // expose some data members
    //
    const char * source_filename() const { return(m_szPConfigName.c_str()); }  //done
    const bool   is_open()         const { return(m_bOpen               ); }  //done
    const bool   is_modified()     const { return(m_bModified           ); }  //done
    const bool   source_readonly() const { return(m_bSourceReadOnly     ); }  //done
    const bool   source_missing()  const { return(m_bSourceMissing      ); }  //done
	const bool   source_toobig()   const { return(m_bSourceTooBig       ); }  //done
    const bool   is_writable()     const { return(m_bWritable           ); }  //done                                 

    const bool set_writable(bool);   // r/w or read-only                       //done

	//
	// Let's imitate the PrivateProfile API
	//
    const size_t get_section_names(__out_bcount_z(dwBufSize) char *       szBuffer,
                                   const size_t  dwBufSize) const;  //done

    const size_t get_section(const char * szSection,
                             __out_bcount_z(dwBufSize) char *       szBuffer,
                             const size_t  dwBufSize) const;        //done

    const bool write_section(const char * szSection,
                              const char * szData);                //done

    const size_t get_value(const char * szSection,
                           const char * szValueName,
                           const char * szDefault,
                           __out_bcount_z(dwBufSize) char *       szBuffer,
                           const size_t  dwBufSize) const;          //done

    const bool write_value(const char * szSection,
                            const char * szValueName,
                            const char * szData);                  //done

	//
	// Standard container interface using STL objects
	//
	const std::string&    get_first_section_name();                                     //done
	const std::string&    get_next_section_name();                                      //done
	const std::string&    get_previous_section_name();                                  //done
	const std::string&    get_last_section_name();                                      //done
    const StringSet& get_section_names() const;                                    //done

	const std::string&    get_first_value_name(const std::string& strSection = "");        //done
	const std::string&    get_next_value_name(const std::string& strSection = "");         //done
	const std::string&    get_previous_value_name(const std::string& strSection = "");     //done
	const std::string&    get_last_value_name(const std::string& strSection = "");         //done

	const StringSet& get_value_names(const std::string&) const;                       //done

	const bool section_exists(const std::string&) const;                              //done
	const bool section_exists(const char *  szSection) const { 
		return(section_exists(std::string(szSection))); 
	}                                                                              //done

	const SectionData& get_section(const std::string&) const;                         //done
	virtual const std::string& get_value(const std::string& strSection,
							       const std::string& strValueName) const;              //done

	const bool write_section(const std::string&, const SectionData&);                 //done
	const bool write_value(const std::string&, const std::string&, const std::string&);         //done

    //
    // constructors and such
    //
    RWPConfig(bool bReadWrite = false)
        :m_bOpen(false),
        m_bModified(false),
        m_bWritable(false),
        m_bSourceReadOnly(false),
        m_bSourceMissing(true),
		m_bSourceTooBig(false),
		m_iCurSection(0),
		m_iCurValuePair(0)
    {
        encrypt_key();

        set_writable(bReadWrite);
    }

    RWPConfig(const char * szFileName,  // PConfig file name
               bool  bReadWrite = false)
        :m_bOpen(false),
        m_bModified(false),
        m_bWritable(false),
        m_bSourceReadOnly(false),
        m_bSourceMissing(true),
		m_bSourceTooBig(false),
		m_iCurSection(0),
		m_iCurValuePair(0)
    {
        encrypt_key();

        m_bOpen = open(szFileName);
        set_writable(bReadWrite);
    }

    ~RWPConfig() { close(); }


private:
	RWPConfig(const RWPConfig&);					// Copy construction not allowed
	const RWPConfig& operator=(const RWPConfig&);	// Assignment is not allowed

    typedef struct {
        unsigned b0 : 1;
        unsigned b1 : 1;
        unsigned b2 : 1;
        unsigned b3 : 1;
        unsigned b4 : 1;
        unsigned b5 : 1;
        unsigned b6 : 1;
        unsigned b7 : 1;
    } bit_field_t;

    bool              m_bOpen;
    bool              m_bModified;
    bool              m_bWritable;
    bool              m_bSourceReadOnly;
    bool              m_bSourceMissing;
	bool              m_bSourceTooBig;
    std::string		  m_szPConfigName;
    unsigned char     m_end_key[ KEY_SIZE + 1 ];
	const std::string m_strNULL;                  // constant null string to return on errors.
	const SectionData m_sectionNULL;              // constant null section to return on errors.
	INIFile           m_PConfig;                  // main container.
	size_t            m_iCurSection;              // use with get first/get next section.
	std::string       m_strCurValueSection;       // use with get first/get next value.
	size_t            m_iCurValuePair;            // use with get first/get next value.


    bool          file_exists(const char *);
	size_t        file_size(const char *);
    bool          file_readonly(const char *);
    bool          dir_exists(const char *);
	void          trim_spaces(std::string&);
	size_t        get_buffersize(const std::string) const;
    void          encrypt_key();
    unsigned char encrypt(unsigned char, ptrdiff_t);
    unsigned char decrypt(unsigned char, ptrdiff_t);
    unsigned char reverse(unsigned char);
    unsigned char rotate(unsigned char, int);
};

#endif
