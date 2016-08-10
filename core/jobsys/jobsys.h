#pragma once
#include "core/jobsys/job.h"
#include "core/sync/event.h"
#include "core/sync/thread.h"

namespace raf
{
	class jobsys {
	public:
		enum {
			MAX_JOB_PER_THREAD = 8,
			MAX_THREADS = 4,
			MAX_JOBS = 32
		};
	private:

		struct thread_func_arg
		{
			event stop_event;
			event next_event;

			job* job_list;
			unsigned int job_count;
		};

		job job_list_[MAX_JOBS];
		thread threads_[MAX_THREADS];
		thread_func_arg thread_args[MAX_THREADS];

		bool started_;
	public:
		jobsys();
		~jobsys();

		bool init(unsigned int job_num, void *func, void*arg);

		void start();

		void next();

		bool wait(unsigned int job_num);

		bool wait(unsigned int job_num, unsigned long ms);

		void done(unsigned int job_num);
	private:
		static DWORD WINAPI thread_func(void *arg);
	};
}