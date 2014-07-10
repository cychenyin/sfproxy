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
			cout << " vsize=" << (v ? v->size() : 0) << endl;
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
		vector<Registry> *v = cache.get(name.c_str());
		cout << " addTest cache size=" << cache.size() << " get result size=" << (v ? v->size() : 0) << " ref=" << &(*v)
				<< endl;

		v = cache.get(r.name.c_str());
		cout << " addTest cache size=" << cache.size() << " get result size=" << (v ? v->size() : 0) << " ref=" << &(*v)
				<< endl;

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
		v = cache.get(r.name.c_str());
		cout << " addTest cache size=" << cache.size() << " get result size=" << (v ? v->size() : 0) << " ref=" << &(*v)
				<< endl;
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
		v = cache.get(r.name.c_str());
		cout << " addTest cache size=" << cache.size() << " get result size=" << (v ? v->size() : 0) << " ref=" << &(*v)
				<< endl;
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
//	v = cache.get(r.name.c_str());
//	cout << " addTest cache size=" << cache.size() << " get result size=" << (v ? v->size() : 0) << " ref=" << &(*v) << endl;
//	if (v && v->begin() != v->end()) {
//		Registry &p = *(v->begin());
//		cout << "ok. name=" << p.name << "	host=" << p.host << "	port=" << p.port << "	w=" << p.weight() << endl;
//	} else {
//		cout << "fail to add or get" << endl;
//	}

	}

	void removeTest() {
		cache.clear();
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

		vector<Registry> *v = cache.get(name.c_str());
		cout << " addTest cache size=" << cache.size() << " get vector size=" << (v ? v->size() : 0) << " ref=" << &(*v)
				<< endl;

		cache.remove(name.c_str(), r1);
		v = cache.get(name.c_str());
		cout << " addTest cache size=" << cache.size() << " get vector size=" << (v ? v->size() : 0) << " ref=" << &(*v)
				<< endl;

		cache.remove(name.c_str(), r2);
		v = cache.get(name.c_str());
		cout << " addTest cache size=" << cache.size() << " get vector size=" << (v ? v->size() : 0) << " ref=" << &(*v)
				<< endl;

		cache.add(r1);
		cache.add(r1);
		cache.add(r2);

		cache.remove(name.c_str(), r2);
		v = cache.get(name.c_str());
		cout << " addTest cache size=" << cache.size() << " get vector size=" << (v ? v->size() : 0) << " ref=" << &(*v)
				<< endl;

		cache.remove(name.c_str(), r1);
		v = cache.get(name.c_str());
		cout << " addTest cache size=" << cache.size() << " get vector size=" << (v ? v->size() : 0) << " ref=" << &(*v)
				<< endl;

	}
	void removeTest1() {
		cache.clear();
		string name("remove");
		Registry r1 = createRegistry();
		r1.name = name;
		cache.add(r1);
		cache.add(r1);

		Registry r2 = createRegistry();
		r2.name = name;
		r2.host = "remove2";
		cache.add(r2);

		vector<Registry> *v = cache.get(name.c_str());
		cout << " addTest cache size=" << cache.size() << " get vector size=" << (v ? v->size() : 0) << endl;
		cout << "\t\t\texpected 2" << endl;

		cache.remove(name.c_str());
		v = cache.get(name.c_str());
		cout << " addTest cache size=" << cache.size() << " get vector size=" << (v ? v->size() : 0) << endl;
		cout << "\t\t\texpected 0" << endl;

		cache.add(r1);
		cache.add(r1);
		cache.add(r2);

		cache.remove(name.c_str());
		v = cache.get(name.c_str());
		cout << " addTest cache size=" << cache.size() << " get vector size=" << (v ? v->size() : 0) << endl;
//		cout << " addTest cache size=" << cache.size() << " get vector size=" << (v ? v->size() : -1) << endl;
		cout << "\t\t\texpected 0" << endl;
	}

	void removeTest2() {
		cache.clear();
		cout << "removeTest2222222222222222222222222222222222" << endl;
		//string name("remove2");
		string name("/soa/services/testservice");
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

		vector<Registry> *v = cache.get(name.c_str());
		cout << " 1addTest cache size=" << cache.size() << " get vector size=" << (v ? v->size() : 0) << endl;
		cout << "\t\t\texpected vs=2" << endl;

		cache.remove(name.c_str(), r1.ephemeral.c_str());
		v = cache.get(name.c_str());
		cout << " 2addTest cache size=" << cache.size() << " get vector size=" << (v ? v->size() : 0) << endl;
		cout << "\t\t\texpected vs=1" << endl;

		cache.remove(name.c_str(), r2.ephemeral.c_str());
		v = cache.get(name.c_str());
		cout << " 3addTest cache size=" << cache.size() << " get vector size=" << (v ? v->size() : 0) << endl;
		//		cout << " addTest cache size=" << cache.size() << " get vector size=" << (v ? v->size() : -1) << endl;
		cout << "\t\t\texpected 0" << endl;

		cache.add(r1);
		cache.add(r1);
		cache.add(r2);
		cache.add(r2);
		v = cache.get(name.c_str());
		cout << " 4addTest cache size=" << cache.size() << " get vector size=" << (v ? v->size() : 0) << endl;
		cout << "\t\t\texpected vs=2" << endl;

		cache.remove(name.c_str(), (r1.ephemeral + " xxx").c_str());
		v = cache.get(name.c_str());
		cout << " 5addTest cache size=" << cache.size() << " get vector size=" << (v ? v->size() : 0) << endl;
		cout << "\t\t\texpected vs=2" << endl;

		cache.remove((name + " xxx").c_str(), r1.ephemeral.c_str());
		v = cache.get(name.c_str());
		cout << " 6addTest cache size=" << cache.size() << " get vector size=" << (v ? v->size() : 0) << endl;
		cout << "\t\t\texpected vs=2" << endl;

		cout << "done.................................." << endl;
	}

	void clearTest() {

	}
	void replaceTest() {
	}

	void emptyTest() {
	}
	void jsonTest() {
		//		Registry r;
		//		r.name = "/soa/testserivce";
		//		r.host = "127.0.0.1";
		//		r.port = 9999;
		//		r.ctime = time(NULL) / 1000;
		//		//		_return = Registry::toJsonString(r);
		//		vector<Registry> v;
		//		v.push_back(r);
		//		v.push_back(Registry("/soa/testserivce", "localhost", 8888));
		//
		//		_return = Registry::toJsonString(v);

	}
};

} /* namespace ut */

#endif /* REGISTRYCACHETEST_H_ */
