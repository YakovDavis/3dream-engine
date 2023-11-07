#pragma once

#include "EASTL/vector.h"
#include "EASTL/string.h"
#include "SimpleMath.h"

namespace D3E
{
	struct StaticMeshComponent
	{
		eastl::vector<DirectX::SimpleMath::Vector4> points;
		eastl::vector<UINT> indices;
		eastl::string pipelineName;
	};
}
