#include "DefaultAssetLoader.h"

#include "D3E/AssetManager.h"
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
	AssetManager::RegisterExternalAssetName(kCubeUUID, "Cube");

	GeometryGenerator::CreateQuad(sm, 0, 0, 1.0f, 1.0f, 0.0f);
	MeshFactory::AddMeshFromData(kPlaneUUID, sm);
	sm.points.clear();
	sm.indices.clear();
	AssetManager::RegisterExternalAssetName(kPlaneUUID, "Plane");

	GeometryGenerator::CreateCylinder(sm, 1.0f, 1.0f,
	                                  1.0f, 16, 1);
	MeshFactory::AddMeshFromData(kCyllinderUUID, sm);
	sm.points.clear();
	sm.indices.clear();
	AssetManager::RegisterExternalAssetName(kCyllinderUUID, "Cyllinder");

	GeometryGenerator::CreateSphere(sm, 1.0f, 16, 16);
	MeshFactory::AddMeshFromData(kSphereUUID, sm);
	sm.points.clear();
	sm.indices.clear();
	AssetManager::RegisterExternalAssetName(kSphereUUID, "Sphere");

	GeometryGenerator::CreateGeosphere(sm, 1, 0);
	MeshFactory::AddMeshFromData(kGeosphereUUID, sm);
	sm.points.clear();
	sm.indices.clear();
	AssetManager::RegisterExternalAssetName(kGeosphereUUID, "Geosphere");

	GeometryGenerator::CreateGrid(sm, 128.0f, 128.0f, 128, 128);
	MeshFactory::AddMeshFromData(kGridUUID, sm);
	sm.points.clear();
	sm.indices.clear();
	AssetManager::RegisterExternalAssetName(kGridUUID, "Grid");
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

	nvrhi::VertexAttributeDesc skyboxAttributes[] = {
		nvrhi::VertexAttributeDesc()
			.setName("POSITION")
			.setFormat(nvrhi::Format::RGB32_FLOAT)
			.setOffset(0)
			.setElementStride(sizeof(DirectX::SimpleMath::Vector3))
	};

	ShaderFactory::AddInputLayout("Skybox", skyboxAttributes, 1, ShaderFactory::GetVertexShader("Skybox"));
}

