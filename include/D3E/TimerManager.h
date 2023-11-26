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
		TimerManager(const TimerManager&) = delete;
		TimerManager& operator=(const TimerManager&) = delete;

		static TimerManager& GetInstance();

		void Init(Game* gameInstance);
		void Update(float dT);
		void SetTimer(TimerHandle& handle, float rate, bool looping = false,
		              float firstDelay = -.1f);
		void SetTimer(TimerHandle& handle, FunctionDelegate delegate,
		              float rate, bool looping = false,
		              float firstDelay = -.1f);
		template<typename T>
		void SetTimer(TimerHandle& handle, T* object, void (T::*delegate)(),
		              float rate, bool looping = false, float firstDelay = -.1f)
		{
			SetTimerInternal(handle, TimerDelegate(), rate, looping,
			                 firstDelay); // TODO(Denis): Find a way to store
			                              // member delegate in TimerDelegate
		};

		// TODO(Denis): implement SetTimerForNextTick
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
		double managerTime_;
		size_t lastTickedFrame_;
		Game* game_;
		std::mt19937_64 gen_;
		TimerHandle executingTimerHandle_;

		eastl::unordered_map<TimerHandle, Timer, TimerHandleHash> timers_;
		eastl::unordered_set<TimerHandle, TimerHandleHash> pendingTimers_;
		eastl::unordered_set<TimerHandle, TimerHandleHash> pausedTimers_;
		eastl::unordered_set<TimerHandle, TimerHandleHash> activeTimers_;

		TimerManager();

		void SetTimerInternal(TimerHandle& handle, TimerDelegate&& delegate,
		                      float rate, bool looping, float firstDelay);
		void SetTimerForNextTickInternal();
		Timer* FindTimer(const TimerHandle& handle);
		Timer const* FindTimer(const TimerHandle& handle) const;
		void RemoveTimer(const TimerHandle& handle);
		bool TickedThisFrame() const;
		TimerHandle AddTimer(Timer& timer);
		void ProcessPendingTimersInternal();
		void ProcessActiveTimersInternal();
	};
} // namespace D3E