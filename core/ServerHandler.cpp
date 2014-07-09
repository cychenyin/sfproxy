// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

#include <boost/smart_ptr/shared_ptr.hpp>
#include <concurrency/Mutex.h>
#include <unistd.h>
#include <iterator>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "../frproxy.h"
#include "Registry.h"

#ifndef DiffMs_
#define DiffMs(end, start) ((double) (end - start) / CLOCKS_PER_SEC * 1000)
#endif

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <concurrency/Exception.h>
#include <concurrency/PosixThreadFactory.h>
#include <concurrency/Thread.h>
#include <concurrency/ThreadManager.h>

#include "../thrift/RegistryProxy.h"

#include "ZkClient.h"
#include "RegistryCache.h"
#include "ClientPool.h"
#include "SkipBuffer.h"
#include "../log/logger.h"

using namespace std;
using namespace apache::thrift::concurrency;
using boost::shared_ptr;

namespace FinagleRegistryProxy {

class WarmTask: public Runnable {
private:
	ClientPool *pool;
	string zk_root;
	ZkClient *c;
public:
	WarmTask(ClientPool *pool, string zkRoot) :
			pool(pool), zk_root(zkRoot) {
		c = 0;
	}

	~WarmTask() {
	}

	void run() {
		c = (ZkClient*) pool->open();
		if (c) {
			vector<string> names = c->get_all_services(zk_root);
			vector<string>::iterator it = names.begin();
			while (it != names.end()) {
				if (c)
					c->get_service(*it);
				++it;
			}
			c->close();
		} else {
			logger:: warn("fail to open zk client when warming. please check out whether zookeeper cluster is valid.");
			cout << "fail to open zk client when warming. please check out whether zookeeper cluster is valid" << endl;
		}
	}
};

class RegisterTask: public Runnable {
private:
	ClientPool *pool;
	string node_name;
	ZkClient *c;

public:
	RegisterTask(ClientPool *pool, string node_name) :
			pool(pool), node_name(node_name) {
		c = 0;
	}

	~RegisterTask() {
	}

	void run() {
		// ZkClient *client = (ZkClient*) pool->open();
		c = (ZkClient*) pool->open();
		if (c && c->get_connected() == true) {
			stringstream ss;
			ss << "/soa";
			c->create_pnode(ss.str());
			ss << "/proxies";
			c->create_pnode(ss.str());
			ss << "/" << node_name;
			int res = c->create_enode(ss.str(), "");
			cout << "register self done. path=" << ss.str() << endl;
			if (res != 0) {
				logger::warn("register_self zoo_create error, path=%s", ss.str().c_str());
			}
		} else {
			logger:: warn("fail to open zk client registering self. please check out whether zookeeper cluster is valid.");
			cout << "fail to open zk client when registering self. please check out whether zookeeper cluster is valid" << endl;
		}
	}
};

class ZkReadTask: public Runnable {
private:
	ClientPool *pool;
	string zkpath;
	SkipBuffer *skip_buf;
	ZkClient *c;
public:
	ZkReadTask(ClientPool *pool, string zkpath, SkipBuffer *skip_set) :
			pool(pool), zkpath(zkpath), skip_buf(skip_set) {
		c = 0;
	}

	~ZkReadTask() {
	}
	void run() {
		c = (ZkClient*) pool->open();
		if (c) {
			c->get_service(zkpath);
			c->close(); // must
		} else {
			logger::warn("fail to open zk client when async get request: %s. pool exhausted maybe. ", zkpath.c_str());
		}
		skip_buf->erase(zkpath);
	}
};
// class serverHandler::Task

class ServerHandler: virtual public RegistryProxyIf {

private:
	RegistryCache *cache;
	ClientPool *pool;
	string *root;
	string split;
	SkipBuffer *skip_buf;
	shared_ptr<ThreadManager> threadManager;
	int async_wait_timeout; // in ms
	int async_exec_timeout; // in ms
	string hostname;
	string zkhosts_;
public:
	ServerHandler(string zkhosts) : zkhosts_(zkhosts) {
		root = new string("/soa/services");
		cache = new RegistryCache();
		pool = new ClientPool(new ZkClientFactory(zkhosts, cache));
		split = "/";
		init_thread_pool();
		async_wait_timeout = -1; // Return immediately if pending task count exceeds specified max
		async_exec_timeout = 1000;
		skip_buf = new SkipBuffer(async_exec_timeout);
		init_hostname();
	}

