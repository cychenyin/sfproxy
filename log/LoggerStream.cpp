/*
 * LoggerStream.cpp
 *
 *  Created on: May 28, 2014
 *      Author: asdf
 */

#include <streambuf>
#include <libio.h>
#include <iosfwd>
#include <iostream>
#include <string>
#include "LoggerStream.h"
#include "logger.h"

using namespace std;
namespace FinagleRegistryProxy {

LoggerStream::LoggerStream(log_fn w) {
	setp(buf_, buf_ + BUFFER_SIZE - 1);
	this->writer = w;
}

LoggerStream::~LoggerStream() {
	sync();
}

LoggerStream::int_type LoggerStream::overflow(LoggerStream::int_type c) {
	if (c != EOF) {
		*pptr() = c;
		pbump(1);
	}

	if (flush_buffer() == EOF) {
		return EOF;
	}

	return c;
}

int LoggerStream::sync() {
	if (flush_buffer() == EOF) {
		return -1;
	}

	return 0;
}

int LoggerStream::flush_buffer() {
	int len = pptr() - pbase();

	if (len > 0) {
		std::string s(buf_, 0, len);
		if (this->writer)
			writer(s.c_str());
	}

	pbump(-len);

	return len;
}

} /* namespace FinagleRegistryProxy */
