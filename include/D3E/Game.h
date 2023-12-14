#pragma once

#include "App.h"
#include "D3E/systems/GameSystem.h"
#include "EASTL/vector.h"
#include "EASTL/hash_set.h"

#include <entt/entt.hpp>
#include <mutex>

namespace D3E
{
	class GameRender;
	class Display;
	class DisplayWin32;
	class InputDevice;	
	class SoundEngine;
	class TimerManager;
	class PhysicsInfo;

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

		bool IsUuidEditorSelected(const String& uuid);

		[[nodiscard]] float GetDeltaTime() const;

		[[nodiscard]] const entt::registry& GetRegistry() const;

		size_t GetFrameCount();

//		void LoadTexture(const String& name, const String& fileName);

		LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

		std::mutex consoleCommandQueueMutex;

		std::string consoleCommandQueue;

		bool isQuitRequested_ = false;

	protected:
		entt::registry registry_;

		eastl::vector<GameSystem*> systems_;

		eastl::vector<GameSystem*> renderPPsystems_;

		virtual void Init();

		virtual void Update(float deltaTime);

		void Pick();

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

		PhysicsInfo* physicsInfo_;

		double totalTime = 0.0;

	private:
		void HandleMessages();

		eastl::hash_set<String> selectedUuids;
	};
} // namespace D3E
