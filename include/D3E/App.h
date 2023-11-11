#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX

#include <Windows.h>
#include "EASTL/string.h"

namespace D3E
{
	class App
	{
	public:
		virtual void Run() {}

		App() = default;
		void SetAppInstance(HINSTANCE hInstance) { mhAppInst = hInstance; }
		virtual ~App() = default;

		virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		static LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);

		[[nodiscard]] eastl::string GetName() const { return appName; }

	protected:
		HINSTANCE mhAppInst;

		eastl::string appName = "3dream App";
	};

	App* CreateApp();
}


