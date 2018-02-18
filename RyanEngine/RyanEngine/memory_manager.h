#pragma once
#include "universal.h"

class MemoryManager : public Singleton<MemoryManager>
{
	friend Singleton<MemoryManager>;
	MemoryManager ();
	~MemoryManager ();
public:

};

