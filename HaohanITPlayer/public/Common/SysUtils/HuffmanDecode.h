//-----------------------------------------------------------------------------
// HuffmanDecode.h
// Copyright (c) 2001 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#ifndef HUFFMAN_DECODE
#define HUFFMAN_DECODE

#include <vector>
#include <iosfwd>

//dpy--> (5.14.02)  LINUX PORT
#ifdef __linux__
#include <stdexcept>
#endif
//<--dpy

#include "HuffmanCode.h"
#include "log2er.h"

namespace Huffman
{


template<typename T, typename StateRef, UInt32 n>
class Transition
// **CodeWizzard** - Violation:  Effective C++ item 20  - Avoid data members in the public interfac
// **CodeWizzard** - Violation:  Universal Coding Standards item 16  - Do not specify public data member
{
public:			// Make these public because I cannot declare AssignTransistion and AddTransistion as friends.
				// Note: this is all part of an elaborate hack to get around bugs in VC++ 6
	StateRef state_;
	T values_[n+1];
private:

#ifdef _MWERKS
	friend std::ostream& operator<< <T,StateRef> (std::ostream&, const Transition<T, StateRef>&);
#endif
	
public:
	typedef T value_type;

	Transition() : state_(0)
	{
		values_[0] = 0;
	}
	Transition(const Transition& arg) : state_(arg.state_)
	{
		values_[0] = arg.values_[0];
		for (SInt32 i = 1; i <= values_[0]; ++i)
			values_[i] = arg.values_[i];
	}
// **CodeWizzard** - Informational:  More Effective C++ item 5  - Be wary of user-defined conversion function
	Transition(const StateRef& s) : state_(s)
	{
		values_[0] = 0;
	}
	Transition(const StateRef& s, T& v) : state_(s)
	{
		values_[0] = 1;
		values_[1] = v;
	}

	Transition& operator=(const Transition& arg)
	{
		state_ = arg.state_;
		values_[0] = arg.values_[0];
		for (SInt32 i = 1; i <= values_[0]; ++i)
			values_[i] = arg.values_[i];
		return *this;
	}

	template<typename OutputIterator>
	StateRef Next(OutputIterator& o) const
	{
		for (const T* it = values_+1; it != values_+values_[0]+1; ++it)
		{
			*o = *it;
			++o;
		}
		return state_;
	}
	
	void AddValue(const T& v)
	{
		assert(values_[0] < n);
		++values_[0];
		values_[values_[0]] = v;
	}
	void SetState(const StateRef& r)
	{
		state_ = r;
	}
	const StateRef& GetState() const
	{
		return state_;
	}

	// This crashes VC++ 6 if you use it.
	template<UInt32 m>
	Transition& operator+=(const Transition<T, StateRef, m>& a)
	{
		for (const T* it = a.values_+1; it != a.values_+a.values_[0]+1; ++it)
		{
			AddValue(*it);
		}
		state_ = a.state_;
		return *this;
	}

};

// This is a substitute for operator= which crashes VC++
template<class ToTransition, class FromTransition>
void AssignTransition(ToTransition& to, const FromTransition& from)
{
	to.state_ = from.state_;
//	assert(from.values_[0] <= n);
	to.values_[0] = from.values_[0];
	for (SInt32 i = 1; i <= from.values_[0]; ++i)
		to.values_[i] = from.values_[i];
}

// This is a substitute for operator+= which crashes VC++
template<class ToTransition, class FromTransition>
void AddTransition(ToTransition& to, const FromTransition& from)
{
	for (const typename ToTransition::value_type* it = from.values_+1; it != from.values_+from.values_[0]+1; ++it)
	{
		to.AddValue(*it);
	}
	to.state_ = from.state_;
}

template<typename T, typename StateRef, UInt32 n, UInt32 m>
Transition<T, StateRef, n> operator+(const Transition<T, StateRef, n>& a, const Transition<T, StateRef, m>& b)
// **CodeWizzard** - Violation:  More Effective C++ item 2 - Consider using op= instead of stand-alone o
{
	Transition<T, StateRef, n> temp(a);
	Add(temp, b);
	return temp;
}

template<typename T, typename StateRef, UInt32 n>
std::ostream& operator<<(std::ostream& os, const Transition<T, StateRef, n>& t)
{
	if (t.values_.size() > 0)
	{
		os << "{";
		os << t.values_.back() << "}";
	}
	os << "->" << t.state_;
	return os;
}

template<typename T, typename StateRef, UInt32 n>
class State
{
public:
	typedef Transition<T, StateRef, log2er<n>::value> Transition_t;

private:
	Transition_t transitions_[n];
	
public:
	Transition_t& Get(UInt32 i) { assert(i < n); return transitions_[i]; }
	const Transition_t& Get(UInt32 i) const { assert(i < n); return transitions_[i]; }
};

template<typename T, typename StateRef, UInt32 n>
std::ostream& operator<<(std::ostream& os, const State<T, StateRef, n>& t)
{
	for (UInt32 i = 0; i < n; ++i)
	{
		os << i << ":" << t.Get(i) << " ";
	}
	return os;
}

template<typename T, UInt32 n>
class StateMachine
{
public:
	typedef State<T, UInt32, n> State_t;
	typedef typename State_t::Transition_t Transition_t;

private:
	std::vector<State_t> states_;
	UInt32 state_;
	
