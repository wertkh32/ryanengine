#pragma once
#include <Windows.h>
#include "core/utility.h"

namespace raf
{
	class event : private noncopyable
	{
	private:
		HANDLE handle_;
	public:
		event();
		~event();

		void set();
		bool wait();
		bool wait(unsigned long ms);
		bool is_set();
	};
}