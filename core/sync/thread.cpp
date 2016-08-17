#include "core/sync/thread.h"

namespace raf
{
	thread::thread() : thread_id_(-1)
	{
	}

	thread::thread(void *func, void *args)
	{
		init(func, args);
	}

	void thread::init(void *func, void *args)
	{
		handle_ = CreateThread(0, 0, static_cast<LPTHREAD_START_ROUTINE>(func), args,
			CREATE_SUSPENDED, static_cast<LPDWORD>(&thread_id_));
	}

	thread::~thread()
	{
		if (handle_)
			CloseHandle(handle_);
	}


	void thread::start()
	{
		if (handle_)
			ResumeThread(handle_);
	}


	void thread::join()
	{
		if (handle_)
			WaitForSingleObject(handle_, INFINITE);
	}

	void thread::join(unsigned long ms)
	{
		if (handle_)
			WaitForSingleObject(handle_, ms);
	}
}
