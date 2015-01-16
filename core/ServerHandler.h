/*
 * ServerHandler.h
 *
 *  Created on: Dec 31, 2014
 *      Author: asdf
 */

#ifndef SERVERHANDLER_H_
#define SERVERHANDLER_H_

#include <time.h>
#include <unistd.h>
#include <iterator>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include <concurrency/Mutex.h>

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/make_shared.hpp>

#include <concurrency/Exception.h>
#include <concurrency/PosixThreadFactory.h>
#include <concurrency/Thread.h>
#include <concurrency/ThreadManager.h>


#include "../frproxy.h"
#include "ClientPool.h"
#include "Registry.h"
#include "RegistryCache.h"

#include "ZkClient.h"
#include "SkipBuffer.h"
#include "../thrift/RegistryProxy.h"
#include "../log/logger.h"

#ifndef DiffMs_
#define DiffMs(end, start) ((double) (end - start) / CLOCKS_PER_SEC * 1000)
#endif

using namespace std;
using namespace apache::thrift::concurrency;
using boost::shared_ptr;

namespace FinagleRegistryProxy {

class ServerHandler;
class ScheduledTask: virtual public Runnable {
protected:
	shared_ptr<ServerHandler> handler;
	shared_ptr<ScheduledTask> runner;
public:
	string name;
	uint32_t interval_in_ms;
	// last run time; accurate to ms,
	uint32_t last_run_ms;
	// bool busy; // remove busy flag cause of when expiration happens in multi thread env, busy state will keep be true forever

public:
	ScheduledTask(shared_ptr<ServerHandler> _handler, string _name="", int _interval_in_ms=0) : handler(_handler),interval_in_ms(_interval_in_ms),last_run_ms(0),name(_name) /*, busy(false)*/  {}
	virtual ~ScheduledTask() {}
	void run();

	string tostring() const;

private:
	virtual void dorun() = 0;
}; // class ScheduledTask


typedef vector<shared_ptr<ScheduledTask> > TaskVector;


class TaskScheduler: virtual public Runnable {
private:
	shared_ptr<ServerHandler> handler;
	int interval_in_us;
	TaskVector runners;
	bool stop_flag;
	int count;
public:
	int first_delay_sec;
public:
	TaskScheduler(shared_ptr<ServerHandler> handler);
	virtual ~TaskScheduler();
	// override Runnable; run scheduler
	void run();
	// scheduler will run next round after interval eclipsed
	void add(shared_ptr<ScheduledTask> _runner);
	// clear scheduled task
	void clear();

	void stop();

