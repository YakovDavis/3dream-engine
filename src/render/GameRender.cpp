#include "GameRender.h"

#include "CameraUtils.h"
#include "D3E/CommonCpp.h"
#include "D3E/CommonHeader.h"
#include "D3E/Debug.h"
#include "D3E/Game.h"
#include "D3E/components/render/StaticMeshComponent.h"
#include "D3E/engine/ConsoleManager.h"
#include "DebugRenderer.h"
#include "DisplayWin32.h"
#include "PbrUtils.h"
#include "PerObjectConstBuffer.h"
#include "PickConstBuffer.h"
#include "ShaderFactory.h"
#include "Vertex.h"
#include "assetmng/DefaultAssetLoader.h"
#include "assetmng/MeshFactory.h"
#include "assetmng/TextureFactory.h"
#include "render/GeometryGenerator.h"
#include "render/systems/StaticMeshInitSystem.h"
#include "render/systems/StaticMeshRenderSystem.h"

#include <nvrhi/utils.h> // for ClearColorAttachment

void D3E::GameRender::Init(eastl::vector<GameSystem*>& systems)
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

	gbuffer_.Initialize(device_, commandList_, display_.get());
	TextureFactory::RegisterGBuffer(&gbuffer_);

	nvrhi::FramebufferDesc frameGBufferDesc = {};
	frameGBufferDesc.addColorAttachment(gbuffer_.albedoBuffer);
	frameGBufferDesc.addColorAttachment(gbuffer_.positionBuffer);
	frameGBufferDesc.addColorAttachment(gbuffer_.normalBuffer);
	frameGBufferDesc.addColorAttachment(gbuffer_.metalRoughnessBuffer);
	frameGBufferDesc.addColorAttachment(gbuffer_.editorIdsBuffer);
	frameGBufferDesc.setDepthAttachment(nvrhiDepthBuffer);
	frameGBuffer = device_->createFramebuffer(frameGBufferDesc);

#ifdef D3E_WITH_EDITOR
	nvrhi::TextureDesc gameFrameTextureDesc = {};
	gameFrameTextureDesc.format = nvrhi::Format::RGBA8_UNORM;
	gameFrameTextureDesc.setWidth(display_->ClientWidth);
	gameFrameTextureDesc.setHeight(display_->ClientHeight);
	gameFrameTextureDesc.isRenderTarget = true;
	gameFrameTextureDesc.sampleCount = 1;
	gameFrameTextureDesc.sampleQuality = 0;
	gameFrameTextureDesc.dimension = nvrhi::TextureDimension::Texture2D;
	gameFrameTextureDesc.initialState = nvrhi::ResourceStates::RenderTarget;
	gameFrameTextureDesc.setKeepInitialState(true);
	gameFrameTextureDesc.setDebugName("Game Frame Texture");
	gameFrameTexture_ = device_->createTexture(gameFrameTextureDesc);

	nvrhi::FramebufferDesc gameFrameBufferDesc = {};
	gameFrameBufferDesc.addColorAttachment(gameFrameTexture_);
	gameFrameBufferDesc.setDepthAttachment(nvrhiDepthBuffer);
	gameFramebuffer_ = device_->createFramebuffer(gameFrameBufferDesc);
#endif

#ifdef USE_IMGUI
	editor_ = D3E::Editor::Init(device_, display_, parentGame);
