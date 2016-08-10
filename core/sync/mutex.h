#pragma once

#include <Windows.h>
#include "core/sync/lock.h"

namespace raf
{
	class mutex : public lock
	{
		friend class condition;

	public:
		mutex();
		virtual ~mutex();
		virtual bool try_acquire();
		virtual bool acquire();
		virtual bool release();

	private:
		CRITICAL_SECTION critical_section_;

	};

	class mutex_lock
	{
	public:
		mutex_lock(mutex* _mutex) :mutex_(_mutex) { mutex_->acquire(); }
		~mutex_lock() { mutex_->release(); }

	private:
		mutex *mutex_;
	};
}
