#include "D3E/Game.h"

#include "D3E/AssetManager.h"
#include "D3E/CommonCpp.h"
#include "D3E/Components/TransformComponent.h"
#include "D3E/Components/navigation/NavmeshComponent.h"
#include "D3E/Components/render/CameraComponent.h"
#include "D3E/Components/render/StaticMeshComponent.h"
#include "D3E/Components/sound/SoundComponent.h"
#include "D3E/Debug.h"
#include "D3E/TimerManager.h"
#include "D3E/engine/ConsoleManager.h"
#include "D3E/scripting/ScriptingEngine.h"
#include "D3E/systems/CreationSystems.h"
#include "EASTL/chrono.h"
#include "EngineState.h"
#include "assetmng/CDialogEventHandler.h"
#include "assetmng/DefaultAssetLoader.h"
#include "assetmng/MeshFactory.h"
#include "assetmng/ScriptFactory.h"
#include "editor/EditorIdManager.h"
#include "editor/EditorUtils.h"
#include "engine/ComponentFactory.h"
#include "engine/systems/CharacterInitSystem.h"
#include "engine/systems/ChildTransformSynchronizationSystem.h"
#include "engine/systems/FPSControllerSystem.h"
#include "engine/systems/PhysicsInitSystem.h"
#include "engine/systems/PhysicsUpdateSystem.h"
#include "engine/systems/ScriptInitSystem.h"
#include "engine/systems/ScriptUpdateSystem.h"
#include "engine/systems/SoundEngineListenerSystem.h"
#include "engine/systems/TPSControllerSystem.h"
#include "imgui.h"
#include "input/InputDevice.h"
#include "json.hpp"
#include "navigation/NavmeshBuilder.h"
#include "physics/PhysicsInfo.h"
#include "render/DisplayWin32.h"
#include "render/GameRenderD3D12.h"
#include "render/RenderUtils.h"
#include "render/systems/EditorUtilsRenderSystem.h"
#include "render/systems/InputSyncSystem.h"
#include "render/systems/LightInitSystem.h"
#include "render/systems/LightRenderSystem.h"
#include "render/systems/StaticMeshInitSystem.h"
#include "render/systems/StaticMeshRenderSystem.h"
#include "sound_engine/SoundEngine.h"
#include "utils/ECSUtils.h"
#include "utils/MeshUtils.h"

#include <filesystem>
#include <thread>

static json currentMapSavedState = json({{"type", "world"},
                                         {"uuid", D3E::EmptyIdStdStr},
                                         {"filename", "NewWorld"},
                                         {"entities", {}}});

