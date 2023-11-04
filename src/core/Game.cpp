#include "Game.h"
#include "EASTL/chrono.h"
#include "render/GameRenderD3D12.h"
#include "Debug.h"

void D3E::Game::Run()
{
	App::Run();

	Init();

	auto* prevCycleTimePoint = reinterpret_cast<eastl::chrono::time_point<eastl::chrono::steady_clock>*>(prevCycleTimePoint_);

	*prevCycleTimePoint = eastl::chrono::steady_clock::now();

	while (!isQuitRequested_)
	{
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
	assert(mhAppInst != 0);
	gameRender_ = new GameRenderD3D12(mhAppInst);
	Debug::ClearLog();
	//gameRender_->Init();
}

void D3E::Game::Update(const float deltaTime)
{
	Debug::LogWarning("frame");
}

void D3E::Game::Draw()
{
}

void D3E::Game::DestroyResources()
{
	Debug::CloseLog();
}

D3E::Game::Game()
{
	prevCycleTimePoint_ = new eastl::chrono::time_point<eastl::chrono::steady_clock>(eastl::chrono::steady_clock::now());
}
