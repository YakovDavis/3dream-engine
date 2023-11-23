#include "D3E/TimerManager.h"

using namespace D3E;

// Public members

void TimerManager::Tick(float dT)
{
	time_ += dT;
}

void TimerManager::SetTimer(TimerHandle& handle, float rate, bool looping,
                            float firstDelay)
{
}

void TimerManager::ClearTimer(TimerHandle& handle)
{
	if (FindTimer(handle))
	{
		// TODO(Denis): Internal clear?
	}

	handle.Invalidate();
}

void TimerManager::PauseTimer(TimerHandle& handle)
{
}

void TimerManager::UnPauseTimer(TimerHandle& handle)
{
}

float TimerManager::GetTimerRate(TimerHandle& handle) const
{
	auto timer = FindTimer(handle);

	if (!timer)
	{
		return -1.f;
	}

	return timer->rate_;
}

bool TimerManager::IsTimerActive(TimerHandle& handle) const
{
	auto timer = FindTimer(handle);

	return timer && timer->state_ != TimerState::Paused;
}

bool TimerManager::IsTimerPaused(TimerHandle& handle) const
{
	auto timer = FindTimer(handle);

	return timer && timer->state_ == TimerState::Paused;
}

bool TimerManager::TimerExists(TimerHandle& handle) const
{
	return FindTimer(handle);
}

float TimerManager::GetTimerElapsed(TimerHandle& handle) const
{
	auto timer = FindTimer(handle);

	if (!timer)
	{
		return -1.f;
	}

	switch (timer->state_)
	{
		case TimerState::Active:
			return timer->rate_ -
			       (timer->expireTime_ - time_); // TODO(Denis): double check
		default:
			return timer->rate_ - timer->expireTime_;
	}
}

float TimerManager::GetTimerRemaining(TimerHandle& handle) const
{
	auto timer = FindTimer(handle);

	if (!timer)
	{
		return -1.f;
	}

	switch (timer->state_)
	{
		case TimerState::Active:
			return timer->expireTime_ - time_; // TODO(Denis): double check x2
		default:
			return timer->expireTime_;
	}
}

// Private members

Timer* TimerManager::FindTimer(const TimerHandle& handle)
{
	if (!handle.IsValid())
	{
		return nullptr;
	}

	if (timers_.find(handle) == timers_.end())
	{
		return nullptr;
	}

	auto& timer = timers_[handle];

	if (timer.state_ == TimerState::PendingRemoval)
	{
		return nullptr;
	}

	return &timer;
}

Timer const* TimerManager::FindTimer(const TimerHandle& handle) const
{
	return const_cast<TimerManager*>(this)->FindTimer(handle);
}
