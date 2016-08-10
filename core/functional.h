#pragma once

namespace raf
{
	template<typename T, typename A>
	class function_static
	{
	public:
		function_static() {}
		function_static(T(*func)(A), A args) : func_(func), args_(A(args)) {}
		~function_static() {}

		void init(T(*func)(A), A args)
		{
			func_ = func;
			args_ = args;
		}

		T operator()()
		{
			return func_(args_);
		}

	private:
		T(*func_)(A);
		A args_;
	};

	template<class C, typename T, typename A>
	class function_dynamic
	{
		function_dynamic(C* obj, T(C::*func)(A), A args) : obj_(obj), func_(func), args_(A(args))
		{
		}
		~function_dynamic() {}

		T operator()()
		{
			return (obj_->*func_)(args_);
		}

	private:
		C* obj_;
		T(C::*func_)(A);
		A args_;
	};

	template<class C, typename T, typename A>
	function_dynamic<C, T, A> bind(C* obj, T(C::*func)(A), A args)
	{
		return function_dynamic<C, T, A>(obj, func, args);
	}

	template<typename T, typename A>
	function_static<T, A> bind(T(*func)(A), A args)
	{
		return function_static<T, A>(func, args);
	}
}