//-----------------------------------------------------------------------------
// GenericNotification.h
// Copyright (c) 2003 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#if !defined(_GenericNotification_h_)
#define _GenericNotification_h_

#include <vector>

/*
 *	The GenericNotification namespace consists of two base classes, Notifier and Notifiee, that define a simple, 
 *  non editor-based notification system.  The Notifier class allows clients (Notifiees) to register for notification.  
 *	Subclasses of Notifier must define the events that trigger that notification.  Notifiee subclasses define the 
 *	types of notifications that will occur.  These subclasses should be created as Notifier/Notifiee pairs.  Clients
 *  create an instance of the desired Notifiee class and register for notifications with the corresponding Notifier
 *  class.
 *
 *	Two additional classes, ClientNotice and NotifyFunctor, support the notification process.  NotifyFunctor is used
 *	by the Notifier class to iterate over its list of Notifiees, sending notifications.  ClientNotice is an abstract
 *	class that represents client notices.  One ClientNotice object should be created for each notification method
 *  defined in the client's Notifiee class.  The job of the each ClientNotice subclass is to call the appropriate
 *  Notifiee subclass notification method. A sample implementation is shown below.
 */

namespace GenericNotification
{

	class Notifier;
	class ClientNotice;

	class Notifiee
	{
	public:
		Notifiee();
		virtual ~Notifiee();	// Virtual; this really is a base class

		virtual void Connect(Notifier* inNotifier);	// Connects the notifiee to a notifier
		void Disconnect();

	private:
		Notifiee(const Notifiee&);					// Copy construction is not allowed
		const Notifiee& operator=(const Notifiee&);	// Assignment is not allowed

		Notifier* mNotifier;	// Points to the connected notifier object, if any
	};

	class Notifier
	{
	public:
		Notifier();
		virtual ~Notifier();	// Virtual; this really is a base class

	public:
		// This member provides the way to clean up any internal members to
		// their initial values.
// **CodeWizzard** - Violation:  More Effective C++ item 24  - Understand the costs of virtual functions ...
		virtual void Reset() {;}  

	protected:
		// This method notifies all registered notifiees:
		void Notify(const ClientNotice& inClientNotice) const;

		// This method sends a notification only to the given notifiee.  It is called
		// automatically when a notifiee registers, but does nothing unless the derived 
		// Notifier class implements it:
		virtual void NotifyOnRegister(Notifiee& inNotifiee) const;

	private:
		typedef std::vector<Notifiee*> NotifieesVector;

		// Allow notifiees to register and unregister:
		friend class Notifiee;

		void RegisterForNotification(Notifiee& inNotifiee);
		void UnregisterFromNotification(Notifiee& inNotifiee);

		Notifier(const Notifier&);					// Copy construction not allowed
		Notifier& operator=(const Notifier&);	// Assignment is not allowed

		NotifieesVector mNotifiees;
	};


	class ClientNotice
	{
	public:
		ClientNotice();
		virtual ~ClientNotice();	// Virtual;  This is a base class. 

	protected:
		virtual void operator()(Notifiee* inNotifiee) const = 0;

		// Sample implementation of this method:
		/*
			void MyNotice::operator()(Notifiee* inNotifiee)
			{
				// Cast to the correct notifiee subclass and call the desired notifiee method.
				// If the notice needs arguements, create them as data members in your ClientNotice
				// and pass them into its constructor:
				dynamic_cast<MyNotifieeClass*>(inNotifiee)->MyNotice(mArguments);
			}
		*/

		friend class NotifyFunctor;

	private:
		ClientNotice(const ClientNotice&);						// Copy construction not allowed
		const ClientNotice& operator=(const ClientNotice&);		// Assignment is not allowed

	};

}	// end namespace GenericNotification

#endif	// !defined(_GenericNotification_h_)
