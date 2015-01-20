/*
 * ZkClient.cpp
 *
 *  Created on: Mar 18, 2014
 *      Author: asdf
 */

#include "ZkClient.h"
#include <exception>

namespace FinagleRegistryProxy {

ZkClient::ZkClient() {
	init();
}
ZkClient::ZkClient(string hosts, RegistryCache* pcache) {
	init();
	this->zk_hosts_ = hosts;
	this->pcache_ = pcache;
}

ZkClient::~ZkClient() {
	close_handle();
	pcache_ = 0;
	// tips: states_ need not to be destroyed because when client became invalid, the states are transfered to new client instance, even the new one is not ready.
}

// valuate fields
void ZkClient::init() {
	zoo_set_debug_level((ZooLogLevel) 0);
//	zoo_set_debug_level(ZOO_LOG_LEVEL_ERROR);
	this->root_ = NULL;
	this->zhandle_ = NULL;
	// this->zk_hosts_ = "";
	// session timeout in ms. value may change after a server re-connect.
	this->timeout_ = ZkClient::RCEV_TIME_OUT_DEF;
}

bool ZkClient::open() {
	if (this->get_connected()) {
		this->set_in_using(true);
		return true;
	} else
		return this->connect_zk();
}

//ClientBase::close interface
void ZkClient::close() {
	this->set_in_using(false);
}

inline void ZkClient::close_handle() {
	if (zhandle_) {
		zookeeper_close(zhandle_);
	}
	zhandle_ = NULL;
}

//connect to zk server
///* zookeeper state constants */
//#define EXPIRED_SESSION_STATE_DEF -112
//#define AUTH_FAILED_STATE_DEF -113
//#define CONNECTING_STATE_DEF 1
//#define ASSOCIATING_STATE_DEF 2
//#define CONNECTED_STATE_DEF 3
//#define NOTCONNECTED_STATE_DEF 999
//
///* zookeeper event type constants */
//#define CREATED_EVENT_DEF 1
//#define DELETED_EVENT_DEF 2
//#define CHANGED_EVENT_DEF 3
//#define CHILD_EVENT_DEF 4
//#define SESSION_EVENT_DEF -1
//#define NOTWATCHING_EVENT_DEF -2
bool ZkClient::connect_zk() {
	if (this->get_connected())
		return true;
	mutex.lock();
	if (zhandle_) {
		zookeeper_close(zhandle_);
		cout << "zookeeper_close *************************************" << zhandle_ << endl;
	}
	zhandle_ = NULL;

	int retry = 0;
	do {
		++retry;
		zhandle_ = zookeeper_init(zk_hosts_.c_str(), global_watcher, timeout_, 0, this, 0);
		cout << "zookeeper_init *************************************" << zhandle_ << endl;

		// 999 is a valid status too.
		// (zhandle_->state == CONNECTED_STATE_DEF || zhandle_->state == ASSOCIATING_STATE_DEF || zhandle_->state == CONNECTING_STATE_DEF))
		if (zhandle_ != 0 && zhandle_->state > 0) {
			this->set_session_id(zhandle_->client_id.client_id);
		} else {
			this->close_handle();
		}
		//sleep(1 * 1000);
	} while (zhandle_ == 0 && retry < ZK_MAX_CONNECT_RETRY_TIMES);
#ifdef DEBUG_
#endif
	if (retry >= ZK_MAX_CONNECT_RETRY_TIMES) {
		logger::warn("ZkClient fail to connect to zk : zk_hosts_=%s; retry times:=%d", zk_hosts_.c_str(), retry);
		cout << "ZkClient fail to connect to zk=" << zk_hosts_ << " exceed retry times." << retry << endl;
	}
	mutex.unlock();
	if (zhandle_ != 0) {
		set_connected(true);
		set_in_using(true);
		restore_states();
		return true;
	} else {
		set_connected(false);
		return false;
	}
}

// accepts states and add watches
int ZkClient::set_states(StateMap *states) {
	for (map<string, ClientState*>::iterator it = states->begin(); it != states->end(); it++) {
		// old code bak
		// ZkState *state = (ZkState *) (it->second);
		ZkState* state = dynamic_cast<ZkState*>(it->second);
		if (state) {
			this->states_.erase(it->first);
			this->states_.insert(pair<string, ClientState*>(it->first, it->second));
			state->client_id = this->id();
			if (state->type == ZkState::TYPE_CREATE_EPHERERAL) {
				this->create_enode(state->path, state->data);
			} else if (state->type == ZkState::TYPE_GET_SERVICE) {
				this->get_service(state->path);
			} else if (state->type == ZkState::TYPE_GET_NODE) {
				this->get_service_instance(state->path);
			} else if (state->type == ZkState::TYPE_GET_ROOT) {
				this->get_all_services(state->path);
			}
		}
	}

	return 0;
}

void ZkClient::restore_states() {
	for (map<string, ClientState*>::iterator it = this->states_.begin(); it != states_.end(); it++) {
		ZkState* state = dynamic_cast<ZkState*>(it->second);
		if (state->type == ZkState::TYPE_CREATE_EPHERERAL) {
			this->create_enode(state->path, state->data);
		} else if (state->type == ZkState::TYPE_GET_SERVICE) {
			this->get_service(state->path);
		} else if (state->type == ZkState::TYPE_GET_NODE) {
			this->get_service_instance(state->path);
		} else if (state->type == ZkState::TYPE_GET_ROOT) {
			this->get_all_services(state->path);
		}
	}
}

void ZkClient::save_state(string &path, int type) {
//	return ;
	string key = ZkState::create_key(path, type);
	StateMap::iterator it = this->states_.find(path);
	if (it == this->states_.end()) {
		ZkState *state = new ZkState();
		state->client_id = this->id();
		state->path = path;
		state->type = type;
		this->states_.insert(pair<string, ClientState*>(key, state));
	}
}

// eg. get_node("/soa/service/rta.counter.thrift/member0000001")
bool ZkClient::get_service_instance(string path) {
	bool ret = true;
	int last = -1;
	for (int i = path.size() - 1; i > -1; i--) {
		if (path[i] == '/' && last == -1) {
			last = i;
			break;
		}
	}
	if (last > -1) {
		string serviceZpath = path.substr(0, last);
		string ephemeralName = path.substr(last);
		ret = get_service_instance(serviceZpath, ephemeralName);
	}
	return ret;
}

// eg. get_node("soa/serivces/rta.counter.thrift", "member00000001")
// {"serviceEndpoint":{"host":"asdf-laptop","port":4794},"additionalEndpoints":{},"status":"ALIVE","shard":1}
bool ZkClient::get_service_instance(string serviceZpath, string ephemeralName) {
	bool ret = true;
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
	int res = zoo_wget(zhandle_, path.c_str(), &this->service_instance_watcher, this, buffer, &buffer_len, &stat);
	this->save_state(path, ZkState::TYPE_GET_NODE);
	mutex.unlock();
	if (res == ZOK) {
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
		if (pcache_) {
			pcache_->add(reg);
		}
	} else if (ret == ZNONODE) {
		remove_state(new ZkState(serviceZpath, ZkState::TYPE_GET_NODE));
		ret = false;
	} else if (ret == ZINVALIDSTATE) {
		this->on_session_timeout();
		ret = false;
	} else {
		logger::warn("get_node zk_wget epheramal node error, ret=%d; msg=%s", ret, zerror(ret));
	}
	delete buffer;
	buffer = 0;
	return ret;
}

void ZkClient::remove_state(ZkState* state) {
#ifdef DEBUG_
	cout << " remove_state" << state->path << state->type << endl;
#endif
	ZkState* p = 0;
	string key = state->key();
	StateMap::iterator find_it = states_.find(key);
	if (find_it != states_.end()) {
		p = (ZkState*) (find_it->second);
		states_.erase(find_it);
		delete p;
		p = 0;
	}
	delete state;
	state = 0;
}
//update servcie list. serviceZpath eg. /sao/services/test.xx
bool ZkClient::get_service(string serviceZpath) {
	bool ret = true;
	if (!this->get_connected()) {
		connect_zk();
	}
	struct String_vector str_vec;
	mutex.lock();
	// zoo_wget_children return value: ZOK=0 ZNONODE=-101 ZNOAUTH=-102 ZBADARGUMENTS=-8 ZINVALIDSTATE=-9 ZMARSHALLINGERROR-5
	int res = zoo_wget_children(zhandle_, serviceZpath.c_str(), &this->service_watcher, this, &str_vec);
	this->save_state(serviceZpath, ZkState::TYPE_GET_SERVICE);
	mutex.unlock();

	if (res == ZOK) {
		if (str_vec.count > 0) {
			// remove before get all children
			if (pcache_)
				pcache_->remove(serviceZpath.c_str());
			// get service stat which contains host:port info
			for (int i = 0; i < str_vec.count; ++i) {
				get_service_instance(serviceZpath, str_vec.data[i]);
				delete str_vec.data[i];
			}
			delete str_vec.data;
		}
	} else if (ret == ZNONODE) {
		remove_state(new ZkState(serviceZpath, ZkState::TYPE_GET_SERVICE));
		ret = false;
	} else if (ret == ZINVALIDSTATE) {
		this->on_session_timeout();
		ret = false;
	} else {
		ret = false;
		logger::warn("get_children zoo_wget_children error, ret=%d; msg=%s; path=%s", ret, zerror(ret), serviceZpath.c_str());
	}
	return ret;
}

// create permanent node
int ZkClient::create_pnode(string abs_path, bool log_when_exists) {
	if (!this->get_connected()) {
		connect_zk();
	}
	mutex.lock();
	// return value: ZOK=0 ZNONODE=-101 ZNOAUTH=-102 ZBADARGUMENTS=-8 ZINVALIDSTATE=-9 ZMARSHALLINGERROR-5
	int ret = zoo_create(zhandle_, abs_path.c_str(), NULL, 0, &ZOO_OPEN_ACL_UNSAFE, 0, NULL, 0);
	mutex.unlock();

	if (ret == ZOK) {
#ifdef DEBUG_
		cout << "create_pnode create persist node successfully. path:" << abs_path << endl;
#endif

	} else if (ret == ZINVALIDSTATE) {
		this->set_connected(false);
	} else if (ret == ZNODEEXISTS) {
		if (log_when_exists)
			logger::info("create_pnode zoo_create failure, ret=%d; msg=%s path=%s", ret, zerror(ret), abs_path.c_str());
	} else {
		logger::warn("create_pnode zoo_create error, ret=%d; msg=%s path=%s", ret, zerror(ret), abs_path.c_str());
	}
	return ret;
}
// creaet ephermeral node
int ZkClient::create_enode(string abs_path, string data) {
	if (!this->get_connected()) {
		connect_zk();
	}
	mutex.lock();
	// return value: ZOK=0 ZNONODE=-101 ZNOAUTH=-102 ZBADARGUMENTS=-8 ZINVALIDSTATE=-9 ZMARSHALLINGERROR-5
	int ret = zoo_create(zhandle_, abs_path.c_str(), data.c_str(), data.length(), &ZOO_OPEN_ACL_UNSAFE, ZOO_EPHEMERAL, 0, 0);

	mutex.unlock();
	if (ret == ZOK) {
#ifdef DEBUG_
		cout << " create_enode create ephemeral node successfully. path:" << abs_path << endl;
#endif
//	} else if (ret == ZNODEEXISTS) {
//		remove_state(&ZkState(abs_path, ZkState::TYPE_CREATE_EPHERERAL));
	} else if (ret == ZINVALIDSTATE) {
		this->on_session_timeout();
	} else {
#ifdef DEBUG_
		logger::warn("create_enode zoo_create error, ret=%d; msg=%s path=%s", ret, zerror(ret), abs_path.c_str());
#endif
	}
	mutex.lock();
	ret = zoo_wget(zhandle_, abs_path.c_str(), &this->create_enode_watcher, this, NULL, 0, NULL);
	this->save_state(abs_path, ZkState::TYPE_CREATE_EPHERERAL);
	mutex.unlock();
	return ret;
}

void ZkClient::create_enode_watcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx) {
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
			break;
		case DELETED_EVENT_DEF:
			client->set_connected(false);
			break;
		default:
			break;
		}
		client->set_in_using(false);
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
void ZkClient::service_instance_watcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx) {
//	cout << "ephemeral_watcher type=" << type << " state=" << state << " path:" << path << endl;
#ifdef DEBUG_
	cout << "ephemeral_watcher type=" << type << " state=" << state << " path:" << path << " client id="
			<< (watcherCtx ? ((ZkClient*) watcherCtx)->id() : 0) << endl;
#endif
	ZkClient *client = (ZkClient*) watcherCtx;
	if (!client)
		return;
	if (state == ZOO_EXPIRED_SESSION_STATE) {
		client->on_session_timeout();
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
			client->get_service_instance(spath, ename);
			break;
		case DELETED_EVENT_DEF:
			//client->pcache->get(spath.c_str());
			if (client->pcache_)
				client->pcache_->remove(spath, ename);
			client->remove_state(new ZkState(path, ZkState::TYPE_GET_NODE));
			break;
		default:
			break;
		}
		client->set_in_using(false);
	}

}

