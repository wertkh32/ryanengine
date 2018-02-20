#pragma once
#include "universal.h"

#define STATIC_MEMORY_SIZE			( 16 * MB )
#define STATIC_MEMORY_ALIGNMENT		16
#define MEMORY_PAGE_SIZE			( 64 * KB )


enum MEMORY_POOL_TYPE
{
	MEMORY_POOL_STATIC,
	MEMORY_POOL_DYNAMIC,

	MEMORY_POOL_COUNT,
};


// for static allocation
struct MemStackAllocator
{

};


// for dynamic allocation
template < uint size >
struct MemBuddyAllocator
{

};


class MemoryManager : public Singleton<MemoryManager>
{
	byte *staticMemPtr;
	uint staticMemAllocated;

	friend Singleton<MemoryManager>;
	MemoryManager ();
	~MemoryManager ();
public:

};

