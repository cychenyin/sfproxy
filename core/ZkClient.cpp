/*
 * ZkClient.cpp
 *
 *  Created on: Mar 18, 2014
 *      Author: asdf
 */

#include "ZkClient.h"
#include <exception>

namespace FinagleRegistryProxy {

ZkClient::ZkClient() :
		ClientBase(NULL) {
	init();
}
ZkClient::ZkClient(string hosts, RegistryCache* pcache, StateBag *shared_states) :
		ClientBase(shared_states) {
	init();
	this->zk_hosts_ = hosts;
	this->pcache_ = pcache;
}

ZkClient::~ZkClient() {
	close_zk_handle();
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
	this->timeout_ = ZkClient::k_receive_time_out_us;
}

bool ZkClient::open() {
	if (this->get_connected()) {
		this->set_in_using(true);
		return true;
	} else {
		return this->connect_zk();
	}
}

//ClientBase::close interface
void ZkClient::close() {
	this->set_in_using(false);
}

inline void ZkClient::close_zk_handle() {
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
	cout << "connect_zk ing" << endl;
	if (this->get_connected())
		return true;
	mutex.lock();
	close_zk_handle();

	int retry = 0;
	do {
		++retry;
		uint64_t start = utils::unix_time();
		zhandle_ = zookeeper_init(zk_hosts_.c_str(), global_watcher, timeout_, 0, this, 0);

		// 999 is a valid status too.
		// (zhandle_->state == CONNECTED_STATE_DEF || zhandle_->state == ASSOCIATING_STATE_DEF || zhandle_->state == CONNECTING_STATE_DEF))
#ifdef DEBUG_
		if(zhandle_)
			cout << "zookeeper_init calledddddddddddddd cost: " << utils::unix_time() - start << " zk conn status:" << zhandle_->state << endl;
#endif
		uint64_t wait = utils::now_in_us();
		// wait for status ready
		while(zhandle_ != 0 && utils::now_in_us() - wait < ZkClient::k_wait_conn_timeout_us) {
			int status = zhandle_->state;
			if(status == ASSOCIATING_STATE_DEF || status == CONNECTING_STATE_DEF) {
				usleep(1 * 1000); // 1 ms
			} else
				break;
		}
		// if not ready, close it, and retry next time
		if(zhandle_ == 0 || zhandle_->state != CONNECTED_STATE_DEF) {
			this->close_zk_handle();
		}
	} while (zhandle_ == 0 && retry < k_max_connect_retry_times);
	if (retry >= k_max_connect_retry_times) {
		logger::warn("ZkClient fail to connect to zk : zk_hosts_=%s; retry times:=%d", zk_hosts_.c_str(), retry);
		cout << "ZkClient fail to connect to zk=" << zk_hosts_ << " exceed retry times." << retry << endl;
	}
#ifdef DEBUG_
	cout << "connect_zk retry = " << retry << " handle=" << zhandle_ << " state=" << (zhandle_ !=0 ? zhandle_->state : -1) << endl;
#endif

	mutex.unlock();
	if (zhandle_ != 0) {
		set_connected(true);
		set_in_using(true);
		this->set_session_id(zhandle_->client_id.client_id);
		// restore states, ATTATION: following code will lead to recurse if zk is break;
		this->restore_states();
		// restore_states();
		return true;
	} else {
		set_connected(false);
		return false;
	}
}

int ZkClient::take_over_states(StateList& states) {
	for (StateList::iterator it = states.begin(); it != states.end(); it++) {
		ZkState* state = dynamic_cast<ZkState*>(*it);
		if (state) {
			// take over
			state->client_id = this->id();
			// add watcher
			if (state->type == ZkState::k_type_create_ephereral) {
				this->create_enode(state->path, state->data);
			} else if (state->type == ZkState::k_type_get_service) {
				this->get_service(state->path);
			} else if (state->type == ZkState::k_type_get_service_instance) {
				this->get_service_instance(state->path);
			} else if (state->type == ZkState::k_type_get_root) {
				this->get_all_services(state->path);
			}
		}
	}
	return 0;
}

void ZkClient::save_state(string &path, int type, string data) {
	if (shared_states_) {
		ZkState *state = new ZkState();
		state->client_id = this->id();
		state->path = path;
		state->type = type;
		state->data = data;
		shared_states_->add(state);
	}
}

void ZkClient::remove_state(ZkState& state) {
	if (shared_states_) {
		shared_states_->remove(state);
	}
}

// when re connect to zk server, the conn needs to restore these watcher state
void ZkClient::restore_states() {
#ifdef DEBUG_
	cout << "restore_states clientId=" << this.id_ << endl;
#endif
	if (shared_states_) {
		cout << "restore_states size=" << shared_states_->size() << endl;
		StateList list = shared_states_->get(this->id());
		for (StateList::iterator it = list.begin(); it != list.end(); it++) {
			ZkState* state = dynamic_cast<ZkState*>(*it);
			if (state->type == ZkState::k_type_create_ephereral) {
				this->create_enode(state->path, state->data);
				cout << "restore EPHERERAL node " << state->path << state->data << endl;
			} else if (state->type == ZkState::k_type_get_service) {
				this->get_service(state->path);
				cout << "restore TYPE_GET_SERVICE node " << state->path << state->data << endl;

			} else if (state->type == ZkState::k_type_get_service_instance) {
				this->get_service_instance(state->path);
				cout << "restore TYPE_GET_SERVICE_INSTANCE node " << state->path << state->data << endl;
			} else if (state->type == ZkState::k_type_get_root) {
				cout << "restore TYPE_GET_ROOT node " << state->path << state->data << endl;
				this->get_all_services(state->path);
			}
		}
		list.clear();
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
	if (!this->get_connected()) {
		connect_zk();
		if (!this->get_connected()) {
			return false;
		}
	}

	bool ret = true;
	string path = serviceZpath + "/" + ephemeralName;
	int buffer_len = 256;
	char *buffer = new char[buffer_len];
	for (int i = 0; i < buffer_len; i++)
		buffer[i] = 0;

	Stat stat;
	mutex.lock();
	int result = zoo_wget(zhandle_, path.c_str(), &this->service_instance_watcher, this, buffer, &buffer_len, &stat);
	this->save_state(path, ZkState::k_type_get_service_instance);
	mutex.unlock();
	if (result == ZOK) {
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
	} else if (result == ZNONODE) {
		ZkState state(serviceZpath, ZkState::k_type_get_service_instance);
		remove_state(state);
		ret = false;
	} else if (result == ZINVALIDSTATE) {
		this->on_session_timeout();
		ret = false;
	} else {
		logger::warn("get_node zk_wget epheramal node error, ret=%d; msg=%s", ret, zerror(ret));
	}
	delete buffer;
	buffer = 0;
	return ret;
}

//update servcie list. serviceZpath eg. /sao/services/test.xx
bool ZkClient::get_service(string serviceZpath) {
	if (!this->get_connected()) {
		connect_zk();
		if (!this->get_connected()) {
			return false;
		}
	}
	bool ret = true;
	struct String_vector str_vec;
	mutex.lock();
	// zoo_wget_children return value: ZOK=0 ZNONODE=-101 ZNOAUTH=-102 ZBADARGUMENTS=-8 ZINVALIDSTATE=-9 ZMARSHALLINGERROR-5
	int result = zoo_wget_children(zhandle_, serviceZpath.c_str(), &this->service_watcher, this, &str_vec);
	this->save_state(serviceZpath, ZkState::k_type_get_service);
	mutex.unlock();

	if (result == ZOK) {
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
	} else if (result == ZNONODE) {
		ZkState state(serviceZpath, ZkState::k_type_get_service);
		remove_state(state);
		ret = false;
	} else if (result == ZINVALIDSTATE) {
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
		if (!this->get_connected()) {
			return ZCONNECTIONLOSS;
		}
	}
	mutex.lock();
	// return value: ZOK=0 ZNONODE=-101 ZNOAUTH=-102 ZBADARGUMENTS=-8 ZINVALIDSTATE=-9 ZMARSHALLINGERROR-5
	int result = zoo_create(zhandle_, abs_path.c_str(), NULL, 0, &ZOO_OPEN_ACL_UNSAFE, 0, NULL, 0);
	mutex.unlock();

	if (result == ZOK) {
#ifdef DEBUG_
		cout << "create_pnode create persist node successfully. path:" << abs_path << endl;
#endif

	} else if (result == ZINVALIDSTATE) {
		this->set_connected(false);
	} else if (result == ZNODEEXISTS) {
		if (log_when_exists)
			logger::info("create_pnode zoo_create failure, ret=%d; msg=%s path=%s", result, zerror(result), abs_path.c_str());
	} else {
		logger::warn("create_pnode zoo_create error, ret=%d; msg=%s path=%s", result, zerror(result), abs_path.c_str());
	}
	return result;
}
// creaet ephermeral node
int ZkClient::create_enode(string abs_path, string data) {
	if (!this->get_connected()) {
		connect_zk();
		if (!this->get_connected()) {
			return ZCONNECTIONLOSS;
		}
	}
	mutex.lock();
	// return value: ZOK=0 ZNONODE=-101 ZNOAUTH=-102 ZBADARGUMENTS=-8 ZINVALIDSTATE=-9 ZMARSHALLINGERROR-5
	int result = zoo_create(zhandle_, abs_path.c_str(), data.c_str(), data.length(), &ZOO_OPEN_ACL_UNSAFE, ZOO_EPHEMERAL, 0, 0);

	mutex.unlock();
	if (result == ZOK) {
#ifdef DEBUG_
		cout << " create_enode create ephemeral node successfully. path:" << abs_path << endl;
#endif
//	} else if (ret == ZNODEEXISTS) {
//		remove_state(&ZkState(abs_path, ZkState::TYPE_CREATE_EPHERERAL));
	} else if (result == ZINVALIDSTATE) {
		this->on_session_timeout();
	} else {
#ifdef DEBUG_
		logger::warn("create_enode zoo_create error, ret=%d; msg=%s path=%s", result, zerror(result), abs_path.c_str());
#endif
	}
	mutex.lock();
	result = zoo_wget(zhandle_, abs_path.c_str(), &this->create_enode_watcher, this, NULL, 0, NULL);
	this->save_state(abs_path, ZkState::k_type_create_ephereral, data);
	mutex.unlock();
	return result;
}

void ZkClient::create_enode_watcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx) {
#ifdef DEBUG_
	cout << "ephemeral_watcher type=" << type << " state=" << state << " path:" << path << " client id="
			<< (watcherCtx ? ((ZkClient*) watcherCtx)->id() : 0) << endl;
#endif
	ZkClient *client = (ZkClient*) watcherCtx;
	if (!client)
		return;

	if (state == ZOO_EXPIRED_SESSION_STATE || type == DELETED_EVENT_DEF) {
		client->set_connected(false);
		// try to reconn, and in the restore_state method, will create the lost ephemeral node.
		client->connect_zk();
		// tips: if connect retry still fails, then no way to solve it in sync mode.
		// TODO ...
		// server handler will create a zk conn deamon task.
	}
//	if (path) {
//		client->set_in_using(true);
//		string serviceZpath(path);
//		unsigned index = serviceZpath.find_last_of('/');
//		const string spath = "" + serviceZpath.substr(0, index);
//		const string ename = "" + serviceZpath.substr(index + 1);
//
//		switch (type) {
//		case CREATED_EVENT_DEF:
//		case CHANGED_EVENT_DEF:
//		case CHILD_EVENT_DEF:
//			break;
//		case DELETED_EVENT_DEF:
//			client->set_connected(false);m
//			break;
//		default:
//			break;
//		}
//		client->set_in_using(false);
//	}
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
		case CHILD_EVENT_DEF: {
			client->get_service_instance(spath, ename);
			break;
		}
		case DELETED_EVENT_DEF: {
			//client->pcache->get(spath.c_str());
			if (client->pcache_)
				client->pcache_->remove(spath, ename);
			ZkState state(path, ZkState::k_type_get_service_instance);
			client->remove_state(state);
			break;
		}
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
			client->get_all_services(string(path));
		}
			break;
		case DELETED_EVENT_DEF: {
			if (client->pcache_)
				client->pcache_->remove(path);
			ZkState state(path, ZkState::k_type_get_service);
			client->remove_state(state);
		}
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
		case DELETED_EVENT_DEF: {
			if (client->pcache_)
				client->pcache_->remove(path);
			ZkState state(path, ZkState::k_type_get_service);
			client->remove_state(state);
		}
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
	//if (type == ZOO_SESSION_EVENT && state == ZOO_EXPIRED_SESSION_STATE) {
	if (type == ZOO_SESSION_EVENT && state != CONNECTED_STATE_DEF) {
		logger::warn("Zookeeper session expired. session_id: %d", ((ZkClient*) watcherCtx)->id());
		ZkClient *client = (ZkClient*) watcherCtx;
		if (client) {
			client->on_session_timeout();
		}
	}
}

