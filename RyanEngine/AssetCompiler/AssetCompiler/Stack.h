#pragma once
#include <stdint.h>

template< typename T >
struct Stack
{
	T* array;
	uint32_t size;
	uint32_t currentSize;

	Stack ( T* arrayPtr, uint32_t arraySize )
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




template< typename T, uint32_t size >
struct StackArray
{
	T array[size];
	uint32_t currentSize;

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

