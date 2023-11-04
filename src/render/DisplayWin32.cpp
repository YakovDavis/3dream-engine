#include <iostream>
#include "DisplayWin32.h"
#include "App.h"

DisplayWin32::DisplayWin32(LPCWSTR applicationName, HINSTANCE hInst, int screenWidth, int screenHeight, D3E::App* a)
{
	std::cout << "Constructing window...\n";

	hInstance = hInst;
	app = a;

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = D3E::App::WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = reinterpret_cast<LPCSTR>(applicationName);
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	if (!RegisterClassEx(&wc))
	{
		std::cout << "Window class registration failed\n";
		//DWORD dw = GetLastError();
	}

	ClientWidth = screenWidth;
	ClientHeight = screenHeight;

	RECT windowRect = { 0, 0, static_cast<LONG>(ClientWidth), static_cast<LONG>(ClientHeight) };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	auto dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

	auto posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
	auto posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;

	std::cout << windowRect.right - windowRect.left << " " << windowRect.bottom - windowRect.top << "\n";

	hWnd = CreateWindow(reinterpret_cast<LPCSTR>(applicationName), reinterpret_cast<LPCSTR>(applicationName),
	                    dwStyle, posX, posY,
	                    windowRect.right - windowRect.left,
	                    windowRect.bottom - windowRect.top,
	                    nullptr, nullptr, hInstance, app);

	if (!hWnd)
	{
		std::cout << "Window creation failed\n";
	}

	if (!ShowWindow(hWnd, SW_SHOW))
	{
		std::cout << "ShowWindow failed\n";
	}

	SetForegroundWindow(hWnd);
	SetFocus(hWnd);

	ShowCursor(true);

	RAWINPUTDEVICE Rid[1];

	Rid[0].usUsagePage = 0x01;          // HID_USAGE_PAGE_GENERIC
	Rid[0].usUsage = 0x02;              // HID_USAGE_GENERIC_MOUSE
	Rid[0].dwFlags = 0;
	Rid[0].hwndTarget = 0;

	if (RegisterRawInputDevices(Rid, 1, sizeof(Rid[0])) == FALSE)
	{
		// Registration failed. Call GetLastError for the cause of the error
	}
}