void D3E::DefaultAssetLoader::LoadDefaultPSOs(nvrhi::IFramebuffer* fb, nvrhi::IFramebuffer* gBuffFb)
{
	ShaderFactory::AddVertexShader("SimpleForward", "SimpleForward.hlsl", "VSMain");
	loadedVertexShaders.push_back("SimpleForward");
	ShaderFactory::AddVertexShader("GBuffer", "GBuffer.hlsl", "VSMain");
	loadedVertexShaders.push_back("GBuffer");

	ShaderFactory::AddVertexShader("Skybox", "Skybox.hlsl", "VSMain");

	LoadDefaultInputLayouts();

	ShaderFactory::AddVertexShader("LightPass", "LightPass.hlsl", "VSMain");
	ShaderFactory::AddVertexShader("Tonemap", "Tonemap.hlsl", "VSMain");
	ShaderFactory::AddVertexShader("EditorHighlightPass", "EditorHighlightPass.hlsl", "VSMain");
	ShaderFactory::AddVertexShader("DebugDraw", "DebugDraw.hlsl", "VSMain");

	ShaderFactory::AddPixelShader("SimpleForward", "SimpleForward.hlsl", "PSMain");
	ShaderFactory::AddPixelShader("GBuffer", "GBuffer.hlsl", "PSMain");
	ShaderFactory::AddPixelShader("LightPass", "LightPass.hlsl", "PSMain");
	ShaderFactory::AddPixelShader("Skybox", "Skybox.hlsl", "PSMain");
	ShaderFactory::AddPixelShader("Tonemap", "Tonemap.hlsl", "PSMain");
	ShaderFactory::AddPixelShader("EditorHighlightPass", "EditorHighlightPass.hlsl", "PSMain");
	ShaderFactory::AddPixelShader("DebugDraw", "DebugDraw.hlsl", "PSMain");

	ShaderFactory::AddComputeShader("Pick", "Pick.hlsl", "CSMain");
	ShaderFactory::AddComputeShader("Equirect2Cube", "Equirect2Cube.hlsl", "CSMain");
	ShaderFactory::AddComputeShader("DownsampleLinear", "Downsample.hlsl", "DownsampleLinear");
	ShaderFactory::AddComputeShader("DownsampleGamma", "Downsample.hlsl", "DownsampleGamma");
	ShaderFactory::AddComputeShader("DownsampleArray", "DownsampleArray.hlsl", "DownsampleLinear");
	ShaderFactory::AddComputeShader("SpMap", "SpMap.hlsl", "CSMain");
	ShaderFactory::AddComputeShader("IrMap", "IrMap.hlsl", "CSMain");
	ShaderFactory::AddComputeShader("SpBrdf", "SpBrdf.hlsl", "CSMain");

	nvrhi::BindingLayoutDesc layoutDescVDefault = {};
	layoutDescVDefault.setVisibility(nvrhi::ShaderType::Vertex);
	layoutDescVDefault.addItem(nvrhi::BindingLayoutItem::ConstantBuffer(0));
	ShaderFactory::AddBindingLayout("SimpleForwardV", layoutDescVDefault);
	ShaderFactory::AddBindingLayout("GBufferV", layoutDescVDefault);
	ShaderFactory::AddBindingLayout("SkyboxV", layoutDescVDefault);

	nvrhi::BindingLayoutDesc layoutDescVNull = {};
	layoutDescVNull.setVisibility(nvrhi::ShaderType::Vertex);
	ShaderFactory::AddBindingLayout("LightPassV", layoutDescVNull);
	ShaderFactory::AddBindingLayout("TonemapV", layoutDescVNull);
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

	nvrhi::BindingLayoutDesc layoutDescSky = {};
	layoutDescSky.setVisibility(nvrhi::ShaderType::Pixel);
	layoutDescSky.addItem(nvrhi::BindingLayoutItem::Texture_SRV(0));
	layoutDescSky.addItem(nvrhi::BindingLayoutItem::Sampler(0));
	ShaderFactory::AddBindingLayout("SkyboxP", layoutDescSky);

	nvrhi::BindingLayoutDesc layoutDescTonemap = {};
	layoutDescTonemap.setVisibility(nvrhi::ShaderType::Pixel);
	layoutDescTonemap.addItem(nvrhi::BindingLayoutItem::Texture_SRV(0));
	layoutDescTonemap.addItem(nvrhi::BindingLayoutItem::Sampler(0));
	ShaderFactory::AddBindingLayout("TonemapP", layoutDescTonemap);

	nvrhi::BindingLayoutDesc pickLayoutDesc = {};
	pickLayoutDesc.setVisibility(nvrhi::ShaderType::Compute);
	pickLayoutDesc.addItem(nvrhi::BindingLayoutItem::ConstantBuffer(0));
	pickLayoutDesc.addItem(nvrhi::BindingLayoutItem::Texture_SRV(0));
	pickLayoutDesc.addItem(nvrhi::BindingLayoutItem::StructuredBuffer_UAV(0));
	ShaderFactory::AddBindingLayout("PickC", pickLayoutDesc);

	nvrhi::BindingLayoutDesc equirect2CubeLayoutDesc = {};
	equirect2CubeLayoutDesc.setVisibility(nvrhi::ShaderType::Compute);
	equirect2CubeLayoutDesc.addItem(nvrhi::BindingLayoutItem::Texture_SRV(0));
	equirect2CubeLayoutDesc.addItem(nvrhi::BindingLayoutItem::Texture_UAV(0));
	equirect2CubeLayoutDesc.addItem(nvrhi::BindingLayoutItem::Sampler(0));
	ShaderFactory::AddBindingLayout("Equirect2CubeC", equirect2CubeLayoutDesc);

	nvrhi::BindingLayoutDesc downsampleLayoutDesc = {};
	downsampleLayoutDesc.setVisibility(nvrhi::ShaderType::Compute);
	downsampleLayoutDesc.addItem(nvrhi::BindingLayoutItem::Texture_SRV(0));
	downsampleLayoutDesc.addItem(nvrhi::BindingLayoutItem::Texture_UAV(0));
	ShaderFactory::AddBindingLayout("DownsampleC", downsampleLayoutDesc);

	nvrhi::BindingLayoutDesc spMapLayoutDesc = {};
	spMapLayoutDesc.setVisibility(nvrhi::ShaderType::Compute);
	spMapLayoutDesc.addItem(nvrhi::BindingLayoutItem::ConstantBuffer(0));
	spMapLayoutDesc.addItem(nvrhi::BindingLayoutItem::Texture_SRV(0));
	spMapLayoutDesc.addItem(nvrhi::BindingLayoutItem::Texture_UAV(0));
	spMapLayoutDesc.addItem(nvrhi::BindingLayoutItem::Sampler(0));;
	ShaderFactory::AddBindingLayout("SpMapC", spMapLayoutDesc);

	nvrhi::BindingLayoutDesc irMapLayoutDesc = {};
	irMapLayoutDesc.setVisibility(nvrhi::ShaderType::Compute);
	irMapLayoutDesc.addItem(nvrhi::BindingLayoutItem::Texture_SRV(0));
	irMapLayoutDesc.addItem(nvrhi::BindingLayoutItem::Texture_UAV(0));
	irMapLayoutDesc.addItem(nvrhi::BindingLayoutItem::Sampler(0));;
	ShaderFactory::AddBindingLayout("IrMapC", irMapLayoutDesc);

	nvrhi::BindingLayoutDesc spBrdfLayoutDesc = {};
	spBrdfLayoutDesc.setVisibility(nvrhi::ShaderType::Compute);
	spBrdfLayoutDesc.addItem(nvrhi::BindingLayoutItem::Texture_UAV(0));
	ShaderFactory::AddBindingLayout("SpBrdfC", spBrdfLayoutDesc);

	nvrhi::DepthStencilState::StencilOpDesc stencilOpDesc1 = {};
	stencilOpDesc1.passOp = nvrhi::StencilOp::Replace;
	stencilOpDesc1.failOp = nvrhi::StencilOp::Keep;
	stencilOpDesc1.stencilFunc = nvrhi::ComparisonFunc::Always;

	nvrhi::DepthStencilState depthStencilState = {};
	depthStencilState.setDepthTestEnable(true);
	depthStencilState.setDepthWriteEnable(true);
	depthStencilState.setDepthFunc(nvrhi::ComparisonFunc::Less);
	depthStencilState.setStencilEnable(true);
	depthStencilState.setStencilWriteMask(0x01);
	depthStencilState.setStencilRefValue(0x01);
	depthStencilState.setStencilReadMask(0x00);
	depthStencilState.setFrontFaceStencil(stencilOpDesc1);
	depthStencilState.setBackFaceStencil(stencilOpDesc1);

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

	nvrhi::GraphicsPipelineDesc skyboxPipelineDesc = {};
	skyboxPipelineDesc.setInputLayout(ShaderFactory::GetInputLayout("Skybox"));
	skyboxPipelineDesc.setVertexShader(ShaderFactory::GetVertexShader("Skybox"));
	skyboxPipelineDesc.setPixelShader(ShaderFactory::GetPixelShader("Skybox"));
	skyboxPipelineDesc.addBindingLayout(ShaderFactory::GetBindingLayout("SkyboxV"));
	skyboxPipelineDesc.addBindingLayout(ShaderFactory::GetBindingLayout("SkyboxP"));
	rasterState.setCullNone();
	nvrhi::DepthStencilState::StencilOpDesc skyStencilOpDesc = {};
	skyStencilOpDesc.passOp = nvrhi::StencilOp::Keep;
	skyStencilOpDesc.stencilFunc = nvrhi::ComparisonFunc::NotEqual;
	skyStencilOpDesc.failOp = nvrhi::StencilOp::Keep;
	nvrhi::DepthStencilState skyDSState = depthStencilState;
	skyDSState.setDepthTestEnable(false);
	skyDSState.setDepthWriteEnable(false);
	skyDSState.setStencilRefValue(0x01);
	skyDSState.setStencilEnable(true);
	skyDSState.setFrontFaceStencil(skyStencilOpDesc);
	skyDSState.setBackFaceStencil(skyStencilOpDesc);
	skyDSState.setStencilReadMask(0x01);
	skyDSState.setStencilWriteMask(0x01);
	renderState.depthStencilState = skyDSState;
	skyboxPipelineDesc.setRenderState(renderState);
	skyboxPipelineDesc.primType = nvrhi::PrimitiveType::TriangleList;
	ShaderFactory::AddGraphicsPipeline("Skybox", skyboxPipelineDesc, fb);
	rasterState.setCullBack();
	renderState.depthStencilState = depthStencilState;

	nvrhi::DepthStencilState::StencilOpDesc stencilOpDesc = {};
	stencilOpDesc.passOp = nvrhi::StencilOp::Replace;
	stencilOpDesc.failOp = nvrhi::StencilOp::Keep;
	stencilOpDesc.stencilFunc = nvrhi::ComparisonFunc::Always;
	depthStencilState.setBackFaceStencil(stencilOpDesc);
	depthStencilState.setFrontFaceStencil(stencilOpDesc);
	depthStencilState.setStencilWriteMask(0x03);
	depthStencilState.setStencilRefValue(0x03);
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
	nvrhi::DepthStencilState::StencilOpDesc lightpassStencilOpDesc = {};
	lightpassStencilOpDesc.passOp = nvrhi::StencilOp::Keep;
	lightpassStencilOpDesc.stencilFunc = nvrhi::ComparisonFunc::Equal;
	lightpassStencilOpDesc.failOp = nvrhi::StencilOp::Keep;
	nvrhi::DepthStencilState lightpassDSState = depthStencilState;
	lightpassDSState.setDepthTestEnable(false);
	lightpassDSState.setDepthWriteEnable(false);
	lightpassDSState.setStencilRefValue(0x01);
	lightpassDSState.setStencilEnable(true);
	lightpassDSState.setFrontFaceStencil(lightpassStencilOpDesc);
	lightpassDSState.setBackFaceStencil(lightpassStencilOpDesc);
	lightpassDSState.setStencilReadMask(0x01);
	lightpassDSState.setStencilWriteMask(0x01);
	renderState.depthStencilState = lightpassDSState;
	lightpassPipelineDesc.setRenderState(renderState);
	lightpassPipelineDesc.primType = nvrhi::PrimitiveType::TriangleStrip;
	ShaderFactory::AddGraphicsPipeline("LightPass", lightpassPipelineDesc, fb);

	nvrhi::GraphicsPipelineDesc tonemapPipelineDesc = {};
	tonemapPipelineDesc.setInputLayout(nullptr);
	tonemapPipelineDesc.setVertexShader(ShaderFactory::GetVertexShader("Tonemap"));
	tonemapPipelineDesc.setPixelShader(ShaderFactory::GetPixelShader("Tonemap"));
	tonemapPipelineDesc.addBindingLayout(ShaderFactory::GetBindingLayout("TonemapV"));
	tonemapPipelineDesc.addBindingLayout(ShaderFactory::GetBindingLayout("TonemapP"));
	renderState.depthStencilState = nullDepthStencilState;
	rasterState.setCullNone();
	renderState.rasterState = rasterState;
	tonemapPipelineDesc.setRenderState(renderState);
	tonemapPipelineDesc.primType = nvrhi::PrimitiveType::TriangleList;
	ShaderFactory::AddGraphicsPipeline("Tonemap", tonemapPipelineDesc, fb);
	rasterState.setCullBack();

	auto ppDepthStencilState = depthStencilState;
	nvrhi::GraphicsPipelineDesc editorHighlightPipelineDesc = {};
	editorHighlightPipelineDesc.setInputLayout(nullptr);
	editorHighlightPipelineDesc.setVertexShader(ShaderFactory::GetVertexShader("EditorHighlightPass"));
	editorHighlightPipelineDesc.setPixelShader(ShaderFactory::GetPixelShader("EditorHighlightPass"));
	editorHighlightPipelineDesc.addBindingLayout(ShaderFactory::GetBindingLayout("EditorHighlightPassV"));
	editorHighlightPipelineDesc.addBindingLayout(ShaderFactory::GetBindingLayout("EditorHighlightPassP"));
	ppDepthStencilState.setStencilRefValue(0x02);
	ppDepthStencilState.setStencilReadMask(0x02);
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

	nvrhi::ComputePipelineDesc equirect2CubePipelineDesc = {};
	equirect2CubePipelineDesc.setComputeShader(ShaderFactory::GetComputeShader("Equirect2Cube"));
	equirect2CubePipelineDesc.addBindingLayout(ShaderFactory::GetBindingLayout("Equirect2CubeC"));
	ShaderFactory::AddComputePipeline("Equirect2Cube", equirect2CubePipelineDesc);

	nvrhi::ComputePipelineDesc downsampleLinearPipelineDesc = {};
	downsampleLinearPipelineDesc.setComputeShader(ShaderFactory::GetComputeShader("DownsampleLinear"));
	downsampleLinearPipelineDesc.addBindingLayout(ShaderFactory::GetBindingLayout("DownsampleC"));
	ShaderFactory::AddComputePipeline("DownsampleLinear", downsampleLinearPipelineDesc);

	nvrhi::ComputePipelineDesc downsampleGammaPipelineDesc = {};
	downsampleGammaPipelineDesc.setComputeShader(ShaderFactory::GetComputeShader("DownsampleGamma"));
	downsampleGammaPipelineDesc.addBindingLayout(ShaderFactory::GetBindingLayout("DownsampleC"));
	ShaderFactory::AddComputePipeline("DownsampleGamma", downsampleGammaPipelineDesc);

	nvrhi::ComputePipelineDesc downsampleArrayPipelineDesc = {};
	downsampleArrayPipelineDesc.setComputeShader(ShaderFactory::GetComputeShader("DownsampleArray"));
	downsampleArrayPipelineDesc.addBindingLayout(ShaderFactory::GetBindingLayout("DownsampleC"));
	ShaderFactory::AddComputePipeline("DownsampleArray", downsampleArrayPipelineDesc);

	nvrhi::ComputePipelineDesc spMapPipelineDesc = {};
	spMapPipelineDesc.setComputeShader(ShaderFactory::GetComputeShader("SpMap"));
	spMapPipelineDesc.addBindingLayout(ShaderFactory::GetBindingLayout("SpMapC"));
	ShaderFactory::AddComputePipeline("SpMap", spMapPipelineDesc);

	nvrhi::ComputePipelineDesc irMapPipelineDesc = {};
	irMapPipelineDesc.setComputeShader(ShaderFactory::GetComputeShader("IrMap"));
	irMapPipelineDesc.addBindingLayout(ShaderFactory::GetBindingLayout("IrMapC"));
	ShaderFactory::AddComputePipeline("IrMap", irMapPipelineDesc);

	nvrhi::ComputePipelineDesc spBrdfPipelineDesc = {};
	spBrdfPipelineDesc.setComputeShader(ShaderFactory::GetComputeShader("SpBrdf"));
	spBrdfPipelineDesc.addBindingLayout(ShaderFactory::GetBindingLayout("SpBrdfC"));
	ShaderFactory::AddComputePipeline("SpBrdf", spBrdfPipelineDesc);

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
	samplerDesc.addressU = nvrhi::SamplerAddressMode::Wrap;
	samplerDesc.addressV = nvrhi::SamplerAddressMode::Wrap;
	samplerDesc.addressW = nvrhi::SamplerAddressMode::Wrap;
	TextureFactory::AddSampler("BaseCompute", device, samplerDesc);

	auto sampler2Desc = nvrhi::SamplerDesc();
	sampler2Desc.setMaxAnisotropy(16.0f);
	sampler2Desc.minFilter = true;
	sampler2Desc.magFilter = true;
	sampler2Desc.mipFilter = true;
	sampler2Desc.addressU = nvrhi::SamplerAddressMode::Wrap;
	sampler2Desc.addressV = nvrhi::SamplerAddressMode::Wrap;
	sampler2Desc.addressW = nvrhi::SamplerAddressMode::Wrap;
	TextureFactory::AddSampler("BaseGraphics", device, sampler2Desc);
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
	bindingSetDescP.addItem(nvrhi::BindingSetItem::Sampler(0, TextureFactory::GetSampler("BaseGraphics")));
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
