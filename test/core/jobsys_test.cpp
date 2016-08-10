#include <iostream>
#include "core/jobsys/jobsys.h"

static int global_int = 0;

int addone(int n)
{
	return n + 1;
}

int getone()
{
	global_int += 1;
	return 1;
}

bool rfalse()
{
	return false;
}

int main(int argc, char**argv)
{
	using namespace raf;
	jobsys js;

	// init job #1 to be executing getone per jobsys::next is called.
	js.init(0, &getone, 0);
	// start jobsys
	js.start();

	// 10 next cycles
	for (int i = 0; i < 10; ++i) {
		// signal jobsys for next cycle
		js.next();

		// now prep other stuff
		std::cout << "Working on other useful stuff" << std::endl;
		int myint = i;

		// done preping other stuff
		// wait for job #1 is done
		js.wait(0);

		// use result from job #1 somehow
		std::cout << global_int + myint << std::endl;

		// mark result from job #1 used
		// jobsys::done needs to be called per jobsys::wait is called.
		js.done(0);
	}

	getchar();
	return 0;
}