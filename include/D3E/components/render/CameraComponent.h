#pragma once

#include "SimpleMath.h"

namespace D3E
{
	struct CameraComponent
	{
		DirectX::SimpleMath::Vector3 offset = {0, 0, 0};
		DirectX::SimpleMath::Vector3 forward = {0, 0, 1};
		DirectX::SimpleMath::Vector3 up = {0, 1, 0};
		bool isOrthographic = false;
		float orthographicWidth = 50.0f;
		float orthographicHeight = 50.0f;
		float fov = 0.785f;
		float aspectRatio = 1920.0f / 1080.0f;
		float nearPlane = 1.0f;
		float farPlane = 1000.f;
	};
}
