#include "core/sync/event.h"

namespace raf
{
	event::event()
	{
		handle_ = CreateEvent(0, FALSE, FALSE, 0);
	}

	event::~event()
	{
		CloseHandle(handle_);
	}

	void event::set()
	{
		SetEvent(handle_);
	}

	bool event::wait()
	{
		WaitForSingleObject(handle_, INFINITE);
		return true;
	}

	bool event::wait(unsigned long ms)
	{
		return WaitForSingleObject(handle_, ms) == WAIT_OBJECT_0;
	}

	bool event::is_set()
	{
		return wait(0);
	}
}