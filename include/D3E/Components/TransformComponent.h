#pragma once

#include "SimpleMath.h"

using namespace DirectX::SimpleMath;

namespace D3E
{
	struct TransformComponent
	{
		Vector3 position_ = Vector3(0.0f, 0.0f, 0.0f);
		Quaternion rotation_ = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
		Vector3 scale_ = Vector3(1.0f, 1.0f, 1.0f);
	};
}
