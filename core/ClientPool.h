/*
 * ClientPool.h
 *
 *  Created on: Mar 21, 2014
 *      Author: asdf
 */

#ifndef CLIENTPOOL_H_
#define CLIENTPOOL_H_

#include <boost/shared_ptr.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <set>
#include <map>
#include <list>
#include <vector>
#include "concurrency/Mutex.h"
#include "SEvent.h"
#include "../frproxy.h"

using namespace std;
using boost::shared_ptr;

namespace FinagleRegistryProxy {

class ClientState {
public:
	ClientState() :
			client_id(0) {
	}
	long client_id;
	virtual string key()=0;
	virtual ~ClientState() {
	}
};

typedef map<string, ClientState*> StateMap;
typedef vector<ClientState*> StateList;

// thread safe add & remove
class StateBag {
private:
	StateMap map_;
	apache::thrift::concurrency::Mutex mutex;

public:
	StateBag() {
	}
	~StateBag() {
		ClientState *p = NULL;
		mutex.lock();
		for (StateMap::iterator it = map_.begin(); it != map_.end(); ++it) {
			p = it->second;
			delete p;
			p = NULL;
		}
		map_.clear();
		mutex.unlock();
	}
	int size() {
		return this->map_.size();
	}
	// thread safe add
	void add(ClientState* state) {
		mutex.lock();
		pair<StateMap::iterator, bool> ret = map_.insert(pair<string, ClientState*>(state->key(), state));
		if(!ret.second) {
			// update client_id
			ret.first->second->client_id = state->client_id;
		}
		mutex.unlock();
	}
	// thread safe remove
	void remove(ClientState& state) {
		ClientState* p = 0;
		mutex.lock();
		StateMap::iterator find_it = map_.find(state.key());
		if (find_it != map_.end()) {
			p = find_it->second;
			map_.erase(find_it);
			delete p;
			p = 0;
		}
		mutex.unlock();
	}

	// simple get, not thread safe
	ClientState* get(string& key) {
		StateMap::iterator find_it = map_.find(key);
		if (find_it != map_.end())
			return find_it->second;
		return NULL;
	}

	void replace_client_id(string key, long int replace_to) {
		ClientState* state = get(key);
		if(state) {
			state->client_id = replace_to;
		}
	}

	void replace_client_id(long int client_id, long int replae_to){
		for (StateMap::iterator it = map_.begin(); it != map_.end(); ++it) {
			if (it->second->client_id == client_id) {
				it->second->client_id = replae_to;
			}
		}
	}
	// set client id to 0
	void reset_client_id(){
		for (StateMap::iterator it = map_.begin(); it != map_.end(); ++it) {
			it->second->client_id = 0;
		}
	}

	// get state list by client_id
	StateList get(long int client_id) {
		StateList ret;
		for (StateMap::iterator it = map_.begin(); it != map_.end(); ++it) {
			if (it->second->client_id == client_id) {
				ret.push_back(it->second);
			}
		}
		return ret;
	}
	void clear(){
		for (StateMap::iterator it = map_.begin(); it != map_.end(); ++it) {
			delete it->second;
			it->second = 0;
		}
		map_.clear();
	}
	string dump();
};

class ClientPool;
class ClientBase;
typedef SEvent<ClientPool, ClientBase, int> PoolEvent;

// abstractor class
class ClientBase {
	friend class ClientPool;
public:
	PoolEvent * pool_event_;
	const static int EVENT_TYPE_CONNECTION_STATE = 1;
	const static int EVENT_TYPE_CLOSEING = 2;
	//const static int EVENT_TYPE_CLOSE_STATE = 3;

private:
	uint64_t last_use_;
	int use_times_;
	volatile int using_counter;
	volatile bool connected_;
	long id_;
	long session_id_;
protected:
	apache::thrift::concurrency::Mutex mutex;
	// StateMap* states_;	// state about watch type, watch path etc..
	StateBag* shared_states_;

public:
	ClientBase(StateBag* _states) {
		use_times_ = 0;
		using_counter = 0;
		connected_ = false;
		pool_event_ = NULL;
		id_ = 0;
		session_id_ = 0;
		shared_states_ = _states;
		last_use_ = 0;
	}
	virtual ~ClientBase() {
		if (pool_event_) {
			delete pool_event_;
			pool_event_ = 0;
		}
		if (shared_states_) {
			// NOTICE: it will be deleted in pool unconstructor;
			shared_states_ = NULL;
		}
	}
public:
	virtual bool open() = 0;
	virtual void close() = 0;
	virtual int set_states(map<string, ClientState*> *states) {
		return 0;
	}

