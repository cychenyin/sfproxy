/*
 * logger.cpp
 *
 *  Created on: Apr 23, 2014
 *      Author: asdf
 */

#include "logger.h"
#include <iostream>

namespace FinagleRegistryProxy {

// logger::switcher = 0;

int logger::switcher = 0;
char *logger::hostname = new char[HOSTNAME_SIZE];

void logger::enable() {
	logger::switcher = 1;
}

void logger::init(const char * hostip, int32_t hostport, int32_t maxcache) {
	hostname[0] = ' ';
	for(int i=1; i<HOSTNAME_SIZE; i++) {
		hostname[i] = 0;
	}
	gethostname(hostname + 1, HOSTNAME_SIZE - 1);
	if (switcher) {
		ganji::util::log::ThriftLog::LogInit(hostip, hostport, maxcache);
	}
}

void logger::destory() {
	if (hostname) {
		delete hostname;
		hostname = 0;
	}
	ganji::util::log::ThriftLog::LogUninit();
}

void logger::error(const char* format, ...) {
	if (logger::switcher) {
		va_list args;
		va_start(args, format);
		char message[1024] = "";
		vsprintf(message, format, args);
		va_end(args);
		LogWrite(ganji::util::log::ThriftLog::FATAL, "frproxy", strcat(message, hostname));
	}
}

void logger::warn(const char* format, ...) {
	if (logger::switcher) {
		va_list args;
		va_start(args, format);
		char message[1024] = "";
		vsprintf(message, format, args);
		va_end(args);
		const char category[] = "sf.frproxy";
		LogWrite(ganji::util::log::ThriftLog::WARNING, category, strcat(message, hostname));
	}
}

void logger::debug(const char* format, ...) {
	if (logger::switcher) {
		va_list args;
		va_start(args, format);
		char message[1024] = "";
		vsprintf(message, format, args);
		va_end(args);
		const char category[] = "frproxy";
		LogWrite(ganji::util::log::ThriftLog::DEBUG, category, strcat(message, hostname));
	}
}


} /* namespace FinagleRegistryProxy */
