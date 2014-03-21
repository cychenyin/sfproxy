/*
 * ZkClient.cpp
 *
 *  Created on: Mar 18, 2014
 *      Author: asdf
 */

#include "ZkClient.h"

namespace FinagleRegistryProxy {

ZkClient::ZkClient(string hosts, RegistryCache* pcache) {
	Init();
	this->zk_hosts_ = hosts;
	this->pcache = pcache;
}

ZkClient::~ZkClient() {
	Close();
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
void ZkClient::UpdateService(string serviceName, string subNodeName) {

	long start = time(NULL);
	string path = serviceName + "/" + subNodeName;
	ZkClientContext context = ZkClientContext(this, path);
	if (zhandle_ == NULL) {
		ConnectZK();
	}
	//	struct String_vector str_vec;
	int buffer_len = 256;
	char *buffer = new char[buffer_len];
	Stat stat;
	int ret = zoo_wget(zhandle_, path.c_str(), &this->GetWatcher, &context, buffer, &buffer_len, &stat);
#ifdef DEBUG_
	cout << "Update --> path:" << path << ", data:" << buffer << endl;
#endif
	if (ret) {
		cout << "UpdateService error, ret=" << ret << endl;
	} else {
		Registry reg;
		reg.name = serviceName;
		reg.ctime = stat.ctime / 1000;

		string str(buffer);
		Document d;
		d.Parse<0>(str.c_str());

		if (!d.HasParseError() && d.HasMember("serviceEndpoint")) {
			const Value &v = d["serviceEndpoint"];
			if (v.IsObject() && v.HasMember("host")) {
				reg.host = v["host"].GetString();
//				cout << "	host: " << reg.host << endl;
				if (v.HasMember("port")) {
					reg.port = v["port"].GetInt();
//					cout << "	port: " << reg.port << endl;
				}
			}
		}
		if(pcache) {
			pcache->add(reg);
		}
//		cout << "now=" << time(NULL) << "\tctime=" << stat.ctime / 1000 << "\tinteval="
//				<< time(NULL) - stat.ctime / 1000 << endl;
	}
	delete buffer;
	cout << "update Service cost " << time(NULL) - start << "ms." << endl;
}

//update servcie list
void ZkClient::UpdateServices(string serviceName) {
	watcher_fn watcher = &this->ChildrenWatcher;
	ZkClientContext context = ZkClientContext(this, serviceName);
	cout << "ZkClient::Update =============================================================" << endl;
	if (zhandle_ == NULL) {
		ConnectZK();
	}
	//got service list
	struct String_vector str_vec;
	int ret = zoo_wget_children(zhandle_, context.serviceName.c_str(), watcher, &context, &str_vec);
	if (ret) {
		cout << str_vec.data << endl;
		cout << "Update fail to read path:" << context.serviceName << " wrong code= " << ret << " msg=" << zerror(ret)
				<< endl;
	}
#ifdef DEBUG_
	cout << " UpdateServices " << serviceName << " result count =" << str_vec.count << endl;
#endif

	// get service stat which contains host:port info
	for (int i = 0; i < str_vec.count; ++i) {
		UpdateService(context.serviceName, str_vec.data[i]);
	}
}

//#define CREATED_EVENT_DEF 1
//#define DELETED_EVENT_DEF 2
//#define CHANGED_EVENT_DEF 3
//#define CHILD_EVENT_DEF 4
//#define SESSION_EVENT_DEF -1
//#define NOTWATCHING_EVENT_DEF -2
void ZkClient::GetWatcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx) {
	ZkClientContext *context = (ZkClientContext*) watcherCtx;
	cout << "GetWatcher path:" << path << endl;
	if (context && context->client) {
		cout << "service name = " << context->serviceName << endl;
		context->client->UpdateServices(context->serviceName);
	}
}

// callback method for zookeeper notifier
void ZkClient::ChildrenWatcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx) {
	cout << "ChildrenWatcher type:" << type << "	watcher ZOO_CHILD_EVENT:" << ZOO_CHILD_EVENT << endl;
	cout << "ChildrenWatcher state:" << state << endl;
	cout << "ChildrenWatcher path:" << path << endl;

	ZkClientContext *context = (ZkClientContext*) watcherCtx;
	if (context && context->client) {
		cout << "service name = " << context->serviceName << endl;
		context->client->UpdateServices(context->serviceName);
	}
//	if (ZOO_CHILD_EVENT == type) {
//	}
}
void ZkClient::GlobalWatcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx) {
	if (type == ZOO_SESSION_EVENT) {
		if (state == ZOO_CONNECTED_STATE) {
			cout << "Connected to zookeeper service successfully!" << endl;
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

ZkClientContext::ZkClientContext() {
	client = NULL;
}
ZkClientContext::ZkClientContext(ZkClient *client, string serviceName) {
	this->client = client;
	this->serviceName = serviceName;
}

ZkClientContext::~ZkClientContext() {
	this->client = NULL;
}

} /* namespace FinagleRegistryProxy */

