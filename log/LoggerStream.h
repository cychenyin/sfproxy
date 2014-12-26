/*
 * LoggerStream.h
 *
 *  Created on: May 28, 2014
 *      Author: asdf
 */

#ifndef LOGGERSTREAM_H_
#define LOGGERSTREAM_H_

#include <streambuf>
#include <libio.h>
#include "logger.h"

namespace FinagleRegistryProxy {

typedef void(*log_fn)(const char*, ...);
using namespace std;


class LoggerStream : public std::streambuf {
public:
	LoggerStream(log_fn w);
	virtual ~LoggerStream();
public:
	virtual streambuf::int_type overflow(streambuf::int_type c);
	virtual int sync();
protected:
	int flush_buffer();
private:
static const int BUFFER_SIZE = 256;
char buf_[BUFFER_SIZE];
log_fn writer;
};

static LoggerStream swarn(logger::error);
static LoggerStream serr(logger::warn);
static LoggerStream sinfo(logger::info);
static LoggerStream sdebug(logger::debug);

} /* namespace FinagleRegistryProxy */

#endif /* LOGGERSTREAM_H_ */