bool D3E::Game::MouseLockedByImGui = false;
bool D3E::Game::KeyboardLockedByImGui = false;

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
			sys->PrePhysicsUpdate(registry_, this, PhysicsInfo::DELTA_TIME);
		}

		physicsInfo_->updatePhysics();

		for (auto& sys : systems_)
		{
			sys->PostPhysicsUpdate(registry_);
		}

		if (isGameRunning_)
		{
			if (!isGamePaused_)
			{
				Update(deltaTime_);
			}
		}
		else
		{
			EditorUpdate(deltaTime_);
		}

		gameRender_->UpdateAnimations(deltaTime_);

		inputDevice_->EndTick();

		if (lmbPressedLastTick && !inputDevice_->IsKeyDown(Keys::LeftButton))
		{
			Pick();
		}
		lmbPressedLastTick = inputDevice_->IsKeyDown(Keys::LeftButton);

		*prevCycleTimePoint = eastl::chrono::steady_clock::now();

		if (isGameRunning_)
		{
			Draw();
		}
		else
		{
			EditorDraw();
		}

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

	for (auto& sys : systems_)
	{
		sys->Init();
	}

	gameRender_ = new GameRenderD3D12(this, mhAppInst);
	EngineState::Initialize(this);
	gameRender_->Init(systems_);

	AssetManager::Get().LoadAssetsInFolder("assets/", true,
	                                       gameRender_->GetDevice(),
	                                       gameRender_->GetCommandList());

	DefaultAssetLoader::LoadEditorDebugAssets(gameRender_->GetDevice(),
	                                          gameRender_->GetCommandList());

	TimerManager::GetInstance().Init(this);

	inputDevice_ = new InputDevice(this);

	// Initialization order is crucial: depends on InputDevice
	ScriptingEngine::GetInstance().Init(this);

	physicsInfo_ = new PhysicsInfo(this);

	gameRender_->PostAssetLoadInit();

	systems_.push_back(new StaticMeshInitSystem);
	systems_.push_back(new StaticMeshRenderSystem);
	systems_.push_back(new FPSControllerSystem);
	systems_.push_back(new TPSControllerSystem);
	systems_.push_back(new ScriptInitSystem(registry_));
	systems_.push_back(new ScriptUpdateSystem);
	systems_.push_back(new InputSyncSystem);
	childTransformSyncSystem = eastl::make_shared<ChildTransformSynchronizationSystem>(registry_);
	systems_.push_back(childTransformSyncSystem.get());
	systems_.push_back(new PhysicsInitSystem(registry_, this,
	                                         physicsInfo_->getPhysicsSystem()));
	systems_.push_back(
		new PhysicsUpdateSystem(physicsInfo_->getPhysicsSystem()));
	systems_.push_back(new CharacterInitSystem(
		registry_, this, physicsInfo_->getPhysicsSystem()));

	renderPPsystems_.push_back(new LightInitSystem(this));
	renderPPsystems_.push_back(new LightRenderSystem);

	editorSystems_.push_back(new ChildTransformSynchronizationSystem(registry_));
	editorSystems_.push_back(new FPSControllerSystem);
	editorSystems_.push_back(new EditorUtilsRenderSystem);

	soundEngine_ = &SoundEngine::GetInstance();
	soundEngine_->Init();

	ClearWorld();

#ifdef D3E_WITH_EDITOR
	EngineState::currentPlayer = editorFakePlayer_;
#else
	EngineState::currentPlayer = FindFirstNonEditorPlayer();
#endif

	ComponentFactory::Initialize(this);
}

void D3E::Game::EditorUpdate(const float deltaTime)
{
	totalTime += deltaTime;

	soundEngine_->Update();

	TimerManager::GetInstance().Update(deltaTime);

	for (auto& sys : editorSystems_)
	{
		sys->Update(registry_, this, deltaTime);
	}

	physicsInfo_->draw();
}

void D3E::Game::EditorDraw()
{
	gameRender_->PrepareFrame();
	gameRender_->BeginDraw(registry_, systems_);
	gameRender_->BeginDraw(registry_, renderPPsystems_);
	gameRender_->BeginDraw(registry_, editorSystems_);
	gameRender_->DrawOpaque(registry_, systems_);
	gameRender_->DrawPostProcessSystems(registry_, renderPPsystems_);
	gameRender_->DrawPostProcessEffects(registry_);
	gameRender_->DrawPostProcessSystems(registry_, editorSystems_);
	gameRender_->DrawDebug();
	gameRender_->EndDraw(registry_, systems_);
	gameRender_->EndDraw(registry_, renderPPsystems_);
	gameRender_->EndDraw(registry_, editorSystems_);
	gameRender_->DrawGUI();
	gameRender_->Present();
	gameRender_->GetDevice()->runGarbageCollection();
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
}

