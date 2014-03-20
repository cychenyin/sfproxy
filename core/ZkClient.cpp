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
ZkClient::ZkClient(string hosts) {
	Init();
	this->zk_hosts_ = hosts;
//	cout << "hosts addr=" << &hosts << " value=" << hosts << endl;
//	cout << "zk_ho addr=" << &zk_hosts_ << " value=" << zk_hosts_ << endl;
//	cout << "who is who: " << (zk_hosts_ == hosts) << endl;
}
ZkClient::ZkClient(zhandle_t *zh) {
	Init();
	this->zhandle_ = zh;
}

ZkClient::~ZkClient() {
	Close();
//	this->connected_ = false;
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
//	connected_ = false;

	int count = 0;
	do {
		++count;
		zhandle_ = zookeeper_init(zk_hosts_.c_str(), GlobalWatcher, timeout_, 0, NULL, 0);
		cout << "retry =" << count << "	zh status=" << zhandle_->state << endl;
//		connected_ = true;
		//sleep(1 * 1000);
	} while (zhandle_ == 0 && count < ZK_MAX_CONNECT_RETRY_TIMES);

	if (count >= ZK_MAX_CONNECT_RETRY_TIMES) {
		cout << "ZkClient::Init --> connecting zookeeper host: " << zk_hosts_ << " over times: " << count << endl;
	}
}

void ZkClient::Init() {
	zoo_set_debug_level(ZOO_LOG_LEVEL_WARN);
	this->zhandle_ = NULL;
//	this->connected_ = false;
	this->zk_hosts_ = "";
	// session timeout in ms. value may change after a server re-connect.
	this->timeout_ = 1000;
}

// ZOOAPI int zoo_wget(zhandle_t *zh, const char *path,watcher_fn watcher, void* watcherCtx, char *buffer, int* buffer_len, struct Stat *stat);
void ZkClient::UpdateService(string path) {
	ZkClientContext context = ZkClientContext(this, path);
	if (zhandle_ == NULL) {
		ConnectZK();
	}
	//	struct String_vector str_vec;
	int buffer_len = 256;
	char *buffer = new char[buffer_len];
	Stat stat;
	int ret = zoo_wget(zhandle_, path.c_str(), &this->GetWatcher, &context, buffer, &buffer_len, &stat);
	cout << "Update --> path:" << path << ", data:" << buffer << endl;
	if (ret) {
		cout << "UpdateService error, ret=" << ret << endl;
	} else {
		cout << "		path" << path <<  "=" << buffer << endl;
	}
}

//update servcie list
void ZkClient::UpdateServices(string serviceName) {
	watcher_fn watcher = &this->ChildrenWatcher;
	ZkClientContext context = ZkClientContext(this, serviceName);
	cout << "ZkClient::Update =============================================================" << endl;
	if (zhandle_ == NULL) {
		//Init();
		ConnectZK();
	}
	//got service list
	struct String_vector str_vec;
	int ret = zoo_wget_children(zhandle_, context.serviceName.c_str(), watcher, &context, &str_vec);
	if (ret) {
		cout << str_vec.data << endl;
		cout << "Update fail to read path:" << context.serviceName << " wrong code= " << ret << " msg=" << zerror(ret)
				<< endl;
		//return;
	}
	cout << " UpdateServices " << serviceName << " result count =" << str_vec.count << endl;
	// get service stat which contains host:port info
	for (int i = 0; i < str_vec.count; ++i) {
		struct String_vector node_vec;
		string path = context.serviceName + "/" + str_vec.data[i];
		cout << "		[" << i << "]=" << path << endl;
//		int ret = zoo_wget_children(zhandle_, path.c_str(), watcher, &context, &node_vec);
//		cout << "Update --> path:" << path << ", ret:" << ret << ", node's size:" << node_vec.count << endl;
		UpdateService(path);
		// TODO ....
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
			if(watcherCtx && ((ZkClientContext*)watcherCtx)->client)
				((ZkClientContext*)watcherCtx)->client->ConnectZK();
		}
	}
}
void ZkClient::DumpStat(struct Stat *stat)
{
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

    fprintf(stderr, "\t ctime =%s \t czxid=%lx\n \t mtime=%s \t mzxid=%lx\n \tversion=%x \t version=%x \n \t ephemeralOwner = %lx\n",
            tctimes, stat->czxid,
            tmtimes, stat->mzxid,
            (unsigned int) stat->version, (unsigned int) stat->aversion,
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

