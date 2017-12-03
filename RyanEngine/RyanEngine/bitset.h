#pragma once
#include "universal.h"

#define DWORD_MASK ( 31U )
#define DWORD_INDEX_MASK ( ~( DWORD_MASK ) )
#define DWORD_SHIFT ( 5U )

struct bitset
{
	uint *bitArray;
	uint numDWords;

	bitset ( uint *_bitArray, uint _numDWords )
	{
		bitArray = _bitArray;
		numDWords = _numDWords;

		clear ();
	}


	void clear ()
	{
		for ( uint i = 0; i < numDWords;i++ )
			bitArray[i] = 0U;
	}


	void setAll ()
	{
		for ( uint i = 0; i < numDWords;i++ )
			bitArray[i] = ~0U;
	}


	bool getBit ( uint index )
	{
		uint dwordIndex = (index & DWORD_INDEX_MASK) >> DWORD_SHIFT;
		uint dwordBitIndex = (index & DWORD_MASK);
		uint dword = bitArray[dwordIndex];

		return !!( dword & ( 1U << dwordBitIndex ) );
	}

	void setBit ( uint index )
	{
		uint dwordIndex = (index & DWORD_INDEX_MASK) >> DWORD_SHIFT;
		uint dwordBitIndex = (index & DWORD_MASK);

		bitArray[dwordIndex] |= (1U << dwordBitIndex);
	}


	bool testDWordMask ( uint dwordMask, uint dwordIndex )
	{
		return !!( bitArray[dwordIndex] & dwordMask );
	}
};