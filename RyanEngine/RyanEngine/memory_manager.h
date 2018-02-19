#pragma once
#include "universal.h"

#define MEMORY_POOL_SIZE	( 16 * MB )
#define MEMORY_PAGE_SIZE	( 64 * KB )


class MemoryManager : public Singleton<MemoryManager>
{
	friend Singleton<MemoryManager>;
	MemoryManager ();
	~MemoryManager ();
public:

};

