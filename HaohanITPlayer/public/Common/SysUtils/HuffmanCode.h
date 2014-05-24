//-----------------------------------------------------------------------------
// HuffmanCode.h
// Copyright (c) 2001 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#ifndef HUFFMAN_CODE
#define HUFFMAN_CODE

#include <iosfwd>
#include <ostream>
#include <iterator>
#include <functional>

//dpy--> (5.14.02)  LINUX PORT
#ifdef __linux__
#include <map>
#endif
//<--dpy

#include "CommonTypes.h"

namespace Huffman
{

typedef UInt32 Code_t;
const UInt32 MAX_CODE_BITS = sizeof(Code_t)*8;

// Codes are stored left justified.
class VLCode
{
	Code_t code_;
	UInt32 length_;
	
public:
	VLCode() : code_(0), length_(0) {}
	VLCode(Code_t bits, UInt32 length) : code_(bits << (MAX_CODE_BITS-length)), length_(length) {}
	
	inline Code_t GetBits() const { return code_ >> (MAX_CODE_BITS-length_); }
	inline Code_t GetCode() const { return code_; }
	inline UInt32 GetLength() const { return length_; }
	
	inline const VLCode& operator+=(const VLCode& a)
	{
		code_ |= a.code_ >> length_;
		length_ += a.length_;
		if (length_ > MAX_CODE_BITS)
			length_ = MAX_CODE_BITS;
		return *this;
	}
	
// **CodeWizzard** - Violation:  Effective C++ item 19  - Differentiate among member functions, global functions and friend function
	inline VLCode operator+(const VLCode& a)
	{
		VLCode temp(*this);
		temp += a;
		return temp;
	}
	
	inline bool operator<(const VLCode& a) const
	{
		if (code_ < a.code_) return true;
		if (code_ > a.code_) return false;
		return length_ < a.length_;
	}

	inline bool operator==(const VLCode& a) const
	{
		if (code_ == a.code_) return true;
		return length_ == a.length_;
	}

	inline bool operator>(const VLCode& a) const
	{
		if (code_ > a.code_) return true;
		if (code_ < a.code_) return false;
		return length_ > a.length_;
	}
	
	inline const VLCode& operator<<=(const UInt32 n)
	{
		if (n < length_)
		{
			code_ <<= n;
			length_ -= n;
		}
		else
		{
			code_ = 0;
			length_ = 0;
		}
		return *this;
	}
};

template<class OutputIterator>
class Packer
{
	OutputIterator i_;
	VLCode c_;
	UInt64 bitsPacked_;

public:
// **CodeWizzard** - Informational:  More Effective C++ item 5  - Be wary of user-defined conversion function
	Packer(const OutputIterator& it) : i_(it), bitsPacked_(0) {}
	~Packer() {}
	
	UInt64 GetBitsPacked() const { return bitsPacked_; }
	
// **CodeWizzard** - Violation:  Effective C++ item 19  - Differentiate among member functions, global functions and friend function
	Packer<OutputIterator>& operator<<(const Huffman::VLCode& c)
	{
		bitsPacked_ += c.GetLength();
		UInt32 totalBits = c_.GetLength() + c.GetLength();
		c_ += c;
		if (totalBits >= MAX_CODE_BITS)
		{
			*i_ = c_.GetCode();
			++i_;
			c_ = c;
			c_ <<= (c.GetLength() - (totalBits - MAX_CODE_BITS));
		}
		return *this;
	}
	
	void Flush()
	{
		if (c_.GetLength() > 0)
		{
			*i_ = c_.GetCode();
			++i_;
			c_ <<= MAX_CODE_BITS;
		}
	}

// **CodeWizzard** - Violation:  Effective C++ item 30  - Avoid member functions that return pointers or references to members less accessible than themselve
	OutputIterator& GetOutputIterator() { return i_; }
};

template<class T>
struct CodePair : public std::pair<T, VLCode>
{
	CodePair() {}
	CodePair(const T& t, const VLCode& c) : std::pair<T,VLCode>(t, c) {}
};

template<class T>
struct ByValue : public std::binary_function< CodePair<T>, CodePair<T>, bool>
{
	bool operator() (const CodePair<T>& a, const CodePair<T>& b) const
	{
		return a.first < b.first;
	};
	bool operator() (const CodePair<T>& a, const T& b) const
	{
		return a.first < b;
	};
	bool operator() (const T& a, const CodePair<T>& b) const
	{
		return a < b.first;
	};
};

template<class T>
struct ByCode : public std::binary_function< CodePair<T>, CodePair<T>, bool>
{
	bool operator() (const CodePair<T>& a, const CodePair<T>& b) const
	{
		return a.second < b.second;
	};
	bool operator() (const CodePair<T>& a, const VLCode& b) const
	{
		return a.second < b;
	};
	bool operator() (const VLCode& a, const CodePair<T>& b) const
	{
		return a < b.second;
	};
};

inline std::ostream& operator<<(std::ostream& os, const VLCode& a)
{
	os << "VLCode(0x";
	os << std::hex << a.GetBits() << "," << std::dec << a.GetLength() << ")";
	return os;
}

}

#ifdef _MSC_VER
namespace std
{
template<>
struct iterator_traits<Huffman::VLCode*>
{
	typedef ptrdiff_t					difference_type;
	typedef Huffman::VLCode				value_type;
	typedef Huffman::VLCode*			pointer;
	typedef Huffman::VLCode&			reference;
	typedef std::random_access_iterator_tag	iterator_category;
};

template<>
struct iterator_traits<const Huffman::VLCode*>
{
	typedef ptrdiff_t					difference_type;
	typedef Huffman::VLCode				value_type;
	typedef const Huffman::VLCode*		pointer;
	typedef const Huffman::VLCode&		reference;
	typedef std::random_access_iterator_tag	iterator_category;
};

}
#endif

#endif
