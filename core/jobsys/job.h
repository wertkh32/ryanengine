#pragma once
#include <assert.h>

#include "core/sync/rwmutex.h"
#include "core/sync/mutex.h"
#include "core/sync/condition.h"
#include "core/functional.h"
#include "core/sync/atomic.h"

namespace raf
{
	namespace job_state
	{
		enum state {
			UNINIT, INIT, RUNNING, READY, DONE
		};
	}
	class job
	{
		friend class jobsys;
	public:
		typedef void(*JOB_FUNC)(void *);
	private:
		void* func_;
		void* arg_;

		volatile job_state::state state_;
		// volatile unsigned int wait_count_;
		atomic_counter wait_count_;

		mutex mutex_;

		condition condition_;
		rwmutex rwmutex_;
		rwmutex wait_count_rwmutex_;

		function_static<bool, job*> wait_ready_predicate_;
		function_static<bool, job*> wait_done_predicate_;
	public:
		job() :func_(nullptr), arg_(nullptr), state_(job_state::UNINIT) //, wait_count_(0)
		{
			wait_ready_predicate_.init(&wait_ready_predicate, this);
			wait_done_predicate_.init(&wait_done_predicate, this);
		}

		job(void* func, void* arg) : func_(func), arg_(arg), state_(job_state::UNINIT) //, wait_count_(0)
		{
			wait_ready_predicate_ = bind(&wait_ready_predicate, this);
			wait_done_predicate_ = bind(&wait_done_predicate, this);
		}

		~job() {}

		bool init(void *func, void *arg)
		{
			assert(state() == job_state::UNINIT);

			mutex_lock lock(&mutex_);
			func_ = func;
			arg_ = arg;
			state(job_state::INIT);

			return true;
		}

		void run()
		{
			assert(state() != job_state::UNINIT);

			condition_.wait(&mutex_, &wait_done_predicate_);

			mutex_lock lock(&mutex_);
			state(job_state::RUNNING);
			static_cast<JOB_FUNC>(func_)(arg_);
			state(job_state::READY);

			condition_.notify_all();
		}

		void done()
		{
			assert(state() == job_state::READY);

			mutex_lock lock(&mutex_);
			assert(wait_count_.value() > 0);
			wait_count_.dec();
			state(job_state::DONE);

			if (wait_count_.value() == 0)
				condition_.notify_all();
		}

		bool wait(unsigned long ms)
		{
			assert(state() != job_state::UNINIT);

			mutex_lock lock(&mutex_);
			wait_count_.inc();
			condition_.wait(&mutex_, &wait_ready_predicate_);

			return true;
		}

		job_state::state state()
		{
			rwmutex_lock lock(&rwmutex_, true);
			return state_;
		}

	private:
		void state(job_state::state _state)
		{
			rwmutex_lock lock(&rwmutex_);
			state_ = _state;
		}

		static bool wait_ready_predicate(job* _job)
		{
			return _job->state() != job_state::READY;
		}

		static bool wait_done_predicate(job* _job)
		{
			job_state::state state = _job->state();
			return state != job_state::DONE && state != job_state::INIT && _job->wait_count_.value() != 0;
		}
	};
}
