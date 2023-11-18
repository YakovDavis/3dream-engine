#pragma once

#include "D3E/CommonHeader.h"

#include "D3E/systems/GameSystem.h"
#include "Display.h"
#include "EASTL/shared_ptr.h"
#include "EASTL/vector.h"
#include "NvrhiMessageCallback.h"
#include "nvrhi/nvrhi.h"

#include <Windows.h>

#define USE_IMGUI

#ifdef USE_IMGUI
#include "imgui.h"
#include "imgui_backend/imgui_nvrhi.h"
#endif // USE_IMGUI

namespace D3E
{
	class App;

	class DisplayWin32;

	// Internal class for managing render devices, swap chains etc.
	class GameRender
	{
	public:
		virtual void Init(eastl::vector<GameSystem*>& systems);
		virtual void OnResize();

#ifdef USE_IMGUI
		void InitImGui();
		void RenderImGui();
#endif // USE_IMGUI

		Display* GetDisplay();
		nvrhi::DeviceHandle& GetDevice();
		nvrhi::CommandListHandle& GetCommandList();

		void CalculateFrameStats();

		virtual void UpdateAnimations(float dT);

		virtual void PrepareDraw(entt::registry& registry, eastl::vector<GameSystem*>& systems);
		virtual void Draw(entt::registry& registry, eastl::vector<GameSystem*>& systems);
		virtual void EndDraw(entt::registry& registry, eastl::vector<GameSystem*>& systems);

		virtual void Present() = 0;
		virtual UINT GetCurrentFrameBuffer() = 0;

		void LoadTexture(const String& name, const String& fileName);

		void DestroyResources();

		explicit GameRender(App* parent, HINSTANCE hInstance);
		virtual ~GameRender() = default;

	protected:
		App* parentApp;

		nvrhi::DeviceHandle device_;

		nvrhi::CommandListHandle commandList_;

		eastl::shared_ptr<Display> display_;

		eastl::vector<nvrhi::TextureHandle> nvrhiSwapChain;

		nvrhi::TextureHandle nvrhiDepthBuffer;

		eastl::vector<nvrhi::FramebufferHandle> nvrhiFramebuffer;

		NvrhiMessageCallback* messageCallback_;

		nvrhi::InputLayoutHandle inputLayout_;

#ifdef USE_IMGUI
		ImGui_NVRHI imGuiNvrhi_;
#endif // USE_IMGUI

		friend class Game;
	};
}

