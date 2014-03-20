/*
 * RegistryCache.cpp
 *
 *  Created on: Mar 13, 2014
 *      Author: asdf
 */

#include "RegistryCache.h"

namespace FinagleRegistryProxy {

RegistryCache::RegistryCache() {

}

RegistryCache::~RegistryCache() {
}

void RegistryCache::add(Registry& proxy) {
	add(proxy.name, proxy);
}
void RegistryCache::add(string name, Registry& proxy) {
	map<string, vector<Registry> >::iterator it = cache.find(name);
	vector<Registry> *ptr = NULL;
	if (it == cache.end()) {
		*ptr = vector<Registry>();
		cache.insert(pair<string, vector<Registry> >(name, *ptr));
	} else {
		*ptr = it->second;
	}

	ptr->push_back(proxy);
}

void RegistryCache::remove(string name, Registry& proxy) {
	map<string, vector<Registry> >::iterator it = cache.find(name);
	vector<Registry> *ptr = NULL;
	if (it != cache.end()) {
		*ptr = it->second;
//		vector<Registry>::iterator itProxy = find(ptr->begin(), ptr->end(), proxy);
//		if (itProxy != ptr->end()) {
//			ptr->erase(itProxy);
//		}
		vector<Registry>::iterator itProxy = ptr->begin();
		while(itProxy != ptr->end()) {
			if (itProxy != ptr->end()) {
				ptr->erase(itProxy);
				break;
			}
		}
	}
}

void RegistryCache::remove(string name) {
	cache.erase(name);
}

void RegistryCache::replace(string name, vector<Registry> l) {
	map<string, vector<Registry> >::iterator it = cache.find(name);
	if (it != cache.end()) {
		it->second.clear();
	}
	cache.insert(pair<string, vector<Registry> >(name, l));
}

bool RegistryCache::isEmpty(string name) {
	return cache.size() == 0;
}

void RegistryCache::clear() {
	return cache.clear();
}

} /* namespace frp */
