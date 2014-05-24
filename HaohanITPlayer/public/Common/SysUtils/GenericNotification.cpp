//-----------------------------------------------------------------------------
// GenericNotification.cpp
// Copyright (c) 2003 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040915]: safe input param; no MBCS; no dangerous API; no registry/sys folder/temp file 

#include "GenericNotification.h"

#include <algorithm>

/*
 *	GenericNotification::Notifiee Class
 */

GenericNotification::Notifiee::Notifiee() : mNotifier(0)
{
}

GenericNotification::Notifiee::~Notifiee()
{ 
	try
	{
		Disconnect();
	}
	catch (std::exception&)
	{
		// This usually means that the notifier was destroyed before the notifiee
		// was, so unregistration isn't necessary.
	}
}

void GenericNotification::Notifiee::Connect(Notifier* inNotifier)
{
	// Connect the notifiee to a notifier. If it's already connected to one, 
	// disconnect it first:
	Disconnect();

	mNotifier = inNotifier;
	if (mNotifier)
		mNotifier->RegisterForNotification(*this);
}

void GenericNotification::Notifiee::Disconnect()
{
	if (mNotifier)
	{
		mNotifier->UnregisterFromNotification(*this); 
		mNotifier = 0;
	}
}




/*
 *	Notifier Class
 */

GenericNotification::Notifier::Notifier()
{
}

GenericNotification::Notifier::~Notifier()
{
}

void GenericNotification::Notifier::RegisterForNotification(Notifiee& inNotifiee)
{
	// Add the notifiee to the list if it's not already there:
	if (std::find(mNotifiees.begin(), mNotifiees.end(), &inNotifiee) == mNotifiees.end())
		mNotifiees.push_back(&inNotifiee);

	// Give the derived notifiers an opportunity to bring this notifiee up to date
	// on current status:
	NotifyOnRegister(inNotifiee);
}

void GenericNotification::Notifier::UnregisterFromNotification(Notifiee& inNotifiee)
{
	// Remove the given notifiee from the list:
	NotifieesVector::iterator notifiee = std::find(mNotifiees.begin(), mNotifiees.end(), &inNotifiee);

	if (notifiee != mNotifiees.end())
		mNotifiees.erase(notifiee);
}

/*
*	The NotifyFunctor class allows us to pass a non-virtual class into the for_each method.  
*  Clients override virtual methods in NotifyFunctor's ClientNotice object instead.
*/
class GenericNotification::NotifyFunctor
{
public:
	NotifyFunctor(const ClientNotice& inClientNotice) : 
	  mClientNotice(inClientNotice) {}
	void operator()(Notifiee* inNotifiee) const
	{
		try 
		{
			mClientNotice(inNotifiee);
		}
		catch (...)
		{
			assert(false);	// this is a programming error -- notification methods should not throw
		}
	}

private:
	const NotifyFunctor& operator=(const NotifyFunctor&);	// Assignment is not allowed

	const ClientNotice& mClientNotice;
};

void GenericNotification::Notifier::Notify(const ClientNotice& inClientNotice) const
{
	// Create a NotifyFunctor object, using the specific ClientNotice object that was 
	// passed in.  NotifyFunctor uses this ClientNotice to call the appropriate method in the
	// Notifiee object.  For_each requires a functor with no virtual methods.  We get around this
	// limitation by constructing it with the a ClientNotice object, which does have virtual methods.

	// Send the client notice to all notifiees:
	std::for_each(mNotifiees.begin(), mNotifiees.end(), NotifyFunctor(inClientNotice));
}

void GenericNotification::Notifier::NotifyOnRegister(Notifiee& /*inNotifiee*/) const
{
	// Base class does nothing.  Derived classes can implement this to bring the just-registered
	// notifiee up to date on current status, usually by calling all of its notification methods.
}


GenericNotification::ClientNotice::ClientNotice()
{
}

GenericNotification::ClientNotice::~ClientNotice()
{
}

