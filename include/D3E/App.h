#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX

#include "CommonHeader.h"

#include <Windows.h>

namespace D3E
{
	class D3EAPI App
	{
	public:
		virtual void Run() {}

		App() = default;
		void SetAppInstance(HINSTANCE hInstance) { mhAppInst = hInstance; }
		virtual ~App() = default;

		virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		static LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);

		[[nodiscard]] String GetName() const { return appName; }

	protected:
		HINSTANCE mhAppInst;

		String appName = "3dream App";
	};

	App* CreateApp();
}


