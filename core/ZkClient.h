/*
 * ZkClient.h
 *
 *  Created on: Mar 18, 2014
 *      Author: asdf
 *
 * zk api:
 //create a handle to used communicate with zookeeper
 zhandle_t *zookeeper_init(const char *host, watcher_fn fn, int recv_timeout, const clientid_t *clientid, void *context, int flags)
 //create a node synchronously
 int zoo_create(zhandle_t *zh, const char *path, const char *value,int valuelen,
 const struct ACL_vector *acl, int flags,char *path_buffer, int path_buffer_len);
 //lists the children of a node synchronously.
 int zoo_wget_children(zhandle_t *zh, const char *path, watcher_fn watcher, void* watcherCtx, struct String_vector *strings)
 //close the zookeeper handle and free up any resources.
 int zookeeper_close(zhandle_t *zh)
 // ZOOAPI int zoo_wget(zhandle_t *zh, const char *path,watcher_fn watcher, void* watcherCtx, char *buffer, int* buffer_len, struct Stat *stat);
 */

#ifndef ZKCLIENT_H_
#define ZKCLIENT_H_

#include <iostream>
#include <string.h>
#include <errno.h>
#include <time.h>

#include <zookeeper/zookeeper.h>
#include <zookeeper/zk_adaptor.h>
#include <zookeeper/zookeeper_log.h>

#include <thrift/concurrency/Mutex.h>

//#include "ganji/util/thread/mutex.h"
#include "ClientPool.h"
#include "JsonUtil.h"
#include "RegistryCache.h"
#include "SEvent.h"

using namespace std;
//using namespace ganji::util::thread;

namespace FinagleRegistryProxy {

typedef SEvent<ClientPool, ClientBase, int> WatchEvent; // client event


class ZkClient: public ClientBase {
public:

	// hosts format: 127.0.0.1:2181,127.0.0.1:2182
	ZkClient();
	ZkClient(string hosts, RegistryCache *pcache);
	void init(string hosts, RegistryCache *pcache);

	virtual ~ZkClient();
	void connect_zk();
	void get_children(string serviceZpath);
	void update_service(string serviceZpath, string subNodeName);
	static void children_watcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx);
	static void ephemeral_watcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx);
	static void global_watcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx);
	vector<string> get_all(string root = "/soa/services");
//	void InitWatcher(zhandle_t *zh, int type, int state, const char *path, void *watcher_ctx);
	void dump_stat(struct Stat *stat);
	void parse(string json);

	void close_handle();
public: // interface imple
	//ClientBase::open
	void open() {
		this->connect_zk();
	}
	//ClientBase::close
	void close() {
		this->set_in_using(false);
	}

public:
	string* root;
	const static int ZK_MAX_CONNECT_RETRY_TIMES = 10;
	const static int RCEV_TIME_OUT_DEF = 10 * 1000 * 1000; // in microsecond

private:
	RegistryCache *pcache;
	zhandle_t *zhandle_;
	string zk_hosts_;
	int timeout_; // zk recv_timeout in microsecond
	void Init();
	// apache::thrift::concurrency::Mutex mutex;

};

class ZkClientContext {
public:
	ZkClient *client;
	string serviceZpath;
	string ephemeralNode;

	ZkClientContext() {
		client = NULL;
	}
	ZkClientContext(ZkClient *client, string serviceZpath) {
		this->client = client;
		this->serviceZpath = serviceZpath;
	}
	ZkClientContext(ZkClient *client, string serviceZpath, string ephemeralNodeName) {
		this->client = client;
		this->serviceZpath = serviceZpath;
		this->ephemeralNode = ephemeralNodeName;
	}
	virtual ~ZkClientContext() {
		client = NULL;
	}
};

class ZkClientFactory: public ClientFactory {
private:
	string zkhosts;
	RegistryCache* cache;
public:
	ZkClientFactory(const string& zookeeperhosts, RegistryCache* cache_) : zkhosts(zookeeperhosts), cache(cache_) {
//		zkhosts = zookeeperhosts;
//		cache = cache_;
	}
	~ZkClientFactory() {
		cache = 0;
	}
	ZkClient* create() {
		return new ZkClient(this->zkhosts, this->cache);
	}
};

} /* namespace FinagleRegistryProxy */

#endif /* ZKCLIENT_H_ */
