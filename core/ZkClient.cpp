/*
 * ZkClient.cpp
 *
 *  Created on: Mar 18, 2014
 *      Author: asdf
 */

#include "ZkClient.h"
#include <boost/shared_ptr.hpp>

using boost::shared_ptr;

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
	Close();
}
void ZkClient::Init(string hosts, RegistryCache *pcache) {
	this->zk_hosts_ = hosts;
	this->pcache = pcache;
}
//connect to zk server
//#define EXPIRED_SESSION_STATE_DEF -112
//#define AUTH_FAILED_STATE_DEF -113
//#define CONNECTING_STATE_DEF 1
//#define ASSOCIATING_STATE_DEF 2
//#define CONNECTED_STATE_DEF 3
//#define NOTCONNECTED_STATE_DEF 999
void ZkClient::ConnectZK() {
	if (zhandle_) {
		zookeeper_close(zhandle_);
	}
	zhandle_ = NULL;

	int count = 0;
	do {
		++count;
		zhandle_ = zookeeper_init(zk_hosts_.c_str(), GlobalWatcher, timeout_, 0, NULL, 0);
#ifdef DEBUG_
		cout << "retry =" << count << "	zh status=" << zhandle_->state << endl;
#endif
		//sleep(1 * 1000);
	} while (zhandle_ == 0 && count < ZK_MAX_CONNECT_RETRY_TIMES);
#ifdef DEBUG_
	if (count >= ZK_MAX_CONNECT_RETRY_TIMES) {
		cout << "ZkClient::ConnectZK connecting to zk host: " << zk_hosts_ << " retry times: " << count << endl;
	}
#endif
}

void ZkClient::Init() {
	zoo_set_debug_level(ZOO_LOG_LEVEL_WARN);
	this->zhandle_ = NULL;
	this->zk_hosts_ = "";
	// session timeout in ms. value may change after a server re-connect.
	this->timeout_ = 1000;
}

// {"serviceEndpoint":{"host":"asdf-laptop","port":4794},"additionalEndpoints":{},"status":"ALIVE","shard":1}
void ZkClient::UpdateService(string serviceZpath, string subNodeName) {
#ifdef DEBUG_
//	long start = clock();
#endif
	string path = serviceZpath + "/" + subNodeName;
	ZkClientContext context = ZkClientContext(this, path);
	if (zhandle_ == NULL) {
		ConnectZK();
	}
	//	struct String_vector str_vec;
	int buffer_len = 256;
	char *buffer = new char[buffer_len];
	Stat stat;
	int ret = zoo_wget(zhandle_, path.c_str(), &this->EphemeralWatcher, &context, buffer, &buffer_len, &stat);
#ifdef DEBUG_
	cout << "UpdateEphemaral. path:" << serviceZpath << ", data:" << buffer << endl;
#endif
	if (ret) {
		cout << "UpdateService error, ret=" << ret << endl;
	} else {
		Registry reg;
		reg.name = serviceZpath;
		reg.ctime = stat.ctime / 1000;

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
	}
	delete buffer;
#ifdef DEBUG_
	// cout << "update Service cost " << clock() - start << "ms." << endl;
#endif
}

//update servcie list
void ZkClient::UpdateServices(string serviceZpath) {
	watcher_fn watcher = &this->ChildrenWatcher;
//	ZkClientContext context = ZkClientContext(this, serviceZpath);

	if (zhandle_ == NULL) {
		ConnectZK();
	}
	//got service list
	struct String_vector str_vec;

// zoo_wget_children return value: ZOK=0 ZNONODE=-101 ZNOAUTH=-102 ZBADARGUMENTS=-8 ZINVALIDSTATE=-9 ZMARSHALLINGERROR-5
//	int ret = zoo_wget_children(zhandle_, context.get()->serviceZpath.c_str(), watcher, context, &str_vec);
	int ret = zoo_wget_children(zhandle_, serviceZpath.c_str(), watcher, this, &str_vec);
#ifdef DEBUG_
	if (ret) {
		cout << "zk_wget_children fail :" << serviceZpath << " wrong code= " << ret << " msg=" << zerror(ret) << endl;
	}
	cout << " UpdateServices " << serviceZpath << " result count =" << str_vec.count << endl;
#endif
	if (ret == ZOK) {
		// remove before get all children
		if (pcache)
			pcache->remove(serviceZpath);
		// get service stat which contains host:port info
		for (int i = 0; i < str_vec.count; ++i) {
			UpdateService(serviceZpath, str_vec.data[i]);
		}
	}
}

