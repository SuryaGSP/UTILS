#pragma once

#include <time.h>

namespace TimeUtil
{
	int TimeDifferenceInMinutes(time_t time1, time_t time2 = time(nullptr));
	void SleepSeconds(int time);
}
