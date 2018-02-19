#pragma once
#include <intrin.h>
#include "feature.h"
#include "types.h"


// bit functions
inline uint CountLeadingZeros32 ( uint value )
{
	ulong index;

	if ( _BitScanReverse ( &index, value ) )
		return static_cast< uint >( index );
	else
		return 0;
}


inline uint CountTrailingZeros32 ( uint value )
{
	ulong index;

	if ( _BitScanForward ( &index, value ) )
		return static_cast< uint >( index );
	else
		return 0;
}


inline uint CountBits32 ( uint value )
{
	return __popcnt ( value );
}


#if IS_X64
inline bool TestBit32 ( uint value, uint bitPos )
{
	return !!( _bittest ( reinterpret_cast< long* >( &value ), bitPos ));
}


inline uint CountLeadingZeros64 ( uint64 value )
{
	ulong index;

	if ( _BitScanReverse64 ( &index, value ) )
		return static_cast< uint >(index);
	else
		return 0;
}


inline uint CountTrailingZeros64 ( uint64 value )
{
	ulong index;

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
inline int32 AtomicCompareExchange ( v_int32 *ptr, int32 exchange, int32 comperand )
{
	return _InterlockedCompareExchange ( ptr, exchange, comperand );
}


inline void* AtomicCompareExchangePtr ( void * volatile *ptr, void *exchangePtr, void *comperandPtr )
{
	return _InterlockedCompareExchangePointer ( ptr, exchangePtr, comperandPtr );
}


inline int32 AtomicExchange ( v_int32 *ptr, int32 value )
{
	return _InterlockedExchange ( ptr, value );
}


inline int32 AtomicExchangeAdd ( v_int32 *ptr, int32 value )
{
	return _InterlockedExchangeAdd ( ptr, value );
}


inline int32 AtomicIncrement ( v_int32 *ptr )
{
	return _InterlockedIncrement ( ptr );
}


inline int32 AtomicDecrement ( v_int32 *ptr )
{
	return _InterlockedDecrement ( ptr );
}


inline int32 AtomicAnd ( v_int32 *ptr, int32 value )
{
	return _InterlockedAnd ( ptr, value );
}


inline int32 AtomicOr ( v_int32 *ptr, int32 value )
{
	return _InterlockedOr ( ptr, value );
}


inline int32 AtomicXor ( v_int32 *ptr, int32 value )
{
	return _InterlockedXor ( ptr, value );
}

