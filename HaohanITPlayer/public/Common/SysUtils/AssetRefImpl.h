//-----------------------------------------------------------------------------
// AssetRefImpl.h
// Copyright (c) 2003 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW 20040911]
//SR FS: Reviewed [wwt 20040913] 

/*********************************************************************

Pure virtual AssetRefImpl base class

*********************************************************************/

#ifndef ASSETREF_IMPL_H
#define ASSETREF_IMPL_H

#include "CommonTypes.h"

// Comparison flags
const UInt32 kAssetRefIgnoreVideo = 0x0001;
const UInt32 kAssetRefIgnoreAudio = 0x0002;
const UInt32 kAssetRefIgnoreTime = 0x0004;
const UInt32 kAssetRefIgnoreSize = 0x0008;
const UInt32 kAssetRefCompareSizeGreaterEqual = 0x0010;

class AssetRefImpl
{
public:

	AssetRefImpl() {}
	virtual ~AssetRefImpl() {}

//	virtual AssetRefImpl* copy() const = 0; 

	//returns -1 if this asset is "less than" referenced object, 0 if it "matches", and +1 if it is "greater than"
	virtual SInt32 Compare(const AssetRefImpl& obj, UInt32 flags) const = 0;

    // true test for equality
    virtual bool IsEqual( const AssetRefImpl& rhs ) const = 0;

private:
	AssetRefImpl(AssetRefImpl const &rhs);
	AssetRefImpl& operator=(AssetRefImpl const &rhs);
};

/*class AssetRefFactory
{
	enum ERefType
	{
		eRef_AS = 1,
		eRef_FSpec,
		eRef_Surface
	};

	AssetRefFactory();
	~AssetRefFactory();

	Register(ERefType type, AssetRefImpl* pImpl);
	UnRegister(ERefType type);
	copy_ptr<AssetRefImpl>& Make(ERefType type);

	typedef std::map<ERefType type, copy_ptr<AssetRefImpl> > MapRefs;
	ref_ptr<MapRefs>		mpMapRefList;

private:
	AssetRefFactory(AssetRefFactory const &rhs);
	AssetRefFactory& operator=(AssetRefFactory const &rhs);
};


//------------------------------
// AssetRefImpl.cpp
//------------------------------


//factory for creating such things
AssetRefFactory::Register(ERefType type, AssetRefImpl* pImpl)
{
	//this should only copy once, when inserting into map
	ref_ptr< AssetRefImpl > impl(pImpl);
	mpMapRefList->insert(make_pair(type, impl));
}
*/

#endif // ASSETREF_IMPL_H
