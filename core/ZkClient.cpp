/*
 * ZkClient.cpp
 *
 *  Created on: Mar 18, 2014
 *      Author: asdf
 */

#include "ZkClient.h"

namespace FinagleRegistryProxy {
ZkClient::ZkClient() {
	Init();
}
ZkClient::ZkClient(string hosts, RegistryCache* pcache) {
	Init();
	this->zk_hosts_ = hosts;
	this->pcache = pcache;
}

ZkClient::~ZkClient() {
//	if (zhandle_) {
//		zookeeper_close(zhandle_);
//	}
//	this->zhandle_ = NULL;
	close_handle();
	pcache = 0;
}

void ZkClient::Init() {
	zoo_set_debug_level(ZOO_LOG_LEVEL_WARN);
	this->root = NULL;
	this->zhandle_ = NULL;
	// this->zk_hosts_ = "";
	// session timeout in ms. value may change after a server re-connect.
	this->timeout_ = ZkClient::RCEV_TIME_OUT_DEF;
}

void ZkClient::init(string hosts, RegistryCache *pcache) {
	zhandle_ = 0;
	this->zk_hosts_ = hosts;
	this->pcache = pcache;

}

void ZkClient::close_handle() {
	if (zhandle_) {
		zookeeper_close(zhandle_);
	}
	zhandle_ = NULL;
}

//connect to zk server
//#define EXPIRED_SESSION_STATE_DEF -112
//#define AUTH_FAILED_STATE_DEF -113
//#define CONNECTING_STATE_DEF 1
//#define ASSOCIATING_STATE_DEF 2
//#define CONNECTED_STATE_DEF 3
//#define NOTCONNECTED_STATE_DEF 999
void ZkClient::connect_zk() {
	if (this->get_connected())
		return;
	mutex.lock();
	if (zhandle_) {
		zookeeper_close(zhandle_);
	}
	zhandle_ = NULL;

	int count = 0;
	do {
		++count;
		zhandle_ = zookeeper_init(zk_hosts_.c_str(), global_watcher, timeout_, 0, this, 0);
		//sleep(1 * 1000);
	} while (zhandle_ == 0 && count < ZK_MAX_CONNECT_RETRY_TIMES);
#ifdef DEBUG_
	if (count >= ZK_MAX_CONNECT_RETRY_TIMES) {
		FinagleRegistryProxy::warn("ZkClient connect to zk error: , zk_hosts_=%s; retry times:=%d", zk_hosts_.c_str(), count);
#ifdef DEBUG_
		cout << "ZkClient connect to zk error: " << zk_hosts_ << " retry times: " << count << " result:"
				<< (zhandle_ != 0) << endl;
#endif
	}
#endif
	mutex.unlock();
	if (zhandle_ != 0) {
		set_connected(true);
		set_in_using(true);
	} else {
		set_connected(false);
	}
}

// {"serviceEndpoint":{"host":"asdf-laptop","port":4794},"additionalEndpoints":{},"status":"ALIVE","shard":1}
void ZkClient::update_service(string serviceZpath, string ephemeralName) {
	string path = serviceZpath + "/" + ephemeralName;
	if (!this->get_connected()) {
		connect_zk();
	}

	int buffer_len = 256;
	char *buffer = new char[buffer_len];
	for (int i = 0; i < buffer_len; i++)
		buffer[i] = 0;

	Stat stat;
	mutex.lock();
	int ret = zoo_wget(zhandle_, path.c_str(), &this->ephemeral_watcher, this, buffer, &buffer_len, &stat);
	mutex.unlock();
	if (ret == ZOK) {
		Registry reg;
		reg.name = serviceZpath;
		reg.ctime = stat.ctime / 1000;
		reg.ephemeral = ephemeralName;
		string str(buffer);
		Document d;
		d.Parse<0>(str.c_str());

		if (!d.HasParseError() && d.HasMember("serviceEndpoint")) {
			const Value &v = d["serviceEndpoint"];
			if (v.IsObject() && v.HasMember("host")) {
				reg.host = v["host"].GetString();
				if (v.HasMember("port")) {
					reg.port = v["port"].GetInt();
				}
			}
		}
		if (pcache) {
			pcache->add(reg);
		}
	} else if (ret == ZINVALIDSTATE) {
		set_connected(false);
	} else {
		FinagleRegistryProxy::warn("update_service zk_wget epheramal node error, ret=%d; msg=%s", ret,zerror(ret));
#ifdef DEBUG_
		cout << "update_service zk_wget epheramal node error, ret=" << ret << endl;
#endif
	}
	delete buffer;
	buffer = 0;
}

//update servcie list
void ZkClient::get_children(string serviceZpath) {
	watcher_fn watcher = &this->children_watcher;
	if (this->get_connected()) {
		connect_zk();
	}
	struct String_vector str_vec;
	mutex.lock();
	// zoo_wget_children return value: ZOK=0 ZNONODE=-101 ZNOAUTH=-102 ZBADARGUMENTS=-8 ZINVALIDSTATE=-9 ZMARSHALLINGERROR-5
	int ret = zoo_wget_children(zhandle_, serviceZpath.c_str(), watcher, this, &str_vec);
	mutex.unlock();

	if (ret == ZOK) {
#ifdef DEBUG_
		cout << " get_children " << serviceZpath << " result count =" << str_vec.count << endl;
#endif
		// remove before get all children
		if (pcache)
			pcache->remove(serviceZpath.c_str());
		// get service stat which contains host:port info
		for (int i = 0; i < str_vec.count; ++i) {
			update_service(serviceZpath, str_vec.data[i]);
		}
	} else if (ret == ZINVALIDSTATE) {
		set_connected(false);
	} else {
		FinagleRegistryProxy::warn("get_children zoo_wget_children error, ret=%d; msg=%s", ret,zerror(ret));
#ifdef DEBUG_
		cout << "get_children zoo_wget_children error, ret=" << ret << " msg=" << zerror(ret) << endl;
#endif
	}
}

// watcher args type
//#define CREATED_EVENT_DEF 1
//#define DELETED_EVENT_DEF 2
//#define CHANGED_EVENT_DEF 3
//#define CHILD_EVENT_DEF 4
//#define SESSION_EVENT_DEF -1
//#define NOTWATCHING_EVENT_DEF -2
// watcher args state
//#define EXPIRED_SESSION_STATE_DEF -112 // extern ZOOAPI const int ZOO_EXPIRED_SESSION_STATE;
//#define AUTH_FAILED_STATE_DEF -113	// extern ZOOAPI const int ZOO_AUTH_FAILED_STATE;
//#define CONNECTING_STATE_DEF 1		// extern ZOOAPI const int ZOO_CONNECTING_STATE;
//#define ASSOCIATING_STATE_DEF 2		// extern ZOOAPI const int ZOO_ASSOCIATING_STATE;
//#define CONNECTED_STATE_DEF 3			// extern ZOOAPI const int ZOO_CONNECTED_STATE;
//#define NOTCONNECTED_STATE_DEF 999
void ZkClient::ephemeral_watcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx) {
#ifdef DEBUG_
	cout << "ephemeral_watcher type=" << type << " state=" << state << " path:" << path << " client id="
			<< (watcherCtx ? ((ZkClient*) watcherCtx)->id() : 0) << endl;
#endif

	ZkClient *client = (ZkClient*) watcherCtx;
	if (!client)
		return;

	if (state == ZOO_EXPIRED_SESSION_STATE) {
		client->set_connected(false);
		return;
	}
	if (path) {
		client->set_in_using(true);
		string serviceZpath(path);
		unsigned index = serviceZpath.find_last_of('/');
		const string spath = "" + serviceZpath.substr(0, index);
		const string ename = "" + serviceZpath.substr(index + 1);

		switch (type) {
		case CREATED_EVENT_DEF:
		case CHANGED_EVENT_DEF:
		case CHILD_EVENT_DEF:
			client->update_service(spath, ename);
			break;
		case DELETED_EVENT_DEF:
			//client->pcache->get(spath.c_str());
			client->pcache->remove(spath, ename);
			break;
		default:

			break;
		}
		client->set_in_using(false);
	}

}

