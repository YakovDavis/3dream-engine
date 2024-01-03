#pragma once

#include "D3E/CommonHeader.h"

namespace D3E
{
	struct FPSControllerComponent
	{
		float yaw = 3.14f;
		float pitch = 0.0f;
		float speed = 0.02f;
		float sensitivityX = 0.01f;
		float sensitivityY = 0.01f;
		bool isLMBActivated = true;
	};

	void to_json(json& j, const FPSControllerComponent& t);

	void from_json(const json& j, FPSControllerComponent& t);
}
