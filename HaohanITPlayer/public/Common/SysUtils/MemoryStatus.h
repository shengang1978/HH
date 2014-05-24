#ifndef  __MEMORY_STATUS_H__
#define  __MEMORY_STATUS_H__

typedef struct tagMemoryPartitionInfo
{
	UInt32	size;
	UInt32	used;
	UInt32	free;
}MemoryPartitionInfo;

extern void MemoryPartitionStatus(MemoryPartitionInfo &partInfo);

#endif	//__MEMORY_LEAK_TRACER_H__