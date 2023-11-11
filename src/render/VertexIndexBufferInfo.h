#pragma once

#include "nvrhi/nvrhi.h"

namespace D3E
{
	struct VertexIndexBufferInfo
	{
		nvrhi::BufferHandle vertexBuffer;
		nvrhi::BufferHandle indexBuffer;
	};
}