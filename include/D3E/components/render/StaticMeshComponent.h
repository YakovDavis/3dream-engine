#pragma once

#include "EASTL/string.h"
#include "EASTL/vector.h"
#include "nvrhi/nvrhi.h"

namespace D3E
{
	struct StaticMeshComponent
	{
		eastl::string meshName;
		eastl::string pipelineName;
		nvrhi::BufferHandle constantBuffer;
		eastl::vector<nvrhi::BindingSetHandle> bindingSets;

		bool initialized = false;
	};
}
