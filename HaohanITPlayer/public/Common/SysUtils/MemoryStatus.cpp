#include "MemoryStatus.h"

#include "malloc.h"

// Memory status of malloc() based heap
#if !defined(_WIN32)
void MemoryPartitionStatus(MemoryPartitionInfo & partInfo)
#else
void MemoryPartitionStatus(MemoryPartitionInfo & /*partInfo*/)
#endif
{
#if !defined(_WIN32)
	struct mallinfo	memInfo;

	memInfo=mallinfo();
	printf("Total = %2.2dMB[%5.5dKB]  Free = %dMB[%5.5dKB]  Used = %dMB[%5.5dKB]\n", 
		memInfo.arena/(1024*1024),		memInfo.arena/1024,
		memInfo.fordblks/(1024*1024),	memInfo.fordblks/1024, 
		memInfo.uordblks/(1024*1024),	memInfo.uordblks/1024);

	partInfo.size = memInfo.arena;
	partInfo.used = memInfo.uordblks;
	partInfo.free = memInfo.fordblks;
#endif	//!defined(_WIN32)
}
