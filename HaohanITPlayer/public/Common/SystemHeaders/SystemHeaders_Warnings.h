//-----------------------------------------------------------------------------
// SystemHeaders_Warning.h
// Copyright (c) 2002 - 2005, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

#pragma warning(error:4013)			// function' undefined - assuming extern returning int
#pragma warning(error:4019)			// empty statement at global scope
#pragma warning(error:4032)			// function arg has different type from declaration
#pragma warning(error:4056)			// overflow in floating point constant arithmetic
#pragma warning(error:4057)			// pointers refer to different base types
#pragma warning(error:4061)			// enumerated identifier in switch of enum not explicitely handled by case label
#pragma warning(error:4071)			// no function prototype given - formals unspecified
#pragma warning(error:4092)			// 'sizeof' value too big
#pragma warning(error:4096)			// '__cdecl' must be used with '...'
#pragma warning(error:4100)			// Unreferenced formal parameter
#pragma warning(error:4101)			// Unreferenced local variable
#pragma warning(error:4121)			// space added for structure alignment
#pragma warning(error:4125)			// decimal digit terminates octal escape
#pragma warning(error:4130)			// logical operation on address of string constant
#pragma warning(error:4131)			// old-style function declarator
#pragma warning(error:4132)			// const object not initialized
#pragma warning(error:4152)			// pointer conversion between function and data
#pragma warning(error:4177)			// pragma data_seg s/b at global scope
#pragma warning(error:4189)			// local variable is initialized but not used
#pragma warning(error:4191)			// unsafe conversion from type of expression to type required
#pragma warning(error:4197)			// illegal use of const/volatile: qualifier ignored (disabled until sources fixed)
#pragma warning(error:4203)			// Truncation from type1 t type2
#pragma warning(error:4206)			// Source File is empty
#pragma warning(error:4208)			// delete[exp] - exp evaluated but ignored
#pragma warning(error:4211)			// extern redefined as static
#pragma warning(error:4212)			// function declaration used ellipsis
#pragma warning(error:4213)			// cast on left side of = is non-standard
#pragma warning(error:4222)			// member function at file scope shouldn't be static
#pragma warning(error:4234)			// keyword not supported or reserved for future
#pragma warning(error:4235)			// keyword not supported or reserved for future
#pragma warning(error:4239)			// standard doesn't allow this conversion -- VC++ Compiler doesn't pick the right forms and generates incorrect warnings -jmr
#pragma warning(error:4242)			// convertion possible loss of data
#pragma warning(error:4251)			// ****** temp ******
#pragma warning(error:4254)			// conversion from type1 to type2 possible loss of data
#pragma warning(error:4259)			// pure virtual function was not defined
#pragma warning(error:4263)			// Derived override doesn't match base - who cares...
#pragma warning(error:4264)			// base function is hidden - again who cares.
#pragma warning(error:4265)			// Class has a virtual method but destructor is not virtual
#pragma warning(error:4266)			// no override available for virtual member function from base class; function is hidden
#pragma warning(error:4268)			// const object not initialized
#pragma warning(error:4274)			// #ident ignored
#pragma warning(error:4287)			// unsigned/negative constant mismatch
#pragma warning(error:4288)			// nonstandard extension used (loop counter)
#pragma warning(error:4290)			// disable warning for unimplemented exception specification  -jmr
#pragma warning(error:4296)			// expression is always true -- Don't know why I needed to add this to kick on the warnings. -jmr
#pragma warning(error:4312)			// conversion to type of greater size
#pragma warning(error:4407)			// NTBUG#47623error:  Silent codegen bug is a warning
#pragma warning(error:4504)			// type ambiguous; simplify code
#pragma warning(error:4505)			// unreferenced local function removed
#pragma warning(error:4507)			// explicit linkage specified after default linkage
#pragma warning(error:4509)			// use of SEH with destructor
#pragma warning(error:4515)			// namespace uses itself
#pragma warning(error:4516)			// access declarations are deprecated
#pragma warning(error:4517)			// access declarations are deprecated
#pragma warning(error:4529)			// forming a pointer-to-member requires explicit use of the address-of operator ('&') and a qualified name
#pragma warning(error:4532)			// jump out of __finally block
#pragma warning(error:4551)			// Function call missing argument list
#pragma warning(error:4552)			// <<, >> ops used to no effect (probably missing an = sign)
#pragma warning(error:4553)			// == op used w/o effect (probably s/b an = sign)
#pragma warning(error:4623)			// default constructor could not be generated because a base class default constructor is inaccessible
#pragma warning(error:4625)			// copy constructor could not be generated because a base class copy constructor is inaccessible
#pragma warning(error:4626)			// assignment operator could not be generated because a base class assignment operator is inaccessible
#pragma warning(error:4670)			// base class of thrown object is inaccessible
#pragma warning(error:4671)			// copy ctor of thrown object is inaccessible
#pragma warning(error:4673)			// thrown object cannot be handled in catch block
#pragma warning(error:4674)			// dtor of thrown object is inaccessible
#pragma warning(error:4700)			// Local used w/o being initialized
#pragma warning(error:4701)			// local variable used without being initialized
#pragma warning(error:4702)			// Unreachable code
#pragma warning(error:4705)			// statement has no effect (example: a+1;)
#pragma warning(error:4706)			// if (a=b) instead of (if a==b)
#pragma warning(error:4709)			// comma in array subscript
#pragma warning(error:4714)			// function marked as __forceinline not inlined
#pragma warning(error:4786)			//"debug string too int" (dpo)
#pragma warning(error:4806)			// unsafe operation involving type 'bool'
#pragma warning(error:4928)			// illegal copy-initialization; more than one user-defined conversion has been implicitly applied
#pragma warning(error:4946)			// reinterpret_cast used between related classes: 'class1' and 'class2'


// stuff i had to remove - hvh
#pragma warning(disable:4061)		// not all enum values tested in switch statement
#pragma warning(disable:4217)		// member template functions cannot be used for copy-assignment or copy-construction
#pragma warning(disable:4267)		// convertion from size_t to smaller type - messes up vector<bool>
#pragma warning(disable:4324)		// structure padded due to __declspec(align())
#pragma warning(disable:4356)		// static member cannot be initialized via derived class
#pragma warning(disable:4514)		// unreferenced inline function has been removed
#pragma warning(disable:4640)		// construction of local static object not thread safe
#pragma warning(disable:4668)		// symbol is not defined as a preprocessor macro, replacing with '0'
#pragma warning(disable:4675)		// picked overload found via Koenig lookup
#pragma warning(disable:4917)		// A GUID can only be associated with a class, interface or namespace

// Temporarily disabled
#pragma warning(disable:4625)			// copy constructor could not be generated because a base class copy constructor is inaccessible
#pragma warning(disable:4626)			// assignment operator could not be generated because a base class assignment operator is inaccessible

// There is nothing we can do about these errors
#pragma warning(disable:4068)		// disable bad pragma warning.
#pragma warning(disable:4503)		// decorated name length exceeded, name was truncated.
#pragma warning(disable:4710)		// inline function was not inlined
#pragma warning(disable:4786)		// identifier was truncated to 255 characters in debug information.