	~ServerHandler() {
		if (pool) {
			delete pool;
			pool = 0;
		}
		if (cache) {
			delete cache;
			cache = 0;
		}
		if (root) {
			delete root;
			root = 0;
		}
		delete skip_buf;
		skip_buf = 0;

		threadManager->stop();
	}

	void get(std::string& _return, const std::string& serviceName) {
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
				try {
					threadManager->add(shared_ptr<ZkReadTask>(new ZkReadTask(pool, path, skip_buf)), async_wait_timeout);
					// (new ZkReadTask(pool, path, skip_buf))->run();
				} catch (TooManyPendingTasksException &ex) {
					logger::warn("too many pending zk task in thread pool. %s", ex.what());
				}
			}
#ifdef DEBUG_
			zk_retrieve_end = utils::now_in_us();
#endif
		}
#ifdef DEBUG_
		got = utils::now_in_us();
#endif
		if (pvector && pvector->size() > 0) {
			_return = Registry::to_json_string(*pvector);
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

	void remove(std::string& _return, const std::string& serviceName, const std::string& host, const int32_t port) {
		_return = "not supported now.";
		// cout << "remove called. " << endl;
	}

	void dump(std::string& _return) {
		stringstream ss;
		ss << "frproxy dump info. " << "hostname:" << hostname << "; zkhosts:" << this->zkhosts_ << endl << endl;
		ss << "connection:" << endl;
		ss << "---------------------------------------------" << endl;
		ss << pool->stat() << endl;

		ss << "cache:" << endl;
		ss << "---------------------------------------------" << endl;
		ss << cache->dump() << endl;

		ss << "threads:" << endl;
		ss << "---------------------------------------------" << endl;
		ss << "worker count:" << threadManager->workerCount() << endl;
		ss << "worker idle:" << threadManager->idleWorkerCount() << endl;
		ss << "expired task:" << threadManager->expiredTaskCount() << endl;
		ss << "pending task:" << threadManager->pendingTaskCount() << endl;
		ss << "pending max:" << threadManager->pendingTaskCountMax() << endl;

		ss << "skip buffer:" << endl;
		ss << "---------------------------------------------" << endl;
		ss << skip_buf->dump() << endl;

		_return = ss.str();
	}

	void warm() {
//		string path = "/soa/services/testservice";
//		Runnable *t = new ZkReadTask(pool, path, skip_buf);
//		t->run();
//		delete t;
//		t = 0;
//
//		t = new WarmTask(pool, *root);
//		t->run();
//		delete t;
//		t = 0;

		threadManager->add(shared_ptr<WarmTask>(new WarmTask(pool, *root)));
		logger::warn("warm server committed. server is getting up");
	}

	void register_self(int port) {
		stringstream name;
		name << this->hostname << ":" << port;
		try {
			threadManager->add(shared_ptr<RegisterTask>(new RegisterTask(pool, name.str())));
			logger::warn("server register self committed. ");
		} catch (TooManyPendingTasksException &e) {
			logger::warn("too many pending task occured  in thread pool when register self. %s", e.what());
		}
	}
private:
	void init_thread_pool() {
		threadManager = ThreadManager::newSimpleThreadManager(1, 100);
		shared_ptr<PosixThreadFactory> threadFactory = shared_ptr<PosixThreadFactory>(new PosixThreadFactory());
		threadManager->threadFactory(threadFactory);
		threadManager->start();
	}
	void init_hostname() {
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
};
// class ServerHandler

} /* namespace FinagleRegistryProxy  */
