#pragma once

#include <cstdint>
#include <functional>

namespace D3E
{
	using FunctionDelegate = std::function<void()>;

	struct TimerDelegate
	{
		FunctionDelegate functionDelegate;

		TimerDelegate(){};
		TimerDelegate(FunctionDelegate const& d) : functionDelegate(d){};

		inline void Execute() {}

		TimerDelegate(TimerDelegate&&) = default;
		TimerDelegate(TimerDelegate&) = delete;
		TimerDelegate& operator=(TimerDelegate&&) = default;
		TimerDelegate& operator=(TimerDelegate&) = delete;
	};

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
		TimerDelegate delegate_;
	};
} // namespace D3E