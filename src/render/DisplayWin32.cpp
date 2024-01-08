#include "DisplayWin32.h"

#include "D3E/CommonCpp.h"
#include "D3E/App.h"
#include "D3E/Debug.h"

D3E::DisplayWin32::DisplayWin32(LPCWSTR applicationName, HINSTANCE hInst, int screenWidth, int screenHeight, D3E::App* a) : Display(screenWidth, screenHeight, a), wc()
{
	Debug::LogMessage("[DisplayWin32] Starting window creation");

	hInstance = hInst;

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_CLASSDC;
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
		Debug::HandleLastWindowsError("DisplayWin32");
	}

	RECT windowRect = { 0, 0, static_cast<LONG>(ClientWidth), static_cast<LONG>(ClientHeight) };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	auto dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

	auto posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
	auto posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;

	hWnd = CreateWindow(reinterpret_cast<LPCSTR>(applicationName), reinterpret_cast<LPCSTR>(applicationName),
	                    dwStyle, posX, posY,
	                    windowRect.right - windowRect.left,
	                    windowRect.bottom - windowRect.top,
	                    nullptr, nullptr, hInstance, app);

	if (hWnd == nullptr)
	{
		Debug::HandleLastWindowsError("DisplayWin32");
	}

	ShowWindow(hWnd, SW_SHOW);

	SetForegroundWindow(hWnd);
	SetFocus(hWnd);

	ShowCursor(true);

	Debug::LogMessage("[DisplayWin32] Finished window creation");

	RAWINPUTDEVICE Rid[1];

	Rid[0].usUsagePage = 0x01;          // HID_USAGE_PAGE_GENERIC
	Rid[0].usUsage = 0x02;              // HID_USAGE_GENERIC_MOUSE
	Rid[0].dwFlags = 0;
	Rid[0].hwndTarget = 0;

	if (RegisterRawInputDevices(Rid, 1, sizeof(Rid[0])) == FALSE)
	{
		Debug::HandleLastWindowsError("DisplayWin32");
	}

	Debug::LogMessage("[DisplayWin32] Registered raw input device");
}
