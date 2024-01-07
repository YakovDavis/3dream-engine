#pragma once

#include "D3E/CommonHeader.h"
#include "D3E/Components/BaseComponent.h"
#include "SimpleMath.h"
#include "nvrhi/nvrhi.h"

using namespace DirectX::SimpleMath;

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
		Vector3 offset = Vector3(0.f, 0.f, 0.f);
		Vector3 direction = Vector3(2.f, -1.f, 1.f);
		Vector3 color = Vector3(2.f, 2.f, 2.f);
		float intensity = 1000.0f;
		bool castsShadows = false;
		nvrhi::BufferHandle lightCBuffer;
		nvrhi::BufferHandle csmCBuffer;
		bool initialized = false;

		void to_json(json& j) const override;
		void from_json(const json& j) override;
	};

	void to_json(json& j, const LightComponent& t);

	void from_json(const json& j, LightComponent& t);
}