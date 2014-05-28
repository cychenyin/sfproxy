/*
 * RegistryCache.cpp
 *
 *  Created on: Mar 13, 2014
 *      Author: asdf
 */

#include "RegistryCache.h"
#include <stdexcept>
#include "../log/logger.h"

namespace FinagleRegistryProxy {

RegistryCache::RegistryCache() {

}

RegistryCache::~RegistryCache() {
}

void RegistryCache::add(Registry& reg) {
	string &name = reg.name;
	if (name == "" || reg.host == "" || reg.ephemeral == "")
		return;
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
//	const string n("" + name + "");
//	 Rmap::iterator it = cache.find(n);
//
//	if (it != cache.end()) {
//		ptr = &(it->second);
//		Rvector::iterator itProxy = ptr->begin();
//		while (itProxy != ptr->end()) {
//			if ((*itProxy).ephemeral == ephemeral) {
//				ptr->erase(itProxy);
//				break;
//			}
//			++itProxy;
//		}
//	}

}

void RegistryCache::remove(const string name, Registry& reg) {
	mutex.lock();
	RMap::iterator it = cache.find(name);
	RVector *ptr = NULL;
	if (it != cache.end()) {
		ptr = &(it->second);
//		Rvector::iterator itProxy = find(ptr->begin(), ptr->end(), proxy);
//		if (itProxy != ptr->end()) {
//			ptr->erase(itProxy);
//		}
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
	return cache.clear();
	mutex.unlock();
}

RVector* RegistryCache::get(const string name) {
	RVector *ptr = NULL;
	try {
		mutex.lock();
		RMap::iterator it = cache.find(name);
		mutex.unlock();
		if (it != cache.end()) {
			ptr = &(it->second);
			return ptr;
		}
	} catch (const std::exception& ex) {
		logger::warn("RegistryCache.get failure, message: %s", ex.what());
	}
	return ptr;
}

int RegistryCache::size() {
	return cache.size();
}

} /* namespace frp */
