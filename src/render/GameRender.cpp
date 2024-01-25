#include "GameRender.h"

#include "CameraUtils.h"
#include "Csm.h"
#include "D3E/CommonCpp.h"
#include "D3E/CommonHeader.h"
#include "D3E/Components/TransformComponent.h"
#include "D3E/Components/render/SkyboxComponent.h"
#include "D3E/Debug.h"
#include "D3E/Game.h"
#include "D3E/components/render/StaticMeshComponent.h"
#include "D3E/engine/ConsoleManager.h"
#include "DebugRenderer.h"
#include "DisplayWin32.h"
#include "PbrUtils.h"
#include "PerObjectConstBuffer.h"
#include "PickConstBuffer.h"
#include "RenderUtils.h"
#include "ShaderFactory.h"
#include "SkyboxCB.h"
#include "Vertex.h"
#include "assetmng/DefaultAssetLoader.h"
#include "assetmng/MeshFactory.h"
#include "assetmng/TextureFactory.h"
#include "core/EngineState.h"
#include "game_ui/GameUi.h"
#include "render/GeometryGenerator.h"
#include "render/systems/StaticMeshInitSystem.h"
#include "render/systems/StaticMeshRenderSystem.h"

#include <nvrhi/utils.h> // for ClearColorAttachment

using namespace DirectX::SimpleMath;

void D3E::GameRender::Init(eastl::vector<GameSystem*>& systems)
{
	Debug::LogMessage("[GameRender] Init started");
	if (!device_.Get())
	{
		Debug::LogError("[GameRender] GAPI not initialized");
		Debug::Assert(false, "GAPI not initialized");
	}

	commandList_ = device_->createCommandList();

	/*auto depthDesc = nvrhi::TextureDesc()
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
	nvrhiFramebuffer.push_back(device_->createFramebuffer(framebufferDesc1));*/

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

	gameFrameTextureDesc.setDebugName("LightPass Texture");
	lightpassTexture_ = device_->createTexture(gameFrameTextureDesc);

	nvrhi::FramebufferDesc gameFrameBufferDesc = {};
	gameFrameBufferDesc.addColorAttachment(gameFrameTexture_);
	gameFrameBufferDesc.setDepthAttachment(nvrhiDepthBuffer);
	gameFramebuffer_ = device_->createFramebuffer(gameFrameBufferDesc);

	nvrhi::FramebufferDesc lightpassBufferDesc = {};
	lightpassBufferDesc.addColorAttachment(lightpassTexture_);
	lightpassBufferDesc.setDepthAttachment(nvrhiDepthBuffer);
	lightpassFramebuffer_ = device_->createFramebuffer(lightpassBufferDesc);
#endif

	gameUi_ = D3E::GameUi::Init(parentGame, gameFramebuffer_);

#ifdef USE_IMGUI
	editor_ = D3E::Editor::Init(device_, display_, parentGame);
#endif

	ShaderFactory::Initialize(parentGame);
	MeshFactory::Initialize(parentGame);

	DefaultAssetLoader::LoadPrimitiveMeshes();
	DefaultAssetLoader::FillPrimitiveMeshBuffers(device_, commandList_);
	DefaultAssetLoader::LoadDefaultPSOs(gameFramebuffer_, frameGBuffer);
	DefaultAssetLoader::LoadDefaultSamplers(device_);
	DefaultAssetLoader::LoadDefaultMaterials();

	debugRenderer_ = new DebugRenderer(device_, dynamic_cast<Game*>(parentGame));

	RenderUtils::Initialize(this);

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

	nvrhi::BindingSetDesc tonemapBSP = {};
	tonemapBSP.addItem(nvrhi::BindingSetItem::PushConstants(0, 2 * sizeof(float)));
	tonemapBSP.addItem(nvrhi::BindingSetItem::Texture_SRV(0, lightpassTexture_));
	tonemapBSP.addItem(nvrhi::BindingSetItem::Sampler(0, TextureFactory::GetSampler("BaseCompute")));
	ShaderFactory::AddBindingSetP("Tonemap", tonemapBSP, "TonemapP");

	nvrhi::BindingSetDesc nullBindingSetDesc = {};
	ShaderFactory::AddBindingSetV("EditorHighlightPass", nullBindingSetDesc, "EditorHighlightPassV");
	ShaderFactory::AddBindingSetP("EditorHighlightPass", nullBindingSetDesc, "EditorHighlightPassP");
	ShaderFactory::AddBindingSetV("Tonemap", nullBindingSetDesc, "TonemapV");

	shadowRenderer_ = D3E::Csm::Init(parentGame);

	Debug::LogMessage("[GameRender] Init finished");
}

