#pragma once

#include <Windows.h>

namespace D3E
{
	class App
	{
	public:
		virtual void Run() {}

		App() = default;
		void SetAppInstance(HINSTANCE hInstance) { mhAppInst = hInstance; }
		virtual ~App() = default;

	protected:
		HINSTANCE mhAppInst;
	};

	App* CreateApp();
}


