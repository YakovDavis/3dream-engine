#pragma once

#include "App.h"
#include "EASTL/chrono.h"

namespace D3E
{
	class Game : public App
	{
	public:
		void Run() final;

		Game() = default;
		~Game() override = default;

	protected:
		void Init();

		void Update(float deltaTime);

		void Draw();

		void DestroyResources();

		bool isQuitRequested_ = false;

		eastl::chrono::time_point<eastl::chrono::steady_clock> prevCycleTimePoint_;
	};
}
