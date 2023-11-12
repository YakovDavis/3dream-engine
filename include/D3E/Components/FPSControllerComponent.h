#pragma once

namespace D3E
{
	struct FPSControllerComponent
	{
		float yaw = 0.0f;
		float pitch = 0.0f;
		float speed = 0.02f;
		float sensitivityX = 0.01f;
		float sensitivityY = 0.01f;
		bool isLMBActivated = true;
	};
}
