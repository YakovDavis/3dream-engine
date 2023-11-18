#include "GameRender.h"

#include "D3E/CommonCpp.h"
#include "CameraUtils.h"
#include "D3E/CommonHeader.h"
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
#include "assetmng/DefaultAssetLoader.h"
#include <nvrhi/utils.h> // for ClearColorAttachment

#ifdef USE_IMGUI
#include "imgui_impl_win32.h"
#endif // USE_IMGUI

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

#ifdef USE_IMGUI
	InitImGui();
#endif // USE_IMGUI

	ShaderFactory::Initialize(dynamic_cast<Game*>(parentApp));
	MeshFactory::Initialize(dynamic_cast<Game*>(parentApp));

	DefaultAssetLoader::LoadPrimitiveMeshes();
	DefaultAssetLoader::FillPrimitiveMeshBuffers(device_, commandList_);
	DefaultAssetLoader::LoadDefaultPSOs(nvrhiFramebuffer[0]);
	DefaultAssetLoader::LoadDefaultSamplers(device_);

	for (auto& sys : systems)
	{
		sys->InitRender();
	}

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

void D3E::GameRender::Draw(entt::registry& registry, eastl::vector<GameSystem*>& systems)
{
	// Obtain the current framebuffer from the graphics API
	nvrhi::IFramebuffer* currentFramebuffer = nvrhiFramebuffer[GetCurrentFrameBuffer()];

	commandList_->open();

	// Clear the primary render target
	nvrhi::utils::ClearColorAttachment(commandList_, currentFramebuffer, 0, nvrhi::Color(0.2f));
	commandList_->clearDepthStencilTexture(nvrhiDepthBuffer, nvrhi::AllSubresources, true, 1.0f, false, 0U);

	for (auto& sys : systems)
	{
		sys->Draw(registry, currentFramebuffer, commandList_, device_);
	}

	// Close and execute the command list
	commandList_->close();
	device_->executeCommandList(commandList_);
}

void D3E::GameRender::PrepareDraw(entt::registry& registry, eastl::vector<GameSystem*>& systems)
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
#ifdef USE_IMGUI
	RenderImGui();
#endif // USE_IMGUI
}

void D3E::GameRender::LoadTexture(const String& name,
                                  const String& fileName)
{
	TextureFactory::LoadTexture(name, fileName, device_, commandList_);
}

void D3E::GameRender::UpdateAnimations(float dT)
{
#ifdef USE_IMGUI
	int w, h;
	float scaleX, scaleY;

	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(float(display_->ClientWidth), float(display_->ClientHeight));
	//io.DisplayFramebufferScale.x = scaleX;
	//io.DisplayFramebufferScale.y = scaleY;

	imGuiNvrhi_.beginFrame(dT);
#endif // USE_IMGUI
}

#ifdef USE_IMGUI
void D3E::GameRender::InitImGui()
{
	auto displayWin32 = dynamic_cast<DisplayWin32*>(display_.get());

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(displayWin32->hWnd);
	imGuiNvrhi_.init(device_);
}

void D3E::GameRender::RenderImGui()
{
	ImGui_ImplWin32_NewFrame();

	ImGui::ShowDemoWindow();
	ImGui::Render();

	// Obtain the current framebuffer from the graphics API
	nvrhi::IFramebuffer* currentFramebuffer = nvrhiFramebuffer[GetCurrentFrameBuffer()];

	imGuiNvrhi_.render(currentFramebuffer);
}
#endif // USE_IMGUI
