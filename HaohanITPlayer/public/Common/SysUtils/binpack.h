//-----------------------------------------------------------------------------
// binpack.h
// Copyright (c) 2000 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW 20040911]
//SR FS: Reviewed [wwt 20040914] 

#ifndef BINPACK
#define BINPACK

#include <map>
#include <vector>
#include <algorithm>

// Fit the values between InputIterators first1 and last1 in to bins of size binsize.
// Return the required number of bins.
// Output the bin number for each value to the OuputIterator first2.

// The best fit binpack algorithm works by taking the each input value in order,
// and putting it in the bin with the smallest amount of available free space
// that has room.  If there is no room in the bin, we create a new one.
// The bins are kept in a map, where the key is the amount of free space.
// This allows for fast searches and reorders as we fill the bins.

template <class InputIterator, class OutputIterator, class Size, class Bin>
typename Bin::result_type
best_fit_binpack(InputIterator first1, InputIterator last1, OutputIterator first2, const typename Size::result_type& binsize, Size size, Bin bin)
{
	typedef typename Size::result_type value_type;
	typedef typename Bin::result_type index_type;
	typedef std::multimap< value_type,  index_type> bins_map_type;

	// This is a map where the key is the space remaining in a bin, and the value is the bin number.
	index_type n_bins = 0;

	bins_map_type bins;

	for (; first1 != last1; ++first1)
	{
		// find the bin with the smallest amount of free space which still fits the value ("best fit").
		typename bins_map_type::iterator b = bins.lower_bound(size(*first1));
		if (b == bins.end())
		{
			// There is no bin with space.  Create a new bin.
			bin.set(*(first2++), n_bins);
			bins.insert(typename bins_map_type::value_type(binsize - size(*first1), n_bins++));
		}
		else
		{
			// Remove space from this bin.
			bin.set(*(first2++), (*b).second);
			typename bins_map_type::value_type temp((*b).first - size(*first1), (*b).second);
			bins.erase(b);
			bins.insert(temp);
		}
	}
	return n_bins;
}

template <class InputIterator, class OutputIterator, class Size>
typename std::iterator_traits<OutputIterator>::value_type
best_fit_binpack(const InputIterator& first1, const InputIterator& last1, const OutputIterator& first2, const typename Size::result_type& binsize, const Size& size)
{
	typedef typename std::iterator_traits<OutputIterator>::value_type index_type;
	class identity_bin : public std::unary_function<index_type, index_type>
	{
	public:
		const index_type& operator()(const index_type& arg) const
		{
			return arg;
		}
		void set(index_type& arg, const index_type& value)
		{
			arg = value;
		}
	};

	return best_fit_binpack(first1, last1, first2, binsize, size, identity_bin());
}

template <class InputIterator, class OutputIterator>
typename std::iterator_traits<OutputIterator>::value_type
best_fit_binpack(const InputIterator& first1, const InputIterator& last1, const OutputIterator& first2, const typename std::iterator_traits<InputIterator>::value_type& binsize)
{
	typedef typename std::iterator_traits<InputIterator>::value_type value_type;
	class identity_size : public std::unary_function<value_type, value_type>
	{
	public:
		const value_type& operator()(const value_type& arg) const { return arg; }
	};

	return best_fit_binpack(first1, last1, first2, binsize, identity_size());
}

// Fit the values between InputIterators first1 and last1 in to bins of size binsize.
// This version sorts the input data by decending size before applying the best fit algorithm.
// Return the required number of bins.
// Output the bin number for each value to the RandomAccessIterator first2.

// This works by building a vector of input-output iterator pairs, sorting
// the vector of iterators, and then using best_fit_binpack on the sorted
// vector.

	// This is the compare function used for the sort.

template<class Size, class sort_type>
class BinpackCompare : public std::binary_function<sort_type, sort_type, bool>
{
private:
	Size size;

public:
// **CodeWizzard** - Informational:  More Effective C++ item 5  - Be wary of user-defined conversion function
	BinpackCompare(const Size& sz) : size(sz) {}

	bool operator()(const sort_type& a1, const sort_type& a2) const
	{
			return size(*(a1.first)) > size(*(a2.first));
	}
};

// This class tells best_fit_binpack how to pull the item size out of sort_type
template<class Size, class sort_type, class value_type>
class BinpackSortSize : public std::unary_function<sort_type, value_type>
{
private:
	Size size;

public:
// **CodeWizzard** - Informational:  More Effective C++ item 5  - Be wary of user-defined conversion function
	BinpackSortSize(const Size& sz) : size(sz) {}

