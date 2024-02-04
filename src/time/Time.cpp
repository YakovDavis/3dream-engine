#include "D3E/time/Time.h"

#include "D3E/Game.h"

using namespace D3E;

Time& Time::GetInstance()
{
	static Time instance;

	return instance;
}

float Time::DeltaTime()
{
	return Time::GetInstance().GetDeltaTime();
}

void Time::Init(Game* g)
{
	game_ = g;
}

float Time::GetDeltaTime() const
{
	return game_->GetDeltaTime() / 1000.0f;
}

Time::Time() : game_(nullptr)
{
}