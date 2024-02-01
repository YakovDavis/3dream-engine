#pragma once

#include "D3E/Components/BaseComponent.h"
#include "DetourCrowd.h"

namespace D3E
{
	struct NavigationAgentComponent : public BaseComponent
	{
		int idx = -1;
		dtPolyRef targetRef = 0;
		float targetPosition[3] = {0};

		void to_json(json& j) const override;
		void from_json(const json& j) override;
	};

	void to_json(json& j, const NavigationAgentComponent& c);

	void from_json(const json& j, NavigationAgentComponent& c);
} // namespace D3E