/******************************************************************************

 Copyright (c) 1999 Advanced Micro Devices, Inc.

 LIMITATION OF LIABILITY:  THE MATERIALS ARE PROVIDED *AS IS* WITHOUT ANY
 EXPRESS OR IMPLIED WARRANTY OF ANY KIND INCLUDING WARRANTIES OF MERCHANTABILITY,
 NONINFRINGEMENT OF THIRD-PARTY INTELLECTUAL PROPERTY, OR FITNESS FOR ANY
 PARTICULAR PURPOSE.  IN NO EVENT SHALL AMD OR ITS SUPPLIERS BE LIABLE FOR ANY
 DAMAGES WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS,
 BUSINESS INTERRUPTION, LOSS OF INFORMATION) ARISING OUT OF THE USE OF OR
 INABILITY TO USE THE MATERIALS, EVEN IF AMD HAS BEEN ADVISED OF THE POSSIBILITY
 OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE EXCLUSION OR LIMITATION
 OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES, THE ABOVE LIMITATION MAY
 NOT APPLY TO YOU.

 AMD does not assume any responsibility for any errors which may appear in the
 Materials nor any responsibility to support or update the Materials.  AMD retains
 the right to make changes to its test specifications at any time, without notice.

 NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
 further information, software, technical information, know-how, or show-how
 available to you.

 So that all may benefit from your experience, please report  any  problems
 or  suggestions about this software to 3dsdk.support@amd.com

 AMD Developer Technologies, M/S 585
 Advanced Micro Devices, Inc.
 5900 E. Ben White Blvd.
 Austin, TX 78741
 3dsdk.support@amd.com

*****************************************************************************

 WinCPUDetect.h

 AMD3D 3D library code: CPU Feature detection

*****************************************************************************/
//SR FS: Reviewed [wwt 20040914] 
//SR FS: Reviewed [DDT 20040928] Second pass.
//SR FS: Reviewed [JAW 20040928] 

#if defined _WIN32

// Miscellaneous routines
#ifndef _AMD_DETECT_H
#define _AMD_DETECT_H

