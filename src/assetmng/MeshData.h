#pragma once

#include "render/Vertex.h"

namespace D3E
{
	struct MeshData
	{
		std::vector<Vertex> points;
		std::vector<std::uint32_t> indices;
		DirectX::BoundingBox boundingBox;
	};
}