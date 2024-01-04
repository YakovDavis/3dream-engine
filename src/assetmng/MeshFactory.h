#pragma once

#include "D3E/CommonHeader.h"
#include "EASTL/unordered_map.h"
#include "EASTL/vector.h"
#include "MeshData.h"
#include "MeshMetaData.h"
#include "assimp/scene.h"
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

		static void ProcessNode(const D3E::MeshMetaData& metaData, aiNode* node, const aiScene* scene);
		static void ProcessMesh(const D3E::MeshMetaData& metaData, aiMesh* mesh, const aiScene* scene);

	public:
		MeshFactory() = delete;
		static void Initialize(Game* game);
		static bool IsInitialized() { return isInitialized_; }
		static void DestroyResources();

		static void AddMeshFromData(const String& uuid, const MeshData& meshData);
		static void LoadMesh(const MeshMetaData& metaData, bool firstLoad, nvrhi::IDevice* device, nvrhi::ICommandList* commandList);
		static void FillMeshBuffers(const String& uuid, nvrhi::IDevice* device, nvrhi::ICommandList* commandList);

		static MeshData& GetMeshData(const String& uuid);
		static nvrhi::BufferHandle GetVertexBuffer(const String& uuid);
		static nvrhi::VertexBufferBinding GetVertexBufferBinding(const String& uuid);
		static nvrhi::BufferHandle GetIndexBuffer(const String& uuid);
		static nvrhi::IndexBufferBinding GetIndexBufferBinding(const String& uuid);
	};
}
