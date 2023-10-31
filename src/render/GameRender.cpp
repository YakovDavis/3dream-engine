#include "GameRender.h"

void D3E::GameRender::Init()
{
	WNDCLASS wc;
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	//wc.lpfnWndProc   = MainWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = mhAppInst;
	wc.hIcon         = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName  = nullptr;
	wc.lpszClassName = reinterpret_cast<LPCSTR>(L"MainWnd");

	if(!RegisterClass(&wc))
	{
		MessageBox(0, reinterpret_cast<LPCSTR>(L"RegisterClass Failed."), 0, 0);
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, mClientWidth, mClientHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width  = R.right - R.left;
	int height = R.bottom - R.top;

	mhMainWnd = CreateWindow(reinterpret_cast<LPCSTR>(L"MainWnd"),
	                         reinterpret_cast<LPCSTR>(mMainWndCaption.c_str()),
	                         WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, nullptr, nullptr, mhAppInst, nullptr);
	if(!mhMainWnd)
	{
		MessageBox(nullptr, reinterpret_cast<LPCSTR>(L"CreateWindow Failed."), nullptr, 0);
	}

	ShowWindow(mhMainWnd, SW_SHOW);
	UpdateWindow(mhMainWnd);
}

void D3E::GameRender::DestroyResources()
{
}

HINSTANCE D3E::GameRender::AppInst() const
{
	return mhAppInst;
}

HWND D3E::GameRender::MainWnd() const
{
	return mhMainWnd;
}

float D3E::GameRender::AspectRatio() const
{
	return static_cast<float>(mClientWidth) / static_cast<float>(mClientHeight);
}

void D3E::GameRender::OnResize()
{
}

void D3E::GameRender::CalculateFrameStats()
{
	/* TODO: recreate using our timers
	// Code computes the average frames per second, and also the
	// average time it takes to render one frame.  These stats
	// are appended to the window caption bar.

	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if((mTimer.TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		std::wstring fpsStr = std::to_wstring(fps);
		std::wstring mspfStr = std::to_wstring(mspf);

		std::wstring windowText = mMainWndCaption +
		                     L"    fps: " + fpsStr +
		                     L"   mspf: " + mspfStr;

		SetWindowText(mhMainWnd, windowText.c_str());

		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
	*/
}

D3E::GameRender::GameRender(HINSTANCE hInstance) : mhAppInst(hInstance)
{
}
