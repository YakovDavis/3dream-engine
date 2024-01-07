#include "D3E/Game.h"

#include "D3E/AssetManager.h"
#include "D3E/CommonCpp.h"
#include "D3E/Components/TransformComponent.h"
#include "D3E/Components/render/CameraComponent.h"
#include "D3E/Components/sound/SoundComponent.h"
#include "D3E/Debug.h"
#include "D3E/TimerManager.h"
#include "D3E/engine/ConsoleManager.h"
#include "D3E/scripting/ScriptingEngine.h"
#include "D3E/systems/CreationSystems.h"
#include "EASTL/chrono.h"
#include "EngineState.h"
#include "assetmng/DefaultAssetLoader.h"
#include "assetmng/ScriptFactory.h"
#include "editor/EditorIdManager.h"
#include "editor/EditorUtils.h"
#include "engine/systems/CharacterInitSystem.h"
#include "engine/systems/ChildTransformSynchronizationSystem.h"
#include "engine/systems/FPSControllerSystem.h"
#include "engine/systems/PhysicsInitSystem.h"
#include "engine/systems/PhysicsUpdateSystem.h"
#include "engine/systems/ScriptInitSystem.h"
#include "engine/systems/ScriptUpdateSystem.h"
#include "engine/systems/SoundEngineListenerSystem.h"
#include "imgui.h"
#include "input/InputDevice.h"
#include "physics/PhysicsInfo.h"
#include "render/DisplayWin32.h"
#include "render/GameRenderD3D12.h"
#include "render/systems/EditorUtilsRenderSystem.h"
#include "render/systems/InputSyncSystem.h"
#include "render/systems/LightInitSystem.h"
#include "render/systems/LightRenderSystem.h"
#include "render/systems/StaticMeshInitSystem.h"
#include "render/systems/StaticMeshRenderSystem.h"
#include "sound_engine/SoundEngine.h"
#include "utils/ECSUtils.h"

#include <filesystem>
#include <thread>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd,
                                                             UINT msg,
                                                             WPARAM wParam,
                                                             LPARAM lParam);

void PollConsoleInput(D3E::Game* game)
{
	while (!game->isQuitRequested_)
	{
		std::string input;
		std::getline(std::cin, input);

		std::lock_guard<std::mutex> lock(game->consoleCommandQueueMutex);
		game->consoleCommandQueue = input;
	}
}

void D3E::Game::Run()
{
	App::Run();

	Init();

	auto* prevCycleTimePoint = reinterpret_cast<
		eastl::chrono::time_point<eastl::chrono::steady_clock>*>(
		prevCycleTimePoint_);

	*prevCycleTimePoint = eastl::chrono::steady_clock::now();

	std::thread inputCheckingThread(PollConsoleInput, this);

	bool lmbPressedLastTick = false; // temp

	while (!isQuitRequested_)
	{
		HandleMessages();

		CheckConsoleInput(); // ConsoleManager::getInstance()->handleConsoleInput();

		{
			using namespace eastl::chrono;
			deltaTime_ = duration_cast<duration<float, milliseconds::period>>(
							 steady_clock::now() - *prevCycleTimePoint)
			                 .count();
		}

		gameRender_->PrimitiveBatchStart();

		for (auto& sys : systems_)
		{
			sys->PrePhysicsUpdate(registry_, this, deltaTime_);
		}

		physicsInfo_->updatePhysics();

		for (auto& sys : systems_)
		{
			sys->PostPhysicsUpdate(registry_);
		}

		Update(deltaTime_);

		if (lmbPressedLastTick && !inputDevice_->IsKeyDown(Keys::LeftButton))
		{
			Pick();
		}
		lmbPressedLastTick = inputDevice_->IsKeyDown(Keys::LeftButton);

		*prevCycleTimePoint = eastl::chrono::steady_clock::now();

		Draw();

		++frameCount_;
	}

	inputCheckingThread.detach();

	DestroyResources();
}

