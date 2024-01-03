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
		String materialUuid;
		eastl::vector<nvrhi::BindingSetHandle> bindingSets;
		bool editorHighlighted = false;

		bool initialized = false;
	};

	void to_json(json& j, const StaticMeshComponent& t);

	void from_json(const json& j, StaticMeshComponent& t);
}