	typedef std::vector< CodePair<T> > CodeVector;

#ifdef _MWERKS
	friend std::ostream& operator<< <T,n> (std::ostream& os, const StateMachine<T,n>& sm);
#endif
	
// **CodeWizzard** - Violation:  Effective C++ item 22  - Pass objects by reference instead of by valu
	void AddState(typename CodeVector::iterator first, typename CodeVector::iterator last)
	{
		typename CodeVector::iterator middle = std::lower_bound(first, last, VLCode(1, 1), ByCode<T>());
		UInt32 newState = static_cast<UInt32>(states_.size());
		states_.push_back(State<T, UInt32, n>());
		
		// Strip off the leading bit
		for (typename CodeVector::iterator it = first; it != last; ++it)
		{
			(*it).second <<= 1;
		}
	
		if (middle == first)
			throw(std::runtime_error("Code has an error state."));
		if (middle == last)
			throw(std::runtime_error("Code has an error state."));

		// Handle the leading bit == 0
		if (middle - first > 1)
		{
			states_[newState].Get(0).SetState(static_cast<UInt32>(states_.size()));
			AddState(first, middle);
		}
		else
		{
			states_[newState].Get(0).SetState(0);
			states_[newState].Get(0).AddValue((*first).first);
		}
		
		// Handle the leading bit == 1
		if (last - middle > 1)
		{
			states_[newState].Get(1).SetState(static_cast<UInt32>(states_.size()));
			AddState(middle, last);
		}
		else
		{
			states_[newState].Get(1).SetState(0);
			states_[newState].Get(1).AddValue((*middle).first);
		}
	}
	
public:
	StateMachine() : state_(0) {}

	template<class InputIterator>
	StateMachine(InputIterator first, InputIterator last) : state_(0)
	{
		CodeVector code;
		for ( ; first != last; ++first)
		{
			code.push_back(static_cast< const CodePair<char>& > (*first));
		}
		std::sort(code.begin(), code.end(), ByCode<T>());
		AddState(code.begin(), code.end());
	}
	
// **CodeWizzard** - Violation:  Effective C++ item 30  - Avoid member functions that return pointers or references to members less accessible than themselve
	std::vector< State<T, UInt32, n> >& GetStates() { return states_; }

	template<typename OutputIterator>
	void Next(UInt32 transition, OutputIterator& out)
	{
		state_ = states_[state_].Get(transition).Next(out);
	}

	void SquareInto(StateMachine<T, n*n>& result)
	{
		UInt32 i, j, k;
		for (k = 0; k < states_.size(); ++k)
		{
			result.GetStates().push_back(State<T, UInt32, n*n>());
			for (i = 0; i < n; ++i)
				for (j = 0; j < n; ++j)
				{
					typename StateMachine<T, n*n>::Transition_t* t = &(result.GetStates()[k].Get(i*n+j));
					AssignTransition(*t, states_[k].Get(i));
					UInt32 next = t->GetState();
					AddTransition(*t, states_.at(next).Get(j));
				}
		}
	}
/*	
	template<SInt32 m>
	StateMachine<T, n*m> operator*(const StateMachine<T, m>& sm) const
	{
		SInt32 i, j, k;
		StateMachine<T, n*m> result;
		if (states_.size() != sm.states_.size())
			throw(std::runtime_error("Mismatched state machines."));
		for (k = 0; k < states_.size(); ++k)
		{
			result.GetStates().push_back(State<T, UInt32, n*m>());
			for (i = 0; i < n; ++i)
				for (j = 0; j < m; ++j)
				{
					Transition<T, UInt32>& t = result.GetStates()[k].Get(i*n+j);
					t = states_[k].Get(i);
					UInt32 next = t.GetState();
					t += sm.states_.at(next).Get(j);
				}
		}
		return result;
	}
*/
};

template<typename T, SInt32 n>
std::ostream& operator<<(std::ostream& os, const Huffman::StateMachine<T,n>& sm)
{
	for (SInt32 i = 0; i < sm.states_.size(); ++i)
	{
		os << std::dec << i << " -- " << sm.states_[i] << std::endl;
	}
	return os;
}

}

#endif
