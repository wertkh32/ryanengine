#pragma once

#include <Windows.h>

namespace raf
{
	class thread
	{
	public:
		typedef DWORD(WINAPI *THREAD_FUNC)(void *);

		thread();
		thread(void *func, void *args);
		~thread();

		void init(void* func, void *args);
		void start();
		void join();
		void join(unsigned long ms);

	private:
		HANDLE handle_ = 0;
		DWORD thread_id_;
	};
}
