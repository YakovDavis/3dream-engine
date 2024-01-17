#pragma once

#include "BaseComponent.h"

#include "navigation/NavmeshConfig.h"

namespace D3E
{
	struct NavmeshComponent : public BaseComponent
	{
		NavmeshConfig config;

		void to_json(json& j) const override;
		void from_json(const json& j) override;
	};

	void to_json(json& j, const NavmeshComponent& c);

	void from_json(const json& j, NavmeshComponent& c);
} // namespace D3E