#pragma once

#include "D3E/Game.h"

namespace D3E
{
	class Time
	{
	public:
		Time(const Time&) = delete;
		Time& operator=(const Time&) = delete;

		static Time& GetInstance();
		static float DeltaTime();

		void Init(Game* g);
		float GetDeltaTime() const;

	private:
		Time();

		Game* game_;
	};
} // namespace D3E