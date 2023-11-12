#pragma once

#include "EASTL/vector.h"
#include "EASTL/unordered_map.h"
#include "EASTL/string.h"
#include "MeshData.h"
#include "render/VertexIndexBufferInfo.h"

namespace D3E
{
	class Game;

	class MeshFactory
	{
	private:
		static bool isInitialized_;
		static Game* activeGame_;

		static eastl::unordered_map<eastl::string, MeshData> meshData_;
		static eastl::unordered_map<eastl::string, nvrhi::BufferHandle> vBuffers_;
		static eastl::unordered_map<eastl::string, nvrhi::VertexBufferBinding> vbBindings_;
		static eastl::unordered_map<eastl::string, nvrhi::BufferHandle> iBuffers_;
		static eastl::unordered_map<eastl::string, nvrhi::IndexBufferBinding> ibBindings_;

	public:
		MeshFactory() = delete;
		static void Initialize(Game* game);
		static bool IsInitialized() { return isInitialized_; }
		static void DestroyResources();

		static void AddMeshFromData(const eastl::string& name, const MeshData& meshData);
		static void FillMeshBuffers(const eastl::string& name, nvrhi::DeviceHandle& device, nvrhi::CommandListHandle& commandList);

		static MeshData GetMeshData(const eastl::string& name);
		static nvrhi::BufferHandle GetVertexBuffer(const eastl::string& name);
		static nvrhi::VertexBufferBinding GetVertexBufferBinding(const eastl::string& name);
		static nvrhi::BufferHandle GetIndexBuffer(const eastl::string& name);
		static nvrhi::IndexBufferBinding GetIndexBufferBinding(const eastl::string& name);
	};
}
