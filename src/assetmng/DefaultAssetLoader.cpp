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

void D3E::DefaultAssetLoader::LoadDefaultPSOs(nvrhi::IFramebuffer* fb, nvrhi::IFramebuffer* gBuffFb)
{
	ShaderFactory::AddVertexShader("SimpleForward", "SimpleForward.hlsl", "VSMain");
	loadedVertexShaders.push_back("SimpleForward");
	ShaderFactory::AddVertexShader("GBuffer", "GBuffer.hlsl", "VSMain");
	loadedVertexShaders.push_back("GBuffer");

	LoadDefaultInputLayouts();

	ShaderFactory::AddVertexShader("LightPass", "LightPass.hlsl", "VSMain");

	ShaderFactory::AddPixelShader("SimpleForward", "SimpleForward.hlsl", "PSMain");
	ShaderFactory::AddPixelShader("GBuffer", "GBuffer.hlsl", "PSMain");
	ShaderFactory::AddPixelShader("LightPass", "LightPass.hlsl", "PSMain");

	nvrhi::BindingLayoutDesc layoutDescVDefault = {};
	layoutDescVDefault.setVisibility(nvrhi::ShaderType::Vertex);
	layoutDescVDefault.addItem(nvrhi::BindingLayoutItem::ConstantBuffer(0));
	ShaderFactory::AddBindingLayout("SimpleForwardV", layoutDescVDefault);
	ShaderFactory::AddBindingLayout("GBufferV", layoutDescVDefault);

	nvrhi::BindingLayoutDesc layoutDescVNull = {};
	layoutDescVNull.setVisibility(nvrhi::ShaderType::Vertex);
	ShaderFactory::AddBindingLayout("LightPassV", layoutDescVNull);

	nvrhi::BindingLayoutDesc layoutDesc1 = {};
	layoutDesc1.setVisibility(nvrhi::ShaderType::Pixel);
	layoutDesc1.addItem(nvrhi::BindingLayoutItem::Texture_SRV(0));
	layoutDesc1.addItem(nvrhi::BindingLayoutItem::Sampler(0));
	ShaderFactory::AddBindingLayout("SimpleForwardP", layoutDesc1);

	nvrhi::BindingLayoutDesc layoutDesc2 = {};
	layoutDesc2.setVisibility(nvrhi::ShaderType::Pixel);
	layoutDesc2.addItem(nvrhi::BindingLayoutItem::Texture_SRV(0));
	layoutDesc2.addItem(nvrhi::BindingLayoutItem::Texture_SRV(1));
	layoutDesc2.addItem(nvrhi::BindingLayoutItem::Texture_SRV(2));
	layoutDesc2.addItem(nvrhi::BindingLayoutItem::Texture_SRV(3));
	layoutDesc2.addItem(nvrhi::BindingLayoutItem::Texture_SRV(4));
	layoutDesc2.addItem(nvrhi::BindingLayoutItem::Texture_SRV(5));
	layoutDesc2.addItem(nvrhi::BindingLayoutItem::Sampler(0));
	layoutDesc2.addItem(nvrhi::BindingLayoutItem::Sampler(1));
	ShaderFactory::AddBindingLayout("GBufferP", layoutDesc2);

	nvrhi::BindingLayoutDesc layoutDescLight = {};
	layoutDescLight.setVisibility(nvrhi::ShaderType::Pixel);
	layoutDescLight.addItem(nvrhi::BindingLayoutItem::ConstantBuffer(0));
	layoutDescLight.addItem(nvrhi::BindingLayoutItem::ConstantBuffer(1));
	layoutDescLight.addItem(nvrhi::BindingLayoutItem::Texture_SRV(0));
	layoutDescLight.addItem(nvrhi::BindingLayoutItem::Texture_SRV(1));
	layoutDescLight.addItem(nvrhi::BindingLayoutItem::Texture_SRV(2));
	layoutDescLight.addItem(nvrhi::BindingLayoutItem::Texture_SRV(3));
	layoutDescLight.addItem(nvrhi::BindingLayoutItem::Texture_SRV(4));
	layoutDescLight.addItem(nvrhi::BindingLayoutItem::Texture_SRV(5));
	layoutDescLight.addItem(nvrhi::BindingLayoutItem::Texture_SRV(6));
	layoutDescLight.addItem(nvrhi::BindingLayoutItem::Texture_SRV(7));
	layoutDescLight.addItem(nvrhi::BindingLayoutItem::Sampler(0));
	layoutDescLight.addItem(nvrhi::BindingLayoutItem::Sampler(1));
	layoutDescLight.addItem(nvrhi::BindingLayoutItem::Sampler(2));
	ShaderFactory::AddBindingLayout("LightPassP", layoutDescLight);

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

	nvrhi::GraphicsPipelineDesc gbufferPipelineDesc = {};
	gbufferPipelineDesc.setInputLayout(ShaderFactory::GetInputLayout("GBuffer"));
	gbufferPipelineDesc.setVertexShader(ShaderFactory::GetVertexShader("GBuffer"));
	gbufferPipelineDesc.setPixelShader(ShaderFactory::GetPixelShader("GBuffer"));
	gbufferPipelineDesc.addBindingLayout(ShaderFactory::GetBindingLayout("GBufferV"));
	gbufferPipelineDesc.addBindingLayout(ShaderFactory::GetBindingLayout("GBufferP"));
	gbufferPipelineDesc.setRenderState(renderState);
	gbufferPipelineDesc.primType = nvrhi::PrimitiveType::TriangleList;
	ShaderFactory::AddGraphicsPipeline("GBuffer", gbufferPipelineDesc, gBuffFb);

	nvrhi::GraphicsPipelineDesc lightpassPipelineDesc = {};
	lightpassPipelineDesc.setInputLayout(nullptr);
	lightpassPipelineDesc.setVertexShader(ShaderFactory::GetVertexShader("LightPass"));
	lightpassPipelineDesc.setPixelShader(ShaderFactory::GetPixelShader("LightPass"));
	lightpassPipelineDesc.addBindingLayout(ShaderFactory::GetBindingLayout("LightPassV"));
	lightpassPipelineDesc.addBindingLayout(ShaderFactory::GetBindingLayout("LightPassP"));
	lightpassPipelineDesc.setRenderState(renderState);
	lightpassPipelineDesc.primType = nvrhi::PrimitiveType::TriangleStrip;
	ShaderFactory::AddGraphicsPipeline("LightPass", lightpassPipelineDesc, fb);

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
