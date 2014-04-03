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
	this->max_client_ = 100;
	this->conn_timeout = 1 * 3600 * 1000;
	this->max_usetimes = 10;
}

ClientPool::~ClientPool() {
	this->reset();
	delete factory;
	factory = 0;
}

ClientBase* ClientPool::create() {
	if (factory) {
		mutex.lock();
		ClientBase *c = factory->create();
		c->id_ = ++ last_id_;
		mutex.unlock();

		c->pool_event_ = new SEvent<ClientPool, ClientBase, int>(this, &ClientPool::on_client_changed);

		return c;
	} else
		return 0;
}

ClientBase* ClientPool::open() {
	ClientBase *client = 0;
	mutex.lock();
	CColl::iterator it = idle_.begin();
	while (it != idle_.end()) {
		client = *it;
		if (!(*it)->connected_ || (*it)->use_times_ >max_usetimes) { // how could this happen, a conn open , then left alone enough time, could happen? doute about it in zk client scenario cause of watch.
			++it;
			destroy(*it);
		} else
			break;
	}
	mutex.unlock();

	if (client == 0 &&  size() < max_client_) {
		client = create(); // will be add to use_list when connected state is ready through poolevent which callback to onClientChanged method.
	}

	if (client) {
		client->open();
	}
	return client;
}

// when disconn or not using then mv to idle;
void ClientPool::on_client_changed(ClientBase* client, int state) {
	if (client == 0)
		return;
	mutex.lock();
	if (state == ClientBase::EVENT_TYPE_CONNECTION_STATE) {
		if (!client->connected_) {
			// make sure in idle list
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
}

void ClientPool::reset() {
	while (using_.size() > 0) {
		ClientBase *client = *using_.begin();
		destroy(client);
	}
	while (idle_.size() > 0) {
		ClientBase *client = *idle_.begin();
		destroy(client);
	}

//	while (use_list.size() > 0) {
//		ClientBase* client = use_list.front();
//		destroy(client);
//		use_list.remove(client);
//	}
//	while (idle_.size() > 0) {
//		ClientBase* client = idle_.front();
//		destroy(client);
//		idle_.remove(client);
//	}
}

void ClientPool::destroy(ClientBase* client) {
	if (client) {
		using_.erase(client);
		idle_.erase(client);

		delete client;
		client = 0;
	}
}

} /* namespace FinagleRegistryProxy */
