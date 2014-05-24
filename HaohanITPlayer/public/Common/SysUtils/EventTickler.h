//-----------------------------------------------------------------------------
// EventTickler.h
// Copyright (c) 2002 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#ifndef EVENT_TICKLER_H
#define EVENT_TICKLER_H

#include "Tickler.h"
#include "PIMessaging.h"

// **CodeWizzard** - Severe Violation:  Effective C++ item 11  - Define a copy constructor and operator= for classes with dynamically allocated memor
class EventTickleImplBase : public TickleImplBase, public PIEventHandler
{
public:
	EventTickleImplBase();
	explicit EventTickleImplBase(PIEventHandler* pRecipient);
	virtual ~EventTickleImplBase();

	virtual void DoAction(UInt32 /*actionID*/){};
	virtual void Tickle(double fraction) = 0;
	virtual void Tickle() = 0;
	virtual void Message(const SonicText & text) = 0;

	virtual void ProcessEvent(PIEvent const *pEvent, PIEventHandler *pOrigin) = 0;

	virtual void SetEventRecipient(PIEventHandler *pHandler){ mpRecipient = pHandler; }
// **CodeWizzard** - Severe Violation:  Effective C++ item 29  - Avoid returning "handles" to internal data from const member function
// **CodeWizzard** - Violation:  Effective C++ item 30  - Avoid member functions that return pointers or references to members less accessible than themselve
	virtual PIEventHandler* const GetEventRecipient() const	{ return(mpRecipient); }

private:
	PIEventHandler		*mpRecipient;
};


class EventTickler : public Tickler, public PIEventHandler
{
public:
	EventTickler();
	virtual ~EventTickler();

	explicit EventTickler(EventTickleImplBase* uip);
	EventTickler(EventTickleImplBase* uip, double startFraction, double deltaFraction);
	EventTickler(const EventTickler&);
	const EventTickler& operator=(const EventTickler&);

	virtual void DoAction(UInt32 actionID);

	virtual EventTickleImplBase* GetEventImplPointer() const;
	virtual void SetEventRecipient(PIEventHandler *pHandler);

	virtual void ProcessEvent(PIEvent const *pEvent, PIEventHandler *pOrigin);

private:

};

#endif
