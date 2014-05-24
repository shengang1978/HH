//-----------------------------------------------------------------------------
// UnixDMAObject.h
// Copyright (c) 2001 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//	MODULE		:	UnixDMAObject.h
//	PROJECT		:	System Utilities Library
//	PROGRAMMER	:	Michael Uman
//	DATE		:	Feb 14, 2001
//-----------------------------------------------------------------------------
//SR FS: Reviewed [wwt 20040914] 
//SR FS: Reviewed [DDT 20040928] Second pass.

#ifndef	_UNIXDMAOBJECT_H
#define _UNIXDMAOBJECT_H

#include "BaseDMAObject.h"

class UnixDMAObject : public BaseDMAObject {
public:
	UnixDMAObject();
	UnixDMAObject(PVIRTMEM pDst, PVIRTMEM pSrc, MEMSIZE size);
	virtual ~UnixDMAObject();
	
	virtual void SetSourceVirtual(PVIRTMEM pSource) THROW_DMAEXCEPT();
	virtual void SetDestVirtual(PVIRTMEM pDest) THROW_DMAEXCEPT();
	virtual void SetXferBytes(MEMSIZE xferBytes) THROW_DMAEXCEPT();
	virtual void StartXfer() THROW_DMAEXCEPT();
	
	virtual PPHYSMEM GetSourcePhysical() THROW_DMAEXCEPT();
	virtual PPHYSMEM GetDestPhysical() THROW_DMAEXCEPT();
	
	virtual void dump(std::ostream& dbgStream) const;
	
protected:

	PVIRTMEM	pSource;		// Pointer to Source in virtmem
	PVIRTMEM	pDest;			// Pointer to Destination in virtmem
	MEMSIZE		xferSize;		// Xfer size In Bytes
	
private:
};

#endif	// _UNIXDMAOBJECT_H

