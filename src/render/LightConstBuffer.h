#pragma once

#include "SimpleMath.h"

namespace D3E
{
	struct LightConstBuffer
	{
		DirectX::SimpleMath::Vector4 gLightDir;
		DirectX::SimpleMath::Vector4 gLightColor;
		DirectX::SimpleMath::Vector4 gEyePosition;
		DirectX::SimpleMath::Matrix gView;
	};
}
