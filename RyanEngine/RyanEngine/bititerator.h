#pragma once
#include "bitset.h"
#include "intrinsics.h"

#define INVALID_BIT_INDEX (~0U)

template< uint numBits >
struct BitIterator
{
	const BitSet<numBits> *bitset;
	uint cursor;
	uint currentDword;
	uint currentDwordIndex;

	BitIterator ( const BitSet<numBits> *_bitset )
	{
		bitset = _bitset;
		reset ();
	}

	void reset ()
	{
		cursor = 0;
		currentDword = bitset->bitArray[0];
		currentDwordIndex = 0;
	}


	uint nextBit () // next bit pos
	{
		while ( !currentDword && currentDwordIndex < bitset->numDWords )
			currentDword = bitset->bitArray[++currentDwordIndex];

		if ( currentDwordIndex == bitset->numDWords )
			return INVALID_BIT_INDEX;

		uint dwordBitPos = CountTrailingZeros32 ( currentDword );

		currentDword &= ~(1U << dwordBitPos);
		cursor = ( currentDwordIndex << DWORD_MASK ) | dwordBitPos;

		return cursor;
	}


	uint getCurBitPos () // current bit pos
	{
		return cursor;
	}


	uint getCurDwordIndex () // current dword index
	{
		return currentDwordIndex;
	}

};
