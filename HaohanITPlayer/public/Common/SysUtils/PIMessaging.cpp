//-----------------------------------------------------------------------------
// PIMessaging.cpp
// Copyright (c) 2002 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------
/*********************************************************************

	PIMessaging.h - Header files for PIEvent-related classes

	History: 2/26/02 sds - first created

*********************************************************************/

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#include "PIMessaging.h"
#include "safe_ptr.h"
// static variables:
safe_ptr<PIEventRegistry>	PIEventRegistry::pTheRegistry(0);



PIEvent::~PIEvent() 
{
}

PIEventHandler::~PIEventHandler()
{ 
	UnregisterEvents(); 
}

// mutators
void PIEventHandler::RegisterEvents(PIEventRegistry *pRegistry)
{
	PIEventRegistry::ValidateSystemRegistry();
	// if we're already registered with someone else, bail!
	assert(mpRegistry == 0);
	// make sure there's a registry to work on
	// now that we know there's a registry available, if
	// we were asked to use the default registry, get
	// a (now valid) pointer to it.
	if (pRegistry == 0)
		pRegistry = PIEventRegistry::GetRegistry();
	
    if (pRegistry->GetNamedEventHandler(GetName()) == 0)
        // register ourselves with whichever registry we got
	    pRegistry->RegisterEvents(*this);

	// and remember who we're registered with
	mpRegistry = pRegistry;
}

void PIEventHandler::UnregisterEvents()
{
	if (mpRegistry && PIEventRegistry::GetRegistry()) {
	// if we were registered with anyone, and the system registry
	// still exists...
		mpRegistry->UnregisterEvents(*this);	// unregister ourselves
		mpRegistry = 0;					// and clear our pointer
	}
}

// utility functions
void PIEventHandler::SendEvent(PIEvent const &rEvent, PIEventHandler *pDestination)
{
	assert(mpRegistry);					// we should already be registered...
	assert(pDestination);				// no sending things to null pointers
	// have the registry send the event
	mpRegistry->SendEvent(rEvent, pDestination, this);
}



// constructors/destructors
PIEventRegistry::PIEventRegistry()
	: mHandlers(), mDestructing(false)
{ }

PIEventRegistry::~PIEventRegistry()
{
	try {
		// OK, this is way paranoid, but you never know...
		assert(pTheRegistry.get() == this);

		SystemSpinLock	spinLock(pTheRegistry->mLock);	// make thread safe
		// the desructing flag is used so the Unregsiter method won't
		// block on the already-held (above) spinlock
		mDestructing = true;
		// make sure nobody still thinks they're registered...
		// yes, this looks odd, but it is correct.  Unregister
		// removes the item from the list...
		while (mHandlers.begin() != mHandlers.end()) {
			mHandlers.begin()->first->UnregisterEvents();
		}
		// clear the system registry pointer
		//	pTheRegistry = 0;
	} catch (std::exception x) {
		// never throw from a destructor (except bus errors and junk like that)
	}
}

void PIEventRegistry::RegisterEvents(PIEventHandler &rEventHandler)
{
	SystemSpinLock	spinLock(mLock);	// make thread safe
#if _DEBUG || defined (DEBUG)
	// if the event handler has a name, ensure it's unique
	if (rEventHandler.GetName().length() != 0)
		assert(NamedEventHandler(rEventHandler.GetName()) == 0);
#endif
	// add it to the list, or update it's name if it's already in the list
	mHandlers[&rEventHandler] = &rEventHandler.GetName();
}

void PIEventRegistry::UnregisterEvents(PIEventHandler &rEventHandler)
{
	// I could be paranoid and check for the item's existence, but since
	// remove won't complain, and will remove all copies if it somehow
	// got registered more than once, the state of the system after
	// the remove is always what was desired.  No harm, no foul.
	ValidateSystemRegistry();						// make sure the registry exists
	// have to use a safe pointer, so that we can either have, or not have,
	// a spinlock based on the mDestructing flag.  Aren't threads fun??
	safe_ptr<SystemSpinLock>	pLock(0);
	if (!mDestructing) {
		pLock = new SystemSpinLock(pTheRegistry->mLock);
	}
	pTheRegistry->mHandlers.erase(&rEventHandler);
}

PIEventHandler* PIEventRegistry::NamedEventHandler(std::string const &rString)
{
	// if the name is empty, always return 0
	if (rString.length()) {
		ValidateSystemRegistry();						// make sure the registry exists
		// go through the list looking for the name...
		std::map<PIEventHandler*, std::string const *>::iterator	iter = pTheRegistry->mHandlers.begin();
		while (iter != pTheRegistry->mHandlers.end()) {
			if (*(iter->second) == rString)
				return(iter->first);
			++iter;
		}
	}
	return(0);
}

// static functions:
PIEventHandler* PIEventRegistry::GetNamedEventHandler(std::string const &rString)
{
	SystemSpinLock	spinLock(pTheRegistry->mLock);	// make thread safe
	return(pTheRegistry->NamedEventHandler(rString));
}

void PIEventRegistry::SendEvent(PIEvent const &rEvent, PIEventHandler *pDestination, PIEventHandler *pOrigin)
{
	ValidateSystemRegistry();
#if _DEBUG || defined (DEBUG)
	{
	// this part needs to be thread-safed, but the call to the destination's code does not.
	// and since this only gets checked in debug mode, the lock only happens in debug mode.
		SystemSpinLock	spinLock(pTheRegistry->mLock);	// make thread safe
		// in debug mode, ensure the destination is valid
		assert(pTheRegistry->mHandlers.find(pDestination) != pTheRegistry->mHandlers.end());
		if (pOrigin) {
		// and if there's an origin, make sure it's valid, too
			assert(pTheRegistry->mHandlers.find(pOrigin) != pTheRegistry->mHandlers.end());
		}
	}
#endif
	pDestination->ProcessEvent(&rEvent, pOrigin);
}

void PIEventRegistry::BroadcastEvent(PIEvent const &rEvent)
{
	ValidateSystemRegistry();
	SystemSpinLock	spinLock(pTheRegistry->mLock);	// make thread safe
	// send all registered objects the event
	std::map<PIEventHandler*, std::string const *>::iterator	iter = pTheRegistry->mHandlers.begin();
	while (iter != pTheRegistry->mHandlers.end()) {
		iter->first->ProcessEvent(&rEvent, static_cast<PIEventHandler*>(0));
		++iter;
	}
}

void PIEventRegistry::ValidateSystemRegistry()
{
	// if there's already a registry, just return
	if (pTheRegistry.get())
		return;
	SpinLockFlag	lock;			// important that we're threadsafe!
	SystemSpinLock	spinLock(lock);
	PIEventRegistry *pRegistry = new PIEventRegistry();
	// if ever this routine is added to, the assignment to
	// the global has to be the last thing we do, 'cause the
	// check for pTheRegistry's existance is not thread-safed.
	// It's not threadsafe so that calling this isn't unduly
	// expensive, but it means that the assignment of the
	// registry pointer has to be atomic, and the last thing
	// we do.  (wow, a comment longer than the routine. sigh.)
	pTheRegistry = pRegistry;
}

