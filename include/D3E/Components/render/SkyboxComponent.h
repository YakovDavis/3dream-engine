#pragma once

#include "D3E/CommonHeader.h"
#include "D3E/Components/BaseComponent.h"
#include "EASTL/vector.h"
#include "nvrhi/nvrhi.h"

namespace D3E
{
	struct D3EAPI SkyboxComponent : public BaseComponent
	{
		nvrhi::BufferHandle constantBuffer;
		eastl::vector<nvrhi::BindingSetHandle> bindingSets;

		void to_json(json& j) const override;
		void from_json(const json& j) override;
	};

	void to_json(json& j, const SkyboxComponent& t);

	void from_json(const json& j, SkyboxComponent& t);
}
