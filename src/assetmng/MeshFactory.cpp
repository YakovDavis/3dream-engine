#include "MeshFactory.h"

#include "D3E/Debug.h"

bool D3E::MeshFactory::isInitialized_ = false;
D3E::Game* D3E::MeshFactory::activeGame_;
eastl::unordered_map<eastl::string, D3E::MeshData> D3E::MeshFactory::meshData_ {};
eastl::unordered_map<eastl::string, nvrhi::BufferHandle> D3E::MeshFactory::vBuffers_ {};
eastl::unordered_map<eastl::string, nvrhi::VertexBufferBinding> D3E::MeshFactory::vbBindings_ {};
eastl::unordered_map<eastl::string, nvrhi::BufferHandle> D3E::MeshFactory::iBuffers_ {};
eastl::unordered_map<eastl::string, nvrhi::IndexBufferBinding> D3E::MeshFactory::ibBindings_ {};

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

void D3E::MeshFactory::AddMeshFromData(const eastl::string& name,
	const D3E::MeshData& meshData)
{
	meshData_.insert({name, meshData});
}

void D3E::MeshFactory::FillMeshBuffers(const eastl::string& name,
                                       nvrhi::DeviceHandle& device,
                                       nvrhi::CommandListHandle& commandList)
{
	auto vertexBufferDesc = nvrhi::BufferDesc()
	                            .setByteSize(meshData_[name].points.size() * sizeof(Vertex))
	                            .setIsVertexBuffer(true)
	                            .setInitialState(nvrhi::ResourceStates::VertexBuffer)
	                            .setKeepInitialState(true)
	                            .setDebugName("Vertex Buffer");

	vBuffers_.insert({name, device->createBuffer(vertexBufferDesc)});

	auto indexBufferDesc = nvrhi::BufferDesc()
	                           .setByteSize(meshData_[name].indices.size() * sizeof(uint32_t))
	                           .setIsIndexBuffer(true)
	                           .setInitialState(nvrhi::ResourceStates::IndexBuffer)
	                           .setKeepInitialState(true)
	                           .setDebugName("Index Buffer");

	iBuffers_.insert({name, device->createBuffer(indexBufferDesc)});

	commandList->open();

	commandList->writeBuffer(vBuffers_[name], &(meshData_[name].points[0]), meshData_[name].points.size() * sizeof(Vertex));
	commandList->writeBuffer(iBuffers_[name], &(meshData_[name].indices[0]), meshData_[name].indices.size() * sizeof(std::uint32_t));

	commandList->close();
	device->executeCommandList(commandList);

	nvrhi::VertexBufferBinding vertexBufferBinding = {};
	vertexBufferBinding.buffer = vBuffers_[name];
	vertexBufferBinding.slot = 0;
	vertexBufferBinding.offset = 0;
	vbBindings_.insert({name, vertexBufferBinding});

	nvrhi::IndexBufferBinding indexBufferBinding = {};
	indexBufferBinding.buffer = iBuffers_[name];
	indexBufferBinding.format = nvrhi::Format::R32_UINT;
	indexBufferBinding.offset = 0;
	ibBindings_.insert({name, indexBufferBinding});
}

nvrhi::BufferHandle D3E::MeshFactory::GetVertexBuffer(const eastl::string& name)
{
	if (vBuffers_.find(name) == vBuffers_.end())
	{
		Debug::LogError("[ShaderFactory] Vertex buffer not found");
	}
	return vBuffers_[name];
}

nvrhi::VertexBufferBinding D3E::MeshFactory::GetVertexBufferBinding(
	const eastl::string& name)
{
	if (vbBindings_.find(name) == vbBindings_.end())
	{
		Debug::LogError("[ShaderFactory] Vertex buffer binding not found");
	}
	return vbBindings_[name];
}

nvrhi::BufferHandle D3E::MeshFactory::GetIndexBuffer(const eastl::string& name)
{
	if (iBuffers_.find(name) == iBuffers_.end())
	{
		Debug::LogError("[ShaderFactory] Index buffer not found");
	}
	return iBuffers_[name];
}

nvrhi::IndexBufferBinding D3E::MeshFactory::GetIndexBufferBinding(
	const eastl::string& name)
{
	if (ibBindings_.find(name) == ibBindings_.end())
	{
		Debug::LogError("[ShaderFactory] Index buffer binding not found");
	}
	return ibBindings_[name];
}

D3E::MeshData D3E::MeshFactory::GetMeshData(const eastl::string& name)
{
	if (meshData_.find(name) == meshData_.end())
	{
		Debug::LogError("[ShaderFactory] Mesh data not found");
	}
	return meshData_[name];
}
