#include "DefaultAssetLoader.h"

#include "D3E/CommonCpp.h"
#include "MeshFactory.h"
#include "TextureFactory.h"
#include "render/GeometryGenerator.h"
#include "render/ShaderFactory.h"
#include "render/Vertex.h"

eastl::vector<D3E::String> D3E::DefaultAssetLoader::loadedVertexShaders {};

void D3E::DefaultAssetLoader::LoadPrimitiveMeshes()
{
	MeshData sm = {};

	GeometryGenerator::CreateBox(sm, 1.0f, 1.0f, 1.0f, 0);
	MeshFactory::AddMeshFromData(kCubeUUID, sm);
	sm.points.clear();
	sm.indices.clear();

	GeometryGenerator::CreateQuad(sm, 0, 0, 1.0f, 1.0f, 0.0f);
	MeshFactory::AddMeshFromData(kPlaneUUID, sm);
	sm.points.clear();
	sm.indices.clear();

	GeometryGenerator::CreateCylinder(sm, 1.0f, 1.0f,
	                                  1.0f, 16, 1);
	MeshFactory::AddMeshFromData(kCyllinderUUID, sm);
	sm.points.clear();
	sm.indices.clear();

	GeometryGenerator::CreateSphere(sm, 1.0f, 16, 16);
	MeshFactory::AddMeshFromData(kSphereUUID, sm);
	sm.points.clear();
	sm.indices.clear();

	GeometryGenerator::CreateGeosphere(sm, 1, 0);
	MeshFactory::AddMeshFromData(kGeosphereUUID, sm);
	sm.points.clear();
	sm.indices.clear();

	GeometryGenerator::CreateGrid(sm, 1.0f, 1.0f, 64, 64);
	MeshFactory::AddMeshFromData(kGridUUID, sm);
	sm.points.clear();
	sm.indices.clear();
}

void D3E::DefaultAssetLoader::FillPrimitiveMeshBuffers(
	nvrhi::DeviceHandle& device, nvrhi::CommandListHandle& commandList)
{
	eastl::vector<String> meshUuids = {kCubeUUID, kPlaneUUID, kCyllinderUUID, kSphereUUID,
	                                         kGeosphereUUID, kGridUUID};

	for (const auto& uuid : meshUuids)
	{
		MeshFactory::FillMeshBuffers(uuid, device, commandList);
	}
}

void D3E::DefaultAssetLoader::LoadDefaultInputLayouts()
{
	nvrhi::VertexAttributeDesc attributes[] = {
		nvrhi::VertexAttributeDesc()
			.setName("POSITION")
			.setFormat(nvrhi::Format::RGBA32_FLOAT)
			.setOffset(offsetof(Vertex, pos))
			.setElementStride(sizeof(Vertex)),
		nvrhi::VertexAttributeDesc()
			.setName("NORMAL")
			.setFormat(nvrhi::Format::RGBA32_FLOAT)
			.setOffset(offsetof(Vertex, normal))
			.setElementStride(sizeof(Vertex)),
		nvrhi::VertexAttributeDesc()
			.setName("TANGENT")
			.setFormat(nvrhi::Format::RGBA32_FLOAT)
			.setOffset(offsetof(Vertex, tangentU))
			.setElementStride(sizeof(Vertex)),
		nvrhi::VertexAttributeDesc()
			.setName("TEXCOORD")
			.setFormat(nvrhi::Format::RGBA32_FLOAT)
			.setOffset(offsetof(Vertex, tex))
			.setElementStride(sizeof(Vertex)),
	};

	for (const auto& vShaderName : loadedVertexShaders)
	{
		ShaderFactory::AddInputLayout(vShaderName, attributes, 4,
		                              ShaderFactory::GetVertexShader(vShaderName));
	}
}

void D3E::DefaultAssetLoader::LoadDefaultPSOs(nvrhi::IFramebuffer* fb)
{
	ShaderFactory::AddVertexShader("SimpleForward", "SimpleForward.hlsl", "VSMain");
	loadedVertexShaders.push_back("SimpleForward");

	LoadDefaultInputLayouts();

	ShaderFactory::AddPixelShader("SimpleForward", "SimpleForward.hlsl", "PSMain");

	nvrhi::BindingLayoutDesc layoutDesc0 = {};
	layoutDesc0.setVisibility(nvrhi::ShaderType::Vertex);
	layoutDesc0.addItem(nvrhi::BindingLayoutItem::ConstantBuffer(0));
	ShaderFactory::AddBindingLayout("SimpleForwardV", layoutDesc0);

	nvrhi::BindingLayoutDesc layoutDesc1 = {};
	layoutDesc1.setVisibility(nvrhi::ShaderType::Pixel);
	layoutDesc1.addItem(nvrhi::BindingLayoutItem::Texture_SRV(0));
	layoutDesc1.addItem(nvrhi::BindingLayoutItem::Sampler(0));
	ShaderFactory::AddBindingLayout("SimpleForwardP", layoutDesc1);

	nvrhi::DepthStencilState depthStencilState = {};
	depthStencilState.setDepthTestEnable(true);
	depthStencilState.setDepthWriteEnable(true);
	depthStencilState.setDepthFunc(nvrhi::ComparisonFunc::Less);
	depthStencilState.setStencilEnable(true);

	nvrhi::RasterState rasterState = {};
	rasterState.fillMode = nvrhi::RasterFillMode::Solid;
	rasterState.frontCounterClockwise = false;
	rasterState.setCullBack();

	nvrhi::BlendState blendState = {};
	blendState.targets[0] = {};

	nvrhi::RenderState renderState = {};
	renderState.depthStencilState = depthStencilState;
	renderState.rasterState = rasterState;
	renderState.blendState = blendState;

	nvrhi::GraphicsPipelineDesc pipelineDesc = {};
	pipelineDesc.setInputLayout(ShaderFactory::GetInputLayout("SimpleForward"));
	pipelineDesc.setVertexShader(ShaderFactory::GetVertexShader("SimpleForward"));
	pipelineDesc.setPixelShader(ShaderFactory::GetPixelShader("SimpleForward"));
	pipelineDesc.addBindingLayout(ShaderFactory::GetBindingLayout("SimpleForwardV"));
	pipelineDesc.addBindingLayout(ShaderFactory::GetBindingLayout("SimpleForwardP"));
	pipelineDesc.setRenderState(renderState);
	pipelineDesc.primType = nvrhi::PrimitiveType::TriangleList;
	ShaderFactory::AddGraphicsPipeline("SimpleForward", pipelineDesc, fb);

	rasterState.fillMode = nvrhi::RasterFillMode::Wireframe;
	renderState.rasterState = rasterState;
	pipelineDesc.renderState = renderState;
	ShaderFactory::AddGraphicsPipeline("WireFrame", pipelineDesc, fb);
}

void D3E::DefaultAssetLoader::LoadDefaultSamplers(nvrhi::DeviceHandle& device)
{
	auto samplerDesc = nvrhi::SamplerDesc();
	TextureFactory::AddSampler("Base", device, samplerDesc);
}