// get all services from service root; return data list;  eg. _serivces_root=/soa/services
bool ZkClient::get_all_services(string _serivces_root) {
	if (!this->get_connected()) {
		connect_zk();
		if (!this->get_connected()) {
			return false;
		}
	}
	bool ret = true;
	vector<string> list;
	//got service list
	struct String_vector str_vec;

	mutex.lock();
	// zoo_wget_children return value: ZOK=0 ZNONODE=-101 ZNOAUTH=-102 ZBADARGUMENTS=-8 ZINVALIDSTATE=-9 ZMARSHALLINGERROR-5
	int rc = zoo_wget_children(zhandle_, _serivces_root.c_str(), &this->root_watcher, this, &str_vec);
	mutex.unlock();
	this->save_state(_serivces_root, ZkState::k_type_get_root);
	if (rc == ZOK) {
		for (int i = 0; i < str_vec.count; ++i) {
			list.push_back(_serivces_root + "/" + str_vec.data[i]);
			// get_children(root + "/" + str_vec.data[i]);
			delete str_vec.data[i];
		}
		delete str_vec.data;

		// get services
		vector<string>::iterator it = list.begin();
		while (it != list.end()) {
			string zkpath = *it;
			//  read all arbitrarily anyway
			// if (client && client->pcache_ && client->pcache_->exists(zkpath) == false) {
			ret = ret && get_service(zkpath);
			// }
			++it;
		}
	} else if (rc == ZINVALIDSTATE) {
		this->on_session_timeout();
		ret = false;
	} else {
		ret = false;
	}
	return ret;
}