// callback method for zookeeper notifier
void ZkClient::root_watcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx) {
//	cout << "root_watcher type=" << type << " state=" << state << " path:" << path << endl;
#ifdef DEBUG_
	cout << " client id=" << (watcherCtx ? ((ZkClient*) watcherCtx)->id() : 0) << endl;
#endif
	ZkClient *client = (ZkClient*) watcherCtx;
	if (!client) {
		return;
	}
	if (state == ZOO_EXPIRED_SESSION_STATE) {
		client->on_session_timeout();
		return;
	}

	if (path) {
		client->set_in_using(true);
		switch (type) {
		case CREATED_EVENT_DEF:
		case CHANGED_EVENT_DEF:
		case CHILD_EVENT_DEF: {
			// re watch
			if (client) {
				client->get_all_services(string(path));
//				vector<string> services = client->get_all_services(string(path));
//				vector<string>::iterator it = services.begin();
//				while (it != services.end()) {
//					string zkpath = *it;
//					if (client && client->pcache_ && client->pcache_->exists(zkpath) == false) {
//						client->get_service(zkpath);
//					}
//					++it;
//				}
			}
			break;
		}
		case DELETED_EVENT_DEF:
			if (client->pcache_)
				client->pcache_->remove(path);
			client->remove_state(new ZkState(path, ZkState::TYPE_GET_SERVICE));
			break;
		default:
			break;
		}
		client->set_in_using(false);
	}
}

