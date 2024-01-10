#pragma once

#include "NavmeshCore.h"

namespace D3E
{
	struct NavmeshConfig
	{
		float cellSize;
		float cellHeight;
		float walkableSlopeAngle;
		float agentHeight;
		float agentRadius;
		float agentMaxClimb;
		float maxEdgeLen;
		float maxSimplificationError;
		float minRegionSize;
		float regionMergeSize;
		int maxVertsPerPoly;
		float detailSampleDist;
		float detailSampleMaxError;
		bool filterLowHangingObstacles;
		bool filterLedgeSpans;
		bool filterWalkableLowHeightSpans;

		PartitionType partitionType;
	};
} // namespace D3E