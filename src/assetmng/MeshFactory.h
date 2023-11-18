#pragma once

#include "D3E/CommonHeader.h"
#include "EASTL/unordered_map.h"
#include "EASTL/vector.h"
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

		static eastl::unordered_map<String, MeshData> meshData_;
		static eastl::unordered_map<String, nvrhi::BufferHandle> vBuffers_;
		static eastl::unordered_map<String, nvrhi::VertexBufferBinding> vbBindings_;
		static eastl::unordered_map<String, nvrhi::BufferHandle> iBuffers_;
		static eastl::unordered_map<String, nvrhi::IndexBufferBinding> ibBindings_;

	public:
		MeshFactory() = delete;
		static void Initialize(Game* game);
		static bool IsInitialized() { return isInitialized_; }
		static void DestroyResources();

		static void AddMeshFromData(const String& name, const MeshData& meshData);
		static void FillMeshBuffers(const String& name, nvrhi::DeviceHandle& device, nvrhi::CommandListHandle& commandList);

		static MeshData GetMeshData(const String& name);
		static nvrhi::BufferHandle GetVertexBuffer(const String& name);
		static nvrhi::VertexBufferBinding GetVertexBufferBinding(const String& name);
		static nvrhi::BufferHandle GetIndexBuffer(const String& name);
		static nvrhi::IndexBufferBinding GetIndexBufferBinding(const String& name);
	};
}
