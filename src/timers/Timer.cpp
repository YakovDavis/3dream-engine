#include "D3E/Timer.h"

using namespace D3E;

Timer::Timer()
	: rate_(0.f), state_(TimerState::Active), looping_(false), expireTime_(0.f)
{
}