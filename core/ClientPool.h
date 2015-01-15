/*
 * ClientPool.h
 *
 *  Created on: Mar 21, 2014
 *      Author: asdf
 */

#ifndef CLIENTPOOL_H_
#define CLIENTPOOL_H_

#include <iostream>
#include <sstream>
#include <string>
#include <set>
#include <map>
#include <list>
#include "concurrency/Mutex.h"
#include "SEvent.h"
#include "../frproxy.h"

using namespace std;

namespace FinagleRegistryProxy {

class ClientState {
public:
	long client_id;
	virtual string key()=0;
};

class ClientPool;
class ClientBase;
// key, string which contains watcher type and zk path; value, conn data, such as watcher
typedef map<string, ClientState*> StateMap;

//typedef SEvent<ClientPool, ClientBase, int> PoolEvent; // client event

// abstractor class
class ClientBase {
	friend class ClientPool;
public:
	SEvent<ClientPool, ClientBase, int> * pool_event_;
//	PoolEvent *poolEvent; // why fail to compile
	const static int EVENT_TYPE_CONNECTION_STATE = 1;
	const static int EVENT_TYPE_USE_STATE = 2;

public:
	ClientBase() {
		use_times_ = 0;
		in_using_ = false;
		connected_ = false;
		pool_event_ = 0;
		id_ = 0;
		session_id_ = 0;
	}
	virtual ~ClientBase() {
		if (pool_event_) {
			delete pool_event_;
			pool_event_ = 0;
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

	string to_string() {
		stringstream ss;
		ss << "id=" << id_ << "	session=" << session_id_ << "	use_times=" << use_times_ << "	in_using_=" << in_using_ << "	connected=" << connected_
				<< "	addesss=" << this;
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
		mutex.lock();
		this->connected_ = connected;
		mutex.unlock();
		if (pool_event_) {
			pool_event_->invoke(this, ClientBase::EVENT_TYPE_CONNECTION_STATE);
		}
	}
	// can be accessed only by friend ClientPool when open
	void set_in_using(bool state) {
		bool origin = this->in_using_;
		mutex.lock();
		this->in_using_ = state;
		if (!origin && state) {
			this->use_times_++;
		}
		mutex.unlock();
		if (origin != in_using_ && pool_event_)
			pool_event_->invoke(this, ClientBase::EVENT_TYPE_USE_STATE);
	}
private:
	int use_times_;
	bool in_using_;
	bool connected_;
	long id_;
	long session_id_;
protected:
	apache::thrift::concurrency::Mutex mutex;
	StateMap states_;	// state about watch type, watch path etc..
};

class ClientFactory {
public:
	virtual ~ClientFactory() {
	}
	virtual ClientBase* create(ClientBase *p) = 0;
};

// client collection
typedef set<ClientBase*> CSet;
// client collection
typedef list<ClientBase*> CList;

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
	string stat();
	int watcher_size();
	void clear();
public:
	int conn_timeout;
	int max_used_times;
	ClientFactory* factory;
	const static int MAX_CLIENT_DEF = 4;
	const static int MAX_USED_TIMES_DEF = 1000000; // 1 million
	const static int CONN_TIMEOUT_DEF = 86400; // 1 day
private:
	CSet using_;
	CSet idle_;
	int max_client_;
	long last_id_;
	apache::thrift::concurrency::Mutex mutex;

protected:
	// clear & destory all clients in using_ and idle_ lists, not atomic method
	virtual void reset();
	// remove client from using_ and idle_ list, and destroy client handler, not atomic method
	virtual void destroy(ClientBase* client);

};

} /* namespace FinagleRegistryProxy */

#endif /* CLIENTPOOL_H_ */
