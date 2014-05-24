//-----------------------------------------------------------------------------
// IndexedIterator.h
// Copyright (c) 2000 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#ifndef __IndexedIterator__
#define __IndexedIterator__

#include <algorithm>

template<class Container>
class IndexedIterator
{
public:
	typedef ptrdiff_t						difference_type; 
	typedef typename Container::value_type		value_type;
	typedef value_type*					pointer;
	typedef value_type&   				reference;
	typedef std::random_access_iterator_tag	iterator_category;

	IndexedIterator() : mContainer(0), mIndex(0) {}
	IndexedIterator(Container& c, size_t index) : mContainer(&c), mIndex(index) {}
	IndexedIterator(const IndexedIterator& i) : mContainer(i.mContainer), mIndex(i.mIndex) {}
	~IndexedIterator() {}

	bool IsBegin() const { return mIndex == 0; }
	bool IsEnd() const { if (mContainer == 0) return true; return mIndex >= mContainer->size(); }
	size_t Index() const { return mIndex; }

	const IndexedIterator& operator=(const IndexedIterator& i)
	{
		if (&i == this) return *this;
		mContainer = i.mContainer;
		mIndex = i.mIndex;
		return *this;
	}

	value_type operator*() const { return (*mContainer)[mIndex]; }

	// Arithmetic
	IndexedIterator& operator++()		{ ++mIndex; return *this; }
	IndexedIterator operator++(int)		{ IndexedIterator temp(*this); ++(*this); return temp; }
	IndexedIterator& operator+=(ptrdiff_t i)	{ mIndex += i; return *this; }
	IndexedIterator operator+(ptrdiff_t i)	{ IndexedIterator temp(*this); return temp += i; }
	IndexedIterator& operator--()		{ --mIndex; return *this; }
	IndexedIterator operator--(int)		{ IndexedIterator temp(*this); --(*this); return temp; }
	IndexedIterator& operator-=(ptrdiff_t i)	{ mIndex -= i; return *this; }
	IndexedIterator operator-(ptrdiff_t i)	{ IndexedIterator temp(*this); return temp -= i; }
	difference_type operator-(const IndexedIterator& i)	{ return mIndex - i.mIndex; }

	// Comparisions
	bool operator==(const IndexedIterator& x) const { return mIndex == x.mIndex; }
	bool operator!=(const IndexedIterator& x) const { return mIndex != x.mIndex; }
	bool operator<(const IndexedIterator& x) const { return mIndex < x.mIndex; }
	bool operator<=(const IndexedIterator& x) const { return mIndex <= x.mIndex; }
	bool operator>(const IndexedIterator& x) const { return mIndex > x.mIndex; }
	bool operator>=(const IndexedIterator& x) const { return mIndex >= x.mIndex; }

private:
	Container*	mContainer;
	size_t		mIndex;
};

namespace ContainerUtilities
{

template <class Iterator, class Predicate>
Iterator find(Iterator first, Iterator last, const Predicate& pred)
{
	for (Iterator i = first; i != last; i++)
	{
		if (pred(*i))
			return i;
	}

	return last;
}

}

#endif
