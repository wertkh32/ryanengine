#pragma once
#include "universal.h"

#define DWORD_MASK ( 31U )
#define DWORD_INDEX_MASK ( ~( DWORD_MASK ) )
#define DWORD_SHIFT ( 5U )

template< uint numBits >
struct BitSet
{
	static const uint numDWords = ( numBits + DWORD_MASK ) >> DWORD_SHIFT;
	uint bitArray[numDWords];

	BitSet ()
	{
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


	bool testDWordMaskAny ( uint dwordMask, uint dwordIndex )
	{
		return !!( bitArray[dwordIndex] & dwordMask );
	}


	bool testDWordMaskAll ( uint dwordMask, uint dwordIndex )
	{
		return ( bitArray[dwordIndex] & dwordMask ) == dwordMask;
	}


	bool testDWordMaskAnyWithOffset ( uint dwordMask, uint bitOffset )
	{
		uint dwordIndex = bitOffset >> DWORD_SHIFT;
		uint dwordBitIndex = (bitOffset & DWORD_MASK);
		uint dwordMaskWithOffset = dwordMask << dwordBitIndex;
		uint dwordMaskOverflow = ( dwordMask >> (32 - dwordBitIndex) );

		return !!( bitArray[dwordIndex] & dwordMaskWithOffset ) ||
			   !!( bitArray[dwordIndex + 1] & dwordMaskOverflow );
	}


	bool testDWordMaskAllWithOffset ( uint dwordMask, uint bitOffset )
	{
		uint dwordIndex = bitOffset >> DWORD_SHIFT;
		uint dwordBitIndex = (bitOffset & DWORD_MASK);
		uint dwordMaskWithOffset = dwordMask << dwordBitIndex;
		uint dwordMaskOverflow = (dwordMask >> (32 - dwordBitIndex));

		return ( bitArray[dwordIndex] & dwordMaskWithOffset ) == dwordMaskWithOffset &&
			   ( bitArray[dwordIndex + 1] & dwordMaskOverflow ) == dwordMaskOverflow;
	}
};
