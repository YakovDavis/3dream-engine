#pragma once

#include <Windows.h>

namespace D3E
{
	class App;

	class DisplayWin32
	{
	private:
		D3E::App* app;

	public:
		int ClientHeight;
		int ClientWidth;
		HINSTANCE hInstance;
		HWND hWnd;
		WNDCLASSEX wc;
		DisplayWin32(LPCWSTR applicationName, HINSTANCE hInst, int screenWidth,
		             int screenHeight, D3E::App* a);
	};
}
