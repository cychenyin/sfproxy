/*
 * ClientPool.cpp
 *
 *  Created on: Mar 21, 2014
 *      Author: asdf
 */

#include "ClientPool.h"
#include "../log/logger.h"
//#define move_to_using_list(client) { \
//	idle_.erase(client); \
//	using_.insert(client); \
//};
//
//#define move_to_idle_list(client) { \
//	using_.erase(client); \
//	idle_.insert(client); \
//};
//
//#define move_to_destroy_list(client) { \
//	using_.erase(client); \
//	idle_.erase(client); \
//	to_destrory_.push_back(client); \
//};


namespace FinagleRegistryProxy {

ClientPool::ClientPool(ClientFactory* factory) :
		factory(factory) {
	this->max_client_ = MAX_CLIENT_DEF;
	this->conn_timeout = CONN_TIMEOUT_DEF; // 1 * 3600 * 1000
	this->used_timeout_ = MAX_USED_TIMES_DEF;
	this->last_id_ = 0;
}

ClientPool::~ClientPool() {
	this->reset();
	delete factory;
	factory = 0;
}

ClientBase* ClientPool::open() {
	ClientBase* c = NULL;
	// destroy clients which are waiting for to be destroyed
	destroy_clients(false);
	mutex.lock();
	// open client
	try {
		// re-use
		while(this->idle_.size() > 0) {
			ClientSet::iterator it = this->idle_.begin();
			c = *(it);
			if(c->get_connected()) {
				c->set_in_using(true);
				// move_to_using_list(c);
				idle_.erase(it);
				using_.insert(c);
				break;
			} else {
				// move_to_destroy_list(c);
				idle_.erase(it);
				to_destrory_.push_back(c);
				state_bag_.replace_client_id(c->id(), 0);
				c = 0;
			}
		}
		// create new client
		if (c == 0 && idle_.size() == 0 && size() < max_client_) {
			c = factory->create(this, &state_bag_);
			c->id_ = ++last_id_;
			c->set_in_using(true);
			using_.insert(c);
			c->pool_event_ = new SEvent<ClientPool, ClientBase, int>(this, &ClientPool::on_client_closing);
		} else if(size() == max_client_) {
			logger::warn("zk conn pool exhausted. move timeout client to destroy list by force if that is used timeout...");
			uint64_t now = utils::now_in_ms();
			bool done = false;
			while(!done) {
				ClientSet::iterator it= using_.begin();
				while( it != using_.end()){
					ClientBase *p = (*it);
					if((now - p->last_use_) > this->used_timeout_) {
						using_.erase(it);
						p->set_in_using(false);
						to_destrory_.push_back(p);
						state_bag_.replace_client_id(p->id(), 0);
						break;
					}
					it++;
				}
				if(it == using_.end()){
					done = true;
				}
			}
		}
	} catch(...) {
	}
	mutex.unlock();
	if (c) {
		c->last_use_ = utils::now_in_ms();
		c->open();
	}
	return c;
}

void ClientPool::print(int i){
	cout << "print " << i << endl;
	for(ClientSet::iterator it = using_.begin(); it != using_.end(); it++){
		ClientBase *p = (*it);
		cout << "	print *p=" << p << " *it" << (*it) << endl;
	}
	cout << "	print " << i << " done" << endl;
}

void ClientPool::set_used_timeout(int timeout_in_ms){
	if(timeout_in_ms > 0) {
		this->used_timeout_ = timeout_in_ms;
	}
}
// when disconn or not used then mv to idle;
void ClientPool::on_client_closing(ClientBase* client, int event) {
	if (client == 0)
		return;
	mutex.lock();
	try {
		if (event == ClientBase::EVENT_TYPE_CLOSEING) {
			using_.erase(client);
			if(client->get_connected() ) {
				// move_to_idle_list(client);
				idle_.insert(client);
			} else {
				// move_to_destroy_list(client);
				to_destrory_.push_back(client);
				state_bag_.replace_client_id(client->id(), 0);
			}
		}
	}catch(...) {}
	mutex.unlock();
}

// atomic method
void ClientPool::reset() {
	mutex.lock();
	while (using_.size() > 0) {
		// move_to_destroy_list(*(using_.end()));
		ClientSet::iterator it = using_.begin();
		to_destrory_.push_back((*it));
		using_.erase(it);
	}
	while (idle_.size() > 0) {
		//move_to_destroy_list(*(idle_.end()));
		ClientSet::iterator it = idle_.begin();
		to_destrory_.push_back(*it);
		idle_.erase(it);
	}
	state_bag_.reset_client_id();

	mutex.unlock();
	uint64_t start = utils::now_in_ms();
	cout << "before release destroy list size: " << to_destrory_.size() << endl;
	// destroy and wait for using_counter released; wait time out 10 second = 10000 ms
	while (utils::now_in_ms() - start < DESTRORY_WAIT_TIMEOUT_MS && to_destrory_.size() > 0 ) {
		destroy_clients(false);
		usleep(1000); // sleep 1 ms
	}
	cout << "before force destroy list size: " << to_destrory_.size() << endl;
	// destroy by force
	destroy_clients(true);
}

// atomic method
void ClientPool::destroy_clients(bool force) {
	mutex.lock();
	try {
		vector<ClientBase*>::iterator it = to_destrory_.begin();
	while( it != to_destrory_.end()) {
		ClientBase* client = *it;
		if(force || client->using_counter < 1) {
			it = to_destrory_.erase(it);
			delete client;
			client = 0;
		} else
			 it++;
	}
	}catch(...){}
	mutex.unlock();
}

int ClientPool::size() {
	return using_.size() + idle_.size();
}
int ClientPool::used() {
	return using_.size();
}
int ClientPool::destroying() {
	return to_destrory_.size();
}
int ClientPool::idle() {
	return idle_.size();
}

int ClientPool::watcher_size() {
	return this->state_bag_.size();
}
string ClientPool::dump() {
	stringstream ss;
	int wc = 0;

	try {
		ss << "client pool:" << "---------------------------------------------" << endl;
		ss << "using" << endl;
		ss << "	id	using	times	conn	watches	address	zh" << endl;
		this->mutex.lock();
		for (ClientSet::iterator it = using_.begin(); it != using_.end(); it++) {
			StateList list = state_bag_.get( (*it)->id());

			ss << "\t" << (*it)->id() << "\t" << (*it)->using_counter << "\t" << (*it)->use_times_ << "\t" << (*it)->connected_
					<< "\t" << list.size() << "\t" << (*it)->to_string() << endl;
			wc += list.size();
		}
		ss << endl;
		ss << "idle" << endl;
		ss << "	id	using	times	conn	watches	address	zh" << endl;
		for (ClientSet::iterator it = idle_.begin(); it != idle_.end(); it++) {
			StateList list = state_bag_.get( (*it)->id());
			ss << "\t" << (*it)->id() << "\t" << (*it)->using_counter << "\t" << (*it)->use_times_ << "\t" << (*it)->connected_
					<< "\t" << list.size() << "\t" << (*it)->to_string() << endl;
			wc += list.size();
		}
		ss << endl;
		ss << "to destroy list" << endl;
				ss << "	id	using	times	conn	watches	address	zh" << endl;
				for (vector<ClientBase*>::iterator it = to_destrory_.begin(); it != to_destrory_.end(); it++) {
					StateList list = state_bag_.get( (*it)->id());
					ss << "\t" << (*it)->id() << "\t" << (*it)->using_counter << "\t" << (*it)->use_times_ << "\t" << (*it)->connected_
							<< "\t" << list.size() << "\t" << (*it)->to_string() << endl;
					// wc += list.size();
				}
		ss << endl;
		ss << "zk conn summary" << endl;
		ss << "	using	idle	watches	valid	to_destroy " << endl;
		ss << "\t" << using_.size() << "\t" << idle_.size() << "\t" << wc << "\t" << size() << "\t" << to_destrory_.size() << endl;
		ss << endl;
		ss << state_bag_.dump() << endl;

	} catch (...) {
	}
	this->mutex.unlock();
	return ss.str();
}

void ClientPool::clear() {
	try {
		this->reset();
	}catch(...){}
}

void ClientPool::set_max_client(int max) {
	if(this->max_client_ > 0)
		this->max_client_ = max;
}


//int ClientPool::get_max_client() {
//	return this->max_client_;
//}

StateBag* ClientPool::get_shared_states() {
	return &state_bag_;
}

void ClientBase::set_in_using(bool in_using)  {
	mutex.lock();
	if(in_using) {
		using_counter ++;
		this->use_times_++;
	} else {
		using_counter --;
	}
	mutex.unlock();
}

void ClientBase::set_connected(bool connected) {
	if (this->connected_ == connected)
		return;
	mutex.lock();
	this->connected_ = connected;
	mutex.unlock();
//	if (pool_event_) {
//		pool_event_->invoke(this, ClientBase::EVENT_TYPE_CONNECTION_STATE);
//	}
}


string StateBag::dump(){
	stringstream ss;
	ss << "watcher path:  -------------------------------------------------" << map_.size() << endl;
	ss << "	clientId	path" << endl;
	mutex.lock();
	for (StateMap::iterator it = map_.begin(); it != map_.end(); ++it) {
		ClientState *state = it->second;
		ss << "\t" << state->client_id << "\t" << state->key() << endl;
		// ss << "\t" << state->client_id << endl; // << "\t" << state->key() << endl;
	}
	mutex.unlock();
	return ss.str();
}
} /* namespace FinagleRegistryProxy */
