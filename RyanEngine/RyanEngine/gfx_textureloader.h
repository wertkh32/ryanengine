#pragma once
#include "universal.h"

class GfxTextureLoader : public Singleton< GfxTextureLoader >
{
	friend Singleton<GfxTextureLoader>;
	GfxTextureLoader ();
	~GfxTextureLoader ();
public:
};

