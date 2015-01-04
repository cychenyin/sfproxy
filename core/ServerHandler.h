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

/*
 * TaskInfo, use to save schedule interval and running time
 * 	TaskInfo will not release runner. YOU SOULD DO IT YOURSELF.
 */
class TaskInfo {
public:
	Runnable* runner;
	int interval_in_ms;
	// last run time; accurate to ms,
	uint32_t last_run_ms;

public:
	TaskInfo(Runnable* runner, int interval_in_ms) : runner(runner), interval_in_ms(interval_in_ms), last_run_ms(0) {}
	~TaskInfo() {
		if(runner) {
			delete runner;
		}
	}
}; // class TaskInfo


typedef vector<TaskInfo> RunerVector;

class ServerHandler;

class TaskScheduler: virtual public Runnable {
private:
	ServerHandler* handler;
	int interval_in_us;
	RunerVector runners;
	bool stop;

public:
	TaskScheduler(ServerHandler* handler);
	~TaskScheduler();
	// run scheduler
	void run();
	// scheduler will run next round after interval eclipsed
	void add(Runnable* runner, int interval_ms);
	// clear scheduled task
	void clear();
private:
	// disallow empty construction
	TaskScheduler();

}; // class TaskScheduler

class ScheduledTask: virtual public Runnable {
protected:
	ServerHandler* handler;
	ScheduledTask(ServerHandler* phandler) : handler(phandler) {}
	~ScheduledTask() { handler = NULL; }
public:
	//void run() = 0;
}; // class TaskScheduler

class ResetTask: public ScheduledTask {
public:
	ResetTask(ServerHandler* handler_) : ScheduledTask(handler_){}
	 void run();
}; // class ResetTask

class AutoBreakZkConnTask: public ScheduledTask{
private:
	ClientPool *pool;
public:
	AutoBreakZkConnTask(ServerHandler* handler_, ClientPool *pool) : ScheduledTask(handler_), pool(pool) {}
	~AutoBreakZkConnTask() {
		pool = NULL;
	}
	void run();
}; // class AutoBreakZkConnTask

class AutoSaveTask: public ScheduledTask {
private:
	string filename;

public:
	AutoSaveTask(ServerHandler* handler_, const string& filename) : ScheduledTask(handler_), filename(filename){}
	void run();
}; // class AutoSaveTask

//class OnceTask: virtual public Runnable {
//public:
////	void run() = 0;
//}; // class OnceTask

class WarmTask: virtual public Runnable {
private:
	ClientPool *pool;
	string zk_root;
	ZkClient *c;
public:
	WarmTask(ClientPool *pool, string zkRoot) : pool(pool), zk_root(zkRoot) {
		c = NULL;
	}
	~WarmTask() {}

	void run();
}; // class WarmTask

class RegisterTask: virtual public Runnable {
private:
	ClientPool *pool;
	string node_name;
	ZkClient *c;

public:
	RegisterTask(ClientPool *pool, string node_name) :
			pool(pool), node_name(node_name) {
		c = 0;
	}
	~RegisterTask() {}

	void run();
}; // class RegisterTask

class ZkReadTask: virtual public Runnable {
private:
	ClientPool *pool;
	string zkpath;
	SkipBuffer *skip_buf;
	ZkClient *c;
public:
	ZkReadTask(ClientPool *pool, string zkpath, SkipBuffer *skip_set) : pool(pool), zkpath(zkpath), skip_buf(skip_set) {
		c = 0;
	}

	~ZkReadTask() {	}
	void run();
}; // class ZkReadTask


class ServerHandler: virtual public RegistryProxyIf {

private:
	RegistryCache *cache;
	ClientPool *pool;
	string *root;
	string split;
	SkipBuffer *skip_buf;
	int thread_count; // one for long running scheduler; one for scheduled task; one for request
	shared_ptr<ThreadManager> threadManager;
	int async_wait_timeout; // in ms
	int async_exec_timeout; // in ms
	string hostname;
	string zkhosts_;
	int port;
	TaskScheduler *scheduler;
public:
	ServerHandler(string zkhosts, int port);
	~ServerHandler();

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

	// save cache into /tmp/frproxy.cache
	void save(string& filename);

	void warm();
	void register_self();
	void start_scheduler();
	void commit_task(TaskInfo& task);

private:
	void init_scheduler();

	void init_thread_pool();

	void init_hostname();
}; // class ServerHandler


} // namespace FinagleRegistryProxy

#endif /* SERVERHANDLER_H_ */
