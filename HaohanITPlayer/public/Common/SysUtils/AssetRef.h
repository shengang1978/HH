//-----------------------------------------------------------------------------
// AssetRef.h
// Copyright (c) 2003 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW 20040911]
//SR FS: Reviewed [wwt 20040913] 

/*********************************************************************

Base AssetRef class

*********************************************************************/

#ifndef ASSETREF_H
#define ASSETREF_H

//This file contains a generic mechanism for encapsulating the information necessary to get to what an actual asset is without having any intermediate 
//code have to include anything unnecessary. This allows us to streamline linkages and libraries. 
//The code we're trying to hide includes Editors and DVDStreams.

#include "AssetRefImpl.h"
#include "ref_ptr.h"

class AssetRef
{
public:
	ref_ptr<const AssetRefImpl> pimpl_;

	AssetRef() : pimpl_(0) { }
	AssetRef(const AssetRef& arg) : pimpl_(arg.pimpl_) { }
	~AssetRef() { }

	AssetRef& operator=(const AssetRef& arg)
	{
		if (this == &arg)
			return *this;

		pimpl_ = arg.pimpl_;
		return *this;
	}

	//returns -1 if this asset is "less than" referenced object, 0 if it "matches", and +1 if it is "greater than"
	SInt32 Compare(const AssetRef& obj, UInt32 flags) const
	{
		//assert(pimpl_.get() != 0);
		//assert(obj.pimpl_.get() != 0);
		return (pimpl_->Compare(*obj.pimpl_, flags));
	}

    // a true test for equality
    bool operator==( const AssetRef& rhs ) const
    {
        return (pimpl_->IsEqual( *rhs.pimpl_.get() ) );
    }

private:

};

#endif
