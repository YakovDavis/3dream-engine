#pragma once

#include "nvrhi/nvrhi.h"
#include <windows.h>

namespace D3E
{
	// Internal class for managing render devices, swap chains etc.
	class GameRender
	{
	public:
		virtual void Init();
		virtual void OnResize();

		void CalculateFrameStats();

		void DestroyResources();

		[[nodiscard]] HINSTANCE AppInst() const;
		[[nodiscard]] HWND MainWnd() const;
		[[nodiscard]] float AspectRatio() const;

		explicit GameRender(HINSTANCE hInstance);
		virtual ~GameRender() = default;

	protected:
		nvrhi::DeviceHandle device_;

		std::wstring mMainWndCaption = L"d3d App";

		HINSTANCE mhAppInst = nullptr;
		HWND mhMainWnd = nullptr;

		int mClientWidth = 800;
		int mClientHeight = 600;

		friend class Game;
	};
}

