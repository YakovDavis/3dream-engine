#pragma once

#include "D3E/CommonHeader.h"
#include "EASTL/vector.h"
#include "nvrhi/nvrhi.h"

namespace D3E
{
	struct StaticMeshComponent
	{
		String meshUuid;
		String pipelineName;
		nvrhi::BufferHandle constantBuffer;
		eastl::vector<nvrhi::BindingSetHandle> bindingSets;
		bool editorHighlighted = false;

		bool initialized = false;
	};
}