void D3E::Game::Init()
{
	D3E::ECSUtils::Init(this);

	assert(mhAppInst != nullptr);
	Debug::ClearLog();

	RegisterDefaultComponents();

	for (auto& sys : systems_)
	{
		sys->Init();
	}

	gameRender_ = new GameRenderD3D12(this, mhAppInst);
	EngineState::Initialize(this);
	gameRender_->Init(systems_);

	// AssetManager::Get().CreateTexture("default-grid",
	// "textures/default-grid.png", gameRender_->GetDevice(),
	// gameRender_->GetCommandList());
	// AssetManager::Get().CreateTexture("white", "textures/white.png",
	// gameRender_->GetDevice(), gameRender_->GetCommandList());
	// AssetManager::Get().CreateTexture("cerberus_A",
	// "textures/cerberus_A.png", gameRender_->GetDevice(),
	// gameRender_->GetCommandList());
	// AssetManager::Get().CreateTexture("cerberus_M",
	// "textures/cerberus_M.png", gameRender_->GetDevice(),
	// gameRender_->GetCommandList());
	// AssetManager::Get().CreateTexture("cerberus_R",
	// "textures/cerberus_R.png", gameRender_->GetDevice(),
	// gameRender_->GetCommandList());
	// AssetManager::Get().CreateTexture("environment",
	// "textures/environment.hdr", gameRender_->GetDevice(),
	// gameRender_->GetCommandList());
	// AssetManager::Get().CreateMesh("cerberus", "models/cerberus.fbx",
	// gameRender_->GetDevice(), gameRender_->GetCommandList());

	AssetManager::Get().LoadAssetsInFolder("textures/", true,
	                                       gameRender_->GetDevice(),
	                                       gameRender_->GetCommandList());
	AssetManager::Get().LoadAssetsInFolder("models/", true,
	                                       gameRender_->GetDevice(),
	                                       gameRender_->GetCommandList());

	AssetManager::Get().LoadAssetsInFolder("scripts/", true, nullptr, nullptr);

	AssetManager::Get().LoadAssetsInFolder("materials/", true,
	                                       gameRender_->GetDevice(),
	                                       gameRender_->GetCommandList());

	DefaultAssetLoader::LoadEditorDebugAssets(gameRender_->GetDevice(),
	                                          gameRender_->GetCommandList());

	ScriptingEngine::GetInstance().Init(this);
	TimerManager::GetInstance().Init(this);

	inputDevice_ = new InputDevice(this);

	physicsInfo_ = new PhysicsInfo(this);

	systems_.push_back(new StaticMeshInitSystem);
	systems_.push_back(new StaticMeshRenderSystem);
	systems_.push_back(new FPSControllerSystem);
	systems_.push_back(new ScriptInitSystem(registry_));
	systems_.push_back(new ScriptUpdateSystem);
	systems_.push_back(new InputSyncSystem);
	systems_.push_back(new ChildTransformSynchronizationSystem(registry_));
	systems_.push_back(
		new PhysicsInitSystem(registry_, physicsInfo_->getPhysicsSystem()));
	systems_.push_back(
		new PhysicsUpdateSystem(physicsInfo_->getPhysicsSystem()));
	systems_.push_back(new CharacterInitSystem(registry_, physicsInfo_->getPhysicsSystem()));

	renderPPsystems_.push_back(new LightInitSystem);
	renderPPsystems_.push_back(new LightRenderSystem);
	renderPPsystems_.push_back(new EditorUtilsRenderSystem);

	soundEngine_ = &SoundEngine::GetInstance();
	soundEngine_->Init();

	CreationSystems::CreateEditorDebugRender(registry_);
}

void D3E::Game::Update(const float deltaTime)
{
	totalTime += deltaTime;

	soundEngine_->Update();

	TimerManager::GetInstance().Update(deltaTime);

	for (auto& sys : systems_)
	{
		sys->Update(registry_, this, deltaTime);
	}

	for (auto& sys : renderPPsystems_)
	{
		sys->Update(registry_, this, deltaTime);
	}

	gameRender_->UpdateAnimations(deltaTime);

	inputDevice_->EndTick();
}

void D3E::Game::Draw()
{
	gameRender_->PrepareDraw(registry_, systems_, renderPPsystems_);
	gameRender_->Draw(registry_, systems_, renderPPsystems_);
	gameRender_->EndDraw(registry_, systems_, renderPPsystems_);

	gameRender_->Present();

	gameRender_->GetDevice()->runGarbageCollection();
}

