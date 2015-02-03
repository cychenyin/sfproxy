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

/*
 * zk client;
 * 	do not use conn, before set is_connected_ and in_using_ flag
 */
class ZkState: virtual public ClientState {
public:
	// use google name style
	const static int k_type_create_ephereral = 0;
	const static int k_type_get_service = 1;
	const static int k_type_get_service_instance = 2;
	const static int k_type_get_root = 3;

	int type;
	string path;
	string data;
public:
	ZkState() {
		type = k_type_get_service_instance;
		data = path = "";
	}
	ZkState(string path, const int type) : path(path), type(type) {
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
	friend class ClientPool;
public:
	// hosts format: 127.0.0.1:2181,127.0.0.1:2182
	ZkClient();
	ZkClient(string hosts, RegistryCache *pcache, StateBag *shared_states=NULL);
public:
	void init(string hosts, RegistryCache *pcache);
	virtual ~ZkClient();
	bool connect_zk();
	// biz interface, get service info by zkpath; eg. serviceZpath = /soa/services/ip2city.thrift
	// add watcher to service instance zk node
	bool get_service(string serviceZpath);
	// biz interface, get server info of service by zkpath; eg. serviceZpath = /soa/services/ip2city.thrift/member_00000001
	// add watcher to service zk node
	bool get_service_instance(string path);
	// biz interface, get server info of serivce by service zkpath and server name;
	// add watcher to service zk node
	// eg. serviceZpath=/soa/services/ip2city.thrift; subNodeName=member_00000001
	bool get_service_instance(string serviceZpath, string subNodeName);
	// get all services; add watcher to services root
	bool get_all_services(string _serivces_root = "/soa/services");
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

	string to_string();
#endif
	// watcher of /soa/services
	static void root_watcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx);
	// watcher of /soa/services
	static void service_watcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx);
	// watcher of /soa/services/xxx
	static void service_instance_watcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx);
	// watcher of /soa/services/xxx/member_yyy
	static void create_enode_watcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx);
	// global watcher of zk conn;
	static void global_watcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx);
	// biz staff, check whether conn is ok; NO watch
	bool check(const string& top_zk_path="/soa");
	// use for debug
	string to_string();
public:
	// simple get children; no watch, no biz
	vector<string> get_children(const string& zkpath );
	// simple get data; no watch, no biz
	string get_data(const string& zkpath);

public: // clientbase interface method imple
	//ClientBase::open
	virtual bool open();
	// now close object really, just return it to pool.
	virtual void close();

public: // state stuff
	// state, path info which is watched
	void save_state(string &path, int type, string data="");
	// receive states(watched path info) from cache, and create one watch for each of these states; used when save states from other zk client
	int take_over_states(StateList& states);
	// ATTION: maybe end-less loop until zk server can be connected.
	void restore_states();

	/*
	 * session timeout event handler
	 * method used in following situation:
	 * 1. access zk, eg. get, create, etc.
	 * 2. watcher callback
	 * when session timeout, need to do:
	 * 1. set conn status false
	 * 2. set in using status false
	 * 3.
	 */
	void on_session_timeout();
	// remove state info when zk node is remove when watcher listens message of node removed
	void remove_state(ZkState& state);

public:
	string* root_;
	const static int k_max_connect_retry_times = 3;
	const static int k_receive_time_out_us = 10 * 1000 * 1000; // in microsecond, us, 10s
	const static int k_wait_conn_timeout_us = 30 * 1000; // 30000us = 30 ms
private:
	apache::thrift::concurrency::Mutex mutex;
	RegistryCache *pcache_;
	zhandle_t *zhandle_;
	// zk conn string, format: host:port[[,host,port]...]
	string zk_hosts_;
	// zk recv_timeout in microsecond
	int timeout_;
	// states owned by pool; the states is shared by all the conn clients contained in the pool;
	// StateBag *self_states_;

	void init();
	// close zk handle
	void close_zk_handle();

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
	ClientBase* create(StateBag* states){
		return new ZkClient(this->zkhosts, this->cache, states);
	}
};

} /* namespace FinagleRegistryProxy */

#endif /* ZKCLIENT_H_ */
