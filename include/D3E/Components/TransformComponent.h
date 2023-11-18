#pragma once

#include "SimpleMath.h"

using namespace DirectX::SimpleMath;

struct TransformComponent
{
	Vector3 position_;
	Vector4 rotation_;
	Vector3 scale_;
};
