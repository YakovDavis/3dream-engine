#pragma once

#include "EASTL/fixed_vector.h"
#include "EASTL/string.h"

namespace D3E
{
	struct SoundComponent
	{
		bool is3D = false;
		bool isLooping = false;
		bool isStreaming = false;
		float volume = 0.5f;
		eastl::fixed_vector<float, 3, false> location = {0.f, 0.f, 0.f};
		eastl::string fileName = "";
	};
} // namespace D3E