#include "MeshFactory.h"

#include "D3E/CommonCpp.h"
#include "D3E/Debug.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"

bool D3E::MeshFactory::isInitialized_ = false;
D3E::Game* D3E::MeshFactory::activeGame_;
eastl::unordered_map<D3E::String, D3E::MeshData> D3E::MeshFactory::meshData_ {};
eastl::unordered_map<D3E::String, nvrhi::BufferHandle> D3E::MeshFactory::vBuffers_ {};
eastl::unordered_map<D3E::String, nvrhi::VertexBufferBinding> D3E::MeshFactory::vbBindings_ {};
eastl::unordered_map<D3E::String, nvrhi::BufferHandle> D3E::MeshFactory::iBuffers_ {};
eastl::unordered_map<D3E::String, nvrhi::IndexBufferBinding> D3E::MeshFactory::ibBindings_ {};

void D3E::MeshFactory::Initialize(D3E::Game* game)
{
	if (isInitialized_)
		return;

	activeGame_ = game;
	isInitialized_ = true;
}

void D3E::MeshFactory::DestroyResources()
{
	if (!isInitialized_)
		return;

	meshData_.clear();
	vBuffers_.clear();
	iBuffers_.clear();

	isInitialized_ = false;
}

void D3E::MeshFactory::AddMeshFromData(const String& uuid,
	const D3E::MeshData& meshData)
{
	meshData_.insert({uuid, meshData});
}

void D3E::MeshFactory::FillMeshBuffers(const String& uuid,
                                       nvrhi::IDevice* device,
                                       nvrhi::ICommandList* commandList)
{
	auto vertexBufferDesc = nvrhi::BufferDesc()
	                            .setByteSize(meshData_[uuid].points.size() * sizeof(Vertex))
	                            .setIsVertexBuffer(true)
	                            .setInitialState(nvrhi::ResourceStates::VertexBuffer)
	                            .setKeepInitialState(true)
	                            .setDebugName("Vertex Buffer");

	vBuffers_.insert({uuid, device->createBuffer(vertexBufferDesc)});

	auto indexBufferDesc = nvrhi::BufferDesc()
	                           .setByteSize(meshData_[uuid].indices.size() * sizeof(uint32_t))
	                           .setIsIndexBuffer(true)
	                           .setInitialState(nvrhi::ResourceStates::IndexBuffer)
	                           .setKeepInitialState(true)
	                           .setDebugName("Index Buffer");

	iBuffers_.insert({uuid, device->createBuffer(indexBufferDesc)});

	commandList->open();

	commandList->writeBuffer(vBuffers_[uuid], &(meshData_[uuid].points[0]), meshData_[uuid].points.size() * sizeof(Vertex));
	commandList->writeBuffer(iBuffers_[uuid], &(meshData_[uuid].indices[0]), meshData_[uuid].indices.size() * sizeof(std::uint32_t));

	commandList->close();
	device->executeCommandList(commandList);

	nvrhi::VertexBufferBinding vertexBufferBinding = {};
	vertexBufferBinding.buffer = vBuffers_[uuid];
	vertexBufferBinding.slot = 0;
	vertexBufferBinding.offset = 0;
	vbBindings_.insert({uuid, vertexBufferBinding});

	nvrhi::IndexBufferBinding indexBufferBinding = {};
	indexBufferBinding.buffer = iBuffers_[uuid];
	indexBufferBinding.format = nvrhi::Format::R32_UINT;
	indexBufferBinding.offset = 0;
	ibBindings_.insert({uuid, indexBufferBinding});
}

nvrhi::BufferHandle D3E::MeshFactory::GetVertexBuffer(const String& uuid)
{
	if (vBuffers_.find(uuid) == vBuffers_.end())
	{
		Debug::LogError("[MeshFactory] Vertex buffer not found. UUID: " + uuid);
	}
	return vBuffers_[uuid];
}

nvrhi::VertexBufferBinding D3E::MeshFactory::GetVertexBufferBinding(
	const String& uuid)
{
	if (vbBindings_.find(uuid) == vbBindings_.end())
	{
		Debug::LogError("[MeshFactory] Vertex buffer binding not found. UUID: " + uuid);
	}
	return vbBindings_[uuid];
}

nvrhi::BufferHandle D3E::MeshFactory::GetIndexBuffer(const String& uuid)
{
	if (iBuffers_.find(uuid) == iBuffers_.end())
	{
		Debug::LogError("[MeshFactory] Index buffer not found. UUID: " + uuid);
	}
	return iBuffers_[uuid];
}

nvrhi::IndexBufferBinding D3E::MeshFactory::GetIndexBufferBinding(
	const String& uuid)
{
	if (ibBindings_.find(uuid) == ibBindings_.end())
	{
		Debug::LogError("[MeshFactory] Index buffer binding not found. UUID: " + uuid);
	}
	return ibBindings_[uuid];
}

D3E::MeshData D3E::MeshFactory::GetMeshData(const String& uuid)
{
	if (meshData_.find(uuid) == meshData_.end())
	{
		Debug::LogError("[MeshFactory] Mesh data not found. UUID: " + uuid);
	}
	return meshData_[uuid];
}

void D3E::MeshFactory::LoadMesh(const D3E::MeshMetaData& metaData, bool firstLoad, nvrhi::IDevice* device, nvrhi::ICommandList* commandList)
{
	meshData_.insert({ metaData.uuid.c_str(), MeshData() });

	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(metaData.filename.c_str(), aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);

	ProcessNode(metaData, pScene->mRootNode, pScene);

	MeshFactory::FillMeshBuffers(metaData.uuid.c_str(), device, commandList);
}

void D3E::MeshFactory::ProcessNode(const D3E::MeshMetaData& metaData, aiNode* node,
                                   const aiScene* scene)
{
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		ProcessMesh(metaData, mesh, scene);
	}
	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(metaData, node->mChildren[i], scene);
	}
}

void D3E::MeshFactory::ProcessMesh(const D3E::MeshMetaData& metaData, aiMesh* mesh,
                                   const aiScene* scene)
{
	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex point;

		point.pos.x = mesh->mVertices[i].x;
		point.pos.y = mesh->mVertices[i].y;
		point.pos.z = mesh->mVertices[i].z;
		point.pos.w = 1.0f;

		if (mesh->mTextureCoords[0])
		{
			point.tex.x = mesh->mTextureCoords[0][i].x;
			point.tex.y = mesh->mTextureCoords[0][i].y;
		}

		point.normal.x = mesh->mNormals[i].x;
		point.normal.y = mesh->mNormals[i].y;
		point.normal.z = mesh->mNormals[i].z;
		point.normal.w = 0.0f;

		meshData_[metaData.uuid.c_str()].points.push_back(point);
	}

	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		const aiFace face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
			meshData_[metaData.uuid.c_str()].indices.push_back(face.mIndices[j]);
	}
}
