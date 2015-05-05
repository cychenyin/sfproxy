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
#include <thrift/concurrency/Mutex.h>

// #include "ganji/util/thread/mutex.h"
#include "FileCache.h"

using namespace std;

namespace FinagleRegistryProxy {

typedef vector<Registry> RVector;
typedef map<string, RVector > RMap;

/**
 * RegistryCache, cache for registry; similar to stl multimap, but not same cause of biz process in detail
 */
class RegistryCache {
public:
	RMap cache;
private:
	// ::ganji::util::thread::Mutex mutex;
	apache::thrift::concurrency::Mutex mutex;

public:
	RegistryCache();
	virtual ~RegistryCache();


	void add(Registry& proxy);
	// remove instance before timestamp;
	void remove_overdue(uint64_t timestamp);
	// name eg /sao/services/test.thrift
	void remove(Registry& reg);
	// name eg /sao/services/test.thrift, ephemeral eg member00000001
	void remove(const string name, const string ephemeral);
	// name eg /sao/services/test.thrift
	void remove(const string name);
	// name eg /sao/services/test.thrift
	void replace(const string name, RVector& l);
	// name eg /sao/services/test.thrift
	bool empty(const string name);
	void clear();
	// count of services; notice: not count of service instances
	int size();
	// name eg /sao/services/test.thrift
	RVector* get(const string name);
	// name eg /sao/services/test.thrift
	bool exists(const string& name);

	bool save(const string& filename);
	void from_file(const string& filename);

	string dump();

};

} /* namespace frp */

#endif /* REGISTRYCACHE_H_ */
