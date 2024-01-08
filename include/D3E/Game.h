#pragma once

#include "App.h"
#include "D3E/systems/GameSystem.h"
#include "EASTL/hash_set.h"
#include "EASTL/unordered_map.h"
#include "EASTL/vector.h"
#include "SimpleMath.h"

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

		void SetUuidEditorSelected(const String& uuid, bool selected, bool resetOthers);

		const eastl::hash_set<String>& GetSelectedUuids() const;

		void OnObjectInfoConstruct(entt::registry& registry, entt::entity entity);

		void OnObjectInfoDestroy(entt::registry& registry, entt::entity entity);

		void CalculateGizmoTransformsOffsets();

		DirectX::SimpleMath::Matrix& GetGizmoTransform();

		const DirectX::SimpleMath::Matrix& GetGizmoOffset(const D3E::String& uuid) const;

		[[nodiscard]] float GetDeltaTime() const;

		[[nodiscard]] const entt::registry& GetRegistry() const;

		size_t GetFrameCount();

		bool FindEntityByID(entt::entity& entity, const D3E::String& uuid);

		LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

		std::mutex consoleCommandQueueMutex;

		std::string consoleCommandQueue;

		bool isQuitRequested_ = false;

	protected:
		DirectX::SimpleMath::Matrix gizmoTransform_;
		eastl::unordered_map<D3E::String, DirectX::SimpleMath::Matrix> gizmoOffsets_;

		entt::registry registry_;

		eastl::vector<GameSystem*> systems_;

		eastl::vector<GameSystem*> renderPPsystems_;

		virtual void OnRegisterCustomComponents() {}

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

		eastl::unordered_map<String, entt::entity> uuidEntityList;

	private:
		void HandleMessages();

		eastl::hash_set<String> selectedUuids;

		void RegisterDefaultComponents();
	};
} // namespace D3E
