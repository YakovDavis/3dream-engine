#pragma once

#include "App.h"
#include "D3E/systems/PerTickSystem.h"
#include "EASTL/vector.h"

#include <entt/entt.hpp>

namespace D3E
{
	class GameRender;
	class Display;
	class DisplayWin32;
	class InputDevice;

	class Game : public App
	{
	public:
		void Run() final;

		explicit Game();
		~Game() override = default;

		GameRender* GetRender();

		Display* GetDisplay();

		DisplayWin32* GetDisplayWin32();

		entt::registry& GetRegistry();

		InputDevice* GetInputDevice();

		float GetDeltaTime() const;

		[[nodiscard]] const entt::registry& GetRegistry() const;

		LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

	protected:
		entt::registry registry_;

		eastl::vector<PerTickSystem*> perTickSystems;

		virtual void Init();

		virtual void Update(float deltaTime);

		virtual void Draw();

		virtual void DestroyResources();

		bool isQuitRequested_ = false;

		void* prevCycleTimePoint_; // eastl::chrono::time_point<eastl::chrono::steady_clock>

		GameRender* gameRender_;

		InputDevice* inputDevice_;

		float deltaTime_;

	private:
		void HandleMessages();
	};
}
