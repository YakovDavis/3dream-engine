#pragma once

#include "D3E/CommonHeader.h"
#include "D3E/Components/BaseComponent.h"
#include "D3E/Uuid.h"
#include "EASTL/string.h"
#include "SimpleMath.h"

namespace D3E
{
	struct SoundComponent : public BaseComponent
	{
		bool is3D = false;
		bool isLooping = false;
		bool isStreaming = false;
		float volume = 0.5f;
		DirectX::SimpleMath::Vector3 location = {0.f, 0.f, 0.f};
		//eastl::string fileName = "";
		String soundUuid = EmptyIdString;

		void to_json(json& j) const override;
		void from_json(const json& j) override;
	};

	void to_json(json& j, const SoundComponent& t);

	void from_json(const json& j, SoundComponent& t);
} // namespace D3E