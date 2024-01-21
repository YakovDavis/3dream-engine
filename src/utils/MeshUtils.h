#pragma once

#include "D3E/Components/TransformComponent.h"
#include "EASTL/vector.h"
#include "SimpleMath.h"
#include "assetmng/MeshData.h"

namespace D3E
{
	static void ConvertVertices(const std::vector<Vertex>& in,
	                            eastl::vector<float>& out,
	                            const TransformComponent& tc)
	{
		using DirectX::SimpleMath::Matrix;
		using DirectX::SimpleMath::Vector4;

		auto translation = Matrix::CreateTranslation(tc.position);
		auto rotation = Matrix::CreateFromQuaternion(tc.rotation);
		auto scale = Matrix::CreateScale(tc.scale);
		auto world = scale * rotation * translation;

		for (auto& v : in)
		{
			auto vertex = Vector4::Transform(v.pos, world);

			out.push_back(vertex.x);
			out.push_back(vertex.y);
			out.push_back(vertex.z);
		}
	}

	static void ConvertIndices(const std::vector<uint32_t>& in,
	                           eastl::vector<int>& out)
	{
		for (auto i : in)
		{
			out.push_back(i);
		}
	}
} // namespace D3E