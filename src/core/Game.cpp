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
#include "assetmng/CDialogEventHandler.h"
#include "assetmng/DefaultAssetLoader.h"
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
#include "imgui.h"
#include "input/InputDevice.h"
#include "json.hpp"
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

#include <filesystem>
#include <thread>

static json currentMapSavedState = json({{"type", "world"}, {"id", D3E::EmptyIdStdStr}, {"entities", {}}});

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

	json j;
	ComponentFactory::SerializeWorld(j);
	std::ofstream o("DuckWorld.meta");
	o << std::setw(4) << j << std::endl;

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

	editorSystems_.push_back(new FPSControllerSystem);
	editorSystems_.push_back(new EditorUtilsRenderSystem);

	soundEngine_ = &SoundEngine::GetInstance();
	soundEngine_->Init();

	ClearWorld();

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
}

void D3E::Game::EditorDraw()
{
	gameRender_->PrepareFrame();
	gameRender_->BeginDraw(registry_, systems_);
	gameRender_->BeginDraw(registry_, renderPPsystems_);
	gameRender_->BeginDraw(registry_, editorSystems_);
	gameRender_->DrawOpaque(registry_, systems_);
	gameRender_->DrawPostProcess(registry_, renderPPsystems_);
	gameRender_->DrawPostProcess(registry_, editorSystems_);
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
	gameRender_->DrawPostProcess(registry_, renderPPsystems_);
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

void D3E::Game::OnEditorPlayPressed()
{
	if (!isGameRunning_)
	{
		ComponentFactory::SerializeWorld(currentMapSavedState);
		selectedUuids.clear();
		EditorUtilsRenderSystem::isSelectionDirty = true;
		ScriptingEngine::GetInstance().Start();
		isGameRunning_ = true;
	}
}

void D3E::Game::OnEditorPausePressed()
{
	if (isGameRunning_)
	{
		isGamePaused_ = !isGamePaused_;
	}
}

void D3E::Game::OnEditorStopPressed()
{
	if (isGameRunning_)
	{
		isGameRunning_ = false;
		isGamePaused_ = false;
		ClearWorld();
		ComponentFactory::ResolveWorld(currentMapSavedState);
	}
}

void D3E::Game::ClearWorld()
{
	RenderUtils::InvalidateWorldBuffers(registry_);
	EditorIdManager::Get()->UnregisterAll();	
	uuidEntityList.clear();
	registry_.clear();
#ifdef D3E_WITH_EDITOR
	CreationSystems::CreateEditorDebugRender(registry_);
#endif
}

HRESULT D3E::Game::AssetFileImport(String currentDir)
{
	// CoCreate the File Open Dialog object.
	IFileDialog *pfd = NULL;
	HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
	if (SUCCEEDED(hr))
	{
		// Create an event handling object, and hook it up to the dialog.
		IFileDialogEvents *pfde = NULL;
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

				// Before setting, always get the options first in order not to override existing options.
				hr = pfd->GetOptions(&dwFlags);
				if (SUCCEEDED(hr))
				{
					// In this case, get shell items only for file system items.
					hr = pfd->SetOptions(dwFlags | FOS_FORCEFILESYSTEM);
					if (SUCCEEDED(hr))
					{
						// Set the file types to display only. Notice that, this is a 1-based array.
						hr = pfd->SetFileTypes(ARRAYSIZE(c_rgSaveTypes), c_rgSaveTypes);
						if (SUCCEEDED(hr))
						{
							// Set the selected file type index to Word Docs for this example.
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
										// Obtain the result, once the user clicks the 'Open' button.
										// The result is an IShellItem object.
										IShellItem *psiResult;
										hr = pfd->GetResult(&psiResult);
										if (SUCCEEDED(hr))
										{
											PWSTR pszFilePath = NULL;
											hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
											if (SUCCEEDED(hr))
											{
												auto sourcePath = std::filesystem::path(pszFilePath);
												auto destinationPath = std::filesystem::path(currentDir.c_str() / sourcePath.filename());
												auto workingDirPath = std::filesystem::current_path();
												auto relDestinationPath = std::filesystem::relative(destinationPath, workingDirPath);
												std::filesystem::copy(sourcePath, destinationPath, std::filesystem::copy_options::overwrite_existing);
												if (AssetManager::IsExtensionTexture(sourcePath.extension().string()))
												{
													AssetManager::Get().CreateTexture(sourcePath.stem().string().c_str(), relDestinationPath.string().c_str(), GetRender()->GetDevice(), GetRender()->GetCommandList());
												}
												else if (AssetManager::IsExtensionModel(sourcePath.extension().string()))
												{
													AssetManager::Get().CreateMesh(sourcePath.stem().string().c_str(), relDestinationPath.string().c_str(), GetRender()->GetDevice(), GetRender()->GetCommandList());
												}
												else if (AssetManager::IsExtensionSound(sourcePath.extension().string()))
												{
													AssetManager::Get().CreateSound(sourcePath.stem().string().c_str(), relDestinationPath.string().c_str());
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
	TaskDialog(NULL,
	           NULL,
	           L"Delete confirm",
	           L"Are you sure you want to delete this item?",
	           NULL,
	           TDCBF_YES_BUTTON | TDCBF_NO_BUTTON,
	           TD_INFORMATION_ICON,
	           &res);

	if (res == IDYES)
	{
		if (is_directory(std::filesystem::path(filename.c_str())))
		{
			std::filesystem::remove_all(std::filesystem::path(filename.c_str()));
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
