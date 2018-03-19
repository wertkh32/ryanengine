#pragma once
#include "universal.h"
#include "stackarray.h"
#include "bitset.h"

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


struct BuddyBlock
{
	uint offset;
	uint size;
};

// for dynamic allocation
// numPages must be power of 2


template < uint numPages >
struct MemBuddyAllocator
{
	static const uint DEPTH = COUNT_TRAILING_ZEROS_CONST( numPages ) + 1;
	static const uint NUM_BUDDY_NODES = ( numPages * 2 - 1 );

	Stack<BuddyBlock> freeBlocks[DEPTH];
	BuddyBlock nodeArray[NUM_BUDDY_NODES];
	BitSet<numPages> freeBlockBits;
	byte *memPtr;

	void init ( byte *ptr )
	{
		BuddyBlock firstNode;

		memPtr = ptr;

		assert ( IS_POWER_2 ( numPages ) );

		for ( uint i = 0; i < DEPTH; i++ )
		{
			uint nodeOffset;

			nodeOffset = i == 0 ? 0 : ( i * 2 - 1 );
			freeBlocks[i] = Stack<BuddyBlock> ( nodeArray + nodeOffset, 1 << i );
		}

		firstNode.offset = 0;
		firstNode.size = numPages;

		freeBlocks[0].push ( firstNode );

		freeBlockBits.setAll ();
	}


	byte *alloc ( uint sizeInBytes )
	{
		uint numPages = NUM_BLOCKS ( sizeInBytes, MEMORY_PAGE_SIZE );
	}
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

