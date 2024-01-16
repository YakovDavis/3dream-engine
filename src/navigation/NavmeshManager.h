#pragma once

#include "Context.h"
#include "DetourCrowd.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshQuery.h"
#include "NavmeshConfig.h"
#include "Recast.h"
#include "assetmng/MeshData.h"

namespace D3E
{
	class NavmeshManager
	{
	public:
		NavmeshManager();
		NavmeshManager(MeshData* meshData, const NavmeshConfig& cfg);

		bool Build();
		void Clear();

	private:
		NavmeshConfig cfg_;
		MeshData* meshData_;
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