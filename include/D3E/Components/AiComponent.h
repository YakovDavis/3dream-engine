#pragma once

#include "D3E/Components/BaseComponent.h"

namespace D3E
{
	struct D3EAPI AiComponent : public BaseComponent
	{
		void to_json(json& j) const override;
		void from_json(const json& j) override;
	};

	void to_json(json& j, const AiComponent& t);

	void from_json(const json& j, AiComponent& t);
} // namespace D3E