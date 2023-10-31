#pragma once

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#if D3E_PLATFORM_WINDOWS
#include <windows.h>
#endif // D3E_PLATFORM_WINDOWS

namespace D3E
{
	class App
	{
	public:
		virtual void Run();

		App() = default;
#if D3E_PLATFORM_WINDOWS
		void SetAppInstance(HINSTANCE hInstance);
#endif // D3E_PLATFORM_WINDOWS
		virtual ~App() = default;

	protected:
#if D3E_PLATFORM_WINDOWS
		HINSTANCE mhAppInst;
#endif // D3E_PLATFORM_WINDOWS
	};

	App* CreateApp();
}
