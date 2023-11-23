#pragma once

#include <cstdint>

namespace D3E
{
	enum class TimerState : uint8_t
	{
		Pending,
		Active,
		Paused,
		PendingRemoval,
		Executing
	};

	class Timer
	{
		friend class TimerManager;

	public:
		Timer();

		Timer(Timer&&) = default;
		Timer(const Timer&) = delete;
		Timer& operator=(Timer&&) = default;
		Timer& operator=(const Timer&) = delete;

	private:
		float rate_;
		TimerState state_;
		bool looping_;
		double expireTime_;
	};
} // namespace D3E