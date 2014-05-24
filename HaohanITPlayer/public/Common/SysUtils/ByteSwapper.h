//-----------------------------------------------------------------------------
// ByteSwapper.h
// Copyright (c) 1997 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Needs Review [JAW 20040911] - check for buffer overrun issues, add some routines to watch list
//SR FS: Reviewed [JAW 20040911]
//SR FS: Reviewed [wwt 20040915] 
//SR FS: Reviewed [JHL 20040924]
//SR FS: Reviewed [JAW 20040924] - Added comments for risky routines and macros 
//SR FS: Reviewed [DDT 20040928] - Corrected some SR grammar and added MS comment.

#ifndef __BYTESWAPPER__
#define __BYTESWAPPER__

#include "CommonTypes.h"


#if defined (_M_IX86)	|| defined (_M_IA64) || defined (_M_AMD64) || defined (__i386__) || defined (__INTEL__) || defined (intel) || defined (__mipsel__) || defined(__sh4__)
	#define BigEnd__ 0
#elif __POWERPC__ || __MC68K__ || __ppc__
	#define BigEnd__ 1
#else
	#error "Unknown processor architecture!"
#endif


#ifdef __cplusplus
//	Inlines to load and store numbers from bytes in memory.  These
//	aren't meant to swap, but to get a number that may not be properly aligned
//	for the desired load or store operation.  Swap later if you need to.

//SR SC: T, I, D [JHL 20040924]
//SR DI: DP1, E1, D1 [JHL 20040924] {dread_score: 1}  source[i] possibly causes some problems if &source[i] is invalid  
//SR MS: M [DDT 20041005] There should be no problem for loading. Only problem here is the wrong usage could crash the application.
inline UInt64
load8(const unsigned char* source){
//	return ((((((((UInt64) (source[0]))<<8 | source[1])<<8 | source[2])<<8 | source[3])<<8
	return (((((((static_cast<UInt64>(source[0]))<<8 | source[1])<<8 | source[2])<<8 | source[3])<<8
		| source[4])<<8 | source[5])<<8 | source[6])<<8 | source[7];
}
//SR SC: T, I, D [JHL 20040924]
//SR DI: DP1, E1, D1 [JHL 20040924] {dread_score: 1}  source[i] possibly causes some problems if &source[i] is invalid  
//SR MS: M [DDT 20041005] There should be no problem for loading. Only problem here is the wrong usage could crash the application.
inline SInt32
load4(const unsigned char* source)  {
	return ((((source[0]<<8 | source[1])<<8) | source[2])<<8) | source[3];
}
//SR SC: T, I, D [JHL 20040924]
//SR DI: DP1, E1, D1 [JHL 20040924] {dread_score: 1}  source[i] possibly causes some problems if &source[i] is invalid  
//SR MS: M [DDT 20041005] There should be no problem for loading. Only problem here is the wrong usage could crash the application.
inline SInt32
load3(const unsigned char* source)  {
	return ((source[0]<<8 | source[1])<<8) | source[2];
}
//SR SC: T, I, D [JHL 20040924]
//SR DI: DP1, E1, D1 [JHL 20040924] {dread_score: 1}  source[i] possibly causes some problems if &source[i] is invalid  
//SR MS: M [DDT 20041005] There should be no problem for loading. Only problem here is the wrong usage could crash the application.
inline SInt16
load2(const unsigned char* source) {
	return source[0]<<8 | source[1];
}

inline UInt64
load8(const char* source)  { return load8(reinterpret_cast<const unsigned char*>(source)); }
inline SInt32
load4(const char* source){ return load4(reinterpret_cast<const unsigned char*>(source)); }
inline SInt32
load3(const char* source){ return load3(reinterpret_cast<const unsigned char*>(source)); }
inline SInt16
load2(const char* source){ return load2(reinterpret_cast<const unsigned char*>(source)); }

