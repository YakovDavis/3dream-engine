#pragma once

#include "D3E/CommonHeader.h"
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
		DirectX::SimpleMath::Vector3 location = {0.f, 0.f, 0.f};
		eastl::string fileName = "";
	};

	void to_json(json& j, const SoundComponent& t);

	void from_json(const json& j, SoundComponent& t);
} // namespace D3E