void D3E::Game::Draw()
{
	gameRender_->PrepareFrame();
	gameRender_->BeginDraw(registry_, systems_);
	gameRender_->BeginDraw(registry_, renderPPsystems_);
	gameRender_->DrawOpaque(registry_, systems_);
	gameRender_->DrawPostProcessSystems(registry_, renderPPsystems_);
	gameRender_->DrawPostProcessEffects(registry_);
	gameRender_->DrawDebug();
	gameRender_->EndDraw(registry_, systems_);
	gameRender_->EndDraw(registry_, renderPPsystems_);
	gameRender_->DrawGUI();
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

	registry_.on_construct<ObjectInfoComponent>()
		.connect<&Game::OnObjectInfoConstruct>(this);
	registry_.on_destroy<ObjectInfoComponent>()
		.connect<&Game::OnObjectInfoDestroy>(this);
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
		// WM_SIZE is sent when the user resizes the window.
		case WM_SIZE:
			if (GetRender() && GetDisplay())
			{
				// Save the new client area dimensions.
				GetDisplay()->ClientWidth = LOWORD(lParam);
				GetDisplay()->ClientHeight = HIWORD(lParam);
				if (wParam == SIZE_MINIMIZED)
				{
					GetDisplay()->IsMinimized = true;
					GetDisplay()->IsMaximized = false;
				}
				else if (wParam == SIZE_MAXIMIZED)
				{
					GetDisplay()->IsMinimized = false;
					GetDisplay()->IsMaximized = true;
					GetRender()->OnResize();
				}
				else if (wParam == SIZE_RESTORED)
				{
					// Restoring from minimized state?
					if (GetDisplay()->IsMinimized)
					{
						GetDisplay()->IsMinimized = false;
						GetRender()->OnResize();
					}
					// Restoring from maximized state?
					else if (GetDisplay()->IsMaximized)
					{
						GetDisplay()->IsMaximized = false;
						GetRender()->OnResize();
					}
					else if (GetDisplay()->IsResizing)
					{
						// If user is dragging the resize bars, we do not resize
						// the buffers here because as the user continuously
						// drags the resize bars, a stream of WM_SIZE messages
						// are sent to the window, and it would be pointless
						// (and slow) to resize for each WM_SIZE message
						// received from dragging the resize bars.  So instead,
						// we reset after the user is done resizing the window
						// and releases the resize bars, which sends a
						// WM_EXITSIZEMOVE message.
					}
					else // API call such as SetWindowPos or
					     // mSwapChain->SetFullscreenState.
					{
						GetRender()->OnResize();
					}
				}
			}
			return 0;

		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
		case WM_ENTERSIZEMOVE:
			if (GetRender() && GetDisplay())
			{
				GetDisplay()->IsResizing = true;
			}
			return 0;

		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
		case WM_EXITSIZEMOVE:
			if (GetRender() && GetDisplay())
			{
				GetDisplay()->IsResizing = false;
				GetRender()->OnResize();
			}
			return 0;

		// WM_DESTROY is sent when the window is being destroyed.
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
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
		OnObjectClicked(uuidEntityList[EditorIdManager::Get()->GetUuid(editorPickedId)]);
	}
	EditorUtilsRenderSystem::isSelectionDirty = true;
	CalculateGizmoTransformsOffsets();
}

