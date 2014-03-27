/*
 * RegistryCacheTest.cpp
 *
 *  Created on: Mar 13, 2014
 *      Author: asdf
 */

// #include "RegistryCacheTest.h"
#ifndef REGISTRYCACHETEST_H_
#define REGISTRYCACHETEST_H_

#include <iostream>
#include "../core/RegistryCache.h"

using namespace std;
using namespace FinagleRegistryProxy;

namespace ut {
class RegistryCacheTest {
private:
	int port;
	RegistryCache cache;

public:

	RegistryCacheTest() {
		port = 1;
	}

	~RegistryCacheTest() {
	}

	Registry createRegistry() {
		Registry r;  // = Registry("name", "host", ++port);
		r.name = "name";
		r.host = "host";
		r.port = ++port;
		return r;
	}

	void getTest() {
		vector<Registry> *v = cache.get("name");
		if (v) {
			cout << " vsize=" << v->size() << endl;
		} else {
			cout << "not found." << endl;
		}
	}
	void addTest() {
		Registry r = createRegistry();
		r.name = "add";
		cache.add(r);
		cache.add(r);
		cache.add(r);
		cache.add(r);
		cache.add(r);

		string name("add");
		vector<Registry> *v = cache.get(name);
		cout << " addTest cache size=" << cache.size() << " get result size=" << v->size() << " ref=" << &(*v) << endl;

		v = cache.get(r.name);
		cout << " addTest cache size=" << cache.size() << " get result size=" << v->size() << " ref=" << &(*v) << endl;

		if (v->begin() == v->end()) {
			cout << "nnnnnnnnnnnnnn" << endl;
		}

		if (v && v->begin() != v->end()) {
			Registry &p = *(v->begin());
			cout << "ok. name=" << p.name << "	host=" << p.host << "	port=" << p.port << "	w=" << p.weight() << endl;
		} else {
			cout << "fail to add or get" << endl;
		}

		cout << "2222222222222222" << endl;
		r = createRegistry();
		r.name = "add2";
		cache.add(r);
		v = cache.get(r.name);
		cout << " addTest cache size=" << cache.size() << " get result size=" << v->size() << " ref=" << &(*v) << endl;
		if (v && v->begin() != v->end()) {
			Registry &p = *(v->begin());
			cout << "ok. name=" << p.name << "	host=" << p.host << "	port=" << p.port << "	w=" << p.weight() << endl;
		} else {
			cout << "fail to add or get" << endl;
		}

		cout << "3333333333333333" << endl;
		r = createRegistry();
		r.name = "add3";
		cache.add(r);
		cache.add(r);
		cache.add(r);
		v = cache.get(r.name);
		cout << " addTest cache size=" << cache.size() << " get result size=" << v->size() << " ref=" << &(*v) << endl;
		if (v && v->begin() != v->end()) {
			Registry &p = *(v->begin());
			cout << "ok. name=" << p.name << "	host=" << p.host << "	port=" << p.port << "	w=" << p.weight() << endl;
		} else {
			cout << "fail to add or get" << endl;
		}

//	cout << "4444444444444444" << endl;
//	r = createRegistry();
//	r.name = "add4";
//	cache.add(r);
//	v = cache.get(r.name);
//	cout << " addTest cache size=" << cache.size() << " get result size=" << v->size() << " ref=" << &(*v) << endl;
//	if (v && v->begin() != v->end()) {
//		Registry &p = *(v->begin());
//		cout << "ok. name=" << p.name << "	host=" << p.host << "	port=" << p.port << "	w=" << p.weight() << endl;
//	} else {
//		cout << "fail to add or get" << endl;
//	}

	}

