/*
 * frproxy.h
 *
 *  Created on: Mar 21, 2014
 *      Author: asdf
 */

#ifndef FRPROXY_H_
#define FRPROXY_H_

#ifndef DEBUG_
//#define DEBUG_

#endif

#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <string>
#include <iostream>
#include <stdint.h>
#include <stdio.h>

//#ifndef INT32T
//typedef int int32_t;
//#endif
//#ifndef UINT32T
//typedef unsigned int uint32_t;
//#endif
//#ifndef UINT64T
//typedef unsigned long uint64_t;
//#endif

using namespace std;

namespace FinagleRegistryProxy {

namespace utils {
inline uint64_t now_in_us() {
	struct timeval tv;
	struct timezone tz;
	tz.tz_minuteswest = 0;
	tz.tz_dsttime = 0;
	gettimeofday(&tv, &tz);
	uint64_t time = tv.tv_sec * (uint64_t) 1000000 + tv.tv_usec;
	return time;
}

//get current time of ms
inline uint32_t now_in_ms() {
	struct timeval tv;
	struct timezone tz;
	tz.tz_minuteswest = 0;
	tz.tz_dsttime = 0;
	gettimeofday(&tv, &tz);
	uint32_t time = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	return time;
}

inline std::string TmToStr(const tm& tmTime) {
	char buf[20];
	snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d", tmTime.tm_year + 1900, tmTime.tm_mon + 1,
			tmTime.tm_mday, tmTime.tm_hour, tmTime.tm_min, tmTime.tm_sec);
	return buf;
}

inline std::string now() {
	time_t timep;
	time(&timep); // get utc time
	struct tm *p;
	p = gmtime(&timep); // convert to struct tm
	std::string r = TmToStr(*p);
	return r;
}

} // class utils

} // namespace
#endif /* FRPROXY_H_ */
