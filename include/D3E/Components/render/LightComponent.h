#pragma once

#include "nvrhi/nvrhi.h"
#include "SimpleMath.h"

using namespace DirectX::SimpleMath;

namespace D3E
{
	enum LightType
	{
		Directional,
		Point,
		Spot
	};

	struct LightComponent
	{
		LightType lightType = LightType::Directional;
		Vector3 offset = Vector3(0.f, 0.f, 0.f);
		Vector3 direction = Vector3(1.f, -1.f, 1.f);
		Vector3 color = Vector3(2.f, 2.f, 2.f);
		float intensity = 1000.0f;
		bool castsShadows = false;
		nvrhi::BufferHandle lightCBuffer;
		nvrhi::BufferHandle csmCBuffer;
		bool initialized = false;
	};
}