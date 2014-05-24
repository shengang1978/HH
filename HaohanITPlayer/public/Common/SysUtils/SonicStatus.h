//-----------------------------------------------------------------------------
// SonicStatus.h
// Copyright (c) 2001 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#ifndef __SonicStatus_h__
#define __SonicStatus_h__

#include "SonicMessage.h"

class SonicStatus 
{

public:

	SonicStatus();
	virtual ~SonicStatus();
	virtual bool GetDisplayCounter() const;
	virtual UInt64 GetDuration() const;
	virtual const SonicMessage & GetMessage() const;
	virtual UInt64 GetPosition() const;
	virtual bool IsCancelling();
	virtual void IsCancelling(bool isCancel);
	virtual bool IsCancelDisabled();
	virtual void IsCancelDisabled(bool isDisabled);
	virtual void Reset();
	virtual void Rundown(UInt64 milliseconds = 1000);
	virtual void SetDisplayCounter(bool displayCounter);
	virtual void SetDuration(UInt64 duration);
	virtual void SetMessage(const SonicText & text);
	virtual void SetPosition(UInt64 position);
	virtual void ThrowIfCancelling();

protected:

	virtual void UpdateNotify(); // user must override to get update notifications

private:

	SonicStatus(const SonicStatus &);
	SonicStatus & operator = (const SonicStatus &);
	void Update();

	SonicMessage m_message;
	UInt64 m_duration;
	UInt64 m_position;
	bool m_displayCounter;
	bool m_cancelDisabled;
	bool m_isCancelling;
};

#endif // __SonicStatus_h__
