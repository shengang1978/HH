//-----------------------------------------------------------------------------
// SonicStatusTickler.cpp
// Copyright (c) 1998 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#include "SonicStatusTickler.h"
#include "SonicException.h"
#include "DVDErrors.h"

SonicStatusTickler::SonicStatusTickler(Tickler & tickler)
:	SonicStatus(),
	m_tickler(&tickler)
{
}

SonicStatusTickler::~SonicStatusTickler()
{
}

void SonicStatusTickler::UpdateNotify()
{
	m_tickler->Message(GetMessage());

	UInt64 duration = GetDuration();
	m_tickler->Tickle(
		duration > 0
			? static_cast<double>(GetPosition()) / GetDuration()
			: 0.0);
}

void SonicStatusTickler::ThrowIfCancelling()
{

#ifndef __linux__

	if (
		m_tickler->IsCancelled()
		&& !IsCancelDisabled())
		throw SonicException(DVDError::userCancel);

#endif

}

SonicStatusTickleImpl::SonicStatusTickleImpl(SonicStatus * status)
:	m_status(status)
{
	if (m_status != 0)
		m_status->SetDuration(100);
}

void SonicStatusTickleImpl::Tickle(double fraction)
{
	if (m_status != 0)
		m_status->SetPosition(static_cast<SInt64>(m_status->GetDuration() * fraction));
}

void SonicStatusTickleImpl::Tickle()
{
}

void SonicStatusTickleImpl::Message(const SonicText & text)
{
	if (m_status != 0)
		m_status->SetMessage(text);
}
