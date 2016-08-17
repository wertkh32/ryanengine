#pragma once
#include <Windows.h>
#include "core/utility.h"
#include "core/sync/mutex.h"
#include "core/sync/rwmutex.h"

namespace raf
{
	class condition : private noncopyable
	{
	public:
		condition()
		{
			InitializeConditionVariable(&condition_variable_);
		}

		virtual ~condition() {}

		void notify()
		{
			WakeConditionVariable(&condition_variable_);
		}
		void notify_all()
		{
			WakeAllConditionVariable(&condition_variable_);
		}

		template<typename T>
		void wait(mutex* _mutex, T* predicate)
		{
			while ((*predicate)())
			{
				SleepConditionVariableCS(&condition_variable_, &_mutex->critical_section_, INFINITE);
			}
		}

		template<typename T>
		void wait(rwmutex* _rwmutex, T* predicate)
		{
			while ((*predicate)())
			{
				SleepConditionVariableSRW(&condition_variable_, &_rwmutex->srwlock_, INFINITE, 0);
			}
		}
	private:
		CONDITION_VARIABLE condition_variable_;
	};
}