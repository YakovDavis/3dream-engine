#include "DefaultAssetLoader.h"

#include "D3E/CommonCpp.h"
#include "D3E/render/Material.h"
#include "MaterialFactory.h"
#include "MeshFactory.h"
#include "TextureFactory.h"
#include "render/GeometryGenerator.h"
#include "render/PerObjectConstBuffer.h"
#include "render/ShaderFactory.h"
#include "render/Vertex.h"

eastl::vector<D3E::String> D3E::DefaultAssetLoader::loadedVertexShaders {};

nvrhi::BufferHandle D3E::DefaultAssetLoader::gridCb = nullptr;

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

	GeometryGenerator::CreateGrid(sm, 128.0f, 128.0f, 128, 128);
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
			.setName("BITANGENT")
			.setFormat(nvrhi::Format::RGBA32_FLOAT)
			.setOffset(offsetof(Vertex, bitangent))
			.setElementStride(sizeof(Vertex)),
		nvrhi::VertexAttributeDesc()
			.setName("TEXCOORD")
			.setFormat(nvrhi::Format::RGBA32_FLOAT)
			.setOffset(offsetof(Vertex, tex))
			.setElementStride(sizeof(Vertex)),
	};

	for (const auto& vShaderName : loadedVertexShaders)
	{
		ShaderFactory::AddInputLayout(vShaderName, attributes, 5,
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
	ShaderFactory::AddVertexShader("EditorHighlightPass", "EditorHighlightPass.hlsl", "VSMain");
	ShaderFactory::AddVertexShader("DebugDraw", "DebugDraw.hlsl", "VSMain");

	ShaderFactory::AddPixelShader("SimpleForward", "SimpleForward.hlsl", "PSMain");
	ShaderFactory::AddPixelShader("GBuffer", "GBuffer.hlsl", "PSMain");
	ShaderFactory::AddPixelShader("LightPass", "LightPass.hlsl", "PSMain");
	ShaderFactory::AddPixelShader("EditorHighlightPass", "EditorHighlightPass.hlsl", "PSMain");
	ShaderFactory::AddPixelShader("DebugDraw", "DebugDraw.hlsl", "PSMain");

	ShaderFactory::AddComputeShader("Pick", "Pick.hlsl", "CSMain");

	nvrhi::BindingLayoutDesc layoutDescVDefault = {};
	layoutDescVDefault.setVisibility(nvrhi::ShaderType::Vertex);
	layoutDescVDefault.addItem(nvrhi::BindingLayoutItem::ConstantBuffer(0));
	ShaderFactory::AddBindingLayout("SimpleForwardV", layoutDescVDefault);
	ShaderFactory::AddBindingLayout("GBufferV", layoutDescVDefault);

	nvrhi::BindingLayoutDesc layoutDescVNull = {};
	layoutDescVNull.setVisibility(nvrhi::ShaderType::Vertex);
	ShaderFactory::AddBindingLayout("LightPassV", layoutDescVNull);
	ShaderFactory::AddBindingLayout("EditorHighlightPassV", layoutDescVNull);

	nvrhi::BindingLayoutDesc layoutDescPNull = {};
	layoutDescPNull.setVisibility(nvrhi::ShaderType::Pixel);
	ShaderFactory::AddBindingLayout("EditorHighlightPassP", layoutDescPNull);

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

	nvrhi::BindingLayoutDesc pickLayoutDesc = {};
	pickLayoutDesc.setVisibility(nvrhi::ShaderType::Compute);
	pickLayoutDesc.addItem(nvrhi::BindingLayoutItem::ConstantBuffer(0));
	pickLayoutDesc.addItem(nvrhi::BindingLayoutItem::Texture_SRV(0));
	pickLayoutDesc.addItem(nvrhi::BindingLayoutItem::StructuredBuffer_UAV(0));
	ShaderFactory::AddBindingLayout("PickC", pickLayoutDesc);

	nvrhi::DepthStencilState depthStencilState = {};
	depthStencilState.setDepthTestEnable(true);
	depthStencilState.setDepthWriteEnable(true);
	depthStencilState.setDepthFunc(nvrhi::ComparisonFunc::Less);
	depthStencilState.setStencilEnable(true);
	depthStencilState.setStencilWriteMask(0xFF);
	depthStencilState.setStencilReadMask(0xFF);

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

	nvrhi::DepthStencilState::StencilOpDesc stencilOpDesc = {};
	stencilOpDesc.passOp = nvrhi::StencilOp::Replace;
	stencilOpDesc.stencilFunc = nvrhi::ComparisonFunc::Always;
	depthStencilState.setBackFaceStencil(stencilOpDesc);
	depthStencilState.setFrontFaceStencil(stencilOpDesc);
	depthStencilState.setStencilRefValue(1);
	renderState.depthStencilState = depthStencilState;
	gbufferPipelineDesc.setRenderState(renderState);
	ShaderFactory::AddGraphicsPipeline("GBufferHighlight", gbufferPipelineDesc, gBuffFb);
	depthStencilState.setStencilRefValue(0);
	renderState.depthStencilState = depthStencilState;

	nvrhi::DepthStencilState nullDepthStencilState = {};
	nullDepthStencilState.setDepthTestEnable(false);
	nullDepthStencilState.setDepthWriteEnable(false);
	nullDepthStencilState.setDepthFunc(nvrhi::ComparisonFunc::Always);
	nullDepthStencilState.setStencilEnable(false);

	nvrhi::GraphicsPipelineDesc lightpassPipelineDesc = {};
	lightpassPipelineDesc.setInputLayout(nullptr);
	lightpassPipelineDesc.setVertexShader(ShaderFactory::GetVertexShader("LightPass"));
	lightpassPipelineDesc.setPixelShader(ShaderFactory::GetPixelShader("LightPass"));
	lightpassPipelineDesc.addBindingLayout(ShaderFactory::GetBindingLayout("LightPassV"));
	lightpassPipelineDesc.addBindingLayout(ShaderFactory::GetBindingLayout("LightPassP"));
	renderState.depthStencilState = nullDepthStencilState;
	lightpassPipelineDesc.setRenderState(renderState);
	lightpassPipelineDesc.primType = nvrhi::PrimitiveType::TriangleStrip;
	ShaderFactory::AddGraphicsPipeline("LightPass", lightpassPipelineDesc, fb);

	auto ppDepthStencilState = depthStencilState;
	nvrhi::GraphicsPipelineDesc editorHighlightPipelineDesc = {};
	editorHighlightPipelineDesc.setInputLayout(nullptr);
	editorHighlightPipelineDesc.setVertexShader(ShaderFactory::GetVertexShader("EditorHighlightPass"));
	editorHighlightPipelineDesc.setPixelShader(ShaderFactory::GetPixelShader("EditorHighlightPass"));
	editorHighlightPipelineDesc.addBindingLayout(ShaderFactory::GetBindingLayout("EditorHighlightPassV"));
	editorHighlightPipelineDesc.addBindingLayout(ShaderFactory::GetBindingLayout("EditorHighlightPassP"));
	ppDepthStencilState.setStencilRefValue(0x01);
	ppDepthStencilState.setStencilReadMask(0xFF);
	ppDepthStencilState.setStencilWriteMask(0x00);
	nvrhi::DepthStencilState::StencilOpDesc highlightPassStencilOpDesc = {};
	highlightPassStencilOpDesc.stencilFunc = nvrhi::ComparisonFunc::Equal;
	ppDepthStencilState.setBackFaceStencil(highlightPassStencilOpDesc);
	ppDepthStencilState.setFrontFaceStencil(highlightPassStencilOpDesc);
	renderState.depthStencilState = ppDepthStencilState;
	editorHighlightPipelineDesc.setRenderState(renderState);
	editorHighlightPipelineDesc.primType = nvrhi::PrimitiveType::TriangleStrip;
	ShaderFactory::AddGraphicsPipeline("EditorHighlightPass", editorHighlightPipelineDesc, fb);

	nvrhi::GraphicsPipelineDesc lineListPipelineDesc = {};
	lineListPipelineDesc.setInputLayout(ShaderFactory::GetInputLayout("SimpleForward"));
	lineListPipelineDesc.setVertexShader(ShaderFactory::GetVertexShader("SimpleForward"));
	lineListPipelineDesc.setPixelShader(ShaderFactory::GetPixelShader("SimpleForward"));
	lineListPipelineDesc.addBindingLayout(ShaderFactory::GetBindingLayout("SimpleForwardV"));
	lineListPipelineDesc.addBindingLayout(ShaderFactory::GetBindingLayout("SimpleForwardP"));
	renderState.depthStencilState = depthStencilState;
	lineListPipelineDesc.setRenderState(renderState);
	lineListPipelineDesc.primType = nvrhi::PrimitiveType::LineList;
	ShaderFactory::AddGraphicsPipeline("LineList", lineListPipelineDesc, fb);

	rasterState.fillMode = nvrhi::RasterFillMode::Wireframe;
	renderState.rasterState = rasterState;
	renderState.depthStencilState = depthStencilState;
	pipelineDesc.renderState = renderState;
	ShaderFactory::AddGraphicsPipeline("WireFrame", pipelineDesc, fb);

	nvrhi::ComputePipelineDesc pickingPipelineDesc = {};
	pickingPipelineDesc.setComputeShader(ShaderFactory::GetComputeShader("Pick"));
	pickingPipelineDesc.addBindingLayout(ShaderFactory::GetBindingLayout("PickC"));
	ShaderFactory::AddComputePipeline("Pick", pickingPipelineDesc);

	nvrhi::BindingLayoutDesc layoutDescVDebugDraw = {};
	layoutDescVDebugDraw.setVisibility(nvrhi::ShaderType::Vertex);
	layoutDescVDebugDraw.addItem(nvrhi::BindingLayoutItem::ConstantBuffer(0));
	layoutDescVDebugDraw.addItem(nvrhi::BindingLayoutItem::ConstantBuffer(1));
	ShaderFactory::AddBindingLayout("DebugDrawV", layoutDescVDebugDraw);

	nvrhi::BindingLayoutDesc layoutDescPDebugDraw = {};
	layoutDescPDebugDraw.setVisibility(nvrhi::ShaderType::Pixel);
	ShaderFactory::AddBindingLayout("DebugDrawP", layoutDescPDebugDraw);

	nvrhi::GraphicsPipelineDesc debugDrawPipelineDesc = {};
	debugDrawPipelineDesc.setInputLayout(nullptr);
	debugDrawPipelineDesc.setVertexShader(ShaderFactory::GetVertexShader("DebugDraw"));
	debugDrawPipelineDesc.setPixelShader(ShaderFactory::GetPixelShader("DebugDraw"));
	debugDrawPipelineDesc.addBindingLayout(ShaderFactory::GetBindingLayout("DebugDrawV"));
	debugDrawPipelineDesc.addBindingLayout(ShaderFactory::GetBindingLayout("DebugDrawP"));
	nvrhi::DepthStencilState ddDepthStencilState = {};
	ddDepthStencilState.setDepthTestEnable(true);
	ddDepthStencilState.setDepthWriteEnable(true);
	ddDepthStencilState.setDepthFunc(nvrhi::ComparisonFunc::Less);
	ddDepthStencilState.setStencilEnable(false);
	nvrhi::RasterState ddRasterState = {};
	ddRasterState.fillMode = nvrhi::RasterFillMode::Solid;
	ddRasterState.frontCounterClockwise = false;
	ddRasterState.setCullNone();
	nvrhi::BlendState ddBlendState = {};
	ddBlendState.targets[0] = {};
	nvrhi::RenderState ddRenderState = {};
	ddRenderState.depthStencilState = ddDepthStencilState;
	ddRenderState.rasterState = ddRasterState;
	ddRenderState.blendState = ddBlendState;
	debugDrawPipelineDesc.renderState = ddRenderState;
	debugDrawPipelineDesc.primType = nvrhi::PrimitiveType::LineList;
	ShaderFactory::AddGraphicsPipeline("DebugDraw", debugDrawPipelineDesc, fb);
}

void D3E::DefaultAssetLoader::LoadDefaultSamplers(nvrhi::DeviceHandle& device)
{
	auto samplerDesc = nvrhi::SamplerDesc();
	samplerDesc.minFilter = true;
	samplerDesc.magFilter = true;
	samplerDesc.mipFilter = true;
	samplerDesc.reductionType = nvrhi::SamplerReductionType::Standard;
	TextureFactory::AddSampler("Base", device, samplerDesc);
}

void D3E::DefaultAssetLoader::LoadEditorDebugAssets(
	nvrhi::DeviceHandle& device, nvrhi::CommandListHandle& commandList)
{
	auto constantBufferDesc = nvrhi::BufferDesc()
	                              .setByteSize(sizeof(PerObjectConstBuffer))
	                              .setIsConstantBuffer(true)
	                              .setIsVolatile(false)
	                              .setMaxVersions(16)
	                              .setKeepInitialState(true);

	gridCb = device->createBuffer(constantBufferDesc);

	nvrhi::BindingSetDesc bindingSetDescV = {};
	bindingSetDescV.addItem(nvrhi::BindingSetItem::ConstantBuffer(0, gridCb));
	ShaderFactory::AddBindingSetV(kDebugLineBindingSetUUID, bindingSetDescV, "SimpleForwardV");

	nvrhi::BindingSetDesc bindingSetDescP = {};
	bindingSetDescP.addItem(nvrhi::BindingSetItem::Texture_SRV(0, TextureFactory::GetTextureHandle(kWhiteTextureUUID)));
	bindingSetDescP.addItem(nvrhi::BindingSetItem::Sampler(0, TextureFactory::GetSampler("Base")));
	ShaderFactory::AddBindingSetP(kDebugLineBindingSetUUID, bindingSetDescP, "SimpleForwardP");
}

nvrhi::IBuffer* D3E::DefaultAssetLoader::GetEditorGridCB()
{
	return gridCb;
}

void D3E::DefaultAssetLoader::LoadDefaultMaterials()
{
	Material defaultGrid = {};
	defaultGrid.name = "DefaultGrid";
	defaultGrid.uuid = kDefaultGridMaterialUUID;
	defaultGrid.type = MaterialType::Lit;
	defaultGrid.albedoTextureUuid = "24c71f11-0d38-4594-ae18-c8eedca9b896";
	defaultGrid.normalTextureUuid = "c2346e38-a332-4c9f-bb91-f22591ce5f52";
	defaultGrid.metalnessTextureUuid = "eb93c841-6911-411c-93cd-54b24861e6e7";
	defaultGrid.roughnessTextureUuid = "eb93c841-6911-411c-93cd-54b24861e6e7";
	MaterialFactory::AddMaterial(defaultGrid);
}