#ifdef __cplusplus
extern "C" {
#endif

// Detected CPU Vendors - returned by GetCPUCaps (CPU_VENDOR_STRING);
typedef enum CPU_VENDORS
{
    VENDOR_UNKNOWN	= 0,
    VENDOR_AMD		= 1,
    VENDOR_INTEL	= 2,
    VENDOR_CYRIX	= 3,
    VENDOR_CENTAUR	= 4
} CPU_VENDORS;

// Detected CPU models - returned by GetCPUCaps (CPU_TYPE);
typedef enum CPU_TYPES
{
    UNKNOWN				= 0,
    AMD_Am486			= 1,
    AMD_K5				= 2,
    AMD_K6				= 3,
    AMD_K6_2			= 4,
    AMD_K6_3			= 5,
    AMD_ATHLON			= 6,

    INTEL_486DX			= 7,
    INTEL_486SX			= 8,
    INTEL_486DX2		= 9,
    INTEL_486SL			= 10,
    INTEL_486SX2		= 11,
    INTEL_486DX2E		= 12,
    INTEL_486DX4		= 13,
    INTEL_Pentium		= 14,
    INTEL_Pentium_MMX	= 15,
    INTEL_Pentium_Pro	= 16,
    INTEL_Pentium_II	= 17,
    INTEL_Celeron		= 18,
    INTEL_Pentium_III	= 19,
} CPU_TYPES;


// Detected CPU capabilities - used as input to the GetCPUCaps() function
typedef enum CPUCAPS
{
    // Synthesized values
    CPU_VENDOR			= 0,		// Manufacturer (returns enum CPU_VENDORS)
    CPU_TYPE			= 1,		// CPU type (return enum CPU_TYPES)
    CPU_VENDOR_STRING	= 2,		// CPU vendor name string (return const char *)
    CPU_NAME_STRING		= 3,		// CPU Processor string (extended functions 0x80000002 - 0x80000004, return const char *)

    // Processor Features - returned as boolean values
    HAS_CPUID			= 104,		// Supports CPUID instruction
    HAS_FPU				= 105,      // FPU present
    HAS_VME				= 106,      // Virtual Mode Extensions
    HAS_DEBUG			= 107,		// Debug extensions
    HAS_PSE				= 108,      // Page Size Extensions
    HAS_TSC				= 109,      // Time Stamp Counter
    HAS_MSR				= 110,      // Model Specific Registers
    HAS_PAE				= 111,      // Page Address Extensions
    HAS_MCE				= 112,      // Machine Check Extensions
    HAS_CMPXCHG8		= 113,		// CMPXCHG8 instruction
    HAS_APIC			= 114,      // APIC
    HAS_SYSENTER		= 115,		// SYSENTER/SYSEXIT instruction
    HAS_MTRR			= 116,      // Memory Type Range Registers
    HAS_GPE				= 117,      // Global Paging Extensions
    HAS_MCA				= 118,      // Machine Check Architecture
    HAS_CMOV			= 119,      // CMOV instruction
    HAS_PAT				= 120,      // Page Attribue Table
    HAS_PSE36			= 121,		// PSE36 (Page Size Extensions)

    HAS_MMX_EXT			= 122,		// MMX Extensions
    HAS_MMX				= 123,      // MMX support
    HAS_FXSAVE			= 124,		// FXSAVE/FXRSTOR instruction

    HAS_3DNOW_EXT		= 125,		// Extended 3DNow! support
    HAS_3DNOW			= 126,		// 3DNow! support

    HAS_SSE_MMX			= 127,		// SSE MMX support (same as HAS_MMXEXT)
    HAS_SSE				= 128,      // SSE
    HAS_SSE_FP			= 129,		// SSE FP support

    // Cache parameters (not all values apply to all cpus)
    CPU_L1_DTLB_ASSOC	= 230,		// L1 Data Cache TLB Associativity
    CPU_L1_DTLB_ENTRIES	= 231,		// L1 Data Cache TLB Entries
    CPU_L1_ITLB_ASSOC	= 232,		// L1 Instruction Cache TLB Associativity (0xff = full associativity)
    CPU_L1_ITLB_ENTRIES	= 233,		// L1 Instruction Cache TLB Entries

    CPU_L1_EDTLB_ASSOC	= 234,		// Extended (2/4 Mbyte) L1 Data Cache TLB Associativity (0xff = full associativity)
    CPU_L1_EDTLB_ENTRIES= 235,		// Extended (2/4 Mbyte) L1 Data Cache TLB Entries
    CPU_L1_EITLB_ASSOC	= 236,		// Extended (2/4 Mbyte) L1 Instruction Cache TLB Associativity
    CPU_L1_EITLB_ENTRIES= 237,		// Extended (2/4 Mbyte) L1 Instruction Cache TLB Entries

    CPU_L1_DCACHE_SIZE	= 238,		// L1 Data Cache Size (kbytes)
    CPU_L1_DCACHE_ASSOC	= 239,		// L1 Data Cache Associativity (0xff = full associativity)
    CPU_L1_DCACHE_LINES	= 240,		// L1 Data Cache Lines
    CPU_L1_DCACHE_LSIZE	= 241,		// L1 Data Cache Line Size (bytes)

    CPU_L1_ICACHE_SIZE	= 242,		// L1 Instruction Cache Size (kbytes)
    CPU_L1_ICACHE_ASSOC	= 243,		// L1 Instruction Cache Associativity (0xff = full associativity)
    CPU_L1_ICACHE_LINES	= 244,		// L1 Instruction Cache Lines
    CPU_L1_ICACHE_LSIZE	= 245,		// L1 Instruction Cache Line Size (bytes)

    CPU_L2_CACHE_SIZE	= 346,		// L2 Unified Cache Size (Kbytes)
    CPU_L2_CACHE_ASSOC	= 347,		// L2 Unified Cache Associativity (0xf = full associativity)
    CPU_L2_CACHE_LINES	= 348,		// L2 Unified Cache Lines (lines per tag)
    CPU_L2_CACHE_LSIZE	= 349,		// L2 Unified Cache Line Size (bytes)

    CPU_L2_DTLB_ASSOC	= 350,		// L2 Data Cache TLB Associativity
    CPU_L2_DTLB_ENTRIES	= 351,		// L2 Data Cache TLB Entries
    CPU_L2_UTLB_ASSOC	= 352,		// L2 Instruction or Unified Cache TLB Associativity (0xf = full associativity)
    CPU_L2_UTLB_ENTRIES	= 353,		// L2 Instruction or Unified Cache TLB Entries

    CPU_L2_EDTLB_ASSOC	= 354,		// Extended (2/4 Mbyte) L2 Data Cache TLB Associativity (0xf = full associativity)
    CPU_L2_EDTLB_ENTRIES= 355,		// Extended (2/4 Mbyte) L2 Data Cache TLB Entries
    CPU_L2_EUTLB_ASSOC	= 356,		// Extended (2/4 Mbyte) L2 Instruction or Unified Cache TLB Associativity
    CPU_L2_EUTLB_ENTRIES= 357,		// Extended (2/4 Mbyte) L2 Instruction or Unified Cache TLB Entries

} CPUCAPS;

ULONG_PTR WinCPUDetectGetCPUCaps (CPUCAPS);

#ifdef __cplusplus
}
#endif

#endif

#endif

// eof

