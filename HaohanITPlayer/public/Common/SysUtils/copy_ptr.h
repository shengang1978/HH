//-----------------------------------------------------------------------------
// copy_ptr.h
// Copyright (c) 1998 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW 20040911]
//SR FS: Reviewed [wwt 20040914] 

#ifndef COPYPTR
#define COPYPTR

#include <cassert>

// copy_ptr is a modified version of auto_ptr.
// Like auto_ptr, it deletes what it points to in it's destructor.
// The difference between auto_ptr and copy_ptr is that
// when copy pointer is copied or assigned, it makes a copy
// of what it points to.
//
// copy_ptr relies on the class it points to for the copy by
// calling the copy() method.  The reason for that is this:
//
// Consider the following example:
//
// class A
// {
//    A(const A&);
//    virtual A* copy() { return new A(*this); }
// };
//
// class B : A
// {
//    B(const B&);
//    virtual A* copy() { return new B(*this); }
// };
//
// {
//     copy_ptr<A> p1 = new B;
//     copy_ptr<A> p2 = p1;
// }
//
// Were the implementation of copy constructor to use the obvious:
//
//	copy_ptr(const copy_ptr& r)
//	{
//		if (r.get())
//			p_ = new T(*r);
//		else
//			p_ = 0;
//	 }
//
// then the copy constuctor for p2 would call A::A(const A&) and
// construct an instance A.  As implemented below, the copy() method
// called is in class B which calls B::B(const B&) p2 ends up pointing
// to an instance of class B.

template<class T>
T* CopyForCopyPtr(T* toCopy)
{
	return toCopy->copy();
}

template<class T>
class copy_ptr
{
private:
	T*		p_;

public:
	copy_ptr(T* p = 0) : p_(p)	{}
	copy_ptr(const copy_ptr& r) : p_(r.p_ ? r.p_->copy() : 0) {}
	~copy_ptr() 					{ delete p_; }

// **CodeWizzard** - Severe Violation:  Effective C++ item 29  - Avoid returning "handles" to internal data from const member function
// **CodeWizzard** - Violation:  Effective C++ item 30  - Avoid member functions that return pointers or references to members less accessible than themselve
	T*		get() const 		{ return p_; }
	void 	set(T* p)				{ delete p_; p_ = p; }
	void	swap(copy_ptr<T>& r)	{ T* temp = p_; p_ = r.p_; r.p_ = temp; }
	T*		release()				{ T* result = get(); p_ = 0; return result; }

	copy_ptr&	operator=(const copy_ptr& r)
	{
		if (this != &r)
		{
			set(r.p_ ? CopyForCopyPtr(r.p_) : 0);
		}
		return *this;
	}

	T& operator*() const	{ assert(p_); return *p_; }
	T* operator->() const	{ assert(p_); return p_; }

};

#endif
