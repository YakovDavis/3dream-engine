#pragma once

#include "EASTL/unordered_map.h"
#include "EASTL/unordered_set.h"
#include "Game.h"
#include "Timer.h"
#include "TimerHandle.h"

#include <random>

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
		double time_;
		size_t lastTickedFrame_;
		Game* game_;
		std::mt19937_64 gen_;

		eastl::unordered_map<TimerHandle, Timer, TimerHandleHash> timers_;
		eastl::unordered_set<TimerHandle, TimerHandleHash> pendingTimers_;
		eastl::unordered_set<TimerHandle, TimerHandleHash> pausedTimers_;
		eastl::unordered_set<TimerHandle, TimerHandleHash> activeTimers_;

		TimerManager();
		TimerManager(const TimerManager&) = delete;

		TimerManager& operator=(const TimerManager&) = delete;

		Timer* FindTimer(const TimerHandle& handle);
		Timer const* FindTimer(const TimerHandle& handle) const;
		void RemoveTimer(const TimerHandle& handle);
		bool TickedThisFrame() const;
		TimerHandle AddTimer(Timer& timer);
	};
} // namespace D3E