// callback method for zookeeper notifier
void ZkClient::service_watcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx) {
//	cout << "children_watcher type=" << type << " state=" << state << " path:" << path << endl;
#ifdef DEBUG_
	cout << "children_watcher type=" << type << " state=" << state << " path:" << path << " client id="
			<< (watcherCtx ? ((ZkClient*) watcherCtx)->id() : 0) << endl;
#endif
	ZkClient *client = (ZkClient*) watcherCtx;
	if (!client) {
		return;
	}
	if (state == ZOO_EXPIRED_SESSION_STATE) {
		client->on_session_timeout();
		return;
	}

	if (path) {
		client->set_in_using(true);
		switch (type) {
		case CREATED_EVENT_DEF:
		case CHANGED_EVENT_DEF:
		case CHILD_EVENT_DEF: {
			client->get_service(path);
			break;
		}
		case DELETED_EVENT_DEF:
			if (client->pcache_)
				client->pcache_->remove(path);
			client->remove_state(new ZkState(path, ZkState::TYPE_GET_SERVICE));
			break;
		default:
			break;
		}
		client->set_in_using(false);
	}
}

void ZkClient::global_watcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx) {
//	cout << "global_watcher type=" << type << " state=" << state << " path:" << path << endl;
#ifdef DEBUG_
	cout << "global_watcher type=" << type << " state=" << state << " path:" << path << " client id="
			<< (watcherCtx ? ((ZkClient*) watcherCtx)->id() : 0) << " zh=" << zh << endl;

	if (state == ZOO_CONNECTED_STATE) {
		cout << " GlobalWatcher state: connected to zookeeper service successfully!" << endl;
	}
#endif
	if (type == ZOO_SESSION_EVENT && state == ZOO_EXPIRED_SESSION_STATE) {
		logger::warn("Zookeeper session expired. session_id: %d", ((ZkClient*) watcherCtx)->id());
		ZkClient *client = (ZkClient*) watcherCtx;
		if (client) {
			client->on_session_timeout();
		}
	}
}

