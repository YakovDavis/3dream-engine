#pragma once

#include "D3E/systems/PerRenderTickSystem.h"
#include "D3E/systems/RenderSystem.h"
#include "Display.h"
#include "EASTL/shared_ptr.h"
#include "EASTL/vector.h"
#include "NvrhiMessageCallback.h"
#include "nvrhi/nvrhi.h"

#include <Windows.h>

#define USE_IMGUI

#ifdef USE_IMGUI
#include "editor/Editor.h"
#endif // USE_IMGUI

namespace D3E
{
	class App;

	class DisplayWin32;

	// Internal class for managing render devices, swap chains etc.
	class GameRender
	{
	public:
		virtual void Init();
		virtual void OnResize();

		Display* GetDisplay();
		nvrhi::DeviceHandle& GetDevice();
		nvrhi::CommandListHandle& GetCommandList();

		void CalculateFrameStats();

		virtual void UpdateAnimations(float dT);

		virtual void PrepareDraw(entt::registry& registry);
		virtual void Draw(entt::registry& registry);
		virtual void EndDraw(entt::registry& registry);

		virtual void Present() = 0;
		virtual UINT GetCurrentFrameBuffer() = 0;

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

		eastl::vector<PerRenderTickSystem*> perTickRenderSystems;

		eastl::vector<RenderSystem*> initRenderSystems;

		NvrhiMessageCallback* messageCallback_;

		nvrhi::InputLayoutHandle inputLayout_;

		D3E::Editor* editor_;

		friend class Game;
	};
}

