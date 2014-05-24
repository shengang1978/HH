// Sync_Locks.h: interface for the CSync_Locks class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYNC_LOCKS_H__C9677AAF_BEED_490F_BFBE_9BA07B840196__INCLUDED_)
#define AFX_SYNC_LOCKS_H__C9677AAF_BEED_490F_BFBE_9BA07B840196__INCLUDED_
#include "Windows.h"

class CCritSec 
{	
    CCritSec(const CCritSec &refCritSec);
    CCritSec &operator=(const CCritSec &refCritSec);
	
    CRITICAL_SECTION m_CritSec;
	
public:
    CCritSec() {
        InitializeCriticalSection(&m_CritSec);
    };
	
    ~CCritSec() {
        DeleteCriticalSection(&m_CritSec);
    };
	
    void Lock() {
        EnterCriticalSection(&m_CritSec);
    };
	
    void Unlock() {
        LeaveCriticalSection(&m_CritSec);
    };
	CRITICAL_SECTION* c_critical_section(){
		return &m_CritSec;
	}
};

class CAutoLock_CS {
    CAutoLock_CS(const CAutoLock_CS &refAutoLock);
    CAutoLock_CS &operator=(const CAutoLock_CS &refAutoLock);
	
protected:
    CCritSec * m_pLock;
	
public:
    CAutoLock_CS(CCritSec * plock)
    {
        m_pLock = plock;
        m_pLock->Lock();
    };
	
    ~CAutoLock_CS() {
        m_pLock->Unlock();
    };
};

//------------------------------------------------------------------------------
//-----------------------同步功能函数-----------------------------------------------
//------------------------------------------------------------------------------
class CMutex
{	
    CMutex(const CMutex &refMutex);
    CMutex &operator=(const CMutex &refMutex);
	
    HANDLE m_hMutex;
	
public:
    CMutex() {
		m_hMutex	 =  ::CreateMutex( NULL, FALSE, _T(""));
    };
	
    ~CMutex() {
        CloseHandle(m_hMutex);
        m_hMutex = NULL;
    };
	
    DWORD Lock( int time = INFINITE ) {
        return WaitForSingleObject( m_hMutex, time );
    };
	
    void Unlock() {
        ReleaseMutex( m_hMutex );
    };
};

class CAutoLock_Mutex
{
    CAutoLock_Mutex(const CAutoLock_Mutex &refAutoLock);
    CAutoLock_Mutex &operator=(const CAutoLock_Mutex &refAutoLock);
	
protected:
    CMutex * m_pLock;
	
public:
    CAutoLock_Mutex(CMutex * plock, int time = INFINITE)
    {
        m_pLock = plock;
        m_pLock->Lock(time);
    };
	
    ~CAutoLock_Mutex() {
        m_pLock->Unlock();
    };
};

// wrapper for event objects
class CAMEvent
{
	
    // make copy constructor and assignment operator inaccessible
	
    CAMEvent(const CAMEvent &refEvent);
    CAMEvent &operator=(const CAMEvent &refEvent);
	
protected:
    HANDLE m_hEvent;
public:
    CAMEvent(BOOL fManualReset = FALSE){
		m_hEvent = CreateEvent(NULL, fManualReset, FALSE, NULL);
    }
    ~CAMEvent(){
		if (m_hEvent) {
			(CloseHandle(m_hEvent));
		}
    }
	
    // Cast to HANDLE - we don't support this as an lvalue
    operator HANDLE () const { return m_hEvent; };
	
    void Set() {(SetEvent(m_hEvent));};
    int Wait(DWORD dwTimeout = INFINITE) 
	{
		return WaitForSingleObject(m_hEvent, dwTimeout);
    };
    void Reset() { ResetEvent(m_hEvent); };
    BOOL Check() { return Wait(0); };
};

class CRWLock   
{   
public:   
	CRWLock()
	{
		m_nReaderCount   =   0;   
		m_hDataLock   =   CreateSemaphore(NULL,   1,   1,   NULL);   
		m_hMutex   =   CreateMutex(NULL,   FALSE,   NULL);   
	}
	~CRWLock()
	{
		CloseHandle(m_hMutex);   
		CloseHandle(m_hDataLock);
	}
	BOOL ReadLock()
	{
		BOOL   bResult   =   TRUE;
		WaitForSingleObject(m_hMutex, INFINITE); 		
		if(::InterlockedIncrement(&m_nReaderCount) == 1)   
			bResult = WaitForSingleObject(m_hDataLock, INFINITE); 		
		ReleaseMutex(m_hMutex);
		return bResult;   
	}
	BOOL ReadUnlock()
	{
		BOOL   bResult   =   TRUE;   
		LONG   lPrevCount; 		
		WaitForSingleObject(m_hMutex, INFINITE); 		
		if(::InterlockedDecrement(&m_nReaderCount) == 0)   
			bResult = ReleaseSemaphore(m_hDataLock, 1, &lPrevCount);   		
		ReleaseMutex(m_hMutex); 		
		return   bResult;   
	}
	BOOL WriteLock()
	{
		if( WaitForSingleObject(m_hDataLock, INFINITE) == WAIT_OBJECT_0 )
			return TRUE;
		return FALSE;
	}
	BOOL   WriteUnlock()
	{
		LONG   lPrevCount;     
		return ReleaseSemaphore(m_hDataLock,   1,   &lPrevCount);   
	}
private:   
	HANDLE m_hMutex;   
	HANDLE m_hDataLock;   
	long                   m_nReaderCount;   
};

class CAutoLock_RW
{
    CAutoLock_RW(const CAutoLock_RW &refAutoLock);
    CAutoLock_RW &operator=(const CAutoLock_RW &refAutoLock);
	
protected:
    CRWLock * m_pLock;
	BOOL	m_bReadLock;
	
public:
    CAutoLock_RW(CRWLock * plock, BOOL bReadLock)
    {
        m_pLock = plock;
		m_bReadLock = bReadLock;
		
		if( m_bReadLock )
			m_pLock->ReadLock();
		else
			m_pLock->WriteLock();
    };
	
    ~CAutoLock_RW() 
	{
        if( m_bReadLock )
			m_pLock->ReadUnlock();
		else
			m_pLock->WriteUnlock();
    };
};

#endif // !defined(AFX_SYNC_LOCKS_H__C9677AAF_BEED_490F_BFBE_9BA07B840196__INCLUDED_)
