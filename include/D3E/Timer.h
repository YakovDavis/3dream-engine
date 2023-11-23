#pragma once

#include <chrono>

namespace D3E
{
	class Timer
	{
	public:
		Timer();

		Timer(Timer&&) = default;
		Timer(const Timer&) = delete;
		Timer& operator=(Timer&&) = default;
		Timer& operator=(const Timer&) = delete;

	private:
		float timerRate_;
		bool active_;
		bool looping_;
		double expireTime_;
	};
} // namespace D3E