	void removeTest() {
		string name("remove");
		Registry r1 = createRegistry();
		r1.name = name;
		cache.add(r1);
		cache.add(r1);
		cout << "aaaaaaaaaaaaaaaaaaaaaaa";

		Registry r2 = createRegistry();
		r2.name = name;
		r2.host = "remove2";
		cache.add(r2);

		vector<Registry> *v = cache.get(name);
		cout << " addTest cache size=" << cache.size() << " get vector size=" << v->size() << " ref=" << &(*v) << endl;

		cache.remove(name, r1);
		v = cache.get(name);
		cout << " addTest cache size=" << cache.size() << " get vector size=" << v->size() << " ref=" << &(*v) << endl;

		cache.remove(name, r2);
		v = cache.get(name);
		cout << " addTest cache size=" << cache.size() << " get vector size=" << v->size() << " ref=" << &(*v) << endl;

		cache.add(r1);
		cache.add(r1);
		cache.add(r2);

		cache.remove(name, r2);
		v = cache.get(name);
		cout << " addTest cache size=" << cache.size() << " get vector size=" << v->size() << " ref=" << &(*v) << endl;

		cache.remove(name, r1);
		v = cache.get(name);
		cout << " addTest cache size=" << cache.size() << " get vector size=" << v->size() << " ref=" << &(*v) << endl;

	}
	void removeTest1() {
		string name("remove");
		Registry r1 = createRegistry();
		r1.name = name;
		cache.add(r1);
		cache.add(r1);

		Registry r2 = createRegistry();
		r2.name = name;
		r2.host = "remove2";
		cache.add(r2);

		vector<Registry> *v = cache.get(name);
		cout << " addTest cache size=" << cache.size() << " get vector size=" << (v ? v->size() : 0) << endl;
		cout << "	expected 0" << endl;

		cache.remove(name);
		v = cache.get(name);
		cout << " addTest cache size=" << cache.size() << " get vector size=" << (v ? v->size() : 0) << endl;
		cout << "	expected 0" << endl;

		cache.add(r1);
		cache.add(r1);
		cache.add(r2);

		cache.remove(name);
		v = cache.get(name);
		cout << " addTest cache size=" << cache.size() << " get vector size=" << (v ? v->size() : 0) << endl;
//		cout << " addTest cache size=" << cache.size() << " get vector size=" << (v ? v->size() : -1) << endl;
		cout << "	expected 1" << endl;

	}

	void removeTest2() {
		string name("remove2");
		Registry r1 = createRegistry();
		r1.name = name;
		r1.ephemeral = r1.name + r1.host;
		cache.add(r1);
		cache.add(r1); // vs=1

		Registry r2 = createRegistry();
		r2.name = name;
		r2.host = "remove2";
		r2.ephemeral = r2.name + r2.host;
		cache.add(r2); // vs=2

		vector<Registry> *v = cache.get(name);
		cout << " 1addTest cache size=" << cache.size() << " get vector size=" << (v ? v->size() : 0) << endl;
		cout << "	expected vs=2" << endl;

		cache.remove(name, r1.ephemeral);
		v = cache.get(name);
		cout << " 2addTest cache size=" << cache.size() << " get vector size=" << (v ? v->size() : 0) << endl;
		cout << "	expected vs=1" << endl;

		cache.remove(name, r2.ephemeral);
		v = cache.get(name);
		cout << " 3addTest cache size=" << cache.size() << " get vector size=" << (v ? v->size() : 0) << endl;
		//		cout << " addTest cache size=" << cache.size() << " get vector size=" << (v ? v->size() : -1) << endl;
		cout << "	expected 0" << endl;

		cache.add(r1);
		cache.add(r1);
		cache.add(r2);
		cache.add(r2);
		v = cache.get(name);
		cout << " 4addTest cache size=" << cache.size() << " get vector size=" << (v ? v->size() : 0) << endl;
		cout << "	expected vs=2" << endl;

		cache.remove(name, r1.ephemeral + " xxx");
		v = cache.get(name);
		cout << " 5addTest cache size=" << cache.size() << " get vector size=" << (v ? v->size() : 0) << endl;
		cout << "	expected vs=2" << endl;

		cache.remove(name + " xxx", r1.ephemeral);
		v = cache.get(name);
		cout << " 6addTest cache size=" << cache.size() << " get vector size=" << (v ? v->size() : 0) << endl;
		cout << "	expected vs=2" << endl;

		cout << "done.................................." << endl;
	}

	void clearTest() {

	}
	void replaceTest() {
	}

	void emptyTest() {
	}
};

} /* namespace ut */

#endif /* REGISTRYCACHETEST_H_ */
