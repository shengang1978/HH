//-----------------------------------------------------------------------------
// SonicStatus.cpp
// Copyright (c) 2001 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#include "SonicStatus.h"
#include "SonicException.h"
#include "SonicThread.h"
#include "time_utils.h"
#include "UnicodeUtilities.h"
#include "CDebugLogFile.h"
#include "DVDErrors.h"

SonicStatus::SonicStatus()
:	m_message(),
	m_position(0),
	m_duration(0),
	m_displayCounter(true),
	m_cancelDisabled(false),
	m_isCancelling(false)
{
}

SonicStatus::~SonicStatus() 
{
}

bool SonicStatus::GetDisplayCounter() const
{
	return m_displayCounter;
}

UInt64 SonicStatus::GetDuration() const 
{
	return m_duration;
}

const SonicMessage & SonicStatus::GetMessage() const 
{
	return m_message;
}

UInt64 SonicStatus::GetPosition() const 
{
	return m_position;
}

bool SonicStatus::IsCancelDisabled() 
{ 
	return m_cancelDisabled; 
}

void SonicStatus::IsCancelDisabled(bool isDisabled) 
{ 
	m_cancelDisabled = isDisabled; 
}

bool SonicStatus::IsCancelling()
{
	return m_isCancelling;
}

void SonicStatus::IsCancelling(bool isCancel)
{
	m_isCancelling = isCancel;
}

void SonicStatus::Reset()
{
	m_message = SonicMessage();
	m_position = 0;
	m_duration = 0;
	m_displayCounter = true;
	m_cancelDisabled = false;
	m_isCancelling = false;
	Update();
}

void SonicStatus::Rundown(UInt64 milliseconds)
{
	if (m_position < m_duration)
		if (milliseconds == 0)
			SetPosition(m_duration);
		else
		{
			UInt64 steps = // number of steps
				std::max(
					(milliseconds + 50) / 100, // ~100 milliseconds per step
					static_cast<UInt64>(1)); // at least one step
					
			UInt64 size = // size of step
				std::max(
					(m_duration - m_position) / steps,
					static_cast<UInt64>(1));

			while (m_position < m_duration)
			{
				SetPosition(m_position + size);
				SonicThread::Sleep(100);
			}
		}
}

void SonicStatus::SetDisplayCounter(bool displayCounter)
{
	m_displayCounter = displayCounter;
}

void SonicStatus::SetDuration(UInt64 duration)
{
	m_position = 0;
	if (m_duration != duration)
	{
		m_duration = duration;
		Update();
	}
}

void SonicStatus::SetMessage(const SonicText & text)
{
	if (m_message.GetNumber() != text.GetNumber())
	{
		m_message = text;
		Update();

#ifdef _DEBUG
		CDebugLogFile::Log(unicode::to_string(m_message.GetText()).c_str()); 
#endif
	}
}

void SonicStatus::SetPosition(UInt64 position) 
{
	position = std::min(position, m_duration);
	if (m_position != position)
	{
		m_position = position; 
		Update();
	}
}

void SonicStatus::ThrowIfCancelling()
{
	if (m_isCancelling) throw SonicException(DVDError::userCancel);
}

void SonicStatus::Update()
{
	try 
	{
		UpdateNotify();
	}
	catch (std::exception & x)
	{
		if (SonicException(&x).GetNumber() == DVDError::userCancel.GetNumber())
			if (!IsCancelDisabled())
				m_isCancelling = true;
			else
				return;

		throw;
	}
}

void SonicStatus::UpdateNotify()
{
}
