#pragma once

#include "D3E/CommonHeader.h"
#include "D3E/Components/BaseComponent.h"
#include "SimpleMath.h"
#include "nvrhi/nvrhi.h"

namespace D3E
{
	enum LightType
	{
		Directional,
		Point,
		Spot
	};

	struct LightComponent : public BaseComponent
	{
		LightType lightType = LightType::Directional;
		DirectX::SimpleMath::Vector3 offset = {0.f, 0.f, 0.f};
		DirectX::SimpleMath::Vector3 direction = {1.0f/1.414f, -1.0f/1.414f, 0.f};
		DirectX::SimpleMath::Vector3 color = 0.8f * DirectX::SimpleMath::Vector3(1.f, 1.f, 1.f);
		float intensity = 1.0f;
		bool castsShadows = false;
		nvrhi::BufferHandle lightCBuffer;
		bool initialized = false;

		void to_json(json& j) const override;
		void from_json(const json& j) override;
	};

	void to_json(json& j, const LightComponent& t);

	void from_json(const json& j, LightComponent& t);
}