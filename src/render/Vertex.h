#pragma once

#include "SimpleMath.h"

namespace D3E
{
	struct Vertex
	{
		DirectX::SimpleMath::Vector4 pos;
		DirectX::SimpleMath::Vector4 tex;
		DirectX::SimpleMath::Vector4 normal;
	};
}
