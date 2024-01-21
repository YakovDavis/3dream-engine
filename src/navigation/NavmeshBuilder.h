#pragma once

#include "Context.h"
#include "D3E/Components/navigation/NavmeshComponent.h"
#include "D3E/Components/navigation/NavmeshConfig.h"
#include "DetourCrowd.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshQuery.h"
#include "EASTL/vector.h"
#include "Recast.h"
#include "assetmng/MeshData.h"

namespace D3E
{
	class NavmeshBuilder
	{
	public:
		NavmeshBuilder();
		NavmeshBuilder(const eastl::vector<float>* v,
		               const eastl::vector<int>* i);
		~NavmeshBuilder();

		bool Build(NavmeshComponent& nc);
		void Clear();

	private:
		const eastl::vector<float>* vertices_;
		const eastl::vector<int>* indices_;

		NavmeshConfig cfg_;
		rcHeightfield* heightField_;
		rcCompactHeightfield* compactHeightField_;
		rcContourSet* contourSet_;
		rcPolyMesh* polyMesh_;
		rcPolyMeshDetail* polyMeshDetail_;
		Context ctx_;
		dtNavMesh* navMesh_;
		dtNavMeshQuery* navQuery_;
		dtCrowd* crowd_;
		unsigned char* triAreas_;
	};
} // namespace D3E