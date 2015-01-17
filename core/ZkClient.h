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
#include <sstream>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <stdint.h>

#include <zookeeper/zookeeper.h>
#include <zookeeper/zk_adaptor.h>
#include <zookeeper/zookeeper_log.h>
#include <concurrency/Mutex.h>

#include "ClientPool.h"
#include "JsonUtil.h"
#include "RegistryCache.h"
#include "SEvent.h"
#include "../log/logger.h"

using namespace std;
using FinagleRegistryProxy::logger;

namespace FinagleRegistryProxy {

typedef SEvent<ClientPool, ClientBase, int> WatchEvent; // client event

class ZkState: public ClientState {
public:
	const static int TYPE_CREATE_EPHERERAL = 0;
	const static int TYPE_GET_SERVICE = 1;
	const static int TYPE_GET_NODE = 2;
	const static int TYPE_GET_ROOT = 3;

	int type;
	string path;
	string data;
public:
	ZkState() {
		type = TYPE_GET_NODE;
		data = path = "";
	}
	ZkState(string path, const int type) :
			path(path), type(type) {
		data = "";
	}

	bool equals(ZkState* c) {
		if (!c)
			return false;
		return c == 0 ? false : c->path == this->path && c->type == this->type;
	}
	bool match_path(ZkState* c) {
		if (!c)
			return false;
		return c == 0 ? false : c->path == this->path;
	}
	string key() {
		stringstream ss;
		ss << type << path;
		return ss.str();
	}
	~ZkState() {
	}
public:
	static string create_key(string &path, int type) {
		stringstream ss;
		ss << type << path;
		return ss.str();
	}
};

class ZkClient: public ClientBase {
	friend class ZkClientFactory;
public:
	// hosts format: 127.0.0.1:2181,127.0.0.1:2182
	ZkClient();
	ZkClient(string hosts, RegistryCache *pcache);
public:
	void init(string hosts, RegistryCache *pcache);
	virtual ~ZkClient();
	bool connect_zk();
	// biz interface, get service info by zkpath; eg. serviceZpath = /soa/services/ip2city.thrift
	void get_service(string serviceZpath);
	// biz interface, get server info of service by zkpath; eg. serviceZpath = /soa/services/ip2city.thrift/member_00000001
	void get_node(string path);
	// biz interface, get server info of serivce by service zkpath and server name; eg. serviceZpath=/soa/services/ip2city.thrift; subNodeName=member_00000001
	void get_node(string serviceZpath, string subNodeName);
	vector<string> get_all_services(string _serivces_root = "/soa/services");
	// parse zk node data into Registry and add it to cache; eg. json = {"serviceEndpoint":{"host":"192.168.113.51","port":21004},"additionalEndpoints":{},"status":"ALIVE","shard":1}
	void parse(string json);
	// create ephermaral zk node; eg. name="/soa/proxies/localhost:9009" data=""
	int create_enode(string name, string data);
	// create normalzk node; eg. abs_path=/soa
	int create_pnode(string abs_path, bool log_when_exists = false);
#ifdef DEBUG_
	void debug() {
		this->set_connected(false);
	}
#endif
	// watcher of /soa/services
	static void root_watcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx);
	// watcher of /soa/services
	static void service_watcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx);
	// watcher of /soa/services/xxx
	static void node_watcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx);
	// watcher of /soa/services/xxx/member_yyy
	static void create_enode_watcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx);
	// global watcher of zk conn;
	static void global_watcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx);
	// state, path info which is watched
	void save_state(string &path, int type);
public:
	// interface imple
	//ClientBase::open
	virtual bool open();
	// now close object really, just return it to pool.
	virtual void close();
	// receive states(watched path info) from cache, and create one watch for each of these states
	virtual int set_states(StateMap *states);
	// session timeout event handler
	void on_session_timeout();
public:
	string* root_;
	const static int ZK_MAX_CONNECT_RETRY_TIMES = 10;
	const static int RCEV_TIME_OUT_DEF = 10 * 1000 * 1000; // in microsecond, us

private:
	RegistryCache *pcache_;
	zhandle_t *zhandle_;
	// zk conn string, format: host:port[[,host,port]...]
	string zk_hosts_;
	// zk recv_timeout in microsecond
	int timeout_;
	void init();
	// apache::thrift::concurrency::Mutex mutex;
	void close_handle();
	// remove state info when zk node is remove when watcher listens message of node removed
	void remove_state(ZkState *state);
};

class ZkClientContext {
public:
	ZkClient *client;
	// eg. /soa/services/xxx
	string serviceZpath;
	// eg. member_00000001
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
	ZkClientFactory(const string& zookeeperhosts, RegistryCache* cache_ = 0) :
			zkhosts(zookeeperhosts), cache(cache_) {
	}
	~ZkClientFactory() {
	}
	ClientBase* create(ClientBase *p){
		p = new ZkClient(this->zkhosts, this->cache);
		return p;
	}
};

} /* namespace FinagleRegistryProxy */

#endif /* ZKCLIENT_H_ */
