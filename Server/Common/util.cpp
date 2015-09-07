#include "stdafx.h"
#include "Util.h"
#include <time.h>

#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
struct tm* localtime_r(const time_t* time, struct tm *result)
{
	localtime_s(result, time);
	return result;
}
#endif