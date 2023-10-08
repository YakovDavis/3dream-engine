#include "Game.h"

void D3E::Game::Run()
{
	App::Run();

	Init();

	prevCycleTimePoint_ = eastl::chrono::steady_clock::now();

	while (!isQuitRequested_)
	{
		float deltaTime = 0;

		{
			using namespace eastl::chrono;
			deltaTime = duration_cast<duration<float, milliseconds::period>>(steady_clock::now() - prevCycleTimePoint_).count();
		}

		Update(deltaTime);

		prevCycleTimePoint_ = eastl::chrono::steady_clock::now();

		Draw();
	}

	DestroyResources();
}

void D3E::Game::Init()
{
}

void D3E::Game::Update(const float deltaTime)
{

}

void D3E::Game::Draw()
{
}

void D3E::Game::DestroyResources()
{
}
