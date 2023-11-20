#include "GameRender.h"

#include "CameraUtils.h"
#include "D3E/Common.h"
#include "D3E/Debug.h"
#include "D3E/Game.h"
#include "D3E/components/render/StaticMeshComponent.h"
#include "DisplayWin32.h"
#include "PerObjectConstBuffer.h"
#include "ShaderFactory.h"
#include "Vertex.h"
#include "assetmng/MeshFactory.h"
#include "assetmng/TextureFactory.h"
#include "render/GeometryGenerator.h"
#include "render/systems/StaticMeshInitSystem.h"
#include "render/systems/StaticMeshRenderSystem.h"

#include <nvrhi/utils.h> // for ClearColorAttachment

void D3E::GameRender::Init()
{
	Debug::LogMessage("[GameRender] Init started");
	if (!device_.Get())
	{
		Debug::LogError("[GameRender] GAPI not initialized");
		Debug::Assert(false, "GAPI not initialized");
	}

	commandList_ = device_->createCommandList();

	auto depthDesc = nvrhi::TextureDesc()
	                     .setDimension(nvrhi::TextureDimension::Texture2D)
	                     .setWidth(display_->ClientWidth)
	                     .setHeight(display_->ClientHeight)
	                     .setFormat(nvrhi::Format::D24S8)
	                     .setInitialState(nvrhi::ResourceStates::DepthWrite)
	                     .setKeepInitialState(true)
	                     .setIsRenderTarget(true)
	                     .setDebugName("Depth Texture");

	nvrhiDepthBuffer = device_->createTexture(depthDesc);

	nvrhi::FramebufferDesc framebufferDesc0 = {};
	framebufferDesc0.addColorAttachment(nvrhiSwapChain[0]);
	framebufferDesc0.setDepthAttachment(nvrhiDepthBuffer);
	nvrhiFramebuffer.push_back(device_->createFramebuffer(framebufferDesc0));

	nvrhi::FramebufferDesc framebufferDesc1 = {};
	framebufferDesc1.addColorAttachment(nvrhiSwapChain[1]);
	framebufferDesc1.setDepthAttachment(nvrhiDepthBuffer);
	nvrhiFramebuffer.push_back(device_->createFramebuffer(framebufferDesc1));

#ifdef USE_IMGUI
	editor_ = D3E::Editor::Init(device_, display_);
#endif

	ShaderFactory::Initialize(dynamic_cast<Game*>(parentApp));
	MeshFactory::Initialize(dynamic_cast<Game*>(parentApp));

	// TEMPORARY SECTION, to be moved

	ShaderFactory::AddVertexShader("SimpleForward", "SimpleForward.hlsl", "VSMain");

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
	ShaderFactory::AddGraphicsPipeline("SimpleForward", pipelineDesc, nvrhiFramebuffer[0]);

	MeshData sm = {};
	GeometryGenerator::CreateBox(sm, 1.0f, 1.0f, 1.0f, 0);

	MeshFactory::AddMeshFromData("Cube", sm);

	auto samplerDesc = nvrhi::SamplerDesc();
	TextureFactory::AddSampler("Base", device_, samplerDesc);

	MeshFactory::FillMeshBuffers("Cube", device_, commandList_);

	TextureFactory::LoadTexture("wood", "wood.png", device_, commandList_);

	// END OF TEMPORARY SECTION

	initRenderSystems.push_back(new StaticMeshInitSystem);

	perTickRenderSystems.push_back(new StaticMeshRenderSystem);

	Debug::LogMessage("[GameRender] Init finished");
}

void D3E::GameRender::DestroyResources()
{
//	editor_->Release();
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
	display_ = eastl::make_shared<DisplayWin32>(reinterpret_cast<LPCWSTR>(parentApp->GetName().c_str()), hInstance, 1920, 1080, parent);
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

void D3E::GameRender::Draw(entt::registry& registry)
{
	// Obtain the current framebuffer from the graphics API
	nvrhi::IFramebuffer* currentFramebuffer = nvrhiFramebuffer[GetCurrentFrameBuffer()];

	commandList_->open();

	// Clear the primary render target
	nvrhi::utils::ClearColorAttachment(commandList_, currentFramebuffer, 0, nvrhi::Color(0.2f));
	commandList_->clearDepthStencilTexture(nvrhiDepthBuffer, nvrhi::AllSubresources, true, 1.0f, false, 0U);

	for (auto& sys : perTickRenderSystems)
	{
		sys->Render(registry, currentFramebuffer, commandList_);
	}

	// Close and execute the command list
	commandList_->close();
	device_->executeCommandList(commandList_);
}

void D3E::GameRender::PrepareDraw(entt::registry& registry)
{
	for (auto& sys : initRenderSystems)
	{
		sys->Run(registry, device_, commandList_);
	}
}

void D3E::GameRender::EndDraw(entt::registry& registry)
{
#ifdef USE_IMGUI
	editor_->EndDraw(nvrhiFramebuffer[GetCurrentFrameBuffer()]);
#endif // USE_IMGUI
}

void D3E::GameRender::UpdateAnimations(float dT)
{
#ifdef USE_IMGUI
	editor_->BeginDraw(dT);
#endif // USE_IMGUI
}
