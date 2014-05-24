//-----------------------------------------------------------------------------
// EventTickler.cpp
// Copyright (c) 2002 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 
#include <locale>
#include "EventTickler.h"
#include "SonicException.h"
#include "SonicException.h"

//______________________________________________________________________________

EventTickleImplBase::EventTickleImplBase()
 : TickleImplBase()
{
	PIEventHandler::RegisterEvents();
}

EventTickleImplBase::EventTickleImplBase(PIEventHandler* pRecipient)
 : TickleImplBase(), mpRecipient(pRecipient)
{
	PIEventHandler::RegisterEvents();
}

EventTickleImplBase::~EventTickleImplBase()
{
	try
	{
		PIEventHandler::UnregisterEvents();
	}
	catch (std::exception&)
	{
	}
}

//______________________________________________________________________________

EventTickler::EventTickler()
:	Tickler()
{
	PIEventHandler::RegisterEvents();
}

EventTickler::~EventTickler()
{
	try
	{
		PIEventHandler::UnregisterEvents();
	}
	catch (std::exception&)
	{
	}
}

EventTickler::EventTickler(EventTickleImplBase* uip, double startFraction, 
										double deltaFraction)
:	Tickler(uip, startFraction, deltaFraction)
{
	PIEventHandler::RegisterEvents();
}

EventTickler::EventTickler(EventTickleImplBase* uip)
:	Tickler(uip)
{
	PIEventHandler::RegisterEvents();
}

EventTickler::EventTickler(const EventTickler& tickler)
:	Tickler(tickler)
/*	mUIp(tickler.mUIp),
	mStartFraction(tickler.mStartFraction),
	mDeltaFraction(tickler.mDeltaFraction)
*/
{
	//any other setting here
}

const EventTickler& EventTickler::operator=(const EventTickler& tickler)
// **CodeWizzard** - Possible Violation:  Effective C++ item 17  - Check for assignment to self in operator
{
	Tickler::operator= (tickler);
/*	mUIp = tickler.mUIp;
	mStartFraction = tickler.mStartFraction;
	mDeltaFraction = tickler.mDeltaFraction;
*/	return *this;
}


EventTickleImplBase* EventTickler::GetEventImplPointer() const
{
	EventTickleImplBase* pImpl = dynamic_cast< EventTickleImplBase* >(GetImplPointer());

	//if we don't have a proper Impl, we don't go forward
	if (pImpl == 0) throw DebugException();

	return(pImpl);
}

//pass all events through to implementation
void EventTickler::ProcessEvent(PIEvent const *pEvent, PIEventHandler *pOrigin)
{
	GetEventImplPointer()->ProcessEvent(pEvent, pOrigin);
}

void EventTickler::DoAction(UInt32 actionID)
{
	GetEventImplPointer()->DoAction(actionID);
}

void EventTickler::SetEventRecipient(PIEventHandler *pHandler)
{
	GetEventImplPointer()->SetEventRecipient(pHandler);
}

