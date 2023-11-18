#include "D3E/Game.h"

#include "D3E/CommonCpp.h"
#include "D3E/Components/TransformComponent.h"
#include "D3E/Debug.h"
#include "D3E/systems/CreationSystems.h"
#include "EASTL/chrono.h"
#include "editor/EditorUtils.h"
#include "engine/systems/FPSControllerSystem.h"
#include "imgui.h"
#include "input/InputDevice.h"
#include "render/DisplayWin32.h"
#include "render/GameRenderD3D12.h"
#include "render/systems/StaticMeshInitSystem.h"
#include "render/systems/StaticMeshRenderSystem.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void D3E::Game::Run()
{
	App::Run();

	Init();

	auto* prevCycleTimePoint = reinterpret_cast<eastl::chrono::time_point<eastl::chrono::steady_clock>*>(prevCycleTimePoint_);

	*prevCycleTimePoint = eastl::chrono::steady_clock::now();

	while (!isQuitRequested_)
	{
		HandleMessages();

		{
			using namespace eastl::chrono;
			deltaTime_ = duration_cast<duration<float, milliseconds::period>>(steady_clock::now() - *prevCycleTimePoint).count();
		}

		Update(deltaTime_);

		*prevCycleTimePoint = eastl::chrono::steady_clock::now();

		Draw();
	}

	DestroyResources();
}

void D3E::Game::Init()
{
	assert(mhAppInst != nullptr);
	Debug::ClearLog();
	gameRender_ = new GameRenderD3D12(this, mhAppInst);
	gameRender_->Init(systems_);

	inputDevice_ = new InputDevice(this);

	systems_.push_back(new StaticMeshInitSystem);
	systems_.push_back(new StaticMeshRenderSystem);
	systems_.push_back(new D3E::FPSControllerSystem);

	for (auto& sys : systems_)
	{
		sys->InitRender();
	}
}

void D3E::Game::Update(const float deltaTime)
{
	for (auto& sys : systems_)
	{
		sys->Update(registry_, this, deltaTime);
	}

	auto view = registry_.view<const TransformComponent>();
	view.each([](const auto entity, const auto &transform) { /* ... */ });
	for(auto [entity, transform]: view.each()) {
		// ...
	}

	gameRender_->UpdateAnimations(deltaTime);

	inputDevice_->EndTick();
}

void D3E::Game::Draw()
{
	gameRender_->PrepareDraw(registry_, systems_);
	gameRender_->Draw(registry_, systems_);
	gameRender_->EndDraw(registry_, systems_);

	gameRender_->Present();

	gameRender_->GetDevice()->runGarbageCollection();
}

void D3E::Game::DestroyResources()
{
	Debug::CloseLog();
}

D3E::Game::Game()
{
	EditorUtils::Initialize(this);
	prevCycleTimePoint_ = new eastl::chrono::time_point<eastl::chrono::steady_clock>(eastl::chrono::steady_clock::now());
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
			if (static_cast<unsigned int>(wParam) == 27) PostQuitMessage(0);
			return 0;
		}
		case WM_INPUT:
		{
			UINT dwSize = 0;
			GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, nullptr, &dwSize, sizeof(RAWINPUTHEADER));  // NOLINT(performance-no-int-to-ptr)
			const auto lpb = new BYTE[dwSize];
			if (lpb == nullptr) {
				return 0;
			}

			if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)  // NOLINT(performance-no-int-to-ptr)
				OutputDebugString(TEXT("GetRawInputData does not return correct size !\n"));

			const auto* raw = reinterpret_cast<RAWINPUT*>(lpb);


			if (raw->header.dwType == RIM_TYPEKEYBOARD)
			{
				inputDevice_->OnKeyDown({
					raw->data.keyboard.MakeCode,
					raw->data.keyboard.Flags,
					raw->data.keyboard.VKey,
					raw->data.keyboard.Message
				});
			}
			else if (raw->header.dwType == RIM_TYPEMOUSE)
			{
				inputDevice_->OnMouseMove({
					raw->data.mouse.usFlags,
					raw->data.mouse.usButtonFlags,
					static_cast<int>(raw->data.mouse.ulExtraInformation),
					static_cast<int>(raw->data.mouse.ulRawButtons),
					static_cast<short>(raw->data.mouse.usButtonData),
					raw->data.mouse.lLastX,
					raw->data.mouse.lLastY
				});
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

void D3E::Game::LoadTexture(const String& name,
                            const String& fileName)
{
	gameRender_->LoadTexture(name, fileName);
}
