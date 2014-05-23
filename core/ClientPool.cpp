/*
 * ClientPool.cpp
 *
 *  Created on: Mar 21, 2014
 *      Author: asdf
 */

#include "ClientPool.h"

namespace FinagleRegistryProxy {

ClientPool::ClientPool(ClientFactory* factory) :
		factory(factory) {
	this->max_client_ = MAX_CLIENT_DEF;
	this->conn_timeout = CONN_TIMEOUT_DEF; // 1 * 3600 * 1000
	this->max_used_times = MAX_USED_TIMES_DEF;
	this->last_id_ = 0;
}

ClientPool::~ClientPool() {
	this->reset();
	delete factory;
	factory = 0;
}

ClientBase* ClientPool::open() {
	ClientBase* c;
	mutex.lock();

	if (idle_.size() == 0) {
		if (size() < max_client_) {
			c = factory->create(c);
			c->id_ = ++last_id_;
			idle_.insert(c);
			c->pool_event_ = new SEvent<ClientPool, ClientBase, int>(this, &ClientPool::on_client_changed);
		} else {
			c = *(this->using_.begin());
		}
	} else {
		c = *(this->idle_.begin());
	}
	mutex.unlock();
	if (c) {
		c->open();
	}
	return c;
}

// when disconn or not used then mv to idle;
void ClientPool::on_client_changed(ClientBase* client, int state) {
#ifdef DEBUG_
//	cout << "***** client stated " << state << " changed id=" << client->to_string() << endl;
#endif
	if (client == 0)
		return;
	StateMap *smap = 0;
	mutex.lock();
	if (state == ClientBase::EVENT_TYPE_CONNECTION_STATE) {
		if (client->connected_) { // means that, client->connnected is changed from false to be true.
			// make sure in idle list
			idle_.erase(client);

			using_.insert(client);
		} else {
			using_.erase(client);
			idle_.insert(client);
		}
	} else if (state == ClientBase::EVENT_TYPE_USE_STATE) {
		if (client->in_using_) {
			idle_.erase(client);
			using_.insert(client);
		} else {
			using_.erase(client);
			idle_.insert(client);
		}
	}
	mutex.unlock();
	if (smap != 0) {
		cout << " watch state reserving....................................... " << endl;
//		ClientBase *c;
//		ClientBase **pp = 0;
//		pp = open(c);
//		c = *pp;
//		if (c )
//			c->set_states(smap);
//
//		if (c)
//			c->set_in_using(false);
//		cout << " watch state reserved....................................... " << &c << c << endl;
		delete smap;
		smap = 0;
	}
}

void ClientPool::reset() {
	while (using_.size() > 0) {
		ClientBase *client = *(using_.begin());
		destroy(client);
	}
	while (idle_.size() > 0) {
		ClientBase *client = *(idle_.begin());
		destroy(client);
	}
}

void ClientPool::destroy(ClientBase* client) {
	if (client) {
#ifdef DEBUG_
		cout << "ClientPool::destroy ing client id = " << client->to_string() << endl;
#endif
		using_.erase(client);
		idle_.erase(client);

		delete client;
		client = 0;
	}
}
int ClientPool::size() {
	return using_.size() + idle_.size();
}
int ClientPool::used() {
	return using_.size();
}
int ClientPool::idle() {
	return idle_.size();
}

string ClientPool::stat() {
	stringstream ss;
	int wc = 0;
	ss << "zk client pool stat info." << endl;
	ss << "using" << endl;
	ss << "	id	using	times	conn	watches	address" << endl;
	for (CSet::iterator it = using_.begin(); it != using_.end(); it++) {
		ss << "\t" << (*it)->id() << "\t" << (*it)->in_using_ << "\t" << (*it)->use_times_ << "\t" << (*it)->connected_ << "\t"
				<< (*it)->states_.size() << "\t" << *it << endl;
		wc += (*it)->states_.size();
	}

	ss << "idle" << endl;
	ss << "	id	using	times	conn	watches	address" << endl;
	for (CSet::iterator it = idle_.begin(); it != idle_.end(); it++) {
		ss << "\t" << (*it)->id() << "\t" << (*it)->in_using_ << "\t" << (*it)->use_times_ << "\t" << (*it)->connected_ << "\t"
				<< (*it)->states_.size() << "\t" << *it << endl;
		wc += (*it)->states_.size();
	}
	ss << "total" << endl;
	ss << "	using	idle	watches" << endl;
	ss << "\t" << using_.size() << "\t" << idle_.size() << "\t" << wc << endl;
	return ss.str();
}

} /* namespace FinagleRegistryProxy */
