//-----------------------------------------------------------------------------
// UnixDMAObject.cpp
// Copyright (c) 2001 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//	MODULE		:	UnixDMAObject.cpp
//	PROJECT		:	System Utilities Library
//	PROGRAMMER	:	Michael Uman
//	DATE		:	Feb 14, 2001
//-----------------------------------------------------------------------------
//SR FS: Reviewed [wwt 20040914] 
//SR FS: Reviewed [DDT 20040928] Second pass.

#define DO_MEMCPY

#include <iostream>
#include "UnixDMAObject.h"
				


#ifndef USE_MEMCPY
extern void my_memcpy(Ptr, Ptr, UInt32);
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

UnixDMAObject::UnixDMAObject()
:	BaseDMAObject(),
	pSource( 0L ),
	pDest( 0L ),
	xferSize( 0L )
{
#ifdef	_DEBUG
#endif	// _DEBUG
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

UnixDMAObject::UnixDMAObject(PVIRTMEM pDst, PVIRTMEM pSrc, MEMSIZE size)
:	BaseDMAObject( ),
	pSource( pSrc ),
	pDest( pDst ),
	xferSize( size )
{
#ifdef	_DEBUG
#endif	// _DEBUG
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

UnixDMAObject::~UnixDMAObject() {
#ifdef	_DEBUG
#endif
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void UnixDMAObject::SetSourceVirtual(PVIRTMEM srcPtr) THROW_DMAEXCEPT() {
	pSource = srcPtr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void UnixDMAObject::SetDestVirtual(PVIRTMEM dstPtr) THROW_DMAEXCEPT() {
	pDest = dstPtr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void UnixDMAObject::SetXferBytes(MEMSIZE bytesXfer) THROW_DMAEXCEPT() {
	xferSize = bytesXfer;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

PPHYSMEM UnixDMAObject::GetSourcePhysical() THROW_DMAEXCEPT() {
	throw BaseDMAObjectException(BaseDMAObjectException::EXCEPT_UNIMPLEMENTED);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

PPHYSMEM UnixDMAObject::GetDestPhysical() THROW_DMAEXCEPT() {
	throw BaseDMAObjectException(BaseDMAObjectException::EXCEPT_UNIMPLEMENTED);
}

//-----------------------------------------------------------------------------
//	FUNCTION	:	UnixDMAObject::StartXfer
//	PURPOSE		:	Perform the transfer, when complete xferSize bytes of
//					data will have been copied from pSource to pDest.
//-----------------------------------------------------------------------------

void UnixDMAObject::StartXfer() THROW_DMAEXCEPT() {
	if ((pSource == 0L) || (pDest == 0L)) {
#ifdef	_DEBUG
#endif
		throw BaseDMAObjectException(BaseDMAObjectException::EXCEPT_INVALIDPARMS);
	} else {
#ifdef DO_MEMCPY
		memcpy( pDest, pSource, xferSize );
#endif
	}

}

//-----------------------------------------------------------------------------
//	FUNCTION	:	UnixDMAObject::dump
//	PURPOSE		:	Debug function to display object attributes
//-----------------------------------------------------------------------------

void UnixDMAObject::dump(ostream& dbgStream) const {
	dbgStream << "DMA Object Dump:" << endl;
	dbgStream << "Source address = 0x" << hex << pSource << endl;
	dbgStream << "Dest address   = 0x" << hex << pDest << endl;
	dbgStream << "Transfer size  = " << dec << xferSize << " [0x" << hex << xferSize << "]" << endl;
}