//SR SC: T, I, D [JHL 20040924]
//SR DI: DP10, E5, D5 [JHL 20040924] {dread_score: 6.66666666666667}  dest[i] possibly causes some problems if &dest[i] is invalid  
//SR SC: T, I, D [JAW 20040924] - Using store8 could cause buffer overflow if reciever is too small and should not used 
//SR DI: DP10, E3, D2 [JAW 20040924] {dread_score: 5} 
//SR MC: No Issue [wwt 20040915] - even the dest param can cause possible BO, the value is fixed by the value param, which is unlikely to be attacked.
//SR MS: M [DDT 20041021] Reverted adding destination buffer size to guard buffer overrun.
inline void
store8(unsigned char* dest, UInt64 value){
	dest[0] = static_cast<UInt8>(value>>56);
	dest[1] = static_cast<UInt8>(value>>48);
	dest[2] = static_cast<UInt8>(value>>40);
	dest[3] = static_cast<UInt8>(value>>32);
	dest[4] = static_cast<UInt8>(value>>24);
	dest[5] = static_cast<UInt8>(value>>16);
	dest[6] = static_cast<UInt8>(value>>8);
	dest[7] = static_cast<UInt8>(value);
}
//SR SC: T, I, D [JHL 20040924]
//SR DI: DP10, E5, D5 [JHL 20040924] {dread_score: 6.66666666666667}  dest[i] possibly causes some problems if &dest[i] is invalid  
//SR SC: T, I, D [JAW 20040924] - Using store3 could cause buffer overflow if reciever is too small and should not used 
//SR DI: DP10, E3, D2 [JAW 20040924] {dread_score: 5} 
//SR MS: M [DDT 20041021] Reverted adding destination buffer size to guard buffer overrun.
inline void
store3(unsigned char* dest, SInt32 value){
	dest[0] = static_cast<UInt8>(value>>16);
	dest[1] = static_cast<UInt8>(value>>8);
	dest[2] = static_cast<UInt8>(value);
}
//SR SC: T, I, D [JHL 20040924]
//SR DI: DP10, E5, D5 [JHL 20040924] {dread_score: 6.66666666666667}  dest[i] possibly causes some problems if &dest[i] is invalid  
//SR MS: M [DDT 20041021] Reverted adding destination buffer size to guard buffer overrun.
inline void
store4(unsigned char* dest, SInt32 value){
	dest[0] = static_cast<UInt8>(value>>24);
	dest[1] = static_cast<UInt8>(value>>16);
	dest[2] = static_cast<UInt8>(value>>8);
	dest[3] = static_cast<UInt8>(value);
}
//SR SC: T, I, D [JHL 20040924]
//SR DI: DP10, E5, D5 [JHL 20040924] {dread_score: 6.66666666666667}  dest[i] possibly causes some problems if &dest[i] is invalid  
//SR SC: T, I, D [JAW 20040924] - Using store2 could cause buffer overflow if reciever is too small and should not used 
//SR MS: M [DDT 20041021] Reverted adding destination buffer size to guard buffer overrun.
inline void
store2(unsigned char* dest, SInt16 value){
	dest[0] = static_cast<UInt8>(value>>8);
	dest[1] = static_cast<UInt8>(value);
}

inline void
store8(char* dest, UInt64 value) { store8(reinterpret_cast<unsigned char*>(dest), value); }

inline void
store3(char* dest, SInt32 value) { store3(reinterpret_cast<unsigned char*>(dest), value); }

inline void
store4(char* dest, SInt32 value) { store4(reinterpret_cast<unsigned char*>(dest), value); }

inline void
store2(char* dest, SInt16 value) { store2(reinterpret_cast<unsigned char*>(dest), value); }

#endif

//	*** to do: replace these with whatever intrinsics are supported by the compiler.

	#define SWAP16(value)				 \
			((((static_cast<UInt16>(value))<<8) & 0xFF00)   | \
			 (((static_cast<UInt16>(value))>>8) & 0x00FF))

	#define SWAP24(value)				 \
			((((static_cast<UInt16>(value))<<8) & 0xFF0000)   | \
			 (((static_cast<UInt16>(value)) ) & 0x00FF00)   | \
			 (((static_cast<UInt16>(value))>>8) & 0x0000FF))

	#define SWAP32(value)					 \
			((((static_cast<UInt32>(value))<<24) & 0xFF000000)  | \
			 (((static_cast<UInt32>(value))<< 8) & 0x00FF0000)  | \
			 (((static_cast<UInt32>(value))>> 8) & 0x0000FF00)  | \
			 (((static_cast<UInt32>(value))>>24) & 0x000000FF))
	#define SWAP64(value)					 			\
			((((static_cast<UInt64>(value))<<56) & (static_cast<UInt64>(0xFF) << 56)) | 	\
			 (((static_cast<UInt64>(value))<<40) & (static_cast<UInt64>(0xFF) << 48)) | 	\
			 (((static_cast<UInt64>(value))<<24) & (static_cast<UInt64>(0xFF) << 40)) | 	\
			 (((static_cast<UInt64>(value))<< 8) & (static_cast<UInt64>(0xFF) << 32)) | 	\
			 (((static_cast<UInt64>(value))>> 8) & (static_cast<UInt64>(0xFF) << 24)) | 	\
			 (((static_cast<UInt64>(value))>>24) & (static_cast<UInt64>(0xFF) << 16))	| 	\
			 (((static_cast<UInt64>(value))>>40) & (static_cast<UInt64>(0xFF) << 8))  | 	\
			 (((static_cast<UInt64>(value))>>56) & (static_cast<UInt64>(0xFF))))

