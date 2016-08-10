#pragma once
#include "core/utility.h"

namespace raf
{
	class lock : private noncopyable
	{
	protected:
		virtual ~lock() {}
	public:
		virtual bool try_acquire() = 0;
		virtual bool acquire() = 0;
		virtual bool release() = 0;
	};
}