void D3E::GameRender::DestroyResources()
{
//	editor_->Release();
	GameUi::DestroyResources();
}

void D3E::GameRender::OnResize()
{
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

	nvrhiFramebuffer.resize(SwapChainBufferCount);
	for (uint32_t index = 0; index < SwapChainBufferCount; index++)
	{
		nvrhiFramebuffer[index] = GetDevice()->createFramebuffer(
			nvrhi::FramebufferDesc().addColorAttachment(nvrhiSwapChain[index]).setDepthAttachment(nvrhiDepthBuffer));
	}

	EngineState::isViewportDirty = true;
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
	commandList_->open();
	commandList_->beginMarker("Opaque");
	commandList_->setTextureState(gameFrameTexture_, nvrhi::AllSubresources, nvrhi::ResourceStates::RenderTarget);
	for (auto& sys : systems)
	{
		sys->Draw(registry, frameGBuffer, commandList_, device_);
	}
	commandList_->endMarker();
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
	nvrhi::utils::ClearColorAttachment(commandList_, lightpassFramebuffer_, 0, nvrhi::Color(0.2f));
#ifdef D3E_WITH_EDITOR
	nvrhi::utils::ClearColorAttachment(commandList_, gameFramebuffer_, 0, nvrhi::Color(0.0f));
#endif
	commandList_->close();
	device_->executeCommandList(commandList_);

	shadowRenderer_->GenerateCascadeMaps();
}