#define MAKE_TAG(a,b,c,d) static_cast<UInt32>(((a & 0xff) << 24) |		\
	((b & 0xff) << 16) |							\
	((c & 0xff) << 8)  |							\
	(d & 0xff))

#define MAKE_TAG2(a,b) static_cast<UInt16>(((a & 0xff) << 8) |		\
	(b & 0xff))

//SR SC: T, I, D [JAW 20040924] - These macros are risky if not used correctly could cause buffer overflow if reciever is too small and should not used 
//SR DI: DP6, E2, D1 [JAW 20040924] {dread_score: 3} 
//SR MS: M [DDT 20041005] Lower the dread_score since even the wrong usage could cause application crash, but it's not easy to attck by swapping bytes in the memory.
//These macros get/put a value TO/from a known Big Endian value
#if BigEnd__

	#define FROMBE64(x) (x)
	#define FROMBE32(x) (x)
	#define FROMBE24(x) (x)
	#define FROMBE16(x) (x)
	#define TOBE64(x) (x)
	#define TOBE32(x) (x)
	#define TOBE24(x) (x)
	#define TOBE16(x) (x)


#else

	#define FROMBE64(x) SWAP64(x)
	#define FROMBE32(x) SWAP32(x)
	#define FROMBE24(x) SWAP24(x)
	#define FROMBE16(x) SWAP16(x)
	#define TOBE64(x) SWAP64(x)
	#define TOBE32(x) SWAP32(x)
	#define TOBE24(x) SWAP24(x)
	#define TOBE16(x) SWAP16(x)

#endif

#define GET64FROMBE(x) FROMBE64(x)
#define GET32FROMBE(x) FROMBE32(x)
#define GET24FROMBE(x) FROMBE24(x)
#define GET16FROMBE(x) FROMBE16(x)

#ifdef __cplusplus
inline SInt16 toBE16(SInt16 x){ return TOBE16(x); };
inline SInt32 toBE24(SInt32 x){ return TOBE24(x); };
inline SInt32 toBE32(SInt32 x){ return TOBE32(x); };
inline UInt64 toBE64 (UInt64 x){ return TOBE64(x); };
inline SInt16 fromBE16(SInt16 x){ return FROMBE16(x); };
inline SInt32 fromBE24(SInt32 x){ return FROMBE24(x); };
inline SInt32 fromBE32(SInt32 x){ return FROMBE32(x); };
inline UInt64 fromBE64(UInt64 x){ return FROMBE64(x); };
#endif


//These macros get value FROM a known Little Endian value
#if BigEnd__
	#define FROMLE64(x) SWAP64(x)
	#define FROMLE32(x) SWAP32(x)
	#define FROMLE24(x) SWAP24(x)
	#define FROMLE16(x) SWAP16(x)
	#define TOLE64(x) SWAP64(x)
	#define TOLE32(x) SWAP32(x)
	#define TOLE24(x) SWAP24(x)
	#define TOLE16(x) SWAP16(x)
#else
	#define FROMLE64(x) (x)
	#define FROMLE32(x) (x)
	#define FROMLE24(x) (x)
	#define FROMLE16(x) (x)
	#define TOLE64(x) (x)
	#define TOLE32(x) (x)
	#define TOLE24(x) (x)
	#define TOLE16(x) (x)
#endif
#define GET64FROMLE(x) FROMLE64(x)
#define GET32FROMLE(x) FROMLE32(x)
#define GET24FROMLE(x) FROMLE24(x)
#define GET16FROMLE(x) FROMLE16(x)

#ifdef __cplusplus
inline SInt16 toLE16(UInt16 x){ return TOLE16(x); };
inline SInt32 toLE24(UInt32 x){ return TOLE24(x); };
inline SInt32 toLE32(UInt32 x){ return TOLE32(x); };
inline UInt64 toLE64(UInt64 x){ return TOLE64(x); };
inline SInt16 fromLE16(SInt16 x){ return FROMLE16(x); };
inline SInt32 fromLE32(SInt32 x){ return FROMLE32(x); };
inline SInt32 fromLE24(SInt32 x){ return FROMLE24(x); };
inline UInt64 fromLE64(UInt64 x){ return FROMLE64(x); };
#endif

#endif
