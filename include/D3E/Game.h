#pragma once

#include "App.h"
#include "D3E/systems/GameSystem.h"
#include "EASTL/vector.h"

#include <entt/entt.hpp>
#include <mutex>

namespace D3E
{
	class GameRender;
	class Display;
	class DisplayWin32;
	class InputDevice;	
	class SoundEngine;

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

		[[nodiscard]] float GetDeltaTime() const;

		[[nodiscard]] const entt::registry& GetRegistry() const;

//		void LoadTexture(const String& name, const String& fileName);

		LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

		std::mutex consoleCommandQueueMutex;

		std::string consoleCommandQueue;

		bool isQuitRequested_ = false;

	protected:
		entt::registry registry_;

		eastl::vector<GameSystem*> systems_;

		virtual void Init();

		virtual void Update(float deltaTime);

		virtual void Draw();

		virtual void DestroyResources();

		void CheckConsoleInput();

		void*
			prevCycleTimePoint_; // eastl::chrono::time_point<eastl::chrono::steady_clock>

		GameRender* gameRender_;

		InputDevice* inputDevice_;

		float deltaTime_;
		size_t frameCount_;

		SoundEngine* soundEngine_;

		double totalTime = 0.0;

	private:
		void HandleMessages();
	};
} // namespace D3E
