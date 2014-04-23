/*
 * frproxy.h
 *
 *  Created on: Mar 21, 2014
 *      Author: asdf
 */

#ifndef FRPROXY_H_
#define FRPROXY_H_

#ifndef DEBUG_
#define DEBUG_

#endif

#include <sys/time.h>
#include <time.h>
#include <string>
#include <iostream>
#include <stdarg.h>
#include <cstdio>

//#include "ganji/util/log/scribe_log.h"
using namespace std;
#ifndef INT32T
typedef int int32_t;
#endif
#ifndef UINT32T
typedef unsigned int uint32_t;
#endif
#ifndef UINT64T
typedef unsigned long uint64_t;
#endif

namespace FinagleRegistryProxy {

inline void logger_init(const char * hostip, int32_t hostport, int32_t maxcache = 999999) {
	// ganji::util::log::ThriftLog::LogInit(hostip, hostport, maxcache);
}

inline void logger_destory() {
//	ganji::util::log::ThriftLog::LogUninit();
}

inline void error(const char* format, ...){
	va_list args;
	va_start(args, format);
    char message[1024] = "";
    vsprintf(message, format, args);
    va_end(args);
//    LogWrite(ganji::util::log::ThriftLog::FATAL, "frproxy", message );
}

inline void warn(const char* format, ...){
	va_list args;
	va_start(args, format);
    char message[1024] = "";
    vsprintf(message, format, args);
    va_end(args);
    const char category[] = "frproxy";
//    LogWrite(ganji::util::log::ThriftLog::WARNING, category , message );
}


inline void debug(const char* format, ...){
	va_list args;
	va_start(args, format);
    char message[1024] = "";
    vsprintf(message, format, args);
    va_end(args);
    const char category[] = "frproxy";
//    LogWrite(ganji::util::log::ThriftLog::DEBUG, category, message );
}

inline uint64_t now_in_us() {
  struct timeval  tv;
  struct timezone tz;
  tz.tz_minuteswest = 0;
  tz.tz_dsttime     = 0;
  gettimeofday(&tv, &tz);
  uint64_t time = tv.tv_sec * (uint64_t)1000000 + tv.tv_usec;
  return time;
}

//get current time of ms
inline uint32_t now_in_ms() {
  struct timeval  tv;
  struct timezone tz;
  tz.tz_minuteswest = 0;
  tz.tz_dsttime = 0;
  gettimeofday(&tv, &tz);
  uint32_t time = tv.tv_sec * 1000 + tv.tv_usec / 1000;
  return time;
}



} // namespace
#endif /* FRPROXY_H_ */
