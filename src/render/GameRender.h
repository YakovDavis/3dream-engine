#pragma once

#include "D3E/systems/PerRenderTickSystem.h"

#include "EASTL/shared_ptr.h"
#include "Display.h"
#include "nvrhi/nvrhi.h"
#include "EASTL/vector.h"

#include "NvrhiMessageCallback.h"

#include <Windows.h>

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

		virtual void PrepareDraw() {}
		virtual void Draw();
		virtual void EndDraw() {}

		virtual void Present() = 0;
		virtual UINT GetCurrentFrameBuffer() = 0;

		void DestroyResources();

		explicit GameRender(App* parent, HINSTANCE hInstance);
		virtual ~GameRender() = default;

		eastl::vector<PerRenderTickSystem> RenderSystems;

	protected:
		App* parentApp;

		nvrhi::DeviceHandle device_;

		nvrhi::CommandListHandle commandList_;

		eastl::shared_ptr<Display> display_;

		eastl::vector<nvrhi::TextureHandle> nvrhiSwapChain;

		eastl::vector<nvrhi::FramebufferHandle> nvrhiFramebuffer;

		nvrhi::BufferHandle constantBuffer;
		nvrhi::BufferHandle vertexBuffer;
		nvrhi::BufferHandle indexBuffer;
		nvrhi::TextureHandle testTexture;
		nvrhi::SamplerHandle testSampler;

		NvrhiMessageCallback* messageCallback_;

		nvrhi::InputLayoutHandle inputLayout_;

		friend class Game;
	};
}

