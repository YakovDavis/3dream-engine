#include "D3E/Timer.h"

using namespace D3E;

Timer::Timer()
	: timerRate_(0.f), active_(true), looping_(false), expireTime_(0.f)
{
}