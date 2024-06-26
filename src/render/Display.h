#pragma once

namespace D3E
{
	class App;

	class Display
	{
	protected:
		D3E::App* app;

	public:
		int ClientHeight;
		int ClientWidth;
		bool IsResizing = false;
		bool IsMaximized = false;
		bool IsMinimized = false;
		Display(int screenWidth, int screenHeight, D3E::App* a) :
			  ClientWidth(screenWidth), ClientHeight(screenHeight), app(a) {}
		virtual ~Display() = default;
	};
}