void D3E::Game::SetUuidEditorSelected(const D3E::String& uuid, bool selected,
                                      bool resetOthers)
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
	if (selectedUuids.empty())
	{
		return;
	}
	FlushChildTransformSync();
	Vector3 gizmoPosition, gizmoScale;
	Quaternion gizmoRotation;
	for (const auto& uuid : selectedUuids)
	{
		auto tc = registry_.try_get<TransformComponent>(uuidEntityList[uuid]);
		if (!tc)
		{
			continue;
		}
		gizmoPosition += tc->position;
		gizmoRotation *= tc->rotation;
		gizmoScale += tc->scale;
	}
	gizmoRotation.Normalize();
	gizmoScale /= selectedUuids.size();
	gizmoTransform_ =
		DirectX::SimpleMath::Matrix::CreateScale(gizmoScale) *
		DirectX::SimpleMath::Matrix::CreateFromQuaternion(gizmoRotation) *
		DirectX::SimpleMath::Matrix::CreateTranslation(gizmoPosition);
	DirectX::SimpleMath::Matrix invGizmoTransform = gizmoTransform_.Invert();
	gizmoOffsets_.clear();
	for (const auto& uuid : selectedUuids)
	{
		auto info = registry_.try_get<ObjectInfoComponent>(uuidEntityList[uuid]);
		auto tc = registry_.try_get<TransformComponent>(uuidEntityList[uuid]);
		if (!tc || !info)
		{
			continue;
		}
		gizmoOffsets_.insert({uuid, invGizmoTransform});
		if (info->parentId == EmptyIdString)
		{
			gizmoOffsets_[uuid] *=
				(DirectX::SimpleMath::Matrix::CreateScale(tc->scale) *
				DirectX::SimpleMath::Matrix::CreateFromQuaternion(
					tc->rotation) *
				DirectX::SimpleMath::Matrix::CreateTranslation(tc->position));
		}
		else
		{
			gizmoOffsets_[uuid] *=
				(DirectX::SimpleMath::Matrix::CreateScale(tc->relativeScale) *
				DirectX::SimpleMath::Matrix::CreateFromQuaternion(
					tc->relativeRotation) *
				DirectX::SimpleMath::Matrix::CreateTranslation(tc->relativePosition));
		}
	}
}

void D3E::Game::OnObjectInfoConstruct(entt::registry& registry,
                                      entt::entity entity)
{
	uuidEntityList.insert(
		{registry.get<ObjectInfoComponent>(entity).id, entity});
}

void D3E::Game::OnObjectInfoDestroy(entt::registry& registry,
                                    entt::entity entity)
{
	uuidEntityList.erase(registry.get<ObjectInfoComponent>(entity).id);
}

DirectX::SimpleMath::Matrix& D3E::Game::GetGizmoTransform()
{
	return gizmoTransform_;
}

const DirectX::SimpleMath::Matrix&
D3E::Game::GetGizmoOffset(const D3E::String& uuid) const
{
	if (gizmoOffsets_.find(uuid) == gizmoOffsets_.end())
	{
		return DirectX::SimpleMath::Matrix::Identity;
	}
	return gizmoOffsets_.at(uuid);
}

void D3E::Game::BuildNavmesh(entt::entity e)
{
	auto& nc = registry_.get<NavmeshComponent>(e);
	auto& tc = registry_.get<TransformComponent>(e);
	auto& smc = registry_.get<StaticMeshComponent>(e);
	auto& meshData = MeshFactory::GetMeshData(smc.meshUuid);

	eastl::vector<float> vertices;
	eastl::vector<int> indices;

	ConvertVertices(meshData.points, vertices, tc);
	ConvertIndices(meshData.indices, indices);

	eastl::unique_ptr<NavmeshBuilder> nb =
		eastl::make_unique<NavmeshBuilder>(&vertices, &indices);

	auto r = nb->Build(nc);
	if (!r)
	{
		Debug::LogError("[Game] : BuildNavmesh(): Navmesh was not built");
	}
}

void D3E::Game::OnEditorPlayPressed()
{
	if (!isGameRunning_)
	{
		ComponentFactory::SerializeWorld(currentMapSavedState);
		selectedUuids.clear();
		EditorUtilsRenderSystem::isSelectionDirty = true;

		AssetManager::Get().LoadScripts("assets/");
		ScriptingEngine::GetInstance().Init(this);
		ScriptingEngine::GetInstance().InitScripts();
		ScriptingEngine::GetInstance().StartScripts();

		isGameRunning_ = true;
		for (auto& sys : systems_)
		{
			sys->Play(registry_, this);
		}

		EngineState::currentPlayer = FindFirstNonEditorPlayer();

		//physicsInfo_->setIsPaused(false);
	}
}

void D3E::Game::OnEditorPausePressed()
{
	if (isGameRunning_)
	{
		isGamePaused_ = !isGamePaused_;
		for (auto& sys : systems_)
		{
			sys->Pause(registry_, this);
		}
		// physicsInfo_->setIsPaused(!(physicsInfo_->getIsPaused()));
	}
}

