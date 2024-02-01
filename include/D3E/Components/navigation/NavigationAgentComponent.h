#pragma once

#include "D3E/Components/BaseComponent.h"
#include "DetourCrowd.h"

namespace D3E
{
	const int kMaxAganets = 16;
	const int kMaxTrails = 8;

	struct D3EAPI AgentTrail
	{
		float trail[kMaxTrails * 3];
		int htrail;
	};

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