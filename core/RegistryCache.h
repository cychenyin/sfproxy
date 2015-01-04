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
#include <sstream>
#include <stdio.h>
#include <map>
#include <string>
#include <list>
#include <vector>
#include <algorithm>

// #include "ganji/util/thread/mutex.h"
#include "concurrency/Mutex.h"
#include "FileCache.h"

using namespace std;

namespace FinagleRegistryProxy {


typedef vector<Registry> RVector;
typedef map<string, RVector > RMap;

class RegistryCache {
public:
	RMap cache;

public:
	RegistryCache();
	virtual ~RegistryCache();


	void add(Registry& proxy);
	// name eg /sao/services/test.thrift, ephemeral eg member00000001
	void remove(const string name, const string ephemeral);
	// name eg /sao/services/test.thrift
	void remove(const string name, Registry& reg);
	// name eg /sao/services/test.thrift
	void remove(const string name);
	// name eg /sao/services/test.thrift
	void replace(const string name, RVector& l);
	// name eg /sao/services/test.thrift
	bool empty(const string name);
	void clear();
	int size();
	// name eg /sao/services/test.thrift
	RVector* get(const string name);
	// name eg /sao/services/test.thrift
	bool exists(const string& name);

	bool save(const string& filename);
	void from_file(const string& filename);

	string dump() {
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

private:
	// ::ganji::util::thread::Mutex mutex;
	apache::thrift::concurrency::Mutex mutex;
};

} /* namespace frp */

#endif /* REGISTRYCACHE_H_ */
