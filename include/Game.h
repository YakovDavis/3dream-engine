#pragma once

#include "App.h"

namespace D3E
{
	class GameRender;

	class Game : public App
	{
	public:
		void Run() final;

		explicit Game();
		~Game() override = default;

	protected:
		void Init();

		void Update(float deltaTime);

		void Draw();

		void DestroyResources();

		bool isQuitRequested_ = false;

		void* prevCycleTimePoint_; // eastl::chrono::time_point<eastl::chrono::steady_clock>

		GameRender* gameRender_;
	};
}
