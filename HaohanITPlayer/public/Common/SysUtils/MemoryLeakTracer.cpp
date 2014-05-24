#if defined USE_MEMORY_LEAK_TRACE

#include "MemoryLeakTracer.h"

#if defined __unix__
#include "UnixMutex.h"
static UnixMutex	mltMutex;
#endif

#include <assert.h>

#if CN_ADIRONDACKS
#define return_address() ({__asm__ __volatile__("":::"$31");__builtin_return_address(0);})
#else
#define return_address() (void *)0
#endif

#define PRINT_TRACED_OBJS	0

#define	DEBUG_MLT	1
#if DEBUG_MLT
#define	DPR_MLT(x)	x
#else
#define	DPR_MLT(x)
#endif

#define MLT_SIZE			0xffffffff
#define MLT_CALLER			0xffffffff

static ULONG MLTSize		= (ULONG)MLT_SIZE;
static ULONG MLTCaller		= (ULONG)MLT_CALLER;

static MLTObjHeader	*mltList1=NULL;		// Accumulator of out of interest memory allocations
static MLTObjHeader	*mltList2=NULL;		// Accumulator of memory allocations while the trace is enabled
static MLTObjHeader	*mltList3=NULL;		// Accumulator of memory allocations while the trace is enabled

static bool			mltTraceInProgress=true;
static ULONG		mltRepetitionPhase=0;


void MemoryLeakTraceEnable (bool traceEnable)
	{
	mltTraceInProgress = traceEnable;
	}

void MemoryLeakTraceAnalyze ()
	{
	MLTObjHeader		*object, *prev=NULL;

	mltMutex.lock();

	object=mltList2;

	if (object == NULL)
		{
		mltMutex.unlock();
		DPR_MLT( printf("MLT: mltList2 list is empty! There is nothing to analyze!\n") );
		return;
		}

	DPR_MLT( printf("MLT: Trace Analysis: Repetition phase = %d\n", mltRepetitionPhase) );
	while (object != NULL)
		{
		prev  = object;
		object = object->next;
		}

	if (mltRepetitionPhase == 0)
		{
		if (mltList1)
			{
			mltList1->prev = prev;
			prev->next = mltList1;
			}

		mltList1 = mltList2;
		}
	else
		{
		if (mltRepetitionPhase > 1)
			{
			if (mltList3 == NULL)
				DPR_MLT( printf("MLT: Not deallocated object list is empty:\n") );
			else
				{
				DPR_MLT( printf("MLT: Not deallocated object list:\n") );

				ULONG	size=0;
				object=mltList3;
				while (object != NULL)
					{
					DPR_MLT( printf("MLT: NotDealloc address=%8.8x size=%5.5d caller=0x%8.8x\n", 
						(ULONG)(object+1), object->size, (ULONG)object->caller) );
					size += object->size;
					object = object->next;
					}

				DPR_MLT( printf("MLT: NotDealloc Objects Size = %6.6d(%6.6dKB)\n", size, size/1024) );
				mltList3->prev = prev;
				prev->next = mltList3;
				}
			}

		mltList3 = mltList2;
		}

	mltList2 = NULL;
	mltRepetitionPhase++;

	mltMutex.unlock();
	}

ULONG * MemoryLeakTraceAddObject (ULONG *p, ULONG size, void *caller)
	{
	MLTObjHeader	*object, *activeList;

	object = (MLTObjHeader *)p;

	object->prev	= NULL;
	object->size	= size;
	object->caller	= caller;

	if (object->size == MLTSize && (ULONG)object->caller == MLTCaller)
		{
		DPR_MLT( printf("MLT: AddAlloc addr=%8.8x size=%5.5d caller=0x%8.8x\n", (ULONG)(object+1), object->size, (ULONG)object->caller) );
		}
#if PRINT_TRACED_OBJS
	else
		{
		DPR_MLT( printf("MLT: AddAlloc addr=%8.8x size=%5.5d caller=0x%8.8x\n", (ULONG)(object+1), object->size, (ULONG)object->caller) );
		}
#endif

	mltMutex.lock();

	if (mltTraceInProgress) activeList = mltList2;
	else					activeList = mltList1;

	object->next = activeList;
	if (activeList)
		activeList->prev = object;

	activeList = object;

	if (mltTraceInProgress) mltList2 = activeList;
	else 					mltList1 = activeList;

	mltMutex.unlock();

	return (ULONG *)(object+1);
	}

void * MemoryLeakTraceRemoveObject (void *p)
	{
	MLTObjHeader		*object, *next, *prev;

	object = (MLTObjHeader *)p - 1;

	if (object->size == MLTSize && (ULONG)object->caller == MLTCaller)
		{
		DPR_MLT( printf("MLT: DelAlloc addr=%8.8x size=%5.5d caller=0x%8.8x\n", (ULONG)(object+1), object->size, (ULONG)object->caller) );
		}
#if PRINT_TRACED_OBJS
	else
		{
		DPR_MLT( printf("MLT: DelAlloc addr=%8.8x size=%5.5d caller=0x%8.8x\n", (ULONG)(object+1), object->size, (ULONG)object->caller) );
		}
#endif

	mltMutex.lock();

	next = object->next;
	prev = object->prev;
	if (next != NULL)
		next->prev = prev;

	if (prev != NULL)
		prev->next = next;
	else
		{
		if (object == mltList1)
			mltList1 = next;
		else if (object == mltList2)
			mltList2 = next;
		else if (object == mltList3)
			mltList3 = next;
		else
			{
			DPR_MLT( printf("MLT: MemoryLeakTraceRemoveObject(): Undefined Error !\n") ); 
			assert (false);
			}
		}

	mltMutex.unlock();

	return (void *)object;
	}

void *__malloc__(unsigned int nSize, void *caller)
{
	void *p=malloc(((nSize+3) & ~3)+sizeof(MLTObjHeader));
	if (p)
		p = (void*)MemoryLeakTraceAddObject ((ULONG *)p, ((nSize+3) & ~3), caller);
	else
	{
		DPR_MLT( printf("__malloc__:: Memory Allocation Failure %d\n", nSize) );
		assert(false);
	}
	return p;
}

void __free__(void *p)
{
	if (p) free (MemoryLeakTraceRemoveObject(p));
}

void * operator new (unsigned int nSize)
{
	return __malloc__(nSize, return_address());
}

void * operator new[](unsigned int nSize)
{
	return __malloc__(nSize, return_address());
}

void operator delete(void *p)
{
	__free__(p);
}

void operator delete[](void *p)
{
	__free__(p);
}

#endif
