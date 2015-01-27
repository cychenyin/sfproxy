// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

#include <boost/smart_ptr/shared_ptr.hpp>

#include <unistd.h>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

#include "ServerHandler.h"

// using namespace boost::static_pointer_cast;
namespace FinagleRegistryProxy {

void OnceTask::run() {
	try {
		dorun();
	} catch (const exception& ex) {
		cout << "OnceTask exception cause of " << ex.what() << endl;
	} catch (const char* e) {
		cout << "OnceTask exception cause of " << e << endl;
	} catch (...) {
		cout << "OnceTask failed" << endl;
	}
}

void CheckTask::dorun() {
	handler->check();
}
void KeepWakeTask::dorun() {
	handler->keep_wake();
}

// init registry cache from file firstly, and then override these registry with zk if possible;
void WarmTask::dorun() {
	handler->warm();
}

void RegisterTask::dorun() {
	handler->register_self();
}

void GetServiceTask::dorun() {
	handler->get_service_from_zk(zkpath);
}

TaskScheduler::TaskScheduler(shared_ptr<ServerHandler> _handler) {
	this->handler = _handler;
	assert(this->handler.get());

	stop_flag = false;
	interval_in_us = 100 * 1000;
	count = 0;
	first_delay_sec = 1;
}

TaskScheduler::~TaskScheduler() {
	this->clear();
}

void TaskScheduler::clear() {
	stop();
	runners.clear();
}

void TaskScheduler::run() {
	stop_flag = false;
	if (first_delay_sec > 0) {
		sleep(first_delay_sec);
	}

	while (!stop_flag) {
		for (TaskVector::iterator it = runners.begin(); it != runners.end(); ++it) {
			uint64_t now = utils::now_in_ms();
			//TaskInfo* task = *it;
			shared_ptr<ScheduledTask> task = *it;
			// cout << utils::now() << " " << task->name << " now=" << now << " last=" << task->last_run_ms << " diff=" << now - task->last_run_ms << " inter=" << task->interval_in_ms << endl;

			if (/*task->busy == false && */now - task->last_run_ms > task->interval_in_ms) {
				try {
					// task->busy = true; // set flag before commit
					handler->commit_task(task);
				} catch (const TooManyPendingTasksException &ex) {
					logger::warn("TaskScheduler::run exception, too many pending task in thread pool when committing %s task. %s",
							task->name.c_str(), ex.what());
					// task->busy = false;
				} catch (const exception& ex) {
					logger::warn("TaskScheduler::run %s exception: %s", task->name.c_str(), ex.what());
					cerr << "TaskScheduler::run " << task->name << " exception: " << ex.what() << endl;
					// task->busy = false;
				} catch (const char* ex) {
					logger::warn("TaskScheduler::run %s exception, msg: %s", task->name.c_str(), ex);
					cerr << "TaskScheduler::run " << task->name << " exception, msg:" << ex << endl;
					// task->busy = false;
				} catch (...) {
					logger::warn("TaskScheduler::run %s fail, and eated exception.", task->name.c_str());
					cerr << "TaskScheduler::run " << task->name << " fail, and eated exception." << endl;
					//task->busy = false;
				}
			}
		}
		usleep(interval_in_us);
	}
#ifdef DEBUG_
	cout << "scheduler quit" << endl;
#endif
}

void TaskScheduler::add(shared_ptr<ScheduledTask> _runner) {
	runners.push_back(_runner);
}

// stop and wait
void TaskScheduler::stop() {
	stop_flag = true;
#ifndef STOP_TIMEOUT_INTERVAL_MULTIPLE
	usleep(this->interval_in_us * 2);
#else
	usleep((long)(this->interval_in_us * STOP_TIMEOUT_INTERVAL_MULTIPLE));
	cout << "task scheduler stop wait timeout: " << (long)(this->interval_in_us * STOP_TIMEOUT_INTERVAL_MULTIPLE) << endl;
#endif
}

string ScheduledTask::tostring() const {
	stringstream ss;
	ss << "\t" << name << "\t" << interval_in_ms << "" << last_run_ms << "" << endl;
	return ss.str();
}
// run task, make sure no exception throw out
void ScheduledTask::run() {
// cout << utils::time_stamp() << " " << name << " task running..." << endl;
#ifdef DEBUG_
#endif
	last_run_ms = utils::now_in_ms();
	try {
		dorun();
	} catch (const exception& ex) {
		cout << "ScheduledTask " << name << " exception cause of " << ex.what() << endl;
	} catch (const char* e) {
		cout << "ScheduledTask" << name << " exception cause of " << e << endl;
	} catch (...) {
		cout << "ScheduledTask" << name << " failed" << endl;
	}
//	cout << utils::time_stamp() << " " << name << " task done" << endl;
}

void SaveTask::dorun() {
	int status = handler->status();
	if (status == 0) { // save to file only if server works healthly
		handler->save(filename);
		cout << status << "cache saved into file "<< filename << endl;
	} else {
		logger::warn("server status %ld is nonzero, auto save cancelled.", status);
	}
}

ServerHandler::ServerHandler(string _zkhosts, int _port) :
		zkhosts(_zkhosts), port(_port), thread_count(3), split("/") {
	root = new string("/soa/services");
	cache = new RegistryCache();
	pool = new ClientPool(new ZkClientFactory(_zkhosts, cache));
	init_thread_pool();
	async_wait_timeout = -1; // Return immediately if pending task count exceeds specified max
	async_exec_timeout = 1000;
	skip_buf = new SkipBuffer(async_exec_timeout);
	init_hostname();

	cache_file_name = "/tmp/frproxy.cache";
	// use shared_from_this, so, CAN NOT initialize scheduler in constructor
	// init_scheduledtask();
}

ServerHandler::~ServerHandler() {
	if (scheduler) {
		delete scheduler;
		scheduler = 0;
	}
	if (pool) {
		delete pool;
		pool = 0;
	}
	if (skip_buf) {
		delete skip_buf;
		skip_buf = 0;
	}
	if (cache) {
		delete cache;
		cache = 0;
	}
	if (root) {
		delete root;
		root = 0;
	}
	threadManager->stop();
}

// RegistryProxyIf::get
void ServerHandler::get(std::string& _return, const std::string& serviceName) {
#ifdef DEBUG_

	uint64_t start = utils::now_in_us();
	uint64_t got(start), zk_retrieve_start(start), zk_retrieve_end(start), serial(start);
#endif
	string path = *root + split + serviceName;
	vector<Registry>* pvector = cache->get(path.c_str());
	if (pvector == 0 || pvector->size() == 0) { // not hit cache, then update cache
#ifdef DEBUG_
		zk_retrieve_start = utils::now_in_us();
#endif
		// if getting from zk, then skip
		pair<map<string, uint32_t>::iterator, bool> insert = skip_buf->insert(path);
		if (insert.second) {
			async_get_service_from_zk(path);
		}
#ifdef DEBUG_
		zk_retrieve_end = utils::now_in_us();
#endif
	}
#ifdef DEBUG_
	got = utils::now_in_us();
#endif
	if (pvector && pvector->size() > 0) {
		_return = Registry::serialize(*pvector);
	} else
		_return = "";
#ifdef DEBUG_
	serial = utils::now_in_us();
	cout << " pool total=" << pool->size() << " used=" << pool->used() << " idle=" << pool->idle() << endl;
	cout << " get total cost=" << DiffMs(serial, start) << " got=" << DiffMs(got, start) << " zk retrieve="
			<< DiffMs(zk_retrieve_end, zk_retrieve_start) << " serial=" << DiffMs(serial, got) << endl;
	// cache->dump();
#endif
}

// RegistryProxyIf::remove
void ServerHandler::remove(std::string& _return, const std::string& serviceName, const std::string& host, const int32_t port) {
	_return = "not supported now.";
	// cout << "remove called. " << endl;
}

// RegistryProxyIf::dump
void ServerHandler::dump(std::string& _return) {
	stringstream ss;
	ss << "frproxy dump info. " << endl;
	ss << utils::time_stamp() << "hostname:" << hostname << "; zkhosts:" << this->zkhosts << endl << endl;

	ss << pool->dump() << endl;

	ss << cache->dump() << endl;

	ss << "threads:    " << "---------------------------------------------" << endl;
	ss << "worker count:" << threadManager->workerCount() << endl;
	ss << "worker idle :" << threadManager->idleWorkerCount() << endl;
	ss << "expired task:" << threadManager->expiredTaskCount() << endl;
	ss << "pending task:" << threadManager->pendingTaskCount() << endl;
	ss << "pending max :" << threadManager->pendingTaskCountMax() << endl;

	ss << skip_buf->dump() << endl;

	_return = ss.str();
}

// RegistryProxyIf::reset
void ServerHandler::reset(std::string& _return) {
	this->pool->clear(); // thread safe
	this->init_scheduledtask();
	this->cache->clear();

	this->threadManager->stop(); // needed ?
	this->threadManager->start(); // needed ?

	register_self();
	warm();

	stringstream ss;
	int res = status();
	switch (res) {
	case 1:
		ss << "fail to create zk conn " << endl;
		break;
	case 2:
		ss << "none watcher created" << endl;
		break;
	case 3:
		ss << "no service configured" << endl;
		break;
	case 4:
		ss << "unable to create enought thread " << endl;
		break;
	default:
		break;
	}

	_return = ss.str();
}
// RegistryProxyIf::status
int32_t ServerHandler::status() {
	int32_t ret = 0; // success
	ret += (pool->size() == 0 ? 1 : 0);
	ret += (cache->size() == 0 ? 2 : 0);
	ret += (pool->watcher_size() < 2 ? 2 : 0); // should watch service root /soa/serives/ and self /soa/proxies/xxx at least
	ret += (threadManager->workerCount() < thread_count ? 8 : 0);
	return ret;
}

void ServerHandler::save(string& filename) {
	this->cache->save(filename);
}

void ServerHandler::get_service_from_zk(string &zkpath) {
	ZkClient* c = (ZkClient*) pool->open();
	if (c) {
		try {
			c->get_service(zkpath);
		} catch (...) {
		}
		c->close(); // must
	} else {
		logger::warn("fail to open zk client when async get request: %s. pool exhausted maybe. ", zkpath.c_str());
	}
	skip_buf->erase(zkpath);
}

void ServerHandler::async_get_service_from_zk(string &zkpath) {
	try {
		threadManager->add(shared_ptr<Runnable>(new GetServiceTask(this->shared_from_this(), zkpath)), async_wait_timeout,
				async_exec_timeout);
		logger::warn("server async_get_service_from_zk task committed. ");
	} catch (const TooManyPendingTasksException &e) {
		logger::warn("too many pending task occured  in thread pool when async_get_service_from_zk. %s", e.what());
	}
}

// load from file firstly; then load all config from zk; finally check it.
void ServerHandler::warm() {
	// from file first
	this->cache->from_file(cache_file_name);

	check();
}

void ServerHandler::async_warm() {
	try {
		threadManager->add(shared_ptr<Runnable>(new WarmTask(this->shared_from_this()))); // NO timeout, wait forvever
		logger::warn("server async_warm task committed. ");
	} catch (const TooManyPendingTasksException &e) {
		logger::warn("too many pending task occured  in thread pool when warming. %s", e.what());
	}
}

void ServerHandler::keep_wake() {
	if(pool->used() == 0) {
		ZkClient* c = (ZkClient*) pool->open();
		c->close();
	}
}
void ServerHandler::check() {
	// 1. check zk conn, if false, then clear pool;
	// 2. scan zk, and reload them all.
	// 3. if zk conn ready, check consistency of cache and zk. traverse the cache, and if not exists in zk, then drop off it from cache
	uint64_t now = utils::now_in_ms();
//	cout << "check 1." << pool->dump() << endl;
//	cout << "check 1." << endl;
	ZkClient* c = (ZkClient*) pool->open();
//	cout << "check 2." << pool->dump() << endl;
//	cout << "check 2." << endl;
	if (c->check(*root)) {
//		cout << "check 3." << pool->dump() << endl;
		c->get_all_services(*root);
//		cout << "check 4." << pool->dump() << endl;
// 		cout << "check 4." << endl;
	} else  {
		logger::warn("check says zk is not ready");
		if(this->cache->size() == 0) {
			this->cache->from_file(this->cache_file_name);
		}
	}
	c->close();
	// remove dead instance info from cache
	cout << "check 5." << "now=" << now << endl;
	cache->remove_before(now);
	// cout << "check 6." << endl;
}

void ServerHandler::register_self() {
	ZkClient *c = (ZkClient*) pool->open();
	if (c) {
		try {
			if (c->get_connected() == true) {

				stringstream ss;
				ss << "/soa";
				c->create_pnode(ss.str());
				ss << "/proxies";
				c->create_pnode(ss.str());

				stringstream name;
				name << this->hostname << ":" << port;
				ss << "/" << name.str();
				int res = c->create_enode(ss.str(), "");
				cout << "register self done. path=" << ss.str() << endl;
				if (res != 0) {
					logger::warn("register_self zoo_create error, path=%s", ss.str().c_str());
				}
			}
		} catch (...) {
			logger::warn("fail to regsister self");
#ifdef DEBUG_
			cout << "fail to regsister self" << endl;
#endif
		}
		c->close();
	} else {
		logger::warn("fail to open zk client registering self. please check out whether zookeeper cluster is valid.");
		cout << "fail to open zk client when registering self. please check out whether zookeeper cluster is valid" << endl;
	}

}
void ServerHandler::async_register_self() {
	try {
		// bak code before 1.1.6
		// threadManager->add(shared_ptr<Runnable>(new RegisterTask(pool, name.str())));
		threadManager->add(shared_ptr<Runnable>(new RegisterTask(this->shared_from_this()))); // NO timeout, wait forvever
		logger::warn("server async_register_self task committed. ");
	} catch (const TooManyPendingTasksException &e) {
		logger::warn("too many pending task occured  in thread pool when register self. %s", e.what());
	}
}

void ServerHandler::start_scheduler() {
	if (scheduler == NULL) {
		init_scheduledtask();
	}
	try {
		// expiration is not expire, and wait forever
		threadManager->add(shared_ptr<TaskScheduler>(scheduler), 0, 0);

	} catch (const TooManyPendingTasksException &ex) {
		logger::warn("too many pending zk task in thread pool. %s", ex.what());
	}
}
// commit task with timeout
void ServerHandler::commit_task(shared_ptr<ScheduledTask> task) {
	// v1
	// threadManager->add(shared_ptr<Runnable>(task->runner), async_wait_timeout);
	// v2
//	shared_ptr<ScheduledTask> task(envlope->runner);
//	threadManager->add(task, async_wait_timeout);
	// v3
	// threadManager->add(envlope->runner, async_wait_timeout);
	// v4
	// threadManager->add(task, async_wait_timeout, 3 * async_wait_timeout); // 10ms wait to add task to pending queue // 200ms expiration in execution
	if(threadManager->pendingTaskCount() < threadManager->pendingTaskCountMax()) {
		threadManager->add(task);
	} else
		logger::warn("thread pool pending task queue is full. %s task is avoid.", task->name.c_str() );
}

void ServerHandler::init_scheduledtask() {
	if (scheduler == NULL)
		scheduler = new TaskScheduler(this->shared_from_this());
	else
		scheduler->clear();
	shared_ptr<KeepWakeTask> wake(new KeepWakeTask(this->shared_from_this(), string("wake"), 5 * 60 * 1000)); // 5 minutes
	shared_ptr<CheckTask> check(new CheckTask(this->shared_from_this(), string("check"), 5 * 60 * 1000));		// 5 minutes
	shared_ptr<SaveTask> autosave(new SaveTask(this->shared_from_this(), string("autosave"), 3600 * 1000, cache_file_name)); // 1 hour

	// TODO ... delete 3 lines followed
#ifdef DEBUG_
	wake->interval_in_ms = 1 * 1000;
	check->interval_in_ms = 10 * 1000;
	autosave->interval_in_ms = 300 * 1000;
#endif

	cout << utils::time_stamp() << " " << check->name << " scheduled task interval(ms): " << check->interval_in_ms << endl;
	cout << utils::time_stamp() << " " << autosave->name << " scheduled task interval(ms): " << autosave->interval_in_ms << endl;

	scheduler->add(wake);
	scheduler->add(check);
	scheduler->add(autosave);
}

void ServerHandler::init_thread_pool() {
	threadManager = ThreadManager::newSimpleThreadManager(3, 100);
	shared_ptr<PosixThreadFactory> threadFactory = shared_ptr<PosixThreadFactory>(new PosixThreadFactory());
	threadManager->threadFactory(threadFactory);
	threadManager->start();
}

void ServerHandler::init_hostname() {
	int size = 128;
	char *buf = new char[size];
	for (int i = 0; i < size; i++) {
		buf[i] = 0;
	}
	gethostname(buf, size);
	// copy to string
	this->hostname = string(buf);
	delete buf;
	buf = 0;
}

} /* namespace FinagleRegistryProxy  */