void D3E::Game::OnEditorStopPressed()
{
	if (isGameRunning_)
	{
		isGameRunning_ = false;
		isGamePaused_ = false;
		for (auto& sys : systems_)
		{
			sys->Stop(registry_, this);
		}
		ClearWorld();
		ComponentFactory::ResolveWorld(currentMapSavedState);
		ScriptingEngine::GetInstance().Clear();
		// physicsInfo_->setIsPaused(true);

		for (auto& sys : systems_)
		{
			sys->StopReset(registry_, this);
		}
		EngineState::currentPlayer = editorFakePlayer_;
	}
}

void D3E::Game::ClearWorld()
{
	RenderUtils::InvalidateWorldBuffers(registry_);
	EditorIdManager::Get()->UnregisterAll();
	uuidEntityList.clear();
	registry_.clear();
	//CreationSystems::CreateSkybox(registry_);
#ifdef D3E_WITH_EDITOR
	CreationSystems::CreateEditorDebugRender(registry_);
	editorFakePlayer_ = CreationSystems::CreateEditorFakePlayer(registry_);
#endif
}

HRESULT D3E::Game::AssetFileImport(String currentDir)
{
	// CoCreate the File Open Dialog object.
	IFileDialog* pfd = NULL;
	HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL,
	                              CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
	if (SUCCEEDED(hr))
	{
		// Create an event handling object, and hook it up to the dialog.
		IFileDialogEvents* pfde = NULL;
		hr = CDialogEventHandler_CreateInstance(IID_PPV_ARGS(&pfde));
		if (SUCCEEDED(hr))
		{
			// Hook up the event handler.
			DWORD dwCookie;
			hr = pfd->Advise(pfde, &dwCookie);
			if (SUCCEEDED(hr))
			{
				// Set the options on the dialog.
				DWORD dwFlags;

				// Before setting, always get the options first in order not to
				// override existing options.
				hr = pfd->GetOptions(&dwFlags);
				if (SUCCEEDED(hr))
				{
					// In this case, get shell items only for file system items.
					hr = pfd->SetOptions(dwFlags | FOS_FORCEFILESYSTEM);
					if (SUCCEEDED(hr))
					{
						// Set the file types to display only. Notice that, this
						// is a 1-based array.
						hr = pfd->SetFileTypes(ARRAYSIZE(c_rgSaveTypes),
						                       c_rgSaveTypes);
						if (SUCCEEDED(hr))
						{
							// Set the selected file type index to Word Docs for
							// this example.
							hr = pfd->SetFileTypeIndex(INDEX_TEXTURE);
							if (SUCCEEDED(hr))
							{
								// Set the default extension to be ".doc" file.
								hr = pfd->SetDefaultExtension(L"doc");
								if (SUCCEEDED(hr))
								{
									// Show the dialog
									hr = pfd->Show(NULL);
									if (SUCCEEDED(hr))
									{
										// Obtain the result, once the user
										// clicks the 'Open' button. The result
										// is an IShellItem object.
										IShellItem* psiResult;
										hr = pfd->GetResult(&psiResult);
										if (SUCCEEDED(hr))
										{
											PWSTR pszFilePath = NULL;
											hr = psiResult->GetDisplayName(
												SIGDN_FILESYSPATH,
												&pszFilePath);
											if (SUCCEEDED(hr))
											{
												auto sourcePath =
													std::filesystem::path(
														pszFilePath);
												auto destinationPath =
													std::filesystem::path(
														currentDir.c_str() /
														sourcePath.filename());
												auto workingDirPath = std::
													filesystem::current_path();
												auto relDestinationPath =
													std::filesystem::relative(
														destinationPath,
														workingDirPath);
												std::filesystem::copy(
													sourcePath, destinationPath,
													std::filesystem::
														copy_options::
															overwrite_existing);
												if (AssetManager::
												        IsExtensionTexture(
															sourcePath
																.extension()
																.string()))
												{
													AssetManager::Get()
														.CreateTexture(
															sourcePath.stem()
																.string()
																.c_str(),
															relDestinationPath
																.string()
																.c_str(),
															GetRender()
																->GetDevice(),
															GetRender()
																->GetCommandList());
												}
												else if (
													AssetManager::
														IsExtensionModel(
															sourcePath
																.extension()
																.string()))
												{
													AssetManager::Get().CreateMesh(
														sourcePath.stem()
															.string()
															.c_str(),
														relDestinationPath
															.string()
															.c_str(),
														GetRender()
															->GetDevice(),
														GetRender()
															->GetCommandList());
												}
												else if (
													AssetManager::
														IsExtensionSound(
															sourcePath
																.extension()
																.string()))
												{
													AssetManager::Get()
														.CreateSound(
															sourcePath.stem()
																.string()
																.c_str(),
															relDestinationPath
																.string()
																.c_str());
												}
												CoTaskMemFree(pszFilePath);
											}
											psiResult->Release();
										}
									}
								}
							}
						}
					}
				}
				// Unhook the event handler.
				pfd->Unadvise(dwCookie);
			}
			pfde->Release();
		}
		pfd->Release();
	}
	return hr;
}