//
void ZkClient::on_session_timeout() {
	this->set_connected(false);
//	if (this->states_) {
//		StateList list = this->states_->get(this->id_);
//		if (list.size() > 0) {
//			int retry = 0;
//			this->connect_zk();
//			while (!this->get_connected()) {
//				retry++;
//				sleep(1);
//				this->connect_zk();
//			}
//			if (!this->get_connected()) {
//				for (StateMap::iterator it = states_.begin(); it != states_.end(); it++) {
//					ZkState *state = (ZkState *) (it->second);
//					if (state) {
//						if (state->type == ZkState::TYPE_CREATE_EPHERERAL) {
//							this->create_enode(state->path, state->data);
//						} else if (state->type == ZkState::TYPE_GET_SERVICE) {
//							this->get_service(state->path);
//						} else if (state->type == ZkState::TYPE_GET_NODE) {
//							this->get_service_instance(state->path);
//						}
//					}
//				}
//			}
//		}
//	}
}

bool ZkClient::check(const string& top_zk_path) {
	// cout << "ZkClient::check 1" << endl;
	if (!this->get_connected()) {
		// cout << "ZkClient::check 2" << endl;
		connect_zk();
		// cout << "ZkClient::check 3" << endl;
		if (!this->get_connected()) {
			// cout << "ZkClient::check 4" << endl;
			return false;
		}
	}
	// cout << "ZkClient::check 5" << endl;

	int ret = false;
	int buffer_len = 256;
	char *buffer = new char[buffer_len];
	for (int i = 0; i < buffer_len; i++)
		buffer[i] = 0;

	Stat stat;
	mutex.lock();
	// zoo_get(zhandle_t *zh, const char *path, int watch, char *buffer,  int* buffer_len, struct Stat *stat);
	int result = zoo_get(zhandle_, top_zk_path.c_str(), 0, buffer, &buffer_len, NULL);
	mutex.unlock();
	delete[] buffer;
	buffer = 0;
	return result == ZOK;
}

