#include <TimeUtil.h>
#ifdef WINDOWS
#include <ELAWindows.h>
#else
#include <unistd.h>
#endif

namespace TimeUtil
{
	int TimeDifferenceInMinutes(time_t time1, time_t time2)
	{
		//time1 lastSysLogTime
		//time2 currSysLogTime
		return (int)((difftime(time2, time1) / 60.0));
	}

	void SleepSeconds(int time)
	{
#ifdef WINDOWS
		Sleep(time * 1000);
#else
		sleep(time);
#endif
	}
}