// callback method for zookeeper notifier
void ZkClient::children_watcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx) {
#ifdef DEBUG_
	cout << "children_watcher type=" << type << " state=" << state << " path:" << path << " client id="
			<< (watcherCtx ? ((ZkClient*) watcherCtx)->id() : 0) << endl;
#endif
	ZkClient *client = (ZkClient*) watcherCtx;
	if (!client) {
		return;
	}
	if (state == ZOO_EXPIRED_SESSION_STATE) {
		client->set_connected(false);
//		client->close_handle();
		return;
	}

	if (path) {
		client->set_in_using(true);
		switch (type) {
		case CREATED_EVENT_DEF:
		case CHANGED_EVENT_DEF:
		case CHILD_EVENT_DEF: {
			client->get_children(path);
			break;
		}
		case DELETED_EVENT_DEF:
			client->pcache->remove(path);
			break;
		default:
			break;
		}
		client->set_in_using(false);
	}
}

void ZkClient::global_watcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx) {
#ifdef DEBUG_
	cout << "global_watcher type=" << type << " state=" << state << " path:" << path << " client id="
			<< (watcherCtx ? ((ZkClient*) watcherCtx)->id() : 0) << endl;
#endif

	if (type == ZOO_SESSION_EVENT) {
		if (state == ZOO_CONNECTED_STATE) {
#ifdef DEBUG_
			cout << " GlobalWatcher state: connected to zookeeper service successfully!" << endl;
#endif
		} else if (state == ZOO_EXPIRED_SESSION_STATE) {
			cout << " Zookeeper session expired!" << endl;
			ZkClient *client = (ZkClient*) watcherCtx;
			if (client) {
				client->set_connected(false);
//				client->close_handle();
			}
		}
	}
}

void ZkClient::dump_stat(struct Stat *stat) {
	char tctimes[40];
	char tmtimes[40];
	time_t tctime;
	time_t tmtime;

	if (!stat) {
		fprintf(stderr, "null\n");
		return;
	}
	tctime = stat->ctime / 1000;
	tmtime = stat->mtime / 1000;

	ctime_r(&tmtime, tmtimes);
	ctime_r(&tctime, tctimes);

	fprintf(stderr,
			"\t ctime =%s \t czxid=%lx\n \t mtime=%s \t mzxid=%lx\n \tversion=%x \t version=%x \n \t ephemeralOwner = %lx\n",
			tctimes, stat->czxid, tmtimes, stat->mzxid, (unsigned int) stat->version, (unsigned int) stat->aversion,
			stat->ephemeralOwner);
}

vector<string> ZkClient::get_all(string root) {
	vector<string> ret;
	if (zhandle_ == NULL) {
		connect_zk();
	}
	//got service list
	struct String_vector str_vec;

	mutex.lock();
	// zoo_wget_children return value: ZOK=0 ZNONODE=-101 ZNOAUTH=-102 ZBADARGUMENTS=-8 ZINVALIDSTATE=-9 ZMARSHALLINGERROR-5
	int rc = zoo_wget_children(zhandle_, root.c_str(), NULL, NULL, &str_vec);
	mutex.unlock();
	if (rc == ZOK) {
		for (int i = 0; i < str_vec.count; ++i) {
			get_children(root + "/" + str_vec.data[i]);
		}
	}

	return ret;
}

} /* namespace FinagleRegistryProxy */

