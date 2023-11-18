#pragma once

#include "SimpleMath.h"
#include "EASTL/string.h"

namespace D3E
{
	struct SoundComponent
	{
		bool is3D = false;
		bool isLooping = false;
		bool isStreaming = false;
		float volume = 0.5f;
		Vector3 location = {0.f, 0.f, 0.f};
		eastl::string fileName = "";
	};
} // namespace D3E