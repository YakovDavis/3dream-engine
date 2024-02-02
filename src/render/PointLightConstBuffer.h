#pragma once

#include "SimpleMath.h"

namespace D3E
{
	struct PointLightConstBuffer
	{
		DirectX::SimpleMath::Vector4 gLightPos;
		DirectX::SimpleMath::Vector4 gLightColor;
		DirectX::SimpleMath::Vector4 gEyePosition;
	};
}
