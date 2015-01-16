/*
 * RegistryCache.cpp
 *
 *  Created on: Mar 13, 2014
 *      Author: asdf
 */
#include <stdexcept>
#include <sstream>
#include <algorithm>

#include "../log/logger.h"
#include "JsonUtil.h"
#include "RegistryCache.h"

namespace FinagleRegistryProxy {

RegistryCache::RegistryCache() {
}

RegistryCache::~RegistryCache() {
	this->clear();
}

void RegistryCache::add(Registry& reg) {
	string &name = reg.name;
	if (name == "" || reg.host == "" || reg.ephemeral == "") {
		cout << "add valid false" << endl;
		return;
	}
	mutex.lock();
	RMap::iterator it = cache.find(name);
	RVector *ptr = NULL;
	if (it == cache.end()) {
		RVector v;
		ptr = &v;
		ptr->push_back(reg);
		// attation: stl copy pair in their implement. so, vector in map this not really the input one.
		cache.insert(pair<string, RVector>(name, *ptr));
	} else {
		ptr = &(it->second);
		RVector::iterator itProxy = ptr->begin();
		while (itProxy != ptr->end()) {
			if ((*itProxy).equals(reg)) {
				ptr->erase(itProxy);
				break;
			}
			++itProxy;
		}
		ptr->push_back(reg);
	}
	mutex.unlock();
}

void RegistryCache::remove(const string name, const string ephemeral) {
	if (name == "" || ephemeral == "")
		return;
	RVector *ptr = NULL;
	mutex.lock();
	try {
		ptr = &(cache.at(name));
	} catch (std::out_of_range &ex) {
	} catch (std::exception &ex) {
	} catch (std::string &ex) {
	}
	if (ptr) {
		RVector::iterator itProxy = ptr->begin();
		while (itProxy != ptr->end()) {
			if ((*itProxy).ephemeral == ephemeral) {
				ptr->erase(itProxy);
				break;
			}
			++itProxy;
		}
	}
	mutex.unlock();
}

void RegistryCache::remove(Registry& reg) {
	mutex.lock();
	// RMap::iterator it = cache.find(name);
	RMap::iterator it = cache.find(reg.name);
	RVector *ptr = NULL;
	if (it != cache.end()) {
		ptr = &(it->second);
		RVector::iterator itProxy = ptr->begin();
		while (itProxy != ptr->end()) {
			if ((*itProxy) == reg) {
				ptr->erase(itProxy);
				break;
			}
			++itProxy;
		}
	}
	mutex.unlock();
}

void RegistryCache::remove(const string name) {
	mutex.lock();
	cache.erase(name);
	mutex.unlock();
}

void RegistryCache::replace(const string name, RVector& l) {
	mutex.lock();
	RMap::iterator it = cache.find(name);
	if (it != cache.end()) {
		it->second.clear();
	}
	cache.insert(pair<string, RVector>(name, l));
	mutex.unlock();
}

bool RegistryCache::empty(const string name) {
	return cache.size() == 0;
}

void RegistryCache::clear() {
	mutex.lock();
	cache.clear();
	mutex.unlock();
}

bool RegistryCache::exists(const string& name) {
	try {
		mutex.lock();
		RMap::iterator it = cache.find(name);
		mutex.unlock();
		return it != cache.end();
	} catch (const std::exception& ex) {
		logger::warn("RegistryCache.exists failure, message: %s", ex.what());
	}
	return false;
}
// return hosts of service name; name eg. /soa/services/test.http
RVector* RegistryCache::get(const string name) {
	RVector *ptr = NULL;
	try {
		mutex.lock();
		RMap::iterator it = cache.find(name);
		mutex.unlock();
		if (it != cache.end()) {
			ptr = &(it->second);
		}
	} catch (const std::exception& ex) {
		logger::warn("RegistryCache.get failure, message: %s", ex.what());
	}
	return ptr;
}

int RegistryCache::size() {
	return cache.size();
}

void RegistryCache::from_file(const string& filename) {
	FileCache f(filename);
	string content;
	if(f.open_read() && f.read_all(content)){
		// parse json doc
		RVector v = Registry::unserialize(content);
		// copy to cache
		for(RVector::iterator it = v.begin(); it != v.end(); ++it ) {
			(*it).ephemeral = "from_file";
			(*it).ctime = utils::now_in_ms();
			this->add(*it);
		}
		logger::warn("loaded from file. service count=%ld, item count= %ld", cache.size(), v.size());
	}
}

string RegistryCache::dump() {
	stringstream ss;
	RMap::iterator mit = cache.begin();
	ss << "	dump cache. address=" << &cache << " size=" << cache.size() << endl;
	int i = 0;
	int max = 1000;
	while(mit != cache.end() && ++i < max) {
		RVector &v = mit->second;
		RVector::iterator vit = v.begin();
		while(vit != v.end() ) {
			Registry &r = *vit;
			ss<< "\t" << Registry::serialize(r) << endl;
			++vit ;
		}
		++mit;
	}
	if(i >= max) {
		ss << "	......" << endl;
	}
	return ss.str();
}


bool RegistryCache::save(const string& filename) {
	// simple format, lucky you, name is zkpath
	RVector v;
	for(RMap::iterator mit = cache.begin(); mit != cache.end(); ++mit ) {
		RVector& pv = mit->second;
		for(RVector::iterator vit = pv.begin(); vit != pv.end(); ++vit ) {
			v.push_back(*vit);
		}
	}

	string json = Registry::serialize(v);
	cout << json << endl;
	try {
		FileCache f(filename);
		if (f.open_write() && f.write(json)) {
			f.flush();
		} else {
			logger::warn("RegistryCache.save fail to open file %s ", filename.c_str());
		}
	} catch (const std::exception& ex) {
		logger::warn("RegistryCache.save fail to save file %s.  cause of %s", filename.c_str(), ex.what());
		return false;
	}
	return true;
}

} /* namespace frp */