vector<string> ZkClient::get_children(const string& zkpath) {
	vector<string> list;
	if (!this->get_connected()) {
		connect_zk();
		if (!this->get_connected()) {
			return list;
		}
	}

	struct String_vector str_vec;
	mutex.lock();
	// zoo_get_children return value: ZOK=0 ZNONODE=-101 ZNOAUTH=-102 ZBADARGUMENTS=-8 ZINVALIDSTATE=-9 ZMARSHALLINGERROR-5
	int result = zoo_get_children(zhandle_, zkpath.c_str(), 0, &str_vec);
	mutex.unlock();
	if (result == ZOK) {
		for (int i = 0; i < str_vec.count; ++i) {
			list.push_back(zkpath + "/" + str_vec.data[i]);
			// get_children(root + "/" + str_vec.data[i]);
			delete str_vec.data[i];
		}
		delete str_vec.data;
	} else if (result == ZINVALIDSTATE) {
		this->on_session_timeout();
	}
	return list;
}

string ZkClient::get_data(const string& zkpath) {
	string ret;
	if (!this->get_connected()) {
		connect_zk();
		if (!this->get_connected()) {
			return false;
		}
	}

	int buffer_len = 256;
	char *buffer = new char[buffer_len];
	for (int i = 0; i < buffer_len; i++)
		buffer[i] = 0;

	Stat stat;
	mutex.lock();
	// (zhandle_t *zh, const char *path, int watch, char *buffer, int* buffer_len, struct Stat *stat);
	int result = zoo_get(zhandle_, zkpath.c_str(), 0, buffer, &buffer_len, 0);
	mutex.unlock();
	if (result == ZOK) {
		ret = string(buffer);
	} else if (result == ZINVALIDSTATE) {
		this->on_session_timeout();
	}
	return ret;
}

string ZkClient::to_string() {
	stringstream ss;
	ss << this << "\t" << zhandle_;
	return ss.str();
}

} /* namespace FinagleRegistryProxy */