// get all services from service root; return data list;  eg. _serivces_root=/soa/services
bool ZkClient::get_all_services(string _serivces_root) {
	bool ret = true;
	vector<string> list;
	if (zhandle_ == NULL) {
		connect_zk();
	}
	//got service list
	struct String_vector str_vec;

	mutex.lock();
	// zoo_wget_children return value: ZOK=0 ZNONODE=-101 ZNOAUTH=-102 ZBADARGUMENTS=-8 ZINVALIDSTATE=-9 ZMARSHALLINGERROR-5
	int rc = zoo_wget_children(zhandle_, _serivces_root.c_str(), &this->root_watcher, this, &str_vec);
	mutex.unlock();
	this->save_state(_serivces_root, ZkState::TYPE_GET_ROOT);
	if (rc == ZOK) {
		for (int i = 0; i < str_vec.count; ++i) {
			list.push_back(_serivces_root + "/" + str_vec.data[i]);
			// get_children(root + "/" + str_vec.data[i]);
			delete str_vec.data[i];
		}
		delete str_vec.data;
	} else {
		ret = false;
	}

	vector<string>::iterator it = list.begin();
	while (it != list.end()) {
		string zkpath = *it;
		//  read all arbitrarily anyway
		// if (client && client->pcache_ && client->pcache_->exists(zkpath) == false) {
		ret = ret && get_service(zkpath);
		// }
		++it;
	}
	return ret;
}

