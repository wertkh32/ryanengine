#pragma once
#include <assert.h>
#include "core/jobsys/jobsys.h"

namespace raf
{
	jobsys::jobsys() :started_(false)
	{
		for (int i = 0; i < MAX_THREADS; ++i)
		{
			thread_args[i].job_list = &job_list_[MAX_JOB_PER_THREAD * i];
			thread_args[i].job_count = MAX_JOB_PER_THREAD;

			threads_[i].init(&jobsys::thread_func, static_cast<void*>(&thread_args[i]));
		}
	}

	jobsys::~jobsys()
	{
		if (started_) {
			for (int i = 0; i < MAX_THREADS; ++i)
			{
				thread_args[i].stop_event.set();
				thread_args[i].next_event.set();
			}

			for (int i = 0; i < MAX_THREADS; ++i)
			{
				threads_[i].join();
			}
		}
	}

	bool jobsys::init(unsigned int job_num, void* func, void* arg)
	{
		return job_list_[job_num].init(func, arg);
	}

	void jobsys::start()
	{
		for (int i = 0; i < MAX_THREADS; ++i)
		{
			threads_[i].start();
		}
		started_ = true;
	}

	void jobsys::next()
	{
		for (int i = 0; i < MAX_THREADS; ++i)
		{
			thread_args[i].next_event.set();
		}
	}

	bool jobsys::wait(unsigned int job_num)
	{
		return wait(job_num, INFINITE);
	}

	bool jobsys::wait(unsigned int job_num, unsigned long ms)
	{
		return job_list_[job_num].wait(ms);
	}

	void jobsys::done(unsigned int job_num)
	{
		job_list_[job_num].done();
	}

	DWORD WINAPI jobsys::thread_func(void *arg)
	{
		thread_func_arg* func_arg = static_cast<thread_func_arg*>(arg);

		func_arg->next_event.wait();
		while (!func_arg->stop_event.is_set())
		{
			for (unsigned int i = 0; i < func_arg->job_count; ++i)
			{
				if (func_arg->job_list[i].state() == job_state::UNINIT)
					continue;
				func_arg->job_list[i].run();
			}
			func_arg->next_event.wait();
		}

		return 0;
	};
}