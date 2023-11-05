#pragma once

#include <Windows.h>
#include "Display.h"

namespace D3E
{
	class DisplayWin32 : public Display
	{
	public:
		HINSTANCE hInstance;
		HWND hWnd;
		WNDCLASSEX wc;
		DisplayWin32(LPCWSTR applicationName, HINSTANCE hInst, int screenWidth,
		             int screenHeight, D3E::App* a);
	};
}
