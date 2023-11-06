#pragma once

#include "EASTL/vector.h"
#include "SimpleMath.h"

namespace D3E
{
	struct BaseRenderComponent
	{
		eastl::vector<DirectX::SimpleMath::Vector4> points;
		eastl::vector<UINT> indices;
	};
}
