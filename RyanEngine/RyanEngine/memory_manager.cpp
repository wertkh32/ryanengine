#include "memory_manager.h"


MemoryManager::MemoryManager ()
{
	staticMemPtr = aligned_malloc ( STATIC_MEMORY_SIZE, STATIC_MEMORY_ALIGNMENT );

	staticAllocator.init ( staticMemPtr, STATIC_MEMORY_SIZE / STATIC_MEMORY_ALIGNMENT );
}


MemoryManager::~MemoryManager ()
{
}
