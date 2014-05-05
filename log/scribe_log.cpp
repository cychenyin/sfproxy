/**
 * @Copyright (c) 2010 Ganji Inc.
 * @file          ganji/util/log/scribe_log.cpp
 * @namespace     ganji::util::log
 * @version       1.0
 * @author        miaoxijun
 * @date          2010-09-26
 *
 * ThriftLog
 */

#include "scribe_log.h"

#include <stdarg.h>
#include <pthread.h>
#include <time.h>

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include "gen-cpp/scribe.h"
#include <protocol/TBinaryProtocol.h>
#include <server/TSimpleServer.h>
#include <transport/TServerSocket.h>
#include <transport/TBufferTransports.h>
#include <transport/TSocket.h>

using namespace apache;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;
using namespace scribe::thrift;
using boost::shared_ptr;

namespace ganji {
namespace util {
namespace log {
namespace ThriftLog {

static std::string str_hostip = "192.168.1.61";
static int n_hostport = 11463;
static int n_maxcache = 999999;

using namespace std;
class LogWriter {
public:
	LogWriter() {
		pclient = NULL;
	}
	~LogWriter() {
		uninit();
	}
	int init() {
		socket = shared_ptr<TTransport>(new TSocket(str_hostip, n_hostport));
		transport = shared_ptr<TTransport>(new TFramedTransport(socket));
		protocol = shared_ptr<TProtocol>(new TBinaryProtocol(transport));
		pclient = new scribeClient(protocol);
		return 0;
	}
	int write(std::string category, std::string message) {
		if (transport == NULL) {
			return -1;
		}
		int rt = 0;
		try {
			if (!transport->isOpen())
				transport->open();
			LogEntry log;
			log.category = category;
			log.message = message;
			std::vector<LogEntry> list;
			list.push_back(log);
			if (pclient->Log(list) != ResultCode::OK)
				rt = -2;
		} catch (...) {
			rt = -3;
			cout << "scribe_log fail to write log cause of exception." << endl;
		}
		return rt;
	}
protected:
	shared_ptr<TTransport> socket;
	shared_ptr<TTransport> transport;
	shared_ptr<TProtocol> protocol;
	scribeClient * pclient;
private:
	int uninit() {
		if (transport) {
			transport->close();
		}
		if (pclient) {
			delete pclient;
			pclient = 0;
		}
		return 0;
	}

};

class BufferedLogSender {
	std::list<LogEntry> msg_list;
	LogWriter * p_writer;
	pthread_t n_work_id;
	pthread_mutex_t lock;
	bool b_exit;
	bool b_valid;
public:
	BufferedLogSender() {
		p_writer = NULL;
		b_exit = false;
		b_valid = false;
		n_work_id = 0;
	}
	~BufferedLogSender() {
	}
	static void * work_thread(void * lparam) {
		BufferedLogSender * p_sender = reinterpret_cast<BufferedLogSender *>(lparam);
		if (p_sender)
			p_sender->worker();
		return 0;
	}
	void worker() {
		while (!b_exit) {
			send_log();
			sleep(10);
		}
		send_log();
	}
	void send_log() {
		if (!p_writer)
			return;
		while (true) {
			bool b_send = false;
			LogEntry begin;
			pthread_mutex_lock(&lock);
			// printf("d   %d\n",m_message_list.size());
			if (msg_list.size() > 0) {
				begin = *msg_list.begin();
				msg_list.pop_front();
				b_send = true;
			}
			pthread_mutex_unlock(&lock);
			if (b_send && p_writer) {
				if (p_writer->write(begin.category, begin.message) != 0) {
					pthread_mutex_lock(&lock);
					if (msg_list.size() <= n_maxcache)
						msg_list.push_front(begin);
					pthread_mutex_unlock(&lock);
					break;
				}
			} else
				break;
		}
	}
	bool valid() {
		return b_valid;
	}
	int init() {
		p_writer = new LogWriter();
		p_writer->init();
		pthread_mutex_init(&lock, NULL);
		int rt = pthread_create(&n_work_id, NULL, work_thread, this);
		if (rt == 0)
			b_valid = true;
		return rt;
	}
	int uninit() {
		b_exit = true;
		pthread_join(n_work_id, NULL);
		if (p_writer) {
			delete p_writer;
			p_writer = NULL;
		}
		pthread_mutex_destroy(&lock);
		return 0;
	}
	int write(std::string category, std::string message) {
		LogEntry log;
		log.category = category;
		log.message = message;
		pthread_mutex_lock(&lock);
		// printf("a   %d\n",m_message_list.size());
		msg_list.push_back(log);
		// printf("b   %d\n",m_message_list.size());

		if (msg_list.size() > n_maxcache) {
			msg_list.pop_front();
			// printf("c   %d\n",m_message_list.size());
		}

		pthread_mutex_unlock(&lock);
		return 0;
	}
};

static BufferedLogSender * p_buffered_sender = NULL;

std::string get_current_time() {
	time_t tt;
	struct tm vtm;
	time(&tt);
	localtime_r(&tt, &vtm);
	char tc[512] = "";
	snprintf(tc, sizeof(tc), "%04d/%02d/%02d %02d:%02d:%02d", vtm.tm_year + 1900, vtm.tm_mon + 1, vtm.tm_mday,
			vtm.tm_hour, vtm.tm_min, vtm.tm_sec);
	return tc;
}

extern int32_t LogInit(const char * hostip, int32_t hostport, int32_t maxcache) {
	str_hostip = hostip;
	n_hostport = hostport;
	n_maxcache = maxcache;
	p_buffered_sender = new BufferedLogSender();
	return p_buffered_sender->init();
}

extern int32_t LogWrite(int32_t log_type, const char * category, const char * message) {
	if (p_buffered_sender && p_buffered_sender->valid()) {
		std::string level;
		switch (log_type) {
		case FATAL:
			level = "FATAL";
			break;
		case WARNING:
			level = "WARNING";
			break;
		case NOTICE:
			level = "NOTICE";
			break;
		case TRACE:
			level = "TRACE";
			break;
		case DEBUG:
			level = "DEBUG";
			break;
		}
		// std::string current_time = get_current_time();
		std::string msg = get_current_time();
		msg += " ";
		msg += "[";
		msg += level;
		msg += "]";
		msg += " ";
		msg += message;
		return p_buffered_sender->write(category, msg);
	}
	return -1;
}

extern int32_t LogWrite(const char * category, const char * format, ...) {
	va_list args;
	if (p_buffered_sender && p_buffered_sender->valid()) {
		va_start(args, format);
		char message[1024] = "";
		vsprintf(message, format, args);
		va_end(args);
		return p_buffered_sender->write(category, message);
	}
	return -1;
}

extern int32_t LogUninit() {
	if (p_buffered_sender)
		return p_buffered_sender->uninit();
	return 0;
}

}
}
}
}  ///< end of namespace ganji::util::log::ThriftLog
