#pragma once

namespace D3E
{
	class App
	{
	public:
		virtual void Run();

		App() = default;
		virtual ~App() = default;
	};

	App* CreateApp();
}
