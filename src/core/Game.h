#pragma once

#include <vector>

struct Game
{
	void Init();

	void Update();

	void Draw();

	void DestroyResources();

	Game() = default;

	virtual ~Game() = default;
};
