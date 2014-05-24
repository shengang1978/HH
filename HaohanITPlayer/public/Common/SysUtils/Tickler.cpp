//-----------------------------------------------------------------------------
// Tickler.cpp
// Copyright (c) 1998 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#include "Tickler.h"

TickleImplBase::TickleImplBase()
:	m_cancelled(false),
	m_stepCount(1),
	m_stepNumber(1)
{
}

TickleImplBase::~TickleImplBase()
{
}

Tickler::Tickler()
:	m_impl(0),
	m_startFraction(1.0),
	m_deltaFraction(0.0)
{
}

Tickler::Tickler(TickleImplBase * impl, double startFraction, double deltaFraction)
:	m_impl(impl),
	m_startFraction(startFraction),
	m_deltaFraction(deltaFraction)
{
}

Tickler::Tickler(TickleImplBase * impl)
:	m_impl(impl),
	m_startFraction(0.0),
	m_deltaFraction(1.0)
{
}

Tickler::Tickler(const Tickler & tickler)
:	m_impl(tickler.m_impl),
	m_startFraction(tickler.m_startFraction),
	m_deltaFraction(tickler.m_deltaFraction)
{
}

bool Tickler::IsCancelled() const
{
	return m_impl ? m_impl->IsCancelled() : false;
}

void Tickler::Message(const SonicText & text) const
{
	if (m_impl) m_impl->Message(text);
}

void Tickler::Tickle(double fraction) const
{
	if (m_impl) m_impl->Tickle(m_startFraction + m_deltaFraction * fraction);
}

void Tickler::Tickle() const
{
	if (m_impl) m_impl->Tickle();
}

void Tickler::NextStep()
{
	if (m_impl)
	{
		UInt32 number = m_impl->GetStepNumber();
		assert(number < m_impl->GetStepCount());
		m_impl->SetStepNumber(number + 1);
	}
}

void Tickler::SetStepNumber(UInt32 number)
{
	if (m_impl)
	{
		assert(number < m_impl->GetStepCount());
		m_impl->SetStepNumber(number);
	}
}

void Tickler::SetStepCount(UInt32 count)
{
	assert(count > 0);
	if (m_impl)
	{
		m_impl->SetStepCount(count);
		m_impl->SetStepNumber(1);
	}
}

TickleCounter::TickleCounter(const Tickler & tickler, size_t total, size_t every)
:	m_tickler(tickler),
	m_count(0),
	m_every(every),
	m_total(total)
{
}

void TickleCounter::Tickle()
{
	if (
		(++m_count % m_every == 0)
		&& (m_count <= m_total))
		m_tickler.Tickle(static_cast<double>(m_count) / m_total);
}

TicklerVector Divide(const Tickler & tickler, size_t n)
{
	TicklerVector ticklers;

	if (n > 0)
	{
		double startFraction = tickler.StartFraction();
		double deltaFraction = tickler.DeltaFraction() / n;
		for (size_t i = 0; i < n; ++i)
		{
			ticklers.push_back(Tickler(tickler.GetImplPointer(), startFraction, deltaFraction));
			startFraction += deltaFraction;
		}
	}

	return ticklers;
}

TicklerVector Divide(const Tickler & tickler, const FractionVector & fractions)
{
	TicklerVector ticklers;
	double startFraction = tickler.StartFraction();
	double deltaFraction = 0.0;
	double totalFraction = 0.0;
	FractionVector::const_iterator it;
	for (it = fractions.begin(); it != fractions.end(); ++it)
		totalFraction += *it;

	if (totalFraction > 0.0)
		for (it = fractions.begin(); it != fractions.end(); ++it)
		{
			deltaFraction = tickler.DeltaFraction() * (*it) / totalFraction;
			ticklers.push_back(Tickler(tickler.GetImplPointer(), startFraction, deltaFraction));
			startFraction += deltaFraction;
		}

	return ticklers;
}
