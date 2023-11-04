#pragma once

#include "EASTL/shared_ptr.h"
#include "DisplayWin32.h"
#include "nvrhi/nvrhi.h"

#include <Windows.h>

namespace D3E
{
	class App;

	// Internal class for managing render devices, swap chains etc.
	class GameRender
	{
	public:
		virtual void Init();
		virtual void OnResize();

		void CalculateFrameStats();

		void DestroyResources();

		explicit GameRender(App* parent, HINSTANCE hInstance);
		virtual ~GameRender() = default;

	protected:
		App* parentApp;

		nvrhi::DeviceHandle device_;

		eastl::shared_ptr<DisplayWin32> displayWin32_;

		friend class Game;
	};
}