void D3E::Game::DestroyResources()
{
	soundEngine_->Release();
	gameRender_->editor_->Release();
	Debug::CloseLog();
}

D3E::Game::Game() : soundEngine_{nullptr}
{
	EditorUtils::Initialize(this);
	prevCycleTimePoint_ =
		new eastl::chrono::time_point<eastl::chrono::steady_clock>(
			eastl::chrono::steady_clock::now());

	registry_.on_construct<ObjectInfoComponent>().connect<&Game::OnObjectInfoConstruct>(this);
	registry_.on_destroy<ObjectInfoComponent>().connect<&Game::OnObjectInfoDestroy>(this);
}

void D3E::Game::HandleMessages()
{
	MSG msg = {};
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_QUIT)
		isQuitRequested_ = true;
}

D3E::GameRender* D3E::Game::GetRender()
{
	return gameRender_;
}

D3E::Display* D3E::Game::GetDisplay()
{
	return gameRender_->GetDisplay();
}

D3E::DisplayWin32* D3E::Game::GetDisplayWin32()
{
	return dynamic_cast<DisplayWin32*>(GetDisplay());
}

entt::registry& D3E::Game::GetRegistry()
{
	return registry_;
}

const entt::registry& D3E::Game::GetRegistry() const
{
	return registry_;
}

size_t D3E::Game::GetFrameCount()
{
	return frameCount_;
}

D3E::InputDevice* D3E::Game::GetInputDevice()
{
	return inputDevice_;
}

LRESULT D3E::Game::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
	{
		return true;
	}

	switch (msg)
	{
		case WM_KEYDOWN:
		{
			if (static_cast<unsigned int>(wParam) == 27)
				PostQuitMessage(0);
			return 0;
		}
		case WM_INPUT:
		{
			UINT dwSize = 0;
			GetRawInputData(
				reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, nullptr,
				&dwSize,
				sizeof(RAWINPUTHEADER)); // NOLINT(performance-no-int-to-ptr)
			const auto lpb = new BYTE[dwSize];
			if (lpb == nullptr)
			{
				return 0;
			}

			if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT,
			                    lpb, &dwSize, sizeof(RAWINPUTHEADER)) !=
			    dwSize) // NOLINT(performance-no-int-to-ptr)
				OutputDebugString(
					TEXT("GetRawInputData does not return correct size !\n"));

			const auto* raw = reinterpret_cast<RAWINPUT*>(lpb);

			if (raw->header.dwType == RIM_TYPEKEYBOARD)
			{
				inputDevice_->OnKeyDown(
					{raw->data.keyboard.MakeCode, raw->data.keyboard.Flags,
				     raw->data.keyboard.VKey, raw->data.keyboard.Message});
			}
			else if (raw->header.dwType == RIM_TYPEMOUSE)
			{
				inputDevice_->OnMouseMove(
					{raw->data.mouse.usFlags, raw->data.mouse.usButtonFlags,
				     static_cast<int>(raw->data.mouse.ulExtraInformation),
				     static_cast<int>(raw->data.mouse.ulRawButtons),
				     static_cast<short>(raw->data.mouse.usButtonData),
				     raw->data.mouse.lLastX, raw->data.mouse.lLastY});
			}

			delete[] lpb;
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
		default:
		{
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
	}
}

float D3E::Game::GetDeltaTime() const
{
	return deltaTime_;
}

void D3E::Game::CheckConsoleInput()
{
	std::lock_guard<std::mutex> lock(consoleCommandQueueMutex);
	if (!consoleCommandQueue.empty())
	{
		ConsoleManager::getInstance()->handleConsoleInput(consoleCommandQueue);
		// std::cout << consoleCommandQueue << '\n';
		consoleCommandQueue = std::string();
	}
}

bool D3E::Game::IsUuidEditorSelected(const D3E::String& uuid)
{
	return selectedUuids.find(uuid) != selectedUuids.end();
}

