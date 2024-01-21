#include "Context.h"

#include "PerformanceTimer.h"

#include <cstdint>
#include <cstring>

using namespace D3E;

Context::Context() : messageCount_(0), textPoolSize_(0)
{
	std::memset(messages_, 0, sizeof(char*) * kMaxMessages);

	doResetTimers();
}

void Context::doLog(const rcLogCategory category, const char* msg, const int len)
{
	if (!len)
		return;

	if (messageCount_ >= kMaxMessages)
		return;

	char* dst = &textPool_[textPoolSize_];
	int n = kTextPoolSize - textPoolSize_;

	if (n < 2)
		return;

	char* cat = dst;
	char* text = dst + 1;
	const int maxText = n - 1;

	*cat = (char)category;

	const int count = rcMin(len + 1, maxText);
	memcpy(text, msg, count);
	text[count - 1] = '\0';
	textPoolSize_ += 1 + count;
	messages_[messageCount_++] = dst;
}

void Context::doResetLog()
{
	messageCount_ = 0;
	textPoolSize_ = 0;
}

void Context::doResetTimers()
{
	for (int i = 0; i < RC_MAX_TIMERS; ++i)
		accTime_[i] = -1;
}

void Context::doStartTimer(const rcTimerLabel label)
{
	startTime_[label] = GetPerfTime();
}

void Context::doStopTimer(const rcTimerLabel label)
{
	const int64_t endTime = GetPerfTime();
	const int64_t deltaTime = endTime - startTime_[label];

	if (accTime_[label] == -1)
		accTime_[label] = deltaTime;
	else
		accTime_[label] += deltaTime;
}

int Context::doGetAccumulatedTime(const rcTimerLabel label) const
{
	return GetPerfTimeUsec(accTime_[label]);
}

int Context::GetLogCount() const
{
	return messageCount_;
}

const char* Context::GetLogText(const int i) const
{
	return messages_[i] + 1;
}
