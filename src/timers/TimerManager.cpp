#include "D3E/TimerManager.h"

#include "D3E/Debug.h"

#include <format>

using namespace D3E;

// Public members

void Foo()
{
}

void TimerManager::Tick(float dT)
{
	time_ += dT;
	lastTickedFrame_ = game_->GetFrameCount();
}

void TimerManager::SetTimer(TimerHandle& handle, float rate, bool looping,
                            float firstDelay)
{
	if (FindTimer(handle))
	{
		ClearTimer(handle);
	}

	if (rate < 0.f)
	{
		handle.Invalidate();
		return;
	}

	Timer newTimer;
	newTimer.rate_ = rate;
	newTimer.looping_ = looping;

	const auto delay = firstDelay >= 0.f ? firstDelay : rate;

	TimerHandle newTimerHandle;

	if (TickedThisFrame())
	{
		newTimer.expireTime_ = time_ + delay;
		newTimer.state_ = TimerState::Active;
		newTimerHandle = AddTimer(newTimer);
		activeTimers_.insert(newTimerHandle);
	}
	else
	{
		newTimer.expireTime_ = delay;
		newTimer.state_ = TimerState::Pending;
		newTimerHandle = AddTimer(newTimer);
		pendingTimers_.insert(newTimerHandle);
	}

	handle = newTimerHandle;
}

void TimerManager::ClearTimer(TimerHandle& handle)
{
	if (const auto timer = FindTimer(handle))
	{
		switch (timer->state_)
		{
			case TimerState::Pending:
			{
				pendingTimers_.erase(handle);
				RemoveTimer(handle);
				break;
			}
			case TimerState::Active:
			{
				timer->state_ = TimerState::PendingRemoval;
				break;
			}
			case TimerState::PendingRemoval:
			{
				break;
			}
			case TimerState::Paused:
			{
				pausedTimers_.erase(handle);
				RemoveTimer(handle);
				break;
			}
			default:
				Debug::LogError(
					"[TimerManager] : ClearTimer() : unknown timer state");
		}
	}

	handle.Invalidate();
}

void TimerManager::PauseTimer(TimerHandle& handle)
{
	auto timer = FindTimer(handle);

	if (!timer || timer->state_ == TimerState::Paused)
	{
		return;
	}

	auto previousState = timer->state_;

	switch (previousState)
	{
		case TimerState::PendingRemoval:
		{
			break;
		}
		case TimerState::Active:
		{
			activeTimers_.erase(handle);
			break;
		}
		case TimerState::Pending:
		{
			pendingTimers_.erase(handle);
			break;
		}
	}

	pausedTimers_.insert(handle);
	timer->state_ = TimerState::Paused;

	if (previousState != TimerState::Pending)
	{
		timer->expireTime_ -= time_;
	}
}

void TimerManager::UnPauseTimer(TimerHandle& handle)
{
	auto timer = FindTimer(handle);

	if (!timer || timer->state_ != TimerState::Paused)
	{
		return;
	}

	if (TickedThisFrame())
	{
		timer->expireTime_ += time_;
		timer->state_ = TimerState::Active;
		activeTimers_.insert(handle);
	}
	else
	{
		timer->state_ = TimerState::Pending;
		pendingTimers_.insert(handle);
	}

	pausedTimers_.erase(handle);
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
			return timer->rate_ - (timer->expireTime_ - time_);
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
			return timer->expireTime_ - time_;
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

void TimerManager::RemoveTimer(const TimerHandle& handle)
{
	try
	{
		timers_.erase(handle);
	}
	catch (const std::exception& e)
	{
		Debug::LogError(
			std::format("[TimerManager] : RemoveTimer() : exception: {}",
		                e.what())
				.c_str());
		throw;
	}
}

bool TimerManager::TickedThisFrame() const
{
	return lastTickedFrame_ == game_->GetFrameCount();
}

TimerHandle TimerManager::AddTimer(Timer& timer)
{
	TimerHandle handle;
	handle.id_ = gen_(); // TODO(Denis): Refactor: Kinda bad idea, collisions may occur

	timers_.insert({handle, std::move(timer)});

	return handle;
}
