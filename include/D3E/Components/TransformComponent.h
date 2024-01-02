#pragma once

#include "SimpleMath.h"
#include "json_fwd.hpp"

using json = nlohmann::json;
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

	void to_json(json& j, const TransformComponent& t);

	void from_json(const json& j, TransformComponent& t);
} // namespace D3E
