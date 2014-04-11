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

ClientBase* ClientPool::create() {
	if (factory) {
		mutex.lock();
		ClientBase *c = factory->create();
		c->id_ = ++last_id_;
		mutex.unlock();

		c->pool_event_ = new SEvent<ClientPool, ClientBase, int>(this, &ClientPool::on_client_changed);

		return c;
	} else
		return 0;
}
ClientBase* ClientPool::open() {
	ClientBase *client = 0;
	mutex.lock();
	CSet::iterator it = idle_.begin();
	while (it != idle_.end()) {
		if (!(*it)->connected_ || (*it)->use_times_ > max_used_times) {
			// how could this happen. a conn open, then left it alone for enough time, could happen?
			// doute about that, cause of in zk client scenario watcher callback happens in unknown future.
			CSet::iterator destroy_it = it;
			++it;
			delete *destroy_it;
		} else {
			client = *it;
			break;
		}
	}
	mutex.unlock();
	if (client == 0 && size() < max_client_) {
		// will be add to use_list when connected state is ready through poolevent which callback to onClientChanged method.
		client = create();
	}
	if (client) {
		client->open();
	}
	return client;
}

ClientBase* ClientPool::open1() {
	ClientBase *client = 0;
	CList *destroy_list = 0;
	mutex.lock();
	CSet::iterator it = idle_.begin();
	while (it != idle_.end()) {
		client = *it;
		if (!(*it)->connected_ || (*it)->use_times_ > max_used_times) {
			// how could this happen. a conn open, then left it alone for enough time, could happen? doute about that, cause of in zk client scenario watcher callback happens in unknown future.
			if (!destroy_list) {
				destroy_list = new CList();
			}
			destroy_list->push_back(client);
			++it;
		} else
			break;
	}
	mutex.unlock();
	if (destroy_list) {
		CList::iterator i = destroy_list->begin();
		while (i != destroy_list->end()) {
			destroy(*i);
		}
		delete destroy_list;
	}
	if (client == 0 && size() < max_client_) {
		// will be add to use_list when connected state is ready through poolevent which callback to onClientChanged method.
		client = create();
	}

	if (client) {
		client->open();
	}
	return client;
}

// when disconn or not used then mv to idle;
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
