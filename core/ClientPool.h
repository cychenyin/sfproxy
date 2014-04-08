/*
 * ClientPool.h
 *
 *  Created on: Mar 21, 2014
 *      Author: asdf
 */

#ifndef CLIENTPOOL_H_
#define CLIENTPOOL_H_

#include <iostream>
#include <string>
#include <set>
#include <list>
#include <thrift/concurrency/Mutex.h>

#include "SEvent.h"

using namespace std;

namespace FinagleRegistryProxy {

class ClientPool;
class ClientBase;

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
	}
	virtual ~ClientBase() {
		if (pool_event_) {
			delete pool_event_;
			pool_event_ = 0;
		}
	}
	virtual void open() = 0;
	virtual void close() = 0;

	bool get_in_using() {
		return in_using_;
	}

	bool get_connected() {
		return this->connected_;
	}
	int id() {
		return id_;
	}

protected:
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
	int id_;
protected:
	apache::thrift::concurrency::Mutex mutex;
};

class ClientFactory {
public:
	virtual ~ClientFactory() {
	}
	;
	virtual ClientBase* create() = 0;
};

// client collection
typedef set<ClientBase*> CSet;
// client collection
typedef list<ClientBase*> CList;

/*
 * create client add to use_list, when client.close then mv to idle_client; when disconnect then destroy it.
 * usage: client
 *
 */
class ClientPool {

public:
	ClientPool(ClientFactory* factory);
	virtual ~ClientPool();

	virtual ClientBase* open();
	void on_client_changed(ClientBase* client, int state);

	int size() {
		return using_.size() + idle_.size();
	}
	int used() {
		return using_.size();
	}
	int idle() {
		return idle_.size();
	}
public:
	int conn_timeout;
	int max_used_times;
	ClientFactory* factory;
	const static int MAX_CLIENT_DEF = 100;
	const static int CONN_TIMEOUT_DEF = 86400; // 1 day
private:
	CSet using_;
	CSet idle_;
	int max_client_;
	long last_id_;
	apache::thrift::concurrency::Mutex mutex;

protected:

	virtual ClientBase* create();

	virtual void reset();
	virtual void destroy(ClientBase* client);

};

} /* namespace FinagleRegistryProxy */

#endif /* CLIENTPOOL_H_ */
