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


using namespace std;

namespace FinagleRegistryProxy {

class RegistryCache {
public:
	RegistryCache();
	virtual ~RegistryCache();

	void add(Registry& proxy);
	void add(string name, Registry& proxy);
	void remove(string name, Registry& proxy);
	void remove(string name);
	void replace(string name, vector<Registry> l);
	bool isEmpty(string name);
	void clear();

private:
	map<string, vector<Registry> > cache;

};

} /* namespace frp */

#endif /* REGISTRYCACHE_H_ */
