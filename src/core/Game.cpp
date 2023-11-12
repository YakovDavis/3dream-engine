#include "D3E/Game.h"
#include "EASTL/chrono.h"
#include "render/GameRenderD3D12.h"
#include "D3E/Debug.h"
#include "render/DisplayWin32.h"
#include "D3E/Components/TransformComponent.h"
#include "editor/EditorUtils.h"
#include "D3E/systems/CreationSystems.h"

void D3E::Game::Run()
{
	App::Run();

	Init();

	auto* prevCycleTimePoint = reinterpret_cast<eastl::chrono::time_point<eastl::chrono::steady_clock>*>(prevCycleTimePoint_);

	*prevCycleTimePoint = eastl::chrono::steady_clock::now();

	while (!isQuitRequested_)
	{
		HandleMessages();

		float deltaTime = 0;

		{
			using namespace eastl::chrono;
			deltaTime = duration_cast<duration<float, milliseconds::period>>(steady_clock::now() - *prevCycleTimePoint).count();
		}

		Update(deltaTime);

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
	gameRender_->Init();

	CreationSystems::CreateCubeSM(registry_);
}

void D3E::Game::Update(const float deltaTime)
{
	auto view = registry_.view<const TransformComponent>();
	view.each([](const auto entity, const auto &transform) { /* ... */ });
	for(auto [entity, transform]: view.each()) {
		// ...
	}
}

void D3E::Game::Draw()
{
	gameRender_->PrepareDraw(registry_);
	gameRender_->Draw(registry_);
	gameRender_->EndDraw(registry_);

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
