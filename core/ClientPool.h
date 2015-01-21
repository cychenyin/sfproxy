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
		map_.insert(pair<string, ClientState*>(state->key(), state));
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
	ClientState* get(string key) {
		StateMap::iterator find_it = map_.find(key);
		if (find_it != map_.end())
			return find_it->second;
		return NULL;
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

	string dump();
};

class ClientPool;
class ClientBase;
// key, string which contains watcher type and zk path; value, conn data, such as watcher

//typedef SEvent<ClientPool, ClientBase, int> PoolEvent; // client event

// abstractor class
class ClientBase {
	friend class ClientPool;
public:
	SEvent<ClientPool, ClientBase, int> * pool_event_;
//	PoolEvent *poolEvent; // why fail to compile
	const static int EVENT_TYPE_CONNECTION_STATE = 1;
	const static int EVENT_TYPE_USE_STATE = 2;

private:
	int use_times_;
	volatile bool in_using_;
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
		in_using_ = false;
		connected_ = false;
		pool_event_ = NULL;
		id_ = 0;
		session_id_ = 0;
		shared_states_ = _states;
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

	bool get_in_using() {
		return in_using_;
	}

	bool get_connected() {
		return this->connected_;
	}
	int id() {
		return id_;
	}
	int session_id() {
		return session_id_;
	}

	virtual string to_string() {
		stringstream ss;
		ss << "id=" << id_ << "	session=" << session_id_ << "	use_times=" << use_times_ << "	in_using_=" << in_using_
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
	void set_connected(bool connected) {
		if (this->connected_ == connected)
			return;
#ifdef DEBUG_
		cout << "set_connected " << connected << " ###################################################" << endl;
#endif
		mutex.lock();
		this->connected_ = connected;
		mutex.unlock();
		if (pool_event_) {
			pool_event_->invoke(this, ClientBase::EVENT_TYPE_CONNECTION_STATE);
		}
	}
	// can be accessed only by friend ClientPool when open
	void set_in_using(bool in_using) {
		bool origin = this->in_using_;
		mutex.lock();
		this->in_using_ = in_using;
		if (!origin && in_using) {
			this->use_times_++;
		}
		mutex.unlock();
		if (origin != in_using_ && pool_event_)
			pool_event_->invoke(this, ClientBase::EVENT_TYPE_USE_STATE);
	}

};

class ClientFactory {
public:
	virtual ~ClientFactory() {
	}
	virtual ClientBase* create(StateBag* states) = 0;
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
	void on_client_changed(ClientBase* client, int state);
	int size();
	int used();
	int idle();
	string dump();
	int watcher_size();
	void clear();
public:
	int conn_timeout;
	int max_used_times;
	ClientFactory* factory;
	const static int MAX_CLIENT_DEF = 4;
	const static int MAX_USED_TIMES_DEF = 1000000; // 1 million // not imple
	const static int CONN_TIMEOUT_DEF = 86400; // 1 day // not imple
private:
	ClientSet using_;
	ClientSet idle_;
	int max_client_;
	long last_id_;
	apache::thrift::concurrency::Mutex mutex;
	StateBag state_bag_;
protected:
	// clear & destory all clients in using_ and idle_ lists, not atomic method
	virtual void reset();
	// remove client from using_ and idle_ list, and destroy client handler, not atomic method
	virtual void destroy(ClientBase* client);

};

} /* namespace FinagleRegistryProxy */

#endif /* CLIENTPOOL_H_ */