void D3E::Game::Pick()
{
	if (!Editor::Get()->IsMouseOnViewport())
	{
		return;
	}
	int mX, mY;
	Editor::Get()->GetMousePositionInViewport(mX, mY);
	auto editorPickedId = gameRender_->EditorPick(mX, mY);
	if (editorPickedId == 0)
	{
		selectedUuids.clear();
	}
	else
	{
		if (!(inputDevice_->IsKeyDown(Keys::LeftControl) ||
		      inputDevice_->IsKeyDown(Keys::LeftShift)))
		{
			selectedUuids.clear();
		}
		selectedUuids.insert(EditorIdManager::Get()->GetUuid(editorPickedId));
	}
	EditorUtilsRenderSystem::isSelectionDirty = true;
	CalculateGizmoTransformsOffsets();
}

void D3E::Game::SetUuidEditorSelected(const D3E::String& uuid, bool selected, bool resetOthers)
{

	if (resetOthers)
	{
		selectedUuids.clear();
	}
	if (selected)
	{
		if (selectedUuids.find(uuid) == selectedUuids.end())
		{
			selectedUuids.insert(uuid);
		}
	}
	else
	{
		if (selectedUuids.find(uuid) != selectedUuids.end())
		{
			selectedUuids.erase(uuid);
		}
	}
	EditorUtilsRenderSystem::isSelectionDirty = true;
	CalculateGizmoTransformsOffsets();
}

const eastl::hash_set<D3E::String>& D3E::Game::GetSelectedUuids() const
{
	return selectedUuids;
}

void D3E::Game::CalculateGizmoTransformsOffsets()
{
	Vector3 gizmoPosition;
	for (const auto& uuid : selectedUuids)
	{
		auto tc = registry_.try_get<TransformComponent>(uuidEntityList[uuid]);
		if (!tc)
		{
			continue;
		}
		gizmoPosition += tc->position;
	}
	gizmoTransform_ = DirectX::SimpleMath::Matrix::CreateTranslation(gizmoPosition);
	DirectX::SimpleMath::Matrix invGizmoTransform = gizmoTransform_.Invert();
	gizmoOffsets_.clear();
	for (const auto& uuid : selectedUuids)
	{
		auto tc = registry_.try_get<TransformComponent>(uuidEntityList[uuid]);
		if (!tc)
		{
			continue;
		}
		gizmoOffsets_.insert({uuid, invGizmoTransform});
		gizmoOffsets_[uuid] *= DirectX::SimpleMath::Matrix::CreateScale(tc->scale) *
		                       DirectX::SimpleMath::Matrix::CreateFromQuaternion(tc->rotation) *
		                       DirectX::SimpleMath::Matrix::CreateTranslation(tc->position);
	}
}

void D3E::Game::OnObjectInfoConstruct(entt::registry& registry, entt::entity entity)
{
	uuidEntityList.insert({registry.get<ObjectInfoComponent>(entity).id, entity});
}

void D3E::Game::OnObjectInfoDestroy(entt::registry& registry, entt::entity entity)
{
	uuidEntityList.erase(registry.get<ObjectInfoComponent>(entity).id);
}

DirectX::SimpleMath::Matrix& D3E::Game::GetGizmoTransform()
{
	return gizmoTransform_;
}

const DirectX::SimpleMath::Matrix& D3E::Game::GetGizmoOffset(const D3E::String& uuid) const
{
	if (gizmoOffsets_.find(uuid) == gizmoOffsets_.end())
	{
		return DirectX::SimpleMath::Matrix::Identity;
	}
	return gizmoOffsets_.at(uuid);
}

void D3E::Game::RegisterDefaultComponents()
{
	using namespace entt::literals;

	entt::meta<ObjectInfoComponent>().type("ObjectInfoComponent"_hs)
		.data<&ObjectInfoComponent::parentId>("parentId"_hs)
		.data<&ObjectInfoComponent::name>("name"_hs)
		.data<&ObjectInfoComponent::id>("id"_hs)
		.data<&ObjectInfoComponent::editorId>("editorId"_hs)
		.data<&ObjectInfoComponent::visible>("visible"_hs);

	entt::meta<TransformComponent>().type("TransformComponent"_hs)
		.data<&TransformComponent::position>("position"_hs)
		.data<&TransformComponent::rotation>("rotation"_hs)
		.data<&TransformComponent::scale>("rotation"_hs)
		.data<&TransformComponent::relativePosition>("relativePosition"_hs)
		.data<&TransformComponent::relativeRotation>("relativeRotation"_hs)
		.data<&TransformComponent::relativeScale>("relativeScale"_hs);
}
