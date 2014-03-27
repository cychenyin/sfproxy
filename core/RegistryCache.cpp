/*
 * RegistryCache.cpp
 *
 *  Created on: Mar 13, 2014
 *      Author: asdf
 */

#include "RegistryCache.h"
#include <stdexcept>

namespace FinagleRegistryProxy {

RegistryCache::RegistryCache() {

}

RegistryCache::~RegistryCache() {
}

void RegistryCache::add(Registry& reg) {
	string &name = reg.name;
	if (name == "")
		return;
	map<string, vector<Registry> >::iterator it = cache.find(name);
	vector<Registry> *ptr = NULL;
	if (it == cache.end()) {
		vector<Registry> v;
		ptr = &v;
		ptr->push_back(reg);
		// attation: stl copy pair in their implement. so, vector in map this not really the input one.
		cache.insert(pair<string, vector<Registry> >(name, *ptr));
//		cout << "	add new vector " << &v ;
	} else {
		ptr = &(it->second);
//		cout << "	add old vector " ;
		vector<Registry>::iterator itProxy = ptr->begin();
		while (itProxy != ptr->end()) {
			if ((*itProxy).equals(reg)) {
				ptr->erase(itProxy);
				break;
			}
			++itProxy;
		}
		ptr->push_back(reg);
	}

//	cout << " vsize=" << ptr->size() << " point=" << &ptr << " ref=" << &(*ptr) << endl;
}

void RegistryCache::remove(string name, string ephemeral) {
	if (name == "" || ephemeral == "")
		return;
	vector<Registry> *ptr = NULL;
	try {
		ptr = & (cache.at(name));
	} catch (std::out_of_range &ex) {
	} catch (std::exception &ex) {
	} catch (std::string &ex) {
	}
	if(ptr) {
		vector<Registry>::iterator itProxy = ptr->begin();
		while (itProxy != ptr->end()) {
			if ((*itProxy).ephemeral == ephemeral) {
				ptr->erase(itProxy);
				break;
			}
			++itProxy;
		}
	}

//	const string n("" + name + "");
//	 map<string, vector<Registry> >::iterator it = cache.find(n);
//
////		cout << "ffffffffffffffffffffffffffffffffffffffffff" << endl;
//	if (it != cache.end()) {
//		ptr = &(it->second);
//		vector<Registry>::iterator itProxy = ptr->begin();
//		while (itProxy != ptr->end()) {
//			if ((*itProxy).ephemeral == ephemeral) {
//				ptr->erase(itProxy);
//				break;
//			}
//			++itProxy;
//		}
//	}

}

void RegistryCache::remove(const string& name, Registry& reg) {
	map<string, vector<Registry> >::iterator it = cache.find(name);
	vector<Registry> *ptr = NULL;
	if (it != cache.end()) {
		ptr = &(it->second);
//		vector<Registry>::iterator itProxy = find(ptr->begin(), ptr->end(), proxy);
//		if (itProxy != ptr->end()) {
//			ptr->erase(itProxy);
//		}
		vector<Registry>::iterator itProxy = ptr->begin();
		while (itProxy != ptr->end()) {
			if ((*itProxy) == reg) {
				ptr->erase(itProxy);
				break;
			}
			++itProxy;
		}
	}
}

void RegistryCache::remove(const string& name) {
	cache.erase(name);
}

void RegistryCache::replace(const string& name, vector<Registry>& l) {
	map<string, vector<Registry> >::iterator it = cache.find(name);
	if (it != cache.end()) {
		it->second.clear();
	}
	cache.insert(pair<string, vector<Registry> >(name, l));
}

bool RegistryCache::isEmpty(const string& name) {
	return cache.size() == 0;
}

void RegistryCache::clear() {
	return cache.clear();
}

vector<Registry>* RegistryCache::get(const string& name) {
	vector<Registry> *ptr = NULL;
	try {
		map<string, vector<Registry> >::iterator it = cache.find(name);
		if (it != cache.end()) {
			ptr = &(it->second);
			return ptr;
		}
	} catch (const std::exception& ex) {
		cout << "RegistryCache.get failure, message:" << ex.what() << endl;
	}
	return ptr;
}

int RegistryCache::size() {
	return cache.size();
}

} /* namespace frp */
