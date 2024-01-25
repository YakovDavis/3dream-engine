#pragma once

#include "NavmeshCore.h"

namespace D3E
{
	struct D3EAPI NavmeshConfig
	{
		float cellSize = 0.3f;
		float cellHeight = 0.2f;
		float walkableSlopeAngle = 45.0f;
		float agentHeight = 2.0f;
		float agentRadius = 0.6f;
		float agentMaxClimb = 0.9f;
		float maxEdgeLen = 12.0f;
		float maxSimplificationError = 1.3f;
		float minRegionSize = 8.0f;
		float regionMergeSize = 20.0f;
		int maxVertsPerPoly = 6;
		float detailSampleDist = 6.0f;
		float detailSampleMaxError = 1.0f;
		bool filterLowHangingObstacles = true;
		bool filterLedgeSpans = true;
		bool filterWalkableLowHeightSpans = true;

		PartitionType partitionType = PartitionType::kWatershed;
	};
} // namespace D3E