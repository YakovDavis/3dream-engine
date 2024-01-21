#pragma once

#include "Recast.h"

#include <cstdint>

namespace D3E
{
	class Context : public rcContext
	{
		static const int kMaxMessages = 1000;
		static const int kTextPoolSize = 8000;

	public:
		Context();

		int GetLogCount() const;
		const char* GetLogText(const int i) const;

	protected:
		virtual void doLog(const rcLogCategory category, const char* msg,
		                   const int len) override;
		virtual void doResetLog() override;
		virtual void doResetTimers() override;
		virtual void doStartTimer(const rcTimerLabel label) override;
		virtual void doStopTimer(const rcTimerLabel label) override;
		virtual int
		doGetAccumulatedTime(const rcTimerLabel label) const override;

		int messageCount_;
		int textPoolSize_;
		const char* messages_[kMaxMessages];
		char textPool_[kTextPoolSize];
		int64_t startTime_[RC_MAX_TIMERS];
		int64_t accTime_[RC_MAX_TIMERS];
	};
} // namespace D3E