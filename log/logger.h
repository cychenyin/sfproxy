/*
 * logger.h
 *
 *  Created on: Apr 23, 2014
 *      Author: asdf
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#define HOSTNAME_SIZE 129

//#ifndef INT32T
//typedef int int32_t;
//#endif
//#ifndef UINT32T
//typedef unsigned int uint32_t;
//#endif
//#ifndef UINT64T
//typedef unsigned long uint64_t;
//#endif

#include <string>
#include <iostream>
#include <string.h>
#include <stdarg.h>
#include <cstdio>
#include <unistd.h>

#include "scribe_log.h"

using namespace std;

namespace FinagleRegistryProxy {

class logger {

private:
static char *hostname; // new char[HOSTNAME_SIZE];
static int switcher;   // new int(0);

public:
static void enable();
static void init(const char * hostip, int32_t hostport, int32_t maxcache = 999999);
static void destory();
static void error(const char* format, ...);
static void warn(const char* format, ...);
static void info(const char* format, ...);
static void debug(const char* format, ...);
};

} /* namespace FinagleRegistryProxy */

#endif /* LOGGER_H_ */
