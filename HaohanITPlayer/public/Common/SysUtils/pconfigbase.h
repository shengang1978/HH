//-----------------------------------------------------------------------------
// pconfigbase.h
// Copyright (c) 2003 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#ifndef __PCONFIGBASE_H
#define __PCONFIGBASE_H

#include <string>

//**********************************************
//   PCONFIG pure virtual base class. Used by ProductFeaturesBase for checking features availability
//	 Implemented by RWPConfig and DonglePConfig
//**********************************************
class RWPConfigBase
{
public:
    //
    // basic functionality
    //
    virtual const bool open(const char *) = 0;								// existing PConfig file name. done.
	virtual const bool open(const std::string& strName) { return(open(strName.c_str())); } // existing PConfig file name. done.

    virtual const bool close() = 0; // write out data and clear out object. done.

	virtual const std::string&  get_value(const std::string& strSection,
		                          const std::string& strValueName) const = 0;              //done

    //
    // constructors and such
    //
	RWPConfigBase() {};	// Default constructor
    RWPConfigBase(const char * szFileName,  // PConfig file name
               bool  bReadWrite = false);
    virtual ~RWPConfigBase() {}

private:
	RWPConfigBase(const RWPConfigBase&);					// Copy construction not allowed
	const RWPConfigBase& operator=(const RWPConfigBase&);	// Assignment is not allowed

};

#endif
