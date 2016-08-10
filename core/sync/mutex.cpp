#include "core/sync/mutex.h"

namespace raf
{
	mutex::mutex()
	{
		InitializeCriticalSection(&critical_section_);
	}


	mutex::~mutex()
	{
		DeleteCriticalSection(&critical_section_);
	}


	bool mutex::try_acquire()
	{
		return TryEnterCriticalSection(&critical_section_) == TRUE;
	}


	bool mutex::acquire()
	{
		EnterCriticalSection(&critical_section_);
		return true;
	}


	bool mutex::release()
	{
		LeaveCriticalSection(&critical_section_);
		return true;
	}
}
