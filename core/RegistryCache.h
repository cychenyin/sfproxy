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

typedef vector<Registry> Rvector;
typedef map<string, Rvector > Rmap;

class RegistryCache {
public:
	RegistryCache();
	virtual ~RegistryCache();

	void add(Registry& proxy);
	void remove(string name, string ephemeral);
	void remove(const string& name, Registry& reg);
	void remove(const string& name);
	void replace(const string& name, vector<Registry>& l);
	bool isEmpty(const string& name);
	void clear();
	int size();
	vector<Registry>* get(const string& name);
private:
	map<string, vector<Registry> > cache;

};

} /* namespace frp */

#endif /* REGISTRYCACHE_H_ */
