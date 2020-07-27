#pragma once
#include <intrin.h>
#include "feature.h"
#include "types.h"


// bit functions
inline uint CountLeadingZeros32 ( uint value )
{
	DWORD index;

	if ( _BitScanReverse ( &index, value ) )
		return static_cast< uint >( index );
	else
		return 0;
}


inline uint CountTrailingZeros32 ( uint value )
{
	DWORD index;

	if ( _BitScanForward ( &index, value ) )
		return static_cast< uint >( index );
	else
		return 0;
}


inline uint CountBits32 ( uint value )
{
	return __popcnt ( value );
}


inline bool TestBit32 ( uint value, uint bitPos )
{
	return !!( _bittest ( reinterpret_cast< long* >( &value ), bitPos ));
}


#if defined( IS_X64 )
inline uint CountLeadingZeros64 ( uint64 value )
{
	DWORD index;

	if ( _BitScanReverse64 ( &index, value ) )
		return static_cast< uint >( index );
	else
		return 0;
}


inline uint CountTrailingZeros64 ( uint64 value )
{
	DWORD index;

	if ( _BitScanForward64 ( &index, value ) )
		return static_cast< uint >(index);
	else
		return 0;
}


inline uint CountBits64 ( uint64 value )
{
	return static_cast< uint >(__popcnt64 ( value ));
}


inline bool TestBit64 ( uint64 value, uint bitPos )
{
	return !!( _bittest64 ( reinterpret_cast< long long* >( &value ), bitPos ) );
}
#endif


// functions for locks
inline int32 AtomicCompareExchange ( vint32 *ptr, int32 exchange, int32 comperand )
{
	return _InterlockedCompareExchange ( reinterpret_cast<volatile long*>( ptr ), exchange, comperand );
}


inline void* AtomicCompareExchangePtr ( void * volatile *ptr, void *exchangePtr, void *comperandPtr )
{
	return _InterlockedCompareExchangePointer ( ptr, exchangePtr, comperandPtr );
}


inline int32 AtomicExchange ( vint32 *ptr, int32 value )
{
	return _InterlockedExchange ( reinterpret_cast<volatile long*>( ptr ), value );
}


inline int32 AtomicExchangeAdd ( vint32 *ptr, int32 value )
{
	return _InterlockedExchangeAdd ( reinterpret_cast<volatile long*>(ptr), value );
}


inline int32 AtomicIncrement ( vint32 *ptr )
{
	return _InterlockedIncrement ( reinterpret_cast<volatile long*>( ptr ) );
}


inline int32 AtomicDecrement ( int32 *ptr )
{
	return _InterlockedDecrement ( reinterpret_cast<volatile long*>( ptr ) );
}


inline int32 AtomicAnd ( int32 *ptr, int32 value )
{
	return _InterlockedAnd ( reinterpret_cast<volatile long*>( ptr ), value );
}


inline int32 AtomicOr ( int32 *ptr, int32 value )
{
	return _InterlockedOr ( reinterpret_cast<volatile long*>( ptr ), value );
}


inline int32 AtomicXor ( int32 *ptr, int32 value )
{
	return _InterlockedXor ( reinterpret_cast<volatile long*>( ptr ), value );
}

