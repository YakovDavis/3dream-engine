#pragma once

#include "assetmng/SkyboxVertex.h"

namespace D3E
{
	struct SkyMeshData
	{
		std::vector<SkyboxVertex> points;
		std::vector<std::uint32_t> indices;
	};
}