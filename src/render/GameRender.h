#pragma once

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

		void DestroyResources();

		explicit GameRender(App* parent, HINSTANCE hInstance);
		virtual ~GameRender() = default;

	protected:
		App* parentApp;

		nvrhi::DeviceHandle device_;

		nvrhi::CommandListHandle commandList_;

		eastl::shared_ptr<Display> display_;

		eastl::vector<nvrhi::TextureHandle> nvrhiSwapChainBuffer;

		NvrhiMessageCallback* messageCallback_;

		nvrhi::InputLayoutHandle inputLayout_;

		friend class Game;
	};
}

