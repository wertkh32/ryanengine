#pragma once
#include <Windows.h>

namespace raf
{
	class atomic_counter
	{
	public:
		atomic_counter() :value_(0) {}

		void inc()
		{
			InterlockedIncrement(&value_);
		}
		void dec()
		{
			InterlockedDecrement(&value_);
		}

		unsigned int value() const
		{
			return value_;
		}
	private:
		volatile unsigned int value_;
	};
}