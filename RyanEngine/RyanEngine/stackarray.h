#pragma once

#include "universal.h"


template< typename T >
struct Stack
{
	T* array;
	uint size;
	uint currentSize;

	Stack ( T* arrayPtr, uint arraySize )
	{
		assert ( arraySize );

		array = arrayPtr;
		size = arraySize;

		reset ();
	}

	void push ( const T &value )
	{
		assert ( currentSize < size );

		array[currentSize++] = value;
	}

	T pop ()
	{
		assert ( currentSize > 0 );

		return array[--currentSize];
	}

	T& peek ()
	{
		assert ( currentSize > 0 );

		return array[currentSize - 1];
	}

	void reset ()
	{
		currentSize = 0;
	}
};




template< typename T, uint size >
struct StackArray
{
	T array[size];
	uint currentSize;

	StackArray ()
	{
		reset ();
	}

	void push ( const T &value )
	{
		assert ( currentSize < size );

		array[currentSize++] = value;
	}

	T pop ()
	{
		assert ( currentSize > 0 );

		return array[--currentSize];
	}

	T& peek ()
	{
		assert ( currentSize > 0 );

		return array[currentSize - 1];
	}

	void reset ()
	{
		currentSize = 0;
	}
};
