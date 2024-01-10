#pragma once

namespace D3E
{
	enum class PolyArea
	{
		kGround,
		kWater,
		kDoor,
		kJump,
	};

	enum class PolyFlag
	{
		kWalk,
		kSwim,
		kDoor,
		kJump,
		kDisabled,
		kAll,
	};

	enum class PartitionType
	{
		kLayers,
		kWatershed,
		kMonotone,
	};
} // namespace D3E