void ZkClient::on_session_timeout() {
	this->set_connected(false);
	if (this->states_.size() > 0) {
		int retry = 0;
		this->connect_zk();
		while (!this->get_connected()) {
			retry++;
			// usleep(100000);
			sleep(1);
			this->connect_zk();
		}
		if (!this->get_connected()) {
			for (StateMap::iterator it = states_.begin(); it != states_.end(); it++) {
				ZkState *state = (ZkState *) (it->second);
				if (state) {
					if (state->type == ZkState::TYPE_CREATE_EPHERERAL) {
						this->create_enode(state->path, state->data);
					} else if (state->type == ZkState::TYPE_GET_SERVICE) {
						this->get_service(state->path);
					} else if (state->type == ZkState::TYPE_GET_NODE) {
						this->get_service_instance(state->path);
					}
				}
			}
		}
	}
}

bool ZkClient::check(const string& top_zk_path) {
	int ret = false;
	if (!this->get_connected()) {
		connect_zk();
	}

	int buffer_len = 256;
	char *buffer = new char[buffer_len];
	for (int i = 0; i < buffer_len; i++)
		buffer[i] = 0;

	Stat stat;
	mutex.lock();
	// zoo_get(zhandle_t *zh, const char *path, int watch, char *buffer,  int* buffer_len, struct Stat *stat);
	int res = zoo_get(zhandle_, top_zk_path.c_str(), 0, buffer, &buffer_len, NULL);
	mutex.unlock();
	delete[] buffer;
	buffer = 0;
	return res == ZOK;
}

vector<string> ZkClient::get_children(const string& zkpath) {
	vector<string> list;
	if (zhandle_ == NULL) {
		connect_zk();
	}
	struct String_vector str_vec;
	mutex.lock();
	// zoo_get_children return value: ZOK=0 ZNONODE=-101 ZNOAUTH=-102 ZBADARGUMENTS=-8 ZINVALIDSTATE=-9 ZMARSHALLINGERROR-5
	int rc = zoo_get_children(zhandle_, zkpath.c_str(), 0, &str_vec);
	mutex.unlock();
	if (rc == ZOK) {
		for (int i = 0; i < str_vec.count; ++i) {
			list.push_back(zkpath + "/" + str_vec.data[i]);
			// get_children(root + "/" + str_vec.data[i]);
			delete str_vec.data[i];
		}
		delete str_vec.data;
	}
	return list;
}

string ZkClient::get_data(const string& zkpath) {
	string ret;
	if (zhandle_ == NULL) {
		connect_zk();
	}
	int buffer_len = 256;
	char *buffer = new char[buffer_len];
	for (int i = 0; i < buffer_len; i++)
		buffer[i] = 0;

	Stat stat;
	mutex.lock();
	// (zhandle_t *zh, const char *path, int watch, char *buffer, int* buffer_len, struct Stat *stat);
	int res = zoo_get(zhandle_, zkpath.c_str(), 0, buffer, &buffer_len, 0);
	mutex.unlock();
	if (res == ZOK) {
		ret = string(buffer);
	}
	return ret;
}

string ZkClient::to_string() {
	stringstream ss;
	ss << this << "\t" << zhandle_;
	return ss.str();
}

} /* namespace FinagleRegistryProxy */

