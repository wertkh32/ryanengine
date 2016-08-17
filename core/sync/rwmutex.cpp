#include "core/sync/rwmutex.h"

namespace raf
{
	rwmutex::rwmutex()
	{
		InitializeSRWLock(&srwlock_);
	}

	rwmutex::~rwmutex()
	{
	}

	bool rwmutex::try_acquire()
	{
		return try_acquire(false);
	}

	bool rwmutex::try_acquire(bool shared)
	{
		if (shared)
			return TryAcquireSRWLockShared(&srwlock_) != 0;
		else
			return TryAcquireSRWLockExclusive(&srwlock_) != 0;
	}

	bool rwmutex::acquire()
	{
		return acquire(false);
	}

	bool rwmutex::acquire(bool shared)
	{
		if (shared)
			AcquireSRWLockShared(&srwlock_);
		else
			AcquireSRWLockExclusive(&srwlock_);
		return true;
	}

	bool rwmutex::release()
	{
		return release(false);
	}

	bool rwmutex::release(bool shared)
	{
		if (shared)
			ReleaseSRWLockShared(&srwlock_);
		else
			ReleaseSRWLockExclusive(&srwlock_);
		return true;
	}
}