#pragma once

#include "App.h"

class GameRender;

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
		virtual void Init();

		virtual void Update(float deltaTime);

		virtual void Draw();

		virtual void DestroyResources();

		bool isQuitRequested_ = false;

		void* prevCycleTimePoint_; // eastl::chrono::time_point<eastl::chrono::steady_clock>

		GameRender* gameRender_;
	};
}