	const value_type operator()(const sort_type& a) const
	{
			return size(*(a.first));
	}
};

// This class tells best_fit_binpack how to fill in the bin index (which is why operator()
// returns a non-const index_type&).
template<class Bin, class sort_type, class index_type>
class BinpackSortBin : public std::unary_function<sort_type, index_type>
{
private:
	Bin bin;

public:
// **CodeWizzard** - Informational:  More Effective C++ item 5  - Be wary of user-defined conversion function
	BinpackSortBin(const Bin& b) : bin(b) {}

	const index_type operator()(const sort_type& a) const
	{
		return bin(*(a.second));
	}
	void set(const sort_type& a, const index_type& val)
	{
		bin.set(*(a.second), val);
	}
};

template <class InputIterator, class OutputIterator, class Size, class Bin>
typename Bin::result_type
sort_best_fit_binpack(InputIterator first1, InputIterator last1, OutputIterator first2, const typename Size::result_type& binsize, Size size, Bin bin)
{
	typedef typename Size::result_type value_type;
	typedef typename Bin::result_type index_type;
	typedef std::pair<InputIterator, OutputIterator> sort_type;
	typedef std::vector<sort_type> sort_vector_type;

	// First, fill in the sort vector.
	sort_vector_type sort_vector;
	for (; first1 != last1; ++first1, ++first2)
	{
		sort_vector.push_back(sort_type(first1, first2));
	}

	// Now sort it.
	std::sort(sort_vector.begin(), sort_vector.end(), BinpackCompare<Size, sort_type>(size));

	// And use the best_fit_binpack to finish the job.
	return best_fit_binpack(sort_vector.begin(), sort_vector.end(), sort_vector.begin(), binsize, BinpackSortSize<Size, sort_type, index_type>(size), BinpackSortBin<Bin, sort_type, index_type>(bin));
}

template <class InputIterator, class OutputIterator, class Size>
typename std::iterator_traits<OutputIterator>::value_type
sort_best_fit_binpack(const InputIterator& first1, const InputIterator& last1, const OutputIterator& first2, const typename Size::result_type& binsize, const Size& size)
{
	typedef typename std::iterator_traits<OutputIterator>::value_type index_type;
	class identity_bin : public std::unary_function<index_type, index_type>
	{
	public:
		const index_type& operator()(const index_type& arg) const
		{
			return arg;
		}
		void set(index_type& arg, const index_type& value)
		{
			arg = value;
		}
	};

	return sort_best_fit_binpack(first1, last1, first2, binsize, size, identity_bin());
}

template <class InputIterator, class OutputIterator>
typename std::iterator_traits<OutputIterator>::value_type
sort_best_fit_binpack(const InputIterator& first1, const InputIterator& last1, const OutputIterator& first2, const typename std::iterator_traits<InputIterator>::value_type& binsize)
{
	typedef typename std::iterator_traits<InputIterator>::value_type value_type;
	class identity_size : public std::unary_function<value_type, value_type>
	{
	public:
		const value_type& operator()(const value_type& arg) const { return arg; }
	};

	return sort_best_fit_binpack(first1, last1, first2, binsize, identity_size());
}

#define TEST_BINPACK 0

#if TEST_BINPACK

#include <ostream>

static std::ostream& test_binpack(std::ostream& os)
{
	typedef double value_type;
	typedef size_t index_type;

	std::vector<value_type> vals;
	for (size_t i = 1; i <= 16; ++i)
	{
		vals.push_back(i);
	}

	std::vector<index_type> bins(vals.size());

	os << "Best fit binpack: ";
	os << "bins = " << best_fit_binpack(vals.begin(), vals.end(), bins.begin(), 20) << std::endl;

	std::vector<index_type>::iterator binit = bins.begin();
	std::vector<value_type>::iterator it = vals.begin();
	for (; it != vals.end(); ++it)
	{
		os << *it << " - " << *(binit++) << std::endl;
	}

	os << std::endl << "Sorted best fit binpack: ";
	os << "bins = " << sort_best_fit_binpack(vals.begin(), vals.end(), bins.begin(), 20) << std::endl;

	binit = bins.begin();
	for (it = vals.begin(); it != vals.end(); ++it)
	{
		os << *it << " - " << *(binit++) << std::endl;
	}

	return os;
}

#endif

#endif
