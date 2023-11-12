#pragma once

#include "EASTL/fixed_vector.h"

namespace D3E
{
	struct CameraComponent
	{
		eastl::fixed_vector<float, 3, false> offset = {0, 0, 0};
		eastl::fixed_vector<float, 3, false> forward = {0, 0, 1};
		eastl::fixed_vector<float, 3, false> up = {0, 1, 0};
		bool isOrthographic = false;
		float orthographicWidth = 50.0f;
		float orthographicHeight = 50.0f;
		float fov = 0.785f;
		float aspectRatio = 640.0f / 480.0f;
		float nearPlane = 1.0f;
		float farPlane = 1000.f;
	};
}
