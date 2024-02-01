#pragma once

#include "D3E/Components/BaseComponent.h"
#include "D3E/Components/navigation/NavmeshConfig.h"
#include "DetourCrowd.h"
#include "DetourNavMeshQuery.h"
#include "DetourNavmesh.h"

namespace D3E
{
	const int kMaxAganets = 16;
	const int kMaxTrails = 8;

	struct D3EAPI AgentTrail
	{
		float trail[kMaxTrails * 3];
		int htrail;
	};

	struct D3EAPI NavmeshComponent : public BaseComponent
	{
		bool isBuilt = false;
		NavmeshConfig config;

		dtNavMesh* navMesh = nullptr;
		dtNavMeshQuery* navQuery = nullptr;
		dtCrowd* crowd = nullptr;
		AgentTrail trails[kMaxAganets];

		void to_json(json& j) const override;
		void from_json(const json& j) override;
	};

	void to_json(json& j, const NavmeshComponent& c);

	void from_json(const json& j, NavmeshComponent& c);
} // namespace D3E