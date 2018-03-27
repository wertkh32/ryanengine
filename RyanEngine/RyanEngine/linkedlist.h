#pragma once
#include "universal.h"

template < typename T >
struct LinkedListNode
{
	T data;
	LinkedListNode<T>* next;
	LinkedListNode<T>* prev;
};


template < typename T, uint numNodes >
struct LinkedList
{
	typedef bool ( *equalCmpFunc )(const T&, const T&);
	typedef void ( *payLoadFunc )(T* data);

	LinkedListNode<T> array[numNodes];

	LinkedListNode<T> freeSentry;
	LinkedListNode<T> usedSentry;

	LinkedList ()
	{


		for ( uint i = 0; i < numNodes; i++ )
		{
			array[i].next = (i == (numNodes - 1)) ?
				&freeSentry :
				array + i + 1;

			array[i].prev = (i == 0) ?
				&freeSentry :
				array + i - 1;
		}


		freeSentry.next = array;
		freeSentry.prev = array + numNodes - 1;

		usedSentry.next = &usedSentry;
		usedSentry.prev = &usedSentry;
	}

	bool isEndOfUsedList ( const LinkedListNode<T> *node )
	{
		return node == &usedSentry;
	}


	bool isEndOfFreeList ( const LinkedListNode<T> *node )
	{
		return node == &freeSentry;
	}


	bool usedListIsEmpty ()
	{
		return isEndOfUsedList ( usedSentry.next );
	}


	bool freeListIsEmpty ()
	{
		return isEndOfFreeList ( freeSentry.next );
	}


	bool isNodeValid ( const LinkedListNode<T> *node )
	{
		return (node == &usedSentry || node == &freeSentry || (node >= array && node <= (array + numNodes - 1))) &&
			(node->next) &&
			(node->prev);
	}


	void addNodeToFreeList ( LinkedListNode<T> *node )
	{
		LinkedListNode<T> *next;
		LinkedListNode<T> *prev;
		LinkedListNode<T> *sentryPrev;

		assert ( isNodeValid ( node ) );

		next = node->next;
		prev = node->prev;
		sentryPrev = freeSentry.prev;

		next->prev = prev;
		prev->next = next;

		node->prev = sentryPrev;
		node->next = &freeSentry;

		sentryPrev->next = node;
		freeSentry.prev = node;
	}


	void addNodeToUsedList ( LinkedListNode<T> *node )
	{
		LinkedListNode<T> *next;
		LinkedListNode<T> *prev;
		LinkedListNode<T> *sentryPrev;

		assert ( isNodeValid ( node ) );

		next = node->next;
		prev = node->prev;
		sentryPrev = usedSentry.prev;

		next->prev = prev;
		prev->next = next;

		node->prev = sentryPrev;
		node->next = &usedSentry;

		sentryPrev->next = node;
		usedSentry.prev = node;
	}


	LinkedListNode<T> *insert ( const T& data )
	{
		if ( freeListIsEmpty () )
			return nullptr;

		LinkedListNode<T> *newNode;

		newNode = freeSentry.prev;
		newNode->data = data;

		addNodeToUsedList ( newNode );

		return newNode;
	}


	bool remove ( LinkedListNode<T> *node )
	{
		if ( usedListIsEmpty () )
			return false;

		addNodeToFreeList ( node );
	}


	bool remove ( const T &data )
	{
		if ( usedListIsEmpty () )
			return false;

		LinkedListNode<T> *node;

		if ( !find ( data, &node ) )
			return false;

		addNodeToFreeList ( node );
	}


	bool find ( const T &data, LinkedListNode<T> **out )
	{
		if ( usedListIsEmpty () )
			return false;

		LinkedListNode<T> *ptr = usedSentry.next;

		do
		{
			if ( ptr->data == data )
				goto found0;
			else
				ptr = ptr->next;
		} while ( !isEndOfUsedList ( ptr ) );

		return false;

	found0:
		*out = ptr;
		return true;
	}


	bool find ( const T &data, LinkedListNode<T> **out, equalCmpFunc func )
	{
		if ( usedListIsEmpty () )
			return false;

		LinkedListNode<T> *ptr = usedSentry.next;

		do
		{
			if ( func ( ptr->data, data ) )
				goto found1;
			else
				ptr = ptr->next;
		} while ( !isEndOfUsedList ( ptr ) );

		return false;

	found1:
		*out = ptr;
		return true;
	}


	void iterate ( payLoadFunc func )
	{
		if ( usedListIsEmpty () )
			return;

		LinkedListNode<T> *ptr = usedSentry.next;

		do
		{
			func ( &ptr->data );
			ptr = ptr->next;
		} while ( !isEndOfUsedList ( ptr ) );
	}
};