void D3E::Game::AssetDeleteDialog(D3E::String filename)
{
	int res = 0;
	TaskDialog(NULL, NULL, L"Delete confirm",
	           L"Are you sure you want to delete this item?", NULL,
	           TDCBF_YES_BUTTON | TDCBF_NO_BUTTON, TD_INFORMATION_ICON, &res);

	if (res == IDYES)
	{
		if (is_directory(std::filesystem::path(filename.c_str())))
		{
			std::filesystem::remove_all(
				std::filesystem::path(filename.c_str()));
		}
		AssetManager::Get().DeleteAsset(filename);
	}
}

bool D3E::Game::FindEntityByID(entt::entity& entity, const D3E::String& uuid)
{
	auto foundElement = uuidEntityList.find(uuid);
	if (foundElement == uuidEntityList.end())
	{
		return false;
	}
	else
	{
		entity = foundElement->second;
		return true;
	}
}

void D3E::Game::OnEditorSaveMapPressed()
{
	if (!isGameRunning_)
	{
		ComponentFactory::SerializeWorld(currentMapSavedState);
		if (currentMapSavedState.at("uuid") == D3E::EmptyIdStdStr)
		{
			currentMapSavedState.at("uuid") = UuidGenerator::NewGuidStdStr();
		}
		if (!currentMapSavedState.contains("filename"))
		{
			currentMapSavedState.emplace("filename", "NewWorld");
		}

		std::string filename = currentMapSavedState.at("filename");

		std::ofstream o(contentBrowserFilePath_ + "\\" + filename + ".meta");
		o << std::setw(4) << currentMapSavedState << std::endl;
		o.close();
	}
}

void D3E::Game::SetContentBrowserFilePath(const std::string& s)
{
	contentBrowserFilePath_ = s;
}

void D3E::Game::OnSaveSelectedToPrefabPressed()
{
	if (selectedUuids.size() != 1)
	{
		return;
	}

	json j;
	ComponentFactory::SerializeEntity(uuidEntityList[*selectedUuids.begin()], j,
	                                  false);
	j.emplace("uuid", UuidGenerator::NewGuidStdStr());
	std::filesystem::path filepath =
		std::filesystem::path(GetContentBrowserFilePath()) / "NewPrefab.meta";
	std::ofstream f(filepath);
	f << std::setw(4) << j << std::endl;
	f.close();

	AssetManager::Get().LoadPrefab(j, filepath.string());
}

