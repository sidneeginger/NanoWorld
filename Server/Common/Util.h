#pragma once


#include <algorithm>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <stdarg.h>
#include <cstring>

struct tm* localtime_r(const time_t* time, struct tm *result);