#endif

	ShaderFactory::Initialize(parentGame);
	MeshFactory::Initialize(parentGame);

	DefaultAssetLoader::LoadPrimitiveMeshes();
	DefaultAssetLoader::FillPrimitiveMeshBuffers(device_, commandList_);
	DefaultAssetLoader::LoadDefaultPSOs(nvrhiFramebuffer[0], frameGBuffer);
	DefaultAssetLoader::LoadDefaultSamplers(device_);
	DefaultAssetLoader::LoadDefaultMaterials();

	debugRenderer_ = new DebugRenderer(device_, dynamic_cast<Game*>(parentGame));

	for (auto& sys : systems)
	{
		sys->InitRender(commandList_, device_);
	}

	ConsoleManager::getInstance()->registerConsoleVariable("renderingMode", 0);
	ConsoleManager::getInstance()->registerConsoleVariable("displayGrid", 1);
	ConsoleManager::getInstance()->registerConsoleVariable("visualizeBounds", 0);

	auto pickingCBDesc = nvrhi::BufferDesc()
	                         .setByteSize(sizeof(PickConstBuffer))
	                         .setIsConstantBuffer(true)
	                         .setIsVolatile(false)
	                         .setMaxVersions(16)
	                         .setKeepInitialState(true);

	pickCb = device_->createBuffer(pickingCBDesc);

	auto pickedIdDesc = nvrhi::BufferDesc()
	                         .setByteSize(sizeof(uint32_t))
	                         .setFormat(nvrhi::Format::R32_UINT)
	                         .setInitialState(nvrhi::ResourceStates::CopySource)
	                         .setCanHaveUAVs(true)
	                         .setStructStride(sizeof(uint32_t))
	                         .setKeepInitialState(true);

	pickedIdBuffer_ = device_->createBuffer(pickedIdDesc);

	auto pickedIdCpuDesc = nvrhi::BufferDesc()
	                           .setByteSize(sizeof(uint32_t))
	                           .setFormat(nvrhi::Format::R32_UINT)
	                           .setInitialState(nvrhi::ResourceStates::CopySource)
	                           .setCanHaveUAVs(false)
	                           .setCpuAccess(nvrhi::CpuAccessMode::Read)
	                           .setStructStride(sizeof(uint32_t))
	                           .setKeepInitialState(true);

	pickedIdBufferCpu_ = device_->createBuffer(pickedIdCpuDesc);

	nvrhi::BindingSetDesc pickingBSC = {};
	pickingBSC.addItem(nvrhi::BindingSetItem::ConstantBuffer(0, pickCb));
	pickingBSC.addItem(nvrhi::BindingSetItem::Texture_SRV(0, gbuffer_.editorIdsBuffer));
	pickingBSC.addItem(nvrhi::BindingSetItem::StructuredBuffer_UAV(0, pickedIdBuffer_));
	ShaderFactory::AddBindingSetC("Pick", pickingBSC, "PickC");

	nvrhi::BindingSetDesc nullBindingSetDesc = {};
	ShaderFactory::AddBindingSetV("EditorHighlightPass", nullBindingSetDesc, "EditorHighlightPassV");
	ShaderFactory::AddBindingSetP("EditorHighlightPass", nullBindingSetDesc, "EditorHighlightPassP");

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

D3E::GameRender::GameRender(Game* parent, HINSTANCE hInstance) : parentGame(parent)
{
	assert(parentGame != nullptr);
	assert(hInstance != nullptr);
	display_ = eastl::make_shared<DisplayWin32>(reinterpret_cast<LPCWSTR>(parentGame->GetName().c_str()), hInstance, 1280, 720, parent);
	messageCallback_ = new NvrhiMessageCallback();
}

D3E::Display* D3E::GameRender::GetDisplay()
{
	return display_ ? display_.get() : nullptr;
}

nvrhi::DeviceHandle& D3E::GameRender::GetDevice()
{
	return device_;
}

nvrhi::CommandListHandle& D3E::GameRender::GetCommandList()
{
	return commandList_;
}

void D3E::GameRender::DrawOpaque(entt::registry& registry, eastl::vector<GameSystem*>& systems)
{
	nvrhi::IFramebuffer* currentFramebuffer = nvrhiFramebuffer[GetCurrentFrameBuffer()];

	commandList_->open();
	for (auto& sys : systems)
	{
		sys->Draw(registry, frameGBuffer, commandList_, device_);
	}
	commandList_->close();
	device_->executeCommandList(commandList_);
}