	int size() { return runners.size(); }

private:
	// disallow empty construction
	TaskScheduler();

}; // class TaskScheduler


class CheckTask: public ScheduledTask {
public:
	CheckTask(shared_ptr<ServerHandler> _handler, string _name, int _interval_in_ms) : ScheduledTask(_handler, _name, _interval_in_ms){}
	virtual ~CheckTask() {}
	void dorun();
}; // class CheckTask

//// reload from zookeeper again, if all zk offline, then use local file
//class ResetTask: public ScheduledTask {
//public:
//	ResetTask(shared_ptr<ServerHandler> _handler, string _name, int _interval_in_ms) : ScheduledTask(_handler, _name, _interval_in_ms){}
//	virtual ~ResetTask() {}
//	void dorun();
//}; // class ReloadTask
//
//class BreakZkConnTask: public ScheduledTask{
//private:
//	ClientPool *pool;
//public:
//	BreakZkConnTask(shared_ptr<ServerHandler> _handler, string _name, int _interval_in_ms, ClientPool *pool) : ScheduledTask(_handler, _name, _interval_in_ms), pool(pool) {}
//	virtual ~BreakZkConnTask() {
//		pool = NULL;
//	}
//	void dorun();
//}; // class AutoBreakZkConnTask

class SaveTask: public ScheduledTask {
private:
	string filename;

public:
	SaveTask(shared_ptr<ServerHandler> _handler, string _name, int _interval_in_ms, const string& filename) : ScheduledTask(_handler, _name, _interval_in_ms), filename(filename){}
	virtual ~SaveTask(){}
	void dorun();
}; // class AutoSaveTask

class OnceTask: virtual public Runnable {
protected:
	shared_ptr<ServerHandler> handler;
public:
	OnceTask(shared_ptr<ServerHandler> _handler) : handler(_handler) {}
	virtual ~OnceTask() {}
	void run();
private:
	virtual void dorun() = 0;
}; // class OnceTask

/**
 * init registry cache from file firstly, and then override these registry with zk if possible;
 */
class WarmTask: public OnceTask { // virtual public Runnable {
private:
//	ClientPool *pool;
//	string zk_root;
//	ZkClient *c;
public:
	WarmTask(shared_ptr<ServerHandler> _handler) : OnceTask(_handler){}
//	WarmTask(ClientPool *pool, string zkRoot) : pool(pool), zk_root(zkRoot) {
//		c = NULL;
//	}
private:
	void dorun();
}; // class WarmTask

class RegisterTask: public OnceTask { // virtual public Runnable {
//private:
//	ClientPool *pool;
//	string node_name;
//	ZkClient *c;

public:
	RegisterTask(shared_ptr<ServerHandler> _handler) : OnceTask(_handler){}
//	RegisterTask(ClientPool *pool, string node_name) :
//			pool(pool), node_name(node_name) {
//		c = 0;
//	}
private:
	void dorun();
}; // class RegisterTask

class GetFromZkTask: public OnceTask { // virtual public Runnable {
//private:
//	ClientPool *pool;
	string zkpath;
//	SkipBuffer *skip_buf;
//	ZkClient *c;
public:
	GetFromZkTask(shared_ptr<ServerHandler> _handler, string& _zkpath) : OnceTask(_handler), zkpath(_zkpath){}
//	ZkReadTask(ClientPool *pool, string zkpath, SkipBuffer *skip_set) : pool(pool), zkpath(zkpath), skip_buf(skip_set) {
//		c = 0;
//	}
private:
	void dorun();
}; // class ZkReadTask


class ServerHandler: virtual public RegistryProxyIf, public boost::enable_shared_from_this<ServerHandler> {

private:
	RegistryCache *cache;
	ClientPool *pool;
	string *root;
	string split;
	SkipBuffer *skip_buf;
	int thread_count; // one for long running scheduler; one for scheduled task; one for request
	shared_ptr<ThreadManager> threadManager;
	int async_wait_timeout; // in ms; default -1, means wait forever
	int async_exec_timeout; // in ms, default 1000ms
	string hostname;
	string zkhosts;
	int port;
	TaskScheduler *scheduler;
	string cache_file_name;
public:
	shared_ptr<ServerHandler> get_sharedPtr_from_this(){
	             return shared_from_this();
	}

public:
	ServerHandler(string zkhosts, int port);
	virtual ~ServerHandler();

	// RegistryProxyIf::get
	void get(std::string& _return, const std::string& serviceName);

	// RegistryProxyIf::remove
	void remove(std::string& _return, const std::string& serviceName, const std::string& host, const int32_t port);

	// RegistryProxyIf::dump
	void dump(std::string& _return);

	// RegistryProxyIf::reset
	void reset(std::string& _return);
	// RegistryProxyIf::status
	int32_t status();

	void get_from_zk(string& zkpath);
	void async_get_from_zk(string& zkpath);

	// save cache into /tmp/frproxy.cache
	void save(string& filename);
	void warm();
	void async_warm();

	void check();


	void register_self();
	void async_register_self();

	void start_scheduler();
	void commit_task(shared_ptr<ScheduledTask> task);

	// for test
	TaskScheduler* get_scheduler() {
		return scheduler;
	}
private:
	void init_scheduledtask();

	void init_thread_pool();

	void init_hostname();
}; // class ServerHandler


} // namespace FinagleRegistryProxy

#endif /* SERVERHANDLER_H_ */