	int get_in_using() {
		return using_counter;
	}

	bool get_connected() {
		return this->connected_;
	}
	long id() {
		return id_;
	}
	int session_id() {
		return session_id_;
	}

	virtual string to_string() {
		stringstream ss;
		ss << "id=" << id_ << "	session=" << session_id_ << "	use_times=" << use_times_ << "	in_using_=" << using_counter
				<< "	connected=" << connected_ << "	addesss=" << this;
		return ss.str();
	}
protected:
	void set_id(long id) {
		this->id_ = id;
	}
	void set_session_id(long session) {
		this->session_id_ = session;
	}
	// should be accessed by derived children; used in close & open method in implement of this abstract class.
	void set_connected(bool connected);
	// can be accessed only by friend ClientPool when open
	void set_in_using(bool in_using);
};

class ClientPool;
class ClientFactory {
public:
	virtual ~ClientFactory() {
	}
	virtual ClientBase* create(ClientPool *pool, StateBag* states) = 0;
};

// client collection
typedef set<ClientBase*> ClientSet;

/*
 * create client add to use_list, when client.close then mv to idle_client; when disconnect then destroy it.
 */
class ClientPool {

public:
	ClientPool(ClientFactory* factory);
	virtual ~ClientPool();
	// get a client, create if not exists, thread safe
	virtual ClientBase* open();
	// raise on any client change, thread safe, IN IMPLEMENT SHOULD BE USE LOCK CAREFULLY
	void on_client_closing(ClientBase* client, int state);
	// return valid client size, using and idle included, and to destroying excluded
	int size();
	int destroying();
	int used();
	int idle();
	string dump();
	int watcher_size();
	void clear();
	void set_max_client(int max);
	// timeout in ms
	void set_used_timeout(int timeout_in_ms);
	StateBag* get_shared_states();
	void print(int i);
public:
	int conn_timeout;
	int max_used_times;
	ClientFactory* factory;
	const static int MAX_CLIENT_DEF = 50;
	const static int MAX_USED_TIMES_DEF = 1000000; // 1 million // not imple
	const static int CONN_TIMEOUT_DEF = 86400; // 1 day // not imple
	const static int USED_TIMEOUT_DEF = 10 * 60 * 1000; // 10 minute
	// const static int DESTRORY_WAIT_TIMEOUT_MS = 10 * 1000; // 10 second, in ms
	const static int DESTRORY_WAIT_TIMEOUT_MS = 1000; // 10 second, in ms

private:
	ClientSet using_;
	ClientSet idle_;
	vector<ClientBase*> to_destrory_;
	int max_client_;
	int used_timeout_;
	long last_id_;
	apache::thrift::concurrency::Mutex mutex;
	StateBag state_bag_;

protected:
	// clear & destory all clients in using_ and idle_ lists, not atomic method
	virtual void reset();
	// remove client from using_ and idle_ list, and destroy client handler, not atomic method
	virtual void destroy_clients(bool force);
//	void move_to_using_list(ClientBase* client);
//	void move_to_idle_list(ClientBase* client);
//	void move_to_destroy_list(ClientBase* client);

};

} /* namespace FinagleRegistryProxy */

#endif /* CLIENTPOOL_H_ */
