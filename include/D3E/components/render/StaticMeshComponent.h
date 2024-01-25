#pragma once

#include "D3E/CommonHeader.h"
#include "D3E/Components/BaseComponent.h"
#include "EASTL/vector.h"
#include "nvrhi/nvrhi.h"

namespace D3E
{
	struct StaticMeshComponent : public BaseComponent
	{
		String meshUuid = kCubeUUID;
		String pipelineName = "GBuffer";
		nvrhi::BufferHandle constantBuffer;
		String materialUuid = kDefaultGridMaterialUUID;
		eastl::vector<nvrhi::BindingSetHandle> bindingSets;
		bool editorHighlighted = false;
		bool castsShadow = true;

		bool initialized = false;

		void to_json(json& j) const override;
		void from_json(const json& j) override;
	};

	void to_json(json& j, const StaticMeshComponent& t);

	void from_json(const json& j, StaticMeshComponent& t);
}
