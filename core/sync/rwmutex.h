#pragma once
#include <Windows.h>
#include "core/sync/lock.h"

namespace raf
{
	class rwmutex : public lock
	{
		friend class condition;
	private:
		SRWLOCK srwlock_;
	public:
		rwmutex();
		virtual ~rwmutex();

		virtual bool try_acquire();
		virtual bool try_acquire(bool shared);
		virtual bool acquire();
		virtual bool acquire(bool shared);
		virtual bool release();
		virtual bool release(bool shared);
	};


	class rwmutex_lock
	{
	public:
		rwmutex_lock(rwmutex* _rwmutex, bool shared) :rwmutex_(_rwmutex), shared_(shared)
		{
			rwmutex_->acquire(shared_);
		}
		rwmutex_lock(rwmutex* _rwmutex) :rwmutex_(_rwmutex), shared_(false)
		{
			rwmutex_->acquire(false);
		}
		~rwmutex_lock() { rwmutex_->release(shared_); }

	private:
		rwmutex *rwmutex_;
		bool shared_;
	};
}