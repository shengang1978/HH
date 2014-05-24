//-----------------------------------------------------------------------------
// PIMessaging.h
// Copyright (c) 2002 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------
/*********************************************************************

	PIMessaging.h - Header files for PIEvent-related classes

	History: 2/26/02 sds - first created

*********************************************************************/

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#include "SonicSpinLock.h"
#include "safe_ptr.h"

#include <map>
#include <cassert>
#include <string>

#ifndef __PIMessaging_h__
#define	__PIMessaging_h__

enum PIEventType {
	eSubclassEvent	= 0,		// use for events which subclass off of PIEvent
	eGenericEvent	= 1
};

class PIEventRegistry;
class PIEventHandler;

class PIEvent {
	public:
	// constructors/destructors
		PIEvent(PIEventType type, ptrdiff_t data1, ptrdiff_t data2)
			: mType(type)
			{ mData[0] = data1; mData[1] = data2; }
		PIEvent(const PIEvent &rOther)
			: mType(rOther.mType)
			{ mData[0] = rOther.mData[0]; mData[1] = rOther.mData[1]; }
		virtual ~PIEvent();

	// setters:
		void	SetData1(ptrdiff_t d)
					{ mData[0] = d; }
		void	SetData2(ptrdiff_t d) 
					{ mData[1] = d; }

	// accessors:
		PIEventType	GetType() const
					{ return(mType); }
		ptrdiff_t	GetData1() const
					{ return(mData[0]); }
		ptrdiff_t	GetData2() const
					{ return(mData[1]); }
		ptrdiff_t	GetData(ptrdiff_t index) const
					{ assert(index >= 0); assert(index < 2); return(mData[index]); }

	// operator overloads:
		PIEvent& operator=(PIEvent const &rhs);

	private:
		PIEventType	mType;
		ptrdiff_t	mData[2];
};

class PIEventHandler {
	public:
		PIEventHandler() : mpRegistry(0), mName("") {};
// **CodeWizzard** - Informational:  More Effective C++ item 5  - Be wary of user-defined conversion function
		PIEventHandler(std::string const &rName) : mpRegistry(0), mName(rName) {};
		virtual ~PIEventHandler();
	// subclasses must override this
		virtual void ProcessEvent(PIEvent const *pEvent, PIEventHandler *pOrigin) = 0;

	// accessors
		std::string const &GetName() const
							{ return(mName); }
	// mutators
		void	RegisterEvents(PIEventRegistry *pRegistry = 0);	// null registry indicates "use default registry"
		void	UnregisterEvents();

	// utility functions
		void	SendEvent(PIEvent const &rEvent, PIEventHandler *pDestination);

	private:
		mutable SpinLockFlag	mLock;			// important that we're threadsafe!
		PIEventRegistry	*mpRegistry;
		std::string		mName;

};

// **CodeWizzard** - Violation:  Universal Coding Standards item 16  - Do not specify public data member
class PIEventRegistry {
	public:
		friend class PIEventHandler;

	// constructors/destructors
		PIEventRegistry();
		virtual ~PIEventRegistry();

	// static functions:
		static void	SendEvent(PIEvent const &rEvent, PIEventHandler *pDestination)
						{ SendEvent(rEvent, pDestination, static_cast<PIEventHandler*>(0)); }
		static void BroadcastEvent(PIEvent const &rEvent);

		static PIEventHandler	*GetNamedEventHandler(std::string const &rString);
		static PIEventRegistry	*GetRegistry()
									{ return(pTheRegistry.get()); }

	protected:
	// current implementation is as a singleton
		static	safe_ptr<PIEventRegistry>	pTheRegistry;

	// checks for an existing registry, and if none found, constructs one
	// note that this function is _called_ from thread-safe routines, but
	// is not, itself, thread-safe.  You have been warned(tm)
		static void	ValidateSystemRegistry();
		static void	SendEvent(PIEvent const &rEvent, PIEventHandler *pDestination, PIEventHandler *pOrigin);
	// EventHandlers are our friends, and can call these.
		void			RegisterEvents(PIEventHandler &rEventHandler);
		void			UnregisterEvents(PIEventHandler &rEventHandler);
		PIEventHandler	*NamedEventHandler(std::string const &rString);


		std::map<PIEventHandler*, std::string const *>	mHandlers;
		mutable SpinLockFlag							mLock;			// important that we're threadsafe!
		bool											mDestructing;	// true during the destructor

	private:
		// private and unimplemented so we can keep the compiler from making them for us...
		PIEventRegistry(const PIEventRegistry &rOther);
		PIEventRegistry& operator=(PIEventRegistry const &rhs);
};

#endif