void D3E::GameRender::DrawPostProcessSystems(entt::registry& registry,
                                      eastl::vector<GameSystem*>& systems, bool afterTonemapper)
{
	nvrhi::IFramebuffer* fb = afterTonemapper ? gameFramebuffer_ : lightpassFramebuffer_;
	commandList_->open();
	for (auto& sys : systems)
	{
		sys->Draw(registry, fb, commandList_, device_);
	}
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

void D3E::GameRender::UpdateAnimations(float dT)
{
	gameUi_->Update();
#ifdef USE_IMGUI
	editor_->BeginDraw(dT);
#endif // USE_IMGUI
}

uint32_t D3E::GameRender::EditorPick(int x, int y)
{
	commandList_->open();

	commandList_->beginMarker("EditorPick");

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

	commandList_->endMarker();

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
	gameUi_->Draw();
	commandList_->open();
	commandList_->setTextureState(gameFrameTexture_, nvrhi::AllSubresources, nvrhi::ResourceStates::ShaderResource);
	commandList_->close();
	device_->executeCommandList(commandList_);
#ifdef USE_IMGUI
	editor_->EndDraw(nvrhiFramebuffer[GetCurrentFrameBuffer()], gameFramebuffer_);
#endif // USE_IMGUI
}

void D3E::GameRender::PostAssetLoadInit()
{
	PbrUtils::Setup(device_, commandList_);
	gameUi_->PostInputInit();
}

void D3E::GameRender::DrawSkybox(entt::registry& registry, nvrhi::IFramebuffer* fb)
{
	if (EngineState::currentPlayer == entt::null)
	{
		return;
	}

	auto skyboxView = registry.view<const ObjectInfoComponent, const TransformComponent, SkyboxComponent>();
	if (skyboxView.begin() == skyboxView.end())
	{
		return;
	}

	const TransformComponent* playerTransform = registry.try_get<TransformComponent>(EngineState::currentPlayer);
	if (!playerTransform)
	{
		return;
	}
	const CameraComponent* camera = registry.try_get<CameraComponent>(EngineState::currentPlayer);
	if (!camera)
	{
		return;
	}
	DirectX::SimpleMath::Vector3 origin = playerTransform->position + camera->offset;

	entt::entity sb = skyboxView.front();

	const ObjectInfoComponent& info = registry.get<ObjectInfoComponent>(sb);
	const TransformComponent& tc = registry.get<TransformComponent>(sb);
	SkyboxComponent& sc = registry.get<SkyboxComponent>(sb);

	if (!sc.constantBuffer)
	{
		auto constantBufferDesc =
			nvrhi::BufferDesc()
				.setByteSize(sizeof(SkyboxCB))
				.setIsConstantBuffer(true)
				.setIsVolatile(false)
				.setMaxVersions(16)
				.setKeepInitialState(true);
		sc.constantBuffer = device_->createBuffer(constantBufferDesc);

		nvrhi::BindingSetDesc bsv = {};
		bsv.addItem(nvrhi::BindingSetItem::ConstantBuffer(0, sc.constantBuffer));
		ShaderFactory::RemoveBindingSetV(info.id);
		ShaderFactory::AddBindingSetV(info.id, bsv, "SkyboxV");
		sc.bindingSets.push_back(ShaderFactory::GetBindingSetV(info.id));

		nvrhi::BindingSetDesc bsp = {};
		bsp.addItem(nvrhi::BindingSetItem::Texture_SRV(0, TextureFactory::GetTextureHandle(kEnvTextureUUID)));
		bsp.addItem(nvrhi::BindingSetItem::Sampler(0, TextureFactory::GetSampler("BaseCompute")));
		ShaderFactory::RemoveBindingSetP(info.id);
		ShaderFactory::AddBindingSetP(info.id, bsp, "SkyboxP");
		sc.bindingSets.push_back(ShaderFactory::GetBindingSetP(info.id));
	}

	commandList_->open();
	commandList_->beginMarker("Skybox");

	SkyboxCB constBufferData = {};

	DirectX::SimpleMath::Matrix viewRot = DirectX::XMMatrixLookAtLH(DirectX::SimpleMath::Vector3::Zero, camera->forward, camera->up);
	constBufferData.skyProjectionMatrix = viewRot * CameraUtils::GetProj(*camera);

	commandList_->writeBuffer(sc.constantBuffer, &constBufferData, sizeof(constBufferData));

	auto graphicsState = nvrhi::GraphicsState()
	                         .setPipeline(ShaderFactory::GetGraphicsPipeline("Skybox"))
	                         .setFramebuffer(fb)
	                         .setViewport(nvrhi::ViewportState().addViewportAndScissorRect(nvrhi::Viewport(EngineState::GetViewportWidth(), EngineState::GetViewportHeight())))
	                         .addVertexBuffer(MeshFactory::GetVertexBufferBinding(kSkyboxMeshUUID))
							 .setIndexBuffer(MeshFactory::GetIndexBufferBinding(kSkyboxMeshUUID));
	graphicsState.bindings = {sc.bindingSets[0], sc.bindingSets[1]};
	commandList_->setGraphicsState(graphicsState);

	auto drawArguments = nvrhi::DrawArguments()
	                         .setVertexCount(MeshFactory::GetSkyMeshData(kSkyboxMeshUUID).indices.size());
	commandList_->drawIndexed(drawArguments);

	commandList_->endMarker();
	commandList_->close();
	device_->executeCommandList(commandList_);
}

nvrhi::IFramebuffer* D3E::GameRender::GetGameFramebuffer()
{
#ifdef D3E_WITH_EDITOR
	return gameFramebuffer_;
#else
	return nvrhiFramebuffer[GetCurrentFrameBuffer()];
#endif
}

nvrhi::IBuffer* D3E::GameRender::GetCsmConstantBuffer()
{
	return shadowRenderer_->GetCsmConstantBuffer();
}

nvrhi::ITexture* D3E::GameRender::GetCsmTexture()
{
	return shadowRenderer_->GetCsmTexture();
}

nvrhi::ISampler* D3E::GameRender::GetCsmSampler()
{
	return shadowRenderer_->GetCsmSampler();
}

void D3E::GameRender::DrawTonemapper(nvrhi::IFramebuffer* fb)
{
	nvrhi::GraphicsState graphicsState = {};
	graphicsState.setPipeline(ShaderFactory::GetGraphicsPipeline("Tonemap"));
	graphicsState.bindings = {ShaderFactory::GetBindingSetV("Tonemap"), ShaderFactory::GetBindingSetP("Tonemap")};
	graphicsState.framebuffer = fb;
	graphicsState.setViewport(nvrhi::ViewportState().addViewportAndScissorRect(nvrhi::Viewport(EngineState::GetViewportWidth(), EngineState::GetViewportHeight())));

	nvrhi::DrawArguments drawArguments = {3};

	commandList_->open();
	commandList_->beginMarker("Tonemapper");
	commandList_->setTextureState(lightpassTexture_, nvrhi::AllSubresources, nvrhi::ResourceStates::ShaderResource);
	commandList_->setGraphicsState(graphicsState);
	commandList_->setPushConstants(&tonemapperConstants_, 2 * sizeof(float));
	commandList_->draw(drawArguments);
	commandList_->endMarker();
	commandList_->close();
	device_->executeCommandList(commandList_);
}

void D3E::GameRender::DrawPostProcessEffects(entt::registry& registry)
{
	nvrhi::IFramebuffer* framebuffer = GetGameFramebuffer();

	DrawSkybox(registry, lightpassFramebuffer_);
	DrawTonemapper(framebuffer);
}

void D3E::GameRender::DrawDebug()
{
	commandList_->open();
	commandList_->beginMarker("DebugDraw");
	debugRenderer_->Begin(commandList_, gameFramebuffer_);
	debugRenderer_->ProcessQueue();
	debugRenderer_->End();
	commandList_->endMarker();
	commandList_->close();
	device_->executeCommandList(commandList_);
}
