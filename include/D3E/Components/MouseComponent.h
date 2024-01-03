#pragma once

#include "D3E/CommonHeader.h"
#include "SimpleMath.h"

using namespace DirectX::SimpleMath;

namespace D3E
{
	struct MouseComponent
	{
		Vector2 position = Vector2(0.0f, 0.0f);
		Vector2 delta = Vector2(0.0f, 0.0f);
	};

	void to_json(json& j, const MouseComponent& t);

	void from_json(const json& j, MouseComponent& t);
}
