#include "GameRender.h"

#include "D3E/Game.h"
#include "D3E/Common.h"
#include "D3E/Debug.h"
#include "DisplayWin32.h"
#include "ShaderFactory.h"
#include "assetmng/MeshFactory.h"
#include "Vertex.h"
#include "D3E/components/render/StaticMeshComponent.h"
#include "render/GeometryGenerator.h"
#include "PerObjectConstBuffer.h"
#include "CameraUtils.h"

#include <nvrhi/utils.h> // for ClearColorAttachment

void D3E::GameRender::Init()
{
	Debug::LogMessage("[GameRender] Init started");
	if (!device_.Get())
	{
		Debug::LogError("[GameRender] GAPI not initialized");
		Debug::Assert(true, "GAPI not initialized");
	}

	nvrhi::FramebufferDesc framebufferDesc0 = {};
	framebufferDesc0.addColorAttachment(nvrhiSwapChain[0]);
	nvrhiFramebuffer.push_back(device_->createFramebuffer(framebufferDesc0));

	Debug::LogMessage("fb0");

	nvrhi::FramebufferDesc framebufferDesc1 = {};
	framebufferDesc1.addColorAttachment(nvrhiSwapChain[1]);
	nvrhiFramebuffer.push_back(device_->createFramebuffer(framebufferDesc1));

	Debug::LogMessage("fb1");

	ShaderFactory::Initialize(dynamic_cast<Game*>(parentApp));
	MeshFactory::Initialize(dynamic_cast<Game*>(parentApp));

	// TEMPORARY SECTION, to be moved

	ShaderFactory::AddVertexShader("SimpleForward", "SimpleForward.hlsl", "VSMain");

	Debug::LogMessage("vertex");

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
	ShaderFactory::AddInputLayout("SimpleForward", attributes, 4, ShaderFactory::GetVertexShader("SimpleForward"));

	Debug::LogMessage("input layout");

	ShaderFactory::AddPixelShader("SimpleForward", "SimpleForward.hlsl", "PSMain");

	Debug::LogMessage("pixel");

	nvrhi::BindingLayoutDesc layoutDesc0 = {};
	layoutDesc0.setVisibility(nvrhi::ShaderType::Vertex);
	layoutDesc0.addItem(nvrhi::BindingLayoutItem::ConstantBuffer(0));
	ShaderFactory::AddBindingLayout("SimpleForwardV", layoutDesc0);

	Debug::LogMessage("binding layout v");

	nvrhi::BindingLayoutDesc layoutDesc1 = {};
	layoutDesc1.setVisibility(nvrhi::ShaderType::Pixel);
	layoutDesc1.addItem(nvrhi::BindingLayoutItem::Texture_SRV(0));
	layoutDesc1.addItem(nvrhi::BindingLayoutItem::Sampler(0));
	ShaderFactory::AddBindingLayout("SimpleForwardP", layoutDesc1);

	Debug::LogMessage("binding layout p");

	nvrhi::DepthStencilState depthStencilState = {};
	depthStencilState.setDepthTestEnable(false);
	depthStencilState.setStencilEnable(false);

	nvrhi::RasterState rasterState = {};
	rasterState.setCullNone();

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
	ShaderFactory::AddGraphicsPipeline("SimpleForward", pipelineDesc, nvrhiFramebuffer[0]);

	Debug::LogMessage("graphics pipeline");

	auto constantBufferDesc = nvrhi::BufferDesc()
	                              .setByteSize(sizeof(PerObjectConstBuffer))
	                              .setIsConstantBuffer(true)
	                              .setIsVolatile(false)
	                              .setMaxVersions(16)
								  .setKeepInitialState(true);

	constantBuffer = device_->createBuffer(constantBufferDesc);

	Debug::LogMessage("cb");

	MeshData sm = {};
	GeometryGenerator::CreateBox(sm, 1.0f, 1.0f, 1.0f, 0);

	MeshFactory::AddMeshFromData("Cube", sm);

	// Assume the texture pixel data is loaded from and decoded elsewhere.
	auto textureDesc = nvrhi::TextureDesc()
	                       .setDimension(nvrhi::TextureDimension::Texture2D)
	                       .setWidth(1024)
	                       .setHeight(1024)
	                       .setFormat(nvrhi::Format::SRGBA8_UNORM)
	                       .setInitialState(nvrhi::ResourceStates::ShaderResource)
	                       .setKeepInitialState(true)
	                       .setDebugName("Geometry Texture");

	testTexture = device_->createTexture(textureDesc);

	Debug::LogMessage("tex");

	auto samplerDesc = nvrhi::SamplerDesc();

	testSampler = device_->createSampler(samplerDesc);

	Debug::LogMessage("sampler");

	commandList_ = device_->createCommandList();

	Debug::LogMessage("command list");

	MeshFactory::FillMeshBuffers("Cube", device_, commandList_);

	nvrhi::BindingSetDesc bindingSetDescV = {};
	bindingSetDescV.addItem(nvrhi::BindingSetItem::ConstantBuffer(0, constantBuffer));
	ShaderFactory::AddBindingSet("SimpleForwardV", bindingSetDescV, "SimpleForwardV");

	nvrhi::BindingSetDesc bindingSetDescP = {};
	bindingSetDescP.addItem(nvrhi::BindingSetItem::Texture_SRV(0, testTexture));
	bindingSetDescP.addItem(nvrhi::BindingSetItem::Sampler(0, testSampler));
	ShaderFactory::AddBindingSet("SimpleForwardP", bindingSetDescP, "SimpleForwardP");

	// END OF TEMPORARY SECTION

	Debug::LogMessage("[GameRender] Init finished");
}