void D3E::Game::DestroyEntity(const D3E::String& uuid)
{
	if (uuidEntityList.find(uuid) == uuidEntityList.end())
	{
		return;
	}

	registry_.destroy(uuidEntityList[uuid]);
}

void D3E::Game::OnObjectClicked(entt::entity entity)
{
	if (!isGameRunning_)
	{
		return;
	}

	if (auto scriptComponent = registry_.try_get<ScriptComponent>(entity))
	{
		scriptComponent->OnClicked(entity);
	}
}

entt::entity D3E::Game::FindFirstNonEditorPlayer()
{
	auto playerView = registry_.view<const ObjectInfoComponent, const TransformComponent, const CameraComponent>();
	for (auto [player, info, transform, camera] : playerView.each())
	{
		if (!info.internalObject)
		{
			return player;
		}
	}
	return entt::null;
}

void D3E::Game::ParentEntitiesById(const D3E::String& childUuid,
                                   const D3E::String& parentUuid)
{
	entt::entity child, parent;
	if (!FindEntityByID(child, childUuid) || !FindEntityByID(parent, parentUuid))
	{
		return;
	}

	auto* childInfo = registry_.try_get<ObjectInfoComponent>(child);
	auto* childTransform = registry_.try_get<TransformComponent>(child);
	auto* parentTransform = registry_.try_get<TransformComponent>(parent);

	if (!childInfo || !childTransform || !parentTransform)
	{
		return;
	}

	childInfo->parentId = parentUuid;

	auto parentMatrix = DirectX::SimpleMath::Matrix::CreateScale(parentTransform->scale) *
	                    DirectX::SimpleMath::Matrix::CreateFromQuaternion(parentTransform->rotation) *
	                    DirectX::SimpleMath::Matrix::CreateTranslation(parentTransform->position);
	auto childMatrix = DirectX::SimpleMath::Matrix::CreateScale(childTransform->scale) *
	                    DirectX::SimpleMath::Matrix::CreateFromQuaternion(childTransform->rotation) *
	                    DirectX::SimpleMath::Matrix::CreateTranslation(childTransform->position);

	auto relChildMatrix = parentMatrix.Invert() * childMatrix;

	relChildMatrix.Decompose(childTransform->relativeScale, childTransform->relativeRotation, childTransform->relativePosition);

	childTransform->relativePosition.x *= 1.0f / parentTransform->scale.x;
	childTransform->relativePosition.y *= 1.0f / parentTransform->scale.y;
	childTransform->relativePosition.z *= 1.0f / parentTransform->scale.z;

	SignalParentingChange(childUuid, parentUuid);

	registry_.patch<TransformComponent>(child);

	FlushChildTransformSync();
}

void D3E::Game::FlushChildTransformSync()
{
	childTransformSyncSystem->Update(registry_, this, 0.0f);
}

void D3E::Game::SignalParentingChange(const D3E::String& entityUuid,
                                      const D3E::String& prevParent)
{
	if (selectedUuids.find(entityUuid) != selectedUuids.end())
	{
		CalculateGizmoTransformsOffsets();
	}

	entt::entity e;
	if (FindEntityByID(e, entityUuid))
	{
		childTransformSyncSystem->OnParentUpdate(registry_, e, prevParent);
	}
}

void D3E::Game::UnparentEntityById(const D3E::String& childUuid)
{
	entt::entity child;
	if (!FindEntityByID(child, childUuid))
	{
		return;
	}

	auto* childInfo = registry_.try_get<ObjectInfoComponent>(child);
	auto* childTransform = registry_.try_get<TransformComponent>(child);

	auto prevParent = childInfo->parentId;
	childInfo->parentId = EmptyIdString;

	childTransform->relativePosition = Vector3(0, 0, 0);
	childTransform->relativeRotation = Quaternion::Identity;
	childTransform->relativeScale = Vector3(1, 1, 1);

	SignalParentingChange(childUuid, prevParent);

	registry_.patch<TransformComponent>(child);

	FlushChildTransformSync();
}
