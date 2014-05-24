#ifndef  __MEMORY_LEAK_TRACER_H__
#define  __MEMORY_LEAK_TRACER_H__

typedef unsigned int  ULONG;

struct MLTObjHeader
	{
	MLTObjHeader	*next;
	MLTObjHeader	*prev;
	ULONG			size;
	void			*caller;
	};

extern void MemoryLeakTraceEnable (bool traceEnable);
extern void MemoryLeakTraceAnalyze ();
extern ULONG * MemoryLeakTraceAddObject (ULONG *p, ULONG size, void *caller);
extern void  * MemoryLeakTraceRemoveObject (void *p);

#endif	//__MEMORY_LEAK_TRACER_H__