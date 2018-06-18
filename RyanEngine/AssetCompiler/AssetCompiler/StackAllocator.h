#pragma once
#include <stdint.h>

typedef uint8_t byte;

struct StackAllocator
{
	byte *array;
	uint32_t totalSize;
	uint32_t currentSize;

	StackAllocator ( byte* _data, uint32_t _totalSize )
	{
		currentSize = 0;
		array = _data;
		totalSize = _totalSize;
	}


	void reset ()
	{
		currentSize = 0;
	}


	byte *allocBytes ( uint32_t sizeInBytes )
	{
		byte *ret;

		if ( currentSize + sizeInBytes > totalSize )
			return nullptr;

		ret = array + currentSize;

		currentSize += sizeInBytes;

		return ret;
	}
};