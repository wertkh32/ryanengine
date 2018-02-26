#pragma once
#include "universal.h"

constexpr uint lvlMasks[6] =
{
	(1 << 1) - 1,
	(1 << 2) - 1,
	(1 << 4) - 1,
	(1 << 8) - 1,
	(1 << 16) - 1,
	(1 << 32) - 1,
};


struct DwordTree
{
	uint bits;

	uint getMask ( uint level, uint levelNode )
	{
		assert ( level <= 5 );
		assert ( levelNode < ( 1 << ( 5 - level ) ) );

		uint mask = lvlMasks[level] << (levelNode << level);
	}

	bool isNodeSet ( uint level, uint levelNode )
	{
		uint mask = getMask ( level, levelNode );
		return !!( bits & mask );
	}

	void setNode ( uint level, uint levelNode )
	{
		uint mask = getMask ( level, levelNode );
		bits |= mask;
	}

	void removeNode ( uint level, uint levelNode )
	{
		uint mask = ~( getMask ( level, levelNode ) );
		bits &= mask;
	}
};