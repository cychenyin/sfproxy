/*
 * RegistryCache.h
 *
 *  Created on: Mar 13, 2014
 *      Author: asdf
 */

#ifndef REGISTRYCACHE_H_
#define REGISTRYCACHE_H_

#include "Registry.h"

#include <iostream>
#include <stdio.h>
#include <map>
#include <string>
#include <list>
#include <vector>
#include <algorithm>

// #include "ganji/util/thread/mutex.h"
#include <thrift/concurrency/Mutex.h>

using namespace std;

namespace FinagleRegistryProxy {


typedef vector<Registry> RVector;
typedef map<string, RVector > RMap;

class RegistryCache {
public:
	RegistryCache();
	virtual ~RegistryCache();

	void add(Registry& proxy);
	void remove(const string name, const string ephemeral);
	void remove(const string name, Registry& reg);
	void remove(const string name);
	void replace(const string name, RVector& l);
	bool empty(const string name);
	void clear();
	int size();
	RVector* get(const string name);

	void dump() {
		RMap::iterator mit = cache.begin();
		cout << "	dump cache. address=" << &cache << " size=" << cache.size() << endl;
		string s;
		while(mit != cache.end()) {
			RVector &v = mit->second;
			RVector::iterator vit = v.begin();
			while(vit != v.end() ) {
				Registry &r = *vit;
				//cout << "	" << Registry::toJsonString(r) << endl;
				s += "	" + Registry::to_json_string(r) + "\n" ;
				++vit ;
			}
			cout << s;
			++mit;
		}
	}
	RMap cache;
private:
	// ::ganji::util::thread::Mutex mutex;
	apache::thrift::concurrency::Mutex mutex;
};

} /* namespace frp */

#endif /* REGISTRYCACHE_H_ */
