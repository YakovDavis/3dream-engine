#pragma once

#include "D3E/TimerHandle.h"

using namespace D3E;

TimerHandle::TimerHandle() : id_(0)
{
}

bool TimerHandle::IsValid() const
{
	return id_ != 0;
}

void TimerHandle::Invalidate()
{
	id_ = 0;
}

bool TimerHandle::operator==(const TimerHandle& other) const
{
	return id_ == other.id_;
}

bool TimerHandle::operator!=(const TimerHandle& other) const
{
	return !(*this == other);
}
