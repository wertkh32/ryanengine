#pragma once
#include "universal.h"


constexpr uint GET_DEPTH ( uint size )
{
	return ( 32 - COUNT_LEADING_ZEROS_CONST ( size ) );
}

constexpr uint GET_TREE_NODE_SIZE ( uint size )
{
	return ( 1 << GET_DEPTH ( size ) ) - 1;
}

// this is a heap.
template< typename T, uint numNodes >
struct SortedQueue
{
	static const uint FLAT_TREE_SIZE = GET_TREE_NODE_SIZE ( numNodes );

	T flatTree[FLAT_TREE_SIZE];
	uint currentSize;
	uint levelCount;

	SortedQueue ()
	{
		memset ( flatTree, 0, sizeof ( flatTree ) );
		currentSize = 0;
	}

	void swap ( T& a, T& b )
	{
		T temp = a;
		a = b;
		b = temp;
	}

	void bubbleUp ( uint index )
	{
		if ( index == 0 )
			return;

		while ( index > 0 )
		{
			uint parentIndex = (index - 1) >> 1;

			if ( flatTree[index] > flatTree[parentIndex] )
			{
				swap ( flatTree[index], flatTree[parentIndex] );
				index = parentIndex;
			}
			else
				break;
		}
	}


	void bubbleDown ( uint index )
	{
		if ( index >= FLAT_TREE_SIZE - 1 )
			return;

		while ( index < FLAT_TREE_SIZE )
		{
			uint  maxChildIndex = (index << 1) + 1;

			if ( flatTree[maxChildIndex] < flatTree[maxChildIndex + 1] )
				maxChildIndex++;

			if ( flatTree[index] < flatTree[maxChildIndex] )
			{
				swap ( flatTree[index], flatTree[maxChildIndex] );
				index = maxChildIndex;
			}
			else
				break;
		}
	}

	bool push ( const T& val )
	{
		if ( currentSize == numNodes )
			return false;

		flatTree[currentSize] = val;

		bubbleUp ( currentSize );

		currentSize++;

		return true;
	}


	bool pop ( T& out )
	{
		if ( currentSize == 0 )
			return false;

		out = flatTree[0];

		flatTree[0] = 0;
		bubbleDown ( 0 );

		currentSize--;

		return true;
	}
};