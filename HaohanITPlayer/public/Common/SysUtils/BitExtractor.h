//-----------------------------------------------------------------------------
// BitExtractor.h
// Copyright (c) 1997 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW 20040911]
//SR FS: Reviewed [wwt 20040914] 

#ifndef BIT_EXTRACTOR
#define BIT_EXTRACTOR

#include <stdexcept>

#include "CommonTypes.h"
#include "ByteSwapper.h"

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4127)	// conditional expression is constant
#endif

template<class InputIterator, bool big_endian = true>
class BitExtractor
{
public:
		BitExtractor(InputIterator first, InputIterator last);
		BitExtractor(const BitExtractor<InputIterator, big_endian>&);
		~BitExtractor() {}

		const BitExtractor<InputIterator, big_endian>& operator=(const BitExtractor<InputIterator, big_endian>&);

	size_t GetBits(size_t n_bits);
	size_t NextBits(size_t n_bits) const;
	void GetBits(size_t n_bits, UInt8 *data);
	size_t  RemainingBits() const { return bits_in_active + 8*(end_addr - current_addr) - padding_bits; }
    void ForceByteAlignment( void ) { if ( bits_in_active < 8 ) { UInt8 dummy; GetBits( bits_in_active, &dummy );  } }    // Rich Fife - 1/12/06 for DTS-HD.
    void SkipBytes( int nBytes ) { UInt8 dummy;  while ( nBytes-- ) GetBits( 8, &dummy ); }     // Rich Fife - 1/12/06 for DTS-HD.
    void SetPaddingBits(size_t bits) { padding_bits = bits; }

	class no_more_bits_error : public std::runtime_error
	{
	public:
		no_more_bits_error() : std::runtime_error("BitExtractor : insufficient data") {}
	};

private:

	InputIterator current_addr;
	InputIterator end_addr;
	UInt16 bits_in_active;
	UInt8 active_byte;
	size_t padding_bits;

	static UInt8 hi_mask[9];
	static UInt8 lo_mask[9];
};

/***********************************************************************
 * class BitExtractor.
 ***********************************************************************/

template<class InputIterator, bool big_endian>
UInt8 BitExtractor<InputIterator, big_endian>::hi_mask[9]	= {0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff};

template<class InputIterator, bool big_endian>
UInt8 BitExtractor<InputIterator, big_endian>::lo_mask[9]	= {0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01, 0x00};


template<class InputIterator, bool big_endian>
BitExtractor<InputIterator, big_endian>::BitExtractor(InputIterator first, InputIterator last)
:	current_addr(first),
	end_addr(last),
	bits_in_active(8),
	padding_bits(0)
{
	if( current_addr < end_addr )
		active_byte = *(current_addr++);
	else
	{
		active_byte = 0;
		current_addr++;
    }
}

template<class InputIterator, bool big_endian>
BitExtractor<InputIterator, big_endian>::BitExtractor(const BitExtractor<InputIterator, big_endian>& arg)
:	current_addr(arg.current_addr),
	end_addr(arg.end_addr),
	bits_in_active(arg.bits_in_active),
	active_byte(arg.active_byte),
	padding_bits(0)
{
}

template<class InputIterator, bool big_endian>
const BitExtractor<InputIterator, big_endian>&
BitExtractor<InputIterator, big_endian>::operator=(const BitExtractor<InputIterator, big_endian>& arg)
{
	current_addr = arg.current_addr;
	end_addr = arg.end_addr;
	bits_in_active = arg.bits_in_active;
	active_byte = arg.active_byte;
	padding_bits = arg.padding_bits;
	return *this;
}

template<class InputIterator, bool big_endian>
size_t
BitExtractor<InputIterator, big_endian>::GetBits(size_t n_bits) 
{
	UInt32 bits = 0;
	if (n_bits <= 32)
	    ;
	else 
	    n_bits = 32;
	    
	GetBits(n_bits, (UInt8 *) &bits);

	if (!big_endian)
	    bits = static_cast<SInt32>(fromLE32(bits));
	else
	{
		bits = static_cast<SInt32>(fromBE32(bits));
		return (bits >> (32-n_bits));
	}
	return bits;
}

template<class InputIterator, bool big_endian>
size_t
BitExtractor<InputIterator, big_endian>::NextBits(size_t n_bits) const
{
	return BitExtractor<InputIterator, big_endian>(*this).GetBits(n_bits);
}

template<class InputIterator, bool big_endian>
void
BitExtractor<InputIterator, big_endian>::GetBits(size_t n_bits, UInt8 *data)
{
	if (n_bits <= RemainingBits())
	    ;
	else
		throw no_more_bits_error();
		
	UInt16 n_output_bits = 0;
	UInt16 output_bits = 0;
	UInt16 bits_to_copy = 0;
	while (n_bits > 0)
	{
		// figure out how many bits we take from the active byte.
		bits_to_copy = static_cast<UInt16>((n_bits > bits_in_active) ? bits_in_active : n_bits);
		n_bits -= bits_to_copy;

		// shift those bits into the holding word.
		output_bits |= (((UInt16) (active_byte & hi_mask[bits_to_copy])) << (8 - n_output_bits));
		n_output_bits = static_cast<UInt16>(n_output_bits + bits_to_copy);

		// if we have a byte worth of bits in the holding word, move it to the output buffer.
		if (n_output_bits >= 8)
		{
			// kick out the next byte.
			n_output_bits -= 8;
			*(data++) = (UInt8) (output_bits >> 8);
			output_bits <<= 8;
		}

		// update the active byte for the next bits to extract.
		active_byte <<= bits_to_copy;
		bits_in_active = static_cast<UInt16>(bits_in_active - bits_to_copy);
		if (bits_in_active <= 0)
		{
			if( current_addr < end_addr )
				active_byte = *(current_addr++);
			else
			{
				active_byte = 0;
				current_addr++;
		    }
		    
			bits_in_active = 8;
		}
	}
	// kick out any remaining bits.
	if (n_output_bits)
		*data = (UInt8) (output_bits >> 8);
}

#ifdef _WIN32
#pragma warning(pop)
#endif

#endif
