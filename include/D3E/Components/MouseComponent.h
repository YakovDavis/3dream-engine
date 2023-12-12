#pragma once

#include "SimpleMath.h"

using namespace DirectX::SimpleMath;

namespace D3E
{
	struct MouseComponent
	{
		Vector2 position = Vector2(0.0f, 0.0f);
		Vector2 delta = Vector2(0.0f, 0.0f);
	};
}
