#include "PerformanceTimer.h"

#include "windows.h"

int64_t D3E::GetPerfTime()
{
	int64_t count = 0;

	QueryPerformanceCounter((LARGE_INTEGER*)&count);

	return count;
}

int D3E::GetPerfTimeUsec(const int64_t duration)
{
	static int64_t freq = 0;

	if (freq == 0)
		QueryPerformanceFrequency((LARGE_INTEGER*)&freq);

	return (int)(duration * 1000000 / freq);
}