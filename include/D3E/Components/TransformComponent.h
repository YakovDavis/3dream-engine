#pragma once

#include "SimpleMath.h"

using namespace DirectX::SimpleMath;

namespace D3E
{
	struct TransformComponent
	{
		Vector3 position = Vector3(0.0f, 0.0f, 0.0f);
		Quaternion rotation = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
		Vector3 scale = Vector3(1.0f, 1.0f, 1.0f);

		Vector3 relativePosition = Vector3(0.f, 0.f, 0.f);
		Quaternion relativeRotation = Quaternion(0.f, 0.f, 0.f, 1.f);
		Vector3 relativeScale = Vector3(1.f, 1.f, 1.f);
	};
} // namespace D3E
