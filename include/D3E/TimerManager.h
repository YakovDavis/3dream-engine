#pragma once

#include "TimerHandle.h"

namespace D3E
{
	class TimerManager
	{
	public:
		void Tick(float dT);
		void SetTimer(TimerHandle& handle, float rate, bool looping = false,
		              float firstDelay = -.1f);
		void SetTimerForNextTick();
		void ClearTimer(TimerHandle& handle);
		void PauseTimer(TimerHandle& handle);
		void UnPauseTimer(TimerHandle& handle);
		float GetTimerRate(TimerHandle& handle) const;
		bool IsTimerActive(TimerHandle& handle) const;
		bool IsTimerPaused(TimerHandle& handle) const;
		bool TimerExists(TimerHandle& handle) const;
		float GetTimerElapsed(TimerHandle& handle) const;
		float GetTimerRemaining(TimerHandle& handle) const;
	private:
		TimerManager();
		TimerManager(const TimerManager&) = delete;

		TimerManager& operator=(const TimerManager&) = delete;
	};
} // namespace D3E