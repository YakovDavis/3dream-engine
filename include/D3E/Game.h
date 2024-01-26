#pragma once

#include "../../src/engine/systems/PhysicsInitSystem.h"
#include "App.h"
#include "D3E/systems/GameSystem.h"
#include "EASTL/hash_set.h"
#include "EASTL/shared_ptr.h"
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
	class ChildTransformSynchronizationSystem;

	class D3EAPI Game : public App
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

		static bool MouseLockedByImGui;
		static bool KeyboardLockedByImGui;

		void BuildNavmesh(entt::entity e);

		void OnEditorPlayPressed();

		void OnEditorPausePressed();

		void OnEditorStopPressed();

		void OnEditorSaveMapPressed();

		void ClearWorld();

		HRESULT AssetFileImport(String currentDir);

		void AssetDeleteDialog(String filename);

		bool IsGameRunning() const { return isGameRunning_; }
		bool IsGamePaused() const { return isGamePaused_; }

		void SetContentBrowserFilePath(const std::string& s);

		std::string GetContentBrowserFilePath() const { return contentBrowserFilePath_; }

		void OnSaveSelectedToPrefabPressed();

		void DestroyEntity(const String& uuid);

		void OnObjectClicked(entt::entity entity);

		entt::entity FindFirstNonEditorPlayer();

		void ParentEntitiesById(const String& childUuid, const String& parentUuid);

		void UnparentEntityById(const String& childUuid);

		void FlushChildTransformSync();

		void SignalParentingChange(const String& entityUuid, const String& prevParent);

		PhysicsInfo* GetPhysicsInfo();

	protected:
		DirectX::SimpleMath::Matrix gizmoTransform_;
		eastl::unordered_map<D3E::String, DirectX::SimpleMath::Matrix> gizmoOffsets_;

		entt::registry registry_;

		eastl::vector<GameSystem*> editorSystems_;

		eastl::vector<GameSystem*> systems_;
		eastl::vector<GameSystem*> renderPPsystems_;

		virtual void OnRegisterCustomComponents() {}

		virtual void Init();

		void EditorUpdate(float deltaTime);

		virtual void Update(float deltaTime);

		void Pick();

		void EditorDraw();

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

		bool isGameRunning_ = false;

		bool isGamePaused_ = false;

	private:
		void HandleMessages();

		eastl::hash_set<String> selectedUuids;

		std::string contentBrowserFilePath_ = "";

		entt::entity editorFakePlayer_;

		eastl::shared_ptr<ChildTransformSynchronizationSystem> childTransformSyncSystem;

		eastl::shared_ptr<PhysicsInitSystem> physicsInitSystem_;
	};
} // namespace D3E
