#pragma once
#include "universal.h"

constexpr uint32 lvlMasksDword[] =
{
	~0UL
	(1UL << 16) - 1,
	(1UL << 8) - 1,
	(1UL << 4) - 1,
	(1UL << 2) - 1,
	(1UL << 1) - 1,
};


//level: 0 - 5
// 0: bottom, 5: top
struct DwordTree
{
	uint32 bits;

	uint32 getMask ( uint level, uint levelNode )
	{
		assert ( level <= 5U );
		assert ( levelNode < (1U << level) );

		uint32 mask = lvlMasksDword[level] << (levelNode << (5U - level));

		return mask;
	}

	bool isNodeSetAny ( uint level, uint levelNode )
	{
		uint32 mask = getMask ( level, levelNode );
		return !!(bits & mask);
	}


	bool isNodeSetAll ( uint level, uint levelNode )
	{
		uint32 mask = getMask ( level, levelNode );
		return (bits & mask) == mask;
	}

	void setNode ( uint level, uint levelNode )
	{
		uint32 mask = getMask ( level, levelNode );
		bits |= mask;
	}

	void removeNode ( uint level, uint levelNode )
	{
		uint32 mask = ~( getMask ( level, levelNode ) );
		bits &= mask;
	}
};


constexpr uint64 lvlMasksQword[] =
{
	~0ULL
	(1ULL << 32) - 1,
	(1ULL << 16) - 1,
	(1ULL << 8) - 1,
	(1ULL << 4) - 1,
	(1ULL << 2) - 1,
	(1ULL << 1) - 1,
};


//level: 0 - 5
// 0: bottom, 5: top
struct QwordTree
{
	uint64 bits;

	uint64 getMask ( uint level, uint levelNode )
	{
		assert ( level <= 6U );
		assert ( levelNode < (1U << level) );

		uint64 mask = lvlMasksQword[level] << (levelNode << (6U - level));

		return mask;
	}

	bool isNodeSetAny ( uint level, uint levelNode )
	{
		uint64 mask = getMask ( level, levelNode );
		return !!(bits & mask);
	}


	bool isNodeSetAll ( uint level, uint levelNode )
	{
		uint64 mask = getMask ( level, levelNode );
		return (bits & mask) == mask;
	}

	void setNode ( uint level, uint levelNode )
	{
		uint64 mask = getMask ( level, levelNode );
		bits |= mask;
	}

	void removeNode ( uint level, uint levelNode )
	{
		uint64 mask = ~(getMask ( level, levelNode ));
		bits &= mask;
	}
};


constexpr uint TREE_NUMDWORDS ( uint DEPTH )
{
	return DEPTH == 0 ? 1U : (TREE_NUMDWORDS ( DEPTH - 1 ) | (1U << (DEPTH * 5)));
}


constexpr uint TREE_DEPTH_DWORD_OFFSET ( uint DEPTH )
{
	return DEPTH == 0 ? 0U : (TREE_DEPTH_DWORD_OFFSET ( DEPTH - 1 ) | (1U << ((DEPTH - 1) * 5)));
}


template<uint numNodes>
struct DWord32Tree
{
	static const uint NUMNODE_DWORDS = NUM_BLOCKS ( numNodes, 32 );
	static const uint MAXDEPTH = NUM_BLOCKS ( (31 - COUNT_LEADING_ZEROS_CONST ( NUMNODE_DWORDS * 2 - 1 )), 5 );
	static const uint NUMLEVELS = MAXDEPTH + 1;
	static const uint NUM_DWORDS = TREE_NUMDWORDS ( MAXDEPTH );

	uint32 treeDWords[NUM_DWORDS];

	void init ()
	{
		memset ( treeDWords, 0, sizeof ( treeDWords ) );
	}

	uint getDWordOffsetFromBitIndex ( uint bitIndex, uint depth )
	{
		if ( depth == 0 )
			return 0;

		uint dwordIndex = bitIndex >> 5;
		uint dwordOffset = TREE_DEPTH_DWORD_OFFSET ( depth );

		return dwordOffset + dwordIndex;
	}


	void setNode ( uint bitIndex, uint depth )
	{
		uint bitOffset = bitIndex & 31;
		uint32 setMask = 1UL << bitOffset;

		uint dwordOffset = getDWordOffsetFromBitIndex ( bitIndex, depth );

		treeDWords[dwordOffset] |= setMask;

		if ( depth == 0 )
			return;

		setNode ( bitIndex >> 5, depth - 1 );
	}


	void setNode ( uint bitIndex )
	{
		setNode ( bitIndex, MAXDEPTH );
	}


	void removeNode ( uint bitIndex, uint depth )
	{
		uint bitOffset = bitIndex & 31;
		uint32 removeMask = ~(1UL << bitOffset);

		uint dwordOffset = getDWordOffsetFromBitIndex ( bitIndex, depth );

		treeDWords[dwordOffset] &= removeMask;

		if ( depth == 0 || treeDWords[dwordOffset] != 0 )
			return;

		removeNode ( bitIndex >> 5, depth - 1 );
	}


	void removeNode ( uint bitIndex )
	{
		removeNode ( bitIndex, MAXDEPTH );
	}


	bool isNodeSet ( uint bitIndex, uint depth )
	{
		uint bitOffset = bitIndex & 31;
		uint setMask = 1UL << bitOffset;

		uint dwordOffset = getDWordOffsetFromBitIndex ( bitIndex, depth );

		return !!(treeDWords[dwordOffset] & setMask);
	}


	bool isNodeSet ( uint bitIndex )
	{
		return isNodeSet ( bitIndex, MAXDEPTH );
	}
};


template<uint numNodes>
struct BinaryBitTree : DWord32Tree<numNodes>
{
	uint32* get32TreeDword ( uint binBitIndex, uint binDepth )
	{
		assert ( binBitIndex < (1 << binDepth) );

		uint depth = binDepth / 6;

		uint dwordOffset = getDWordOffsetFromBitIndex ( binBitIndex, depth );

		return treeDWords + dwordOffset;
	}

	bool allBinNodeSet ( uint binBitIndex, uint binDepth )
	{
		DwordTree *dTree;
		uint dwordTreeDepth = binDepth % 6;
		uint dwordTreeNode = binBitIndex & 31;

		dTree = reinterpret_cast<DwordTree *>(get32TreeDword ( binBitIndex, binDepth ));

		return dTree->isNodeSetAll ( dwordTreeDepth, dwordTreeNode );
	}


	bool anyBinNodeSet ( uint binBitIndex, uint binDepth )
	{
		DwordTree *dTree;
		uint dwordTreeDepth = binDepth % 6;
		uint dwordTreeNode = binBitIndex & 31;

		dTree = reinterpret_cast<DwordTree *>(get32TreeDword ( binBitIndex, binDepth ));

		return dTree->isNodeSetAny ( dwordTreeDepth, dwordTreeNode );
	}
};
