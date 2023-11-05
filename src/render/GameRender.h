#pragma once

#include "EASTL/shared_ptr.h"
#include "Display.h"
#include "nvrhi/nvrhi.h"

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

		void CalculateFrameStats();

		void DestroyResources();

		explicit GameRender(App* parent, HINSTANCE hInstance);
		virtual ~GameRender() = default;

	protected:
		App* parentApp;

		nvrhi::DeviceHandle device_;

		eastl::shared_ptr<Display> display_;

		friend class Game;
	};
}

