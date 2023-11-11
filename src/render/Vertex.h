#pragma once

#include "SimpleMath.h"

namespace D3E
{
	struct Vertex
	{
		DirectX::SimpleMath::Vector4 pos;
		DirectX::SimpleMath::Vector4 normal;
		DirectX::SimpleMath::Vector4 tangentU;
		DirectX::SimpleMath::Vector4 tex;

		Vertex() = default;
		Vertex(
			const DirectX::SimpleMath::Vector4& p,
			const DirectX::SimpleMath::Vector4& n,
			const DirectX::SimpleMath::Vector4& t,
			const DirectX::SimpleMath::Vector4& uv) :
			  pos(p),
			  normal(n),
			  tangentU(t),
			  tex(uv) {}
		Vertex(
			float px, float py, float pz,
			float nx, float ny, float nz,
			float tx, float ty, float tz,
			float u, float v) :
			  pos(px, py, pz, 1.0f),
			  normal(nx, ny, nz, 0.0f),
			  tangentU(tx, ty, tz, 0.0f),
			  tex(u, v, 0.0f, 0.0f) {}
	};
}
