#pragma once

namespace D3E
{
	struct CrowdConfig
	{
		bool expandOptions = true;
		bool anticipateTurns = true;
		bool optimizeVis = true;
		bool optimizeTopo = true;
		bool obstacleAvoidance = true;
		float obstacleAvoidanceType = 3.0f;
		bool separation = false;
		float separationWeight = 2.0f;
	};
} // namespace D3E
