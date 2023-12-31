#include "D3E/TimerManager.h"

#include "D3E/Debug.h"

#include <format>

using namespace D3E;

// Public members

TimerManager& TimerManager::GetInstance()
{
	static TimerManager instance;

	return instance;
}

void TimerManager::Init(Game* gameInstance)
{
	game_ = gameInstance;
}

void TimerManager::Update(float dT)
{
	if (TickedThisFrame())
	{
		return;
	}

	// Increase timer manager clock
	managerTime_ += dT;

	ProcessActiveTimersInternal();

	// Tick
	lastTickedFrame_ = game_->GetFrameCount();

	ProcessPendingTimersInternal();
}

void TimerManager::SetTimer(TimerHandle& handle, float rate, bool looping,
                            float firstDelay)
{
	SetTimerInternal(handle, TimerDelegate(), rate, looping, firstDelay);
}

void TimerManager::SetTimer(TimerHandle& handle, FunctionDelegate delegate,
                            float rate, bool looping, float firstDelay)
{
	SetTimerInternal(handle, delegate, rate, looping, firstDelay);
}

TimerHandle TimerManager::SetTimerForNextTick(FunctionDelegate delegate)
{
	return SetTimerForNextTickInternal(delegate);
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
			case TimerState::Executing:
			{
				Debug::Assert(executingTimerHandle_ == handle,
				              "[TimerManager] : ClearTimer() : "
				              "executingTimerHandle_ != handle");

				executingTimerHandle_.Invalidate();
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
		case TimerState::Executing:
		{
			Debug::Assert(executingTimerHandle_ == handle,
			              "[TimerManager] : ClearTimer() : "
			              "executingTimerHandle_ != handle");

			executingTimerHandle_.Invalidate();
			break;
		}
	}

	pausedTimers_.insert(handle);
	timer->state_ = TimerState::Paused;

	if (previousState != TimerState::Pending)
	{
		timer->expireTime_ -= managerTime_;
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
		timer->expireTime_ += managerTime_;
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
		case TimerState::Executing:
			return timer->rate_ - (timer->expireTime_ - managerTime_);
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
			return timer->expireTime_ - managerTime_;
		case TimerState::Executing:
			return 0.f;
		default:
			return timer->expireTime_;
	}
}

// Private members

TimerManager::TimerManager()
	: game_(nullptr), lastTickedFrame_{0}, managerTime_{0}
{
}

void TimerManager::SetTimerInternal(TimerHandle& handle,
                                    TimerDelegate&& delegate, float rate,
                                    bool looping, float firstDelay)
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
	newTimer.delegate_ = std::move(delegate);

	const auto delay = firstDelay >= 0.f ? firstDelay : rate;

	TimerHandle newTimerHandle;

	if (TickedThisFrame())
	{
		newTimer.expireTime_ = managerTime_ + delay;
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

TimerHandle TimerManager::SetTimerForNextTickInternal(TimerDelegate&& delegate)
{
	Timer timer;
	timer.rate_ = 0.f;
	timer.looping_ = false;
	timer.delegate_ = std::move(delegate);
	timer.expireTime_ = managerTime_;
	timer.state_ = TimerState::Active;

	TimerHandle newTimerHandle = AddTimer(timer);
	activeTimers_.insert(newTimerHandle);

	return newTimerHandle;
}

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
	handle.id_ =
		gen_(); // TODO(Denis): Refactor: Kinda bad idea, collisions may occur

	timers_.insert({handle, std::move(timer)});

	return handle;
}

void TimerManager::ProcessPendingTimersInternal()
{
	if (pendingTimers_.empty())
	{
		return;
	}

	for (const auto& handle : pendingTimers_)
	{
		auto& timerToActivate = timers_[handle];

		timerToActivate.expireTime_ += managerTime_;
		timerToActivate.state_ = TimerState::Active;
		activeTimers_.insert(handle);
	}

	pendingTimers_.clear();
}

void TimerManager::ProcessActiveTimersInternal()
{
	for (auto it = activeTimers_.begin(); it != activeTimers_.end();)
	{
		// Make local copy to prevent invalidation
		TimerHandle handle = (*it);
		auto timer = &timers_[handle];

		if (timer->state_ == TimerState::PendingRemoval)
		{
			RemoveTimer(handle);
			it = activeTimers_.erase(it);

			continue;
		}

		if (managerTime_ > timer->expireTime_)
		{
			executingTimerHandle_ = handle;
			timer->state_ = TimerState::Executing;

			int callCount =
				timer->looping_
					? int((managerTime_ - timer->expireTime_) / timer->rate_) +
						  1
					: 1;

			for (int i = 0; i < callCount; ++i)
			{
				timer->delegate_.Execute();

				// Check if timer invalidated itself during delegate call
				if (!FindTimer(executingTimerHandle_))
				{
					break;
				}
			}

			if (timer)
			{
				if (timer->looping_)
				{
					timer->expireTime_ += callCount * timer->rate_;
					timer->state_ = TimerState::Active;
					++it;
				}
				else
				{
					RemoveTimer(handle);
					it = activeTimers_.erase(it);
				}

				executingTimerHandle_.Invalidate();
				continue;
			}
		}

		++it;
	}
}