void D3E::GameRender::PrepareFrame()
{
	nvrhi::IFramebuffer* currentFramebuffer = nvrhiFramebuffer[GetCurrentFrameBuffer()];
	commandList_->open();
	nvrhi::utils::ClearColorAttachment(commandList_, currentFramebuffer, 0, nvrhi::Color(0.2f));
	commandList_->clearTextureFloat(gbuffer_.albedoBuffer, nvrhi::AllSubresources, nvrhi::Color(0.0f));
	commandList_->clearTextureFloat(gbuffer_.positionBuffer, nvrhi::AllSubresources, nvrhi::Color(0.0f));
	commandList_->clearTextureFloat(gbuffer_.normalBuffer, nvrhi::AllSubresources, nvrhi::Color(0.0f));
	commandList_->clearTextureFloat(gbuffer_.metalRoughnessBuffer, nvrhi::AllSubresources, nvrhi::Color(0.0f));
	commandList_->clearTextureUInt(gbuffer_.editorIdsBuffer, nvrhi::AllSubresources, 0);
	commandList_->clearDepthStencilTexture(nvrhiDepthBuffer, nvrhi::AllSubresources, true, 1.0f, true, 0U);
#ifdef D3E_WITH_EDITOR
	nvrhi::utils::ClearColorAttachment(commandList_, gameFramebuffer_, 0, nvrhi::Color(0.2f));
#endif
	commandList_->close();
	device_->executeCommandList(commandList_);
}

void D3E::GameRender::DrawPostProcess(entt::registry& registry,
                                      eastl::vector<GameSystem*>& systems)
{
	commandList_->open();
#ifdef D3E_WITH_EDITOR
	for (auto& sys : systems)
	{
		sys->Draw(registry, gameFramebuffer_, commandList_, device_);
	}
	debugRenderer_->Begin(commandList_, gameFramebuffer_);
#else
	nvrhi::IFramebuffer* currentFramebuffer = nvrhiFramebuffer[GetCurrentFrameBuffer()];
	for (auto& sys : systems)
	{
		sys->Draw(registry, currentFramebuffer, commandList_, device_);
	}
	debugRenderer_->Begin(commandList_, currentFramebuffer);
#endif
	debugRenderer_->ProcessQueue();
	debugRenderer_->End();
	commandList_->close();
	device_->executeCommandList(commandList_);
}

void D3E::GameRender::BeginDraw(entt::registry& registry, eastl::vector<GameSystem*>& systems)
{
	for (auto& sys : systems)
	{
		sys->PreDraw(registry, commandList_, device_);
	}
}

void D3E::GameRender::EndDraw(entt::registry& registry, eastl::vector<GameSystem*>& systems)
{
	for (auto& sys : systems)
	{
		sys->PostDraw(registry, commandList_, device_);
	}
}

//void D3E::GameRender::LoadTexture(const String& name,
//                                  const String& fileName)
//{
//	TextureFactory::LoadTexture(name, fileName, device_, commandList_);
//}

void D3E::GameRender::UpdateAnimations(float dT)
{
#ifdef USE_IMGUI
	editor_->BeginDraw(dT);
#endif // USE_IMGUI
}

uint32_t D3E::GameRender::EditorPick(int x, int y)
{
	commandList_->open();

	PickConstBuffer constants = {};
	constants.mouseX = x;
	constants.mouseY = y;
	commandList_->writeBuffer(pickCb, &constants, sizeof(constants));

	nvrhi::ComputeState state;
	state.pipeline = ShaderFactory::GetComputePipeline("Pick");
	state.bindings = { ShaderFactory::GetBindingSetC("Pick") };
	commandList_->setComputeState(state);
	commandList_->dispatch(1, 1, 1);

	commandList_->copyBuffer(pickedIdBufferCpu_, 0, pickedIdBuffer_, 0, pickedIdBufferCpu_->getDesc().byteSize);

	commandList_->close();
	device_->executeCommandList(commandList_);

	void* pData = device_->mapBuffer(pickedIdBufferCpu_, nvrhi::CpuAccessMode::Read);
	assert(pData);

	uint32_t value = *static_cast<uint32_t*>(pData);

	device_->unmapBuffer(pickedIdBufferCpu_);
	return value;
}

D3E::DebugRenderer* D3E::GameRender::GetDebugRenderer()
{
	return debugRenderer_;
}

void D3E::GameRender::DrawGUI()
{
#ifdef USE_IMGUI
	editor_->EndDraw(nvrhiFramebuffer[GetCurrentFrameBuffer()], gameFramebuffer_);
#endif // USE_IMGUI
}

void D3E::GameRender::PostAssetLoadInit()
{
	PbrUtils::Setup(device_, commandList_);
}
