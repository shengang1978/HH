//-----------------------------------------------------------------------------
// HuffmanAnalysis.h
// Copyright (c) 2001 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#ifndef HUFFMAN_ANALYSIS
#define HUFFMAN_ANALYSIS

#include <map>
#include <queue>
#include <cassert>
#include "ref_ptr.h"

#include "HuffmanCode.h"

namespace Huffman
{

template<typename InputIterator>
std::map<typename std::iterator_traits<InputIterator>::value_type, UInt32> Histogram(InputIterator first, InputIterator last)
{
	typedef typename std::iterator_traits<InputIterator>::value_type value_type;
	std::map<value_type, UInt32> hist;
	for ( ; first != last; ++first)
	{
		if (hist.count(*first) == 0)
			hist[*first] = 0;
		++(hist[*first]);
	}
	return hist;
}

template<class T>
class TreeNode
// **CodeWizzard** - Severe Violation:  Effective C++ item 11  - Define a copy constructor and operator= for classes with dynamically allocated memor
{
	UInt32 	freq_;
	T				val_;
	ref_ptr<TreeNode> left_, right_;

public:
// **CodeWizzard** - Violation:  Effective C++ item 13  - List members in an initialization list in the order in which they are declare
	TreeNode() : val_(), freq_(0) {}
// **CodeWizzard** - Violation:  Effective C++ item 13  - List members in an initialization list in the order in which they are declare
	TreeNode(const T& val, const UInt32 freq) : val_(val), freq_(freq) {}
	TreeNode(const TreeNode<T>& left, const TreeNode<T>& right)
	:	freq_(left.GetFreq() + right.GetFreq()),
		left_(new TreeNode<T>(left)),
		right_(new TreeNode<T>(right))
	{
	}
	TreeNode(const TreeNode<T>& a) : freq_(a.freq_), val_(a.val_), left_(a.left_), right_(a.right_) {}
	
	UInt32 GetFreq() const { return freq_; }
	const T& GetVal() const { return val_; }
	const TreeNode<T>& GetLeft() const { assert(left_.get()); return *left_; }
	const TreeNode<T>& GetRight() const { assert(right_.get()); return *right_; }
	TreeNode<T>& GetLeft() { assert(left_.get()); return *left_; }
	TreeNode<T>& GetRight() { assert(right_.get()); return *right_; }
	
	void Swap()
	{
		ref_ptr<TreeNode> temp = left_;
		left_ = right_;
		right_ = temp;
	}
	
	bool IsLeaf() const { return (left_.get() == 0) || (right_.get() == 0); }
	
	bool operator<(const TreeNode& arg) const { return GetFreq() < arg.GetFreq(); }
	bool operator>(const TreeNode& arg) const { return GetFreq() > arg.GetFreq(); }
};

template<typename T, typename InputIterator>
const TreeNode<T> BuildTree(InputIterator first, InputIterator last, const T&)
{
	std::priority_queue< TreeNode<T>, std::vector< TreeNode<T> >, std::greater< TreeNode<T> > > q;
	for (InputIterator i = first; i != last; ++i)
	{
		q.push(TreeNode<T>((*i).first, (*i).second));
	}
	while(q.size() > 1)
	{
		TreeNode<T> left = q.top();
		q.pop();
		TreeNode<T> right = q.top();
		q.pop();
		q.push(TreeNode<T>(left, right));
	}
	return q.top();
}

template<typename T, class InputIterator>
class ArrayIterator
{
	typedef std::pair<T, typename std::iterator_traits<InputIterator>::value_type> value_type;
	InputIterator array_;
	T count_;

public:
// **CodeWizzard** - Informational:  More Effective C++ item 5  - Be wary of user-defined conversion function
	ArrayIterator(InputIterator array) : array_(array)
	{
// **CodeWizzard** - Violation:  Effective C++ item 12  - Prefer initialization to assignment in constructor
		count_ = 0;
	}
	
	ArrayIterator& operator++()
	{
		++count_;
		++array_;
		return *this;
	}
	
	const value_type operator*() const
	{
		return value_type(count_, *array_);
	}
	
	bool operator==(const ArrayIterator<T, InputIterator>& arg) const
	{
		return array_ == arg.array_;
	}
	bool operator!=(const ArrayIterator<T, InputIterator>& arg) const
	{
		return array_ != arg.array_;
	}
};

template<class T>
bool SetLeftmostValue(TreeNode<T>& tree, T value)
{
	if (tree.IsLeaf())
		return tree.GetVal() == value;
	if (SetLeftmostValue(tree.GetLeft(), value))
		return true;
	if (SetLeftmostValue(tree.GetRight(), value))
	{
		tree.Swap();
		return true;
	}
	return false;
}

template<class T>
void BuildCode(std::vector<CodePair<T> >& code, const TreeNode<T>& tree, VLCode base = VLCode())
// **CodeWizzard** - Violation:  Effective C++ item 22  - Pass objects by reference instead of by valu
{
	if (tree.IsLeaf())
	{
		code.push_back(CodePair<T>(tree.GetVal(), base));
		return;
	}
	BuildCode(code, tree.GetLeft(), base + VLCode(0, 1));
	BuildCode(code, tree.GetRight(), base + VLCode(1, 1));
}

}

#endif
