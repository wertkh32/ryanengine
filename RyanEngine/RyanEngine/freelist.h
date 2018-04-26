#pragma once
#include "types.h"
#include "stackarray.h"

template< typename T, uint size >
struct Freelist
{
	union Node
	{
		byte data[sizeof(T)];
		uint next;
	};

	Node array[size];
	uint freeHead;

	Freelist ()
	{
		assert ( sizeof ( Node ) == sizeof ( T ) );
		assert ( sizeof ( T ) >= sizeof ( uint ) );

		freeHead = 0;

		for ( uint i = 0; i < size - 1; i++ )
			array[i].next = i + 1;

		array[size - 1].next = INVALID_NODE;
	}


	T* alloc ()
	{
		if ( freeHead != INVALID_NODE )
		{
			uint index = freeHead;
			freeHead = array[freeHead].next;

			return reinterpret_cast<T*>(array[index].data);
		}

		return nullptr;
	}


	void dealloc ( T* ptr )
	{
		T* arrayPtr = reinterpret_cast<T*>(array);

		if ( ptr >= arrayPtr && ptr < arrayPtr + size ) // well-defined cpp11
		{
			uint index = ptr - arrayPtr;

			if ( freeHead == INVALID_NODE )
			{
				freeHead = index;
				array[index].next = INVALID_NODE;
			}
			else
			{
				array[index].next = freeHead;
				freeHead = index;
			}
		}
	}


	template <typename... Args>
	T *allocObj ( Args&&... args )
	{
		T* ptr = alloc ();

		if ( ptr )
			new (ptr) T ( std::forward<Args> ( args )... );

		return ptr;
	}
};


template< typename T, uint size >
struct FreelistEx
{
	T array[size];
	StackArray< uint, size > freeNodes;

	Freelist ()
	{
		for ( int i = size - 1; i >= 0; i-- )
			freeNodes.push ( i );
	}


	T* alloc ()
	{
		if ( freeNodes.currentSize )
			return array + freeNodes.pop ();

		return nullptr;
	}


	void dealloc ( T* ptr )
	{
		assert ( ptr >= array && ptr < array + size ) // well-defined cpp11
		
		uint index = ptr - array;

		freeNodes.push ( index );	
	}


	template <typename... Args>
	T *allocObj ( Args&&... args )
	{
		T* ptr = alloc ();

		if ( ptr )
			new (ptr) T ( std::forward<Args> ( args )... );

		return ptr;
	}
};