void D3E::GameRender::DestroyResources()
{
}

void D3E::GameRender::OnResize()
{
}

void D3E::GameRender::CalculateFrameStats()
{
	/* TODO: recreate using our timers
	// Code computes the average frames per second, and also the
	// average time it takes to render one frame.  These stats
	// are appended to the window caption bar.

	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if((mTimer.TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		std::wstring fpsStr = std::to_wstring(fps);
		std::wstring mspfStr = std::to_wstring(mspf);

		std::wstring windowText = mMainWndCaption +
		                     L"    fps: " + fpsStr +
		                     L"   mspf: " + mspfStr;

		SetWindowText(mhMainWnd, windowText.c_str());

		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
	*/
}

D3E::GameRender::GameRender(App* parent, HINSTANCE hInstance) : parentApp(parent)
{
	assert(parentApp != nullptr);
	assert(hInstance != nullptr);
	display_ = eastl::make_shared<DisplayWin32>(reinterpret_cast<LPCWSTR>(parentApp->GetName().c_str()), hInstance, 640, 480, parent);
	messageCallback_ = new NvrhiMessageCallback();
}

D3E::Display* D3E::GameRender::GetDisplay()
{
	return display_.get();
}

nvrhi::DeviceHandle& D3E::GameRender::GetDevice()
{
	return device_;
}

nvrhi::CommandListHandle& D3E::GameRender::GetCommandList()
{
	return commandList_;
}

void D3E::GameRender::Draw()
{
	// Obtain the current framebuffer from the graphics API
	nvrhi::IFramebuffer* currentFramebuffer = nvrhiFramebuffer[GetCurrentFrameBuffer()];

	commandList_->open();

	// Clear the primary render target
	nvrhi::utils::ClearColorAttachment(commandList_, currentFramebuffer, 0, nvrhi::Color(0.2f));

	// Fill the constant buffer
	PerObjectConstBuffer constBufferData = {};

	const DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(1) * DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0, 0, 0));

	CameraComponent cameraComponent;

	eastl::fixed_vector<float, 3, false> origin = {0, 0, 0};

	constBufferData.gWorldViewProj = world * CameraUtils::GetViewProj(origin, cameraComponent);
	constBufferData.gWorld = world;
	constBufferData.gWorldView = world * CameraUtils::GetView(origin, cameraComponent);
	constBufferData.gInvTrWorldView = (DirectX::SimpleMath::Matrix::CreateScale(1)).Invert().Transpose() * CameraUtils::GetViewProj(origin, cameraComponent);

	commandList_->writeBuffer(constantBuffer, &constBufferData, sizeof(constBufferData));


	// Set the graphics state: pipeline, framebuffer, viewport, bindings.
	auto graphicsState = nvrhi::GraphicsState()
	                         .setPipeline(ShaderFactory::GetGraphicsPipeline("SimpleForward"))
	                         .setFramebuffer(currentFramebuffer)
	                         .setViewport(nvrhi::ViewportState().addViewportAndScissorRect(nvrhi::Viewport(display_->ClientWidth, display_->ClientHeight)))
	                         .addBindingSet(ShaderFactory::GetBindingSet("SimpleForwardV"))
	                         .addBindingSet(ShaderFactory::GetBindingSet("SimpleForwardP"))
	                         .addVertexBuffer(MeshFactory::GetVertexBufferBinding("Cube"));
	graphicsState.setIndexBuffer(MeshFactory::GetIndexBufferBinding("Cube"));
	commandList_->setGraphicsState(graphicsState);

	// Draw our geometry
	auto drawArguments = nvrhi::DrawArguments()
	                         .setVertexCount(MeshFactory::GetMeshData("Cube").indices.size());
	commandList_->drawIndexed(drawArguments);

	// Close and execute the command list
	commandList_->close();
	device_->executeCommandList(commandList_);
}
