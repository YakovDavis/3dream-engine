#pragma once

#include "EASTL/vector.h"
#include "EASTL/string.h"
#include "render/Vertex.h"

namespace D3E
{
	struct StaticMeshComponent
	{
		eastl::string meshName;
		eastl::string pipelineName;
	};
}
