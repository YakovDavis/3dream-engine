#pragma once

#include "SimpleMath.h"

namespace D3E
{
	struct LightVolumeConstBuffer
	{
		DirectX::SimpleMath::Matrix gWorldViewProj;
	};
}
