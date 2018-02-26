#pragma once
#include "universal.h"
#include "stackarray.h"

#define MEMORY_PAGE_SIZE			( 64U * KB )
#define STATIC_MEMORY_SIZE			( 64U * MB )
#define STATIC_MEMORY_ALIGNMENT		( 16U )



enum MEMORY_POOL_TYPE
{
	MEMORY_POOL_STATIC,
	MEMORY_POOL_DYNAMIC,

	MEMORY_POOL_COUNT,
};


inline byte *aligned_malloc ( uint sizeInBytes, uint alignment )
{
	byte *retPtr;
	uintptr_t allocedPtr;
	
	assert ( IS_POWER_2 ( alignment ) );

	uint numBlocks = NUM_BLOCKS ( sizeInBytes, alignment ) + 1;
	
	allocedPtr = reinterpret_cast< uintptr_t >( malloc ( numBlocks * alignment ) );
	
	assert ( allocedPtr );
	
	retPtr = reinterpret_cast< byte *>( ALIGN_UP ( allocedPtr, alignment ) );

	return retPtr;
}


// very simple allocator for static mem. allocations are 16 byte aligned
struct MemSimpleAllocator
{
	uint blockCount;
	uint usedBlockCount;
	byte *memStartPtr;
	byte *memCurPtr;

	void init ( byte *memPtr, uint numAlignedBlocks )
	{
		assert ( IS_ALIGNED_PTR ( memPtr, STATIC_MEMORY_ALIGNMENT ) );

		blockCount = numAlignedBlocks;
		usedBlockCount = 0;
		memCurPtr = memPtr;
		memStartPtr = memPtr;
	}


	void *alloc ( uint sizeInBytes )
	{
		uint numBlocks = NUM_BLOCKS ( sizeInBytes, STATIC_MEMORY_ALIGNMENT );

		if ( usedBlockCount + numBlocks > blockCount )
			return nullptr;

		byte *ret = memCurPtr;
		memCurPtr += ( STATIC_MEMORY_ALIGNMENT * numBlocks );

		assert ( IS_ALIGNED_PTR ( ret, STATIC_MEMORY_ALIGNMENT ) );

		return ret;
	}

	void reset ()
	{
		usedBlockCount = 0;
		memCurPtr = memStartPtr;
	}
};


// for dynamic allocation
template < uint numPages >
struct MemBuddyAllocator
{

};


class MemoryManager : public Singleton<MemoryManager>
{
	byte *staticMemPtr;
	MemSimpleAllocator staticAllocator;


	friend Singleton<MemoryManager>;
	MemoryManager ();
	~MemoryManager ();
public:

};