// type
//#define CREATED_EVENT_DEF 1
//#define DELETED_EVENT_DEF 2
//#define CHANGED_EVENT_DEF 3
//#define CHILD_EVENT_DEF 4
//#define SESSION_EVENT_DEF -1
//#define NOTWATCHING_EVENT_DEF -2
void ZkClient::EphemeralWatcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx) {
//	ZkClientContext *context = (ZkClientContext*) watcherCtx;
	ZkClient *client = (ZkClient*) watcherCtx;
#ifdef DEBUG_
	cout << "EphemeralWatcher type=" << type << " state=" << state << " path:" << path << endl;
#endif
	if (client && path) {
		if (state == ZOO_EXPIRED_SESSION_STATE) {
			client->ConnectZK();
		}

		string serviceZpath(path);
		unsigned index = serviceZpath.find_last_of('/');
		const string spath = "" + serviceZpath.substr(0, index);
		const string ename = "" + serviceZpath.substr(index + 1);
		cout << "spath & ename =" << spath << "                " << ename << endl;

		switch (type) {
		case CREATED_EVENT_DEF:
		case CHANGED_EVENT_DEF:
		case CHILD_EVENT_DEF:
			client->UpdateService(spath, ename);
			break;
		case DELETED_EVENT_DEF:
			client->pcache->remove(spath, ename);
			break;
		default:

			break;
		}
	}
}

// callback method for zookeeper notifier
void ZkClient::ChildrenWatcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx) {

#ifdef DEBUG_
	cout << "ChildrenWatcher type=" << type << " state=" << state << " path:" << path << endl;
#endif
//	 ZkClientContext *context = (ZkClientContext*)watcherCtx;
//	ZkClientContext *context = ((shared_ptr<ZkClientContext> ) watcherCtx).px;
	ZkClient *client = (ZkClient*) watcherCtx;

	if (client && path) {
		if (state == ZOO_EXPIRED_SESSION_STATE) {
			client->ConnectZK();
		}

		switch (type) {
		case CREATED_EVENT_DEF:
		case CHANGED_EVENT_DEF:
		case CHILD_EVENT_DEF: {
			client->UpdateServices(path);
			break;
		}
		case DELETED_EVENT_DEF:
			client->pcache->remove(path);
			break;
		default:
			if (state == ZOO_EXPIRED_SESSION_STATE) {
				client->ConnectZK();
			}
			break;
		}
	}
}

void ZkClient::GlobalWatcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx) {
#ifdef DEBUG_
	cout << "GlobalWatcher type=" << type << " state=" << state << " path:" << path << endl;
#endif

	if (type == ZOO_SESSION_EVENT) {
		if (state == ZOO_CONNECTED_STATE) {
#ifdef DEBUG_
			cout << "Connected to zookeeper service successfully!" << endl;
#endif
		} else if (state == ZOO_EXPIRED_SESSION_STATE) {
			cout << "Zookeeper session expired!" << endl;
			// reconnect
			if (watcherCtx && ((ZkClientContext*) watcherCtx)->client)
				((ZkClientContext*) watcherCtx)->client->ConnectZK();
		}
	}
}

void ZkClient::DumpStat(struct Stat *stat) {
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
void ZkClient::Close() {
	if (zhandle_) {
		zookeeper_close(zhandle_);
	}
	this->zhandle_ = NULL;
}

vector<string> ZkClient::getChildren(string root) {
	vector<string> ret;
	if (zhandle_ == NULL) {
		ConnectZK();
	}
	//got service list
	struct String_vector str_vec;

	// zoo_wget_children return value: ZOK=0 ZNONODE=-101 ZNOAUTH=-102 ZBADARGUMENTS=-8 ZINVALIDSTATE=-9 ZMARSHALLINGERROR-5
	int rc = zoo_wget_children(zhandle_, root.c_str(), NULL, NULL, &str_vec);
	if (rc == ZOK) {
		for (int i = 0; i < str_vec.count; ++i) {
			UpdateServices(root + "/" + str_vec.data[i]);
		}
	}

	return ret;
}

} /* namespace FinagleRegistryProxy */

