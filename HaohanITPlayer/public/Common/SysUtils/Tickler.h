//-----------------------------------------------------------------------------
// Tickler.h
// Copyright (c) 1998 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#ifndef __Tickler_h__
#define __Tickler_h__

#include <vector>

class SonicText;

class TickleImplBase
{

public:

	TickleImplBase();
	virtual ~TickleImplBase();

	// base functionality
	virtual bool IsCancelled() const { return m_cancelled; }
	virtual void Message(const SonicText & text) = 0;
	virtual void Tickle() = 0;
	virtual void Tickle(double fraction) = 0;

	// global step count (optional)
	virtual UInt32 GetStepCount() const { return m_stepCount; }
	virtual UInt32 GetStepNumber() const { return m_stepNumber; }
	virtual void SetStepCount(UInt32 count) { m_stepCount = count; }
	virtual void SetStepNumber(UInt32 number) { m_stepNumber = number; }

protected:

	bool m_cancelled; // cancel requested?
	UInt32 m_stepCount; // global step count (default == 1)
	UInt32 m_stepNumber; // global step number (default == 1)
};

class Tickler
{

public:

	Tickler();
	Tickler(const Tickler & tickler);
	explicit Tickler(TickleImplBase * impl);
	Tickler(TickleImplBase * impl, double startFraction, double deltaFraction);

	// base functionality
	bool IsCancelled() const;
	void Message(const SonicText & text) const;
	void Tickle() const;
	void Tickle(double fraction) const;

	// configuration support
	double DeltaFraction() const { return m_deltaFraction; }
	TickleImplBase* GetImplPointer() const { return m_impl; }
	bool IsValid() const { return m_impl != 0; }
	void SetDeltaFraction(double deltaFraction) { m_deltaFraction = deltaFraction; }
	void SetImplPointer(TickleImplBase* impl) { m_impl = impl; }
	void SetStartFraction(double startFraction) { m_startFraction = startFraction; }
	double StartFraction() const { return m_startFraction; }

	// global step count (optional)
	void NextStep();
	void SetStepCount(UInt32 count);
	void SetStepNumber(UInt32 number);

private:

	TickleImplBase * m_impl;
	double m_startFraction;
	double m_deltaFraction;
};

class TickleCounter
{

public:

	TickleCounter(const Tickler & tickler, size_t total, size_t every);

	void Tickle();

private:

	Tickler m_tickler;
	size_t m_count;
	size_t m_total;
	size_t m_every;
};

typedef std::vector<Tickler> TicklerVector;
typedef std::vector<double>  FractionVector;

TicklerVector Divide(const Tickler & Tickler, size_t n);
TicklerVector Divide(const Tickler & Tickler, const FractionVector & fractions);

#endif // __Tickler_h__

