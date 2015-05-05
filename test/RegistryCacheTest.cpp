/*
 * RegistryCacheTest.cpp
 *
 *  Created on: Mar 13, 2014
 *      Author: asdf
 */

#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <boost/test/unit_test.hpp>

#include "../core/RegistryCache.h"
#include "../core/FileCache.h"

using namespace std;
using namespace FinagleRegistryProxy;

BOOST_AUTO_TEST_SUITE( registry_cache_suite )

static int counter = 0;
struct F {
	int port;
	RegistryCache cache;
	F() {
		port = 1;
	}
	~F() {
		BOOST_TEST_MESSAGE("============== end of test method " << ++ counter);
	}
	Registry createRegistry() {
		Registry r;  // = Registry("name", "host", ++port);
		r.name = "name";
		r.host = "host";
		r.port = ++port;
		r.ephemeral = "test";
		r.ctime = utils::now_in_ms();
		return r;
	}
};

BOOST_FIXTURE_TEST_CASE( empty , F) {
	BOOST_CHECK(true);
}

BOOST_FIXTURE_TEST_CASE( add_test , F) {
	Registry r = createRegistry();
	r.name = "add";
	cache.add(r);
	cache.add(r);
	cache.add(r);
	cache.add(r);
	cache.add(r);
	BOOST_CHECK(cache.size() == 1);

	string name("add");
	vector<Registry> *v = cache.get(name.c_str());
//	cout << " addTest cache size=" << cache.size() << " get result size=" << (v ? v->size() : 0) << " ref=" << &(*v) << endl;

	v = cache.get(r.name.c_str());
//	cout << " addTest cache size=" << cache.size() << " get result size=" << (v ? v->size() : 0) << " ref=" << &(*v) << endl;

	BOOST_CHECK(v != NULL && v->size() == 1);

	if (v && v->begin() != v->end()) {
		Registry &p = *(v->begin());
//		cout << "ok. name=" << p.name << "	host=" << p.host << "	port=" << p.port << "	w=" << p.weight() << endl;
	} else {
//		cout << "fail to add or get" << endl;
	}

	//cout << "2222222222222222" << endl;
	r = createRegistry();
	r.name = "add2";
	cache.add(r);
	v = cache.get(r.name.c_str());
//	cout << " addTest cache size=" << cache.size() << " get result size=" << (v ? v->size() : 0) << " ref=" << &(*v) << endl;
	BOOST_CHECK(v->size() > 0);
	if (v && v->begin() != v->end()) {
		Registry &p = *(v->begin());
//		cout << "ok. name=" << p.name << "	host=" << p.host << "	port=" << p.port << "	w=" << p.weight() << endl;
	} else {
//		cout << "fail to add or get" << endl;
	}

	// cout << "3333333333333333" << endl;
	r = createRegistry();
	r.name = "add3";
	cache.add(r);
	cache.add(r);
	cache.add(r);
	v = cache.get(r.name.c_str());
//	cout << " addTest cache size=" << cache.size() << " get result size=" << (v ? v->size() : 0) << " ref=" << &(*v) << endl;
	BOOST_CHECK(v->size() > 0);
	if (v && v->begin() != v->end()) {
		Registry &p = *(v->begin());
//		cout << "ok. name=" << p.name << "	host=" << p.host << "	port=" << p.port << "	w=" << p.weight() << endl;
	} else {
//		cout << "fail to add or get" << endl;
	}
}

BOOST_FIXTURE_TEST_CASE( replace_test , F) {
	Registry r = createRegistry();
	r.name = "/soa";
	Registry r2 = createRegistry();
	r2.name = "/soa2";
	cache.add(r);
	const string name = r.name;
	RVector v;
	v.push_back(r2);
	cache.replace(r.name, v);

	BOOST_CHECK(cache.size() == 1);
}
BOOST_FIXTURE_TEST_CASE( clear_test , F) {
	Registry r = createRegistry();
	cache.add(r);
	cache.add(r);
	cache.add(r);
	cache.clear();
	BOOST_CHECK(cache.size() == 0);

	r = createRegistry();
	cache.add(r);
	cache.clear();
	BOOST_CHECK(cache.size() == 0);

	for (int i = 0; i < 10; i++) {
		r = createRegistry();
		cache.add(r);
	}
	cache.clear();
	BOOST_CHECK(cache.size() == 0);

	for (int i = 0; i < 10; i++) {
		r = createRegistry();
		stringstream ss;
		ss << r.name << i;
		r.name = ss.str();
		cache.add(r);
	}
	cache.clear();
	BOOST_CHECK(cache.size() == 0);

}

BOOST_FIXTURE_TEST_CASE( get_test , F) {
	Registry r = createRegistry();
	r.name = "abc";
	r.ephemeral = r.name + r.host;
	cache.add(r);

	vector<Registry> *v = cache.get(r.name);
	BOOST_CHECK((v ? v->size() : 0) == 1);

}

BOOST_FIXTURE_TEST_CASE( remove1 , F) {
	cache.clear();
	BOOST_CHECK(cache.size() == 0);
	//string name("remove2");
	string name("/soa/services/testservice");
	Registry r1 = createRegistry();
	r1.name = name;
	r1.ephemeral = r1.name + r1.host;
	cache.add(r1);
	cache.add(r1); // vs=1
	BOOST_CHECK(cache.size() == 1);

	Registry r2 = createRegistry();
	r2.name = name;
	r2.host = "remove2";
	r2.ephemeral = r2.name + r2.host;
	cache.add(r2); // vs=2
//	cout << Registry::serialize(r1) << endl;
//	cout << Registry::serialize(r2) << endl;
//	cout << "size-===============" << cache.size() << endl;
	BOOST_CHECK(cache.size() == 1);

	vector<Registry> *v = cache.get(name.c_str());
//	cout << " 1addTest cache size=" << cache.size() << " get vector size=" << (v ? v->size() : 0) << endl;
//	cout << "\t\t\texpected vs=2" << endl;
	BOOST_CHECK(v->size() == 2);

	cache.remove(name.c_str(), r1.ephemeral.c_str());
	v = cache.get(name.c_str());
//	cout << " 2addTest cache size=" << cache.size() << " get vector size=" << (v ? v->size() : 0) << endl;
//	cout << "\t\t\texpected vs=1" << endl;
	BOOST_CHECK(v->size() == 1);
	cache.remove(name.c_str(), r2.ephemeral.c_str());
	v = cache.get(name.c_str());
//	cout << " 3addTest cache size=" << cache.size() << " get vector size=" << (v ? v->size() : 0) << endl;
//	//		cout << " addTest cache size=" << cache.size() << " get vector size=" << (v ? v->size() : -1) << endl;
//	cout << "\t\t\texpected 0" << endl;
	BOOST_CHECK(v->size() == 0);

	cache.add(r1);
	cache.add(r1);
	cache.add(r2);
	cache.add(r2);
	v = cache.get(name.c_str());
//	cout << " 4addTest cache size=" << cache.size() << " get vector size=" << (v ? v->size() : 0) << endl;
//	cout << "\t\t\texpected vs=2" << endl;
	BOOST_CHECK(v->size() == 2);

	cache.remove(name.c_str(), (r1.ephemeral + " xxx").c_str());
	v = cache.get(name.c_str());
//	cout << " 5addTest cache size=" << cache.size() << " get vector size=" << (v ? v->size() : 0) << endl;
//	cout << "\t\t\texpected vs=2" << endl;
	BOOST_CHECK(v->size() == 2);

	cache.remove((name + " xxx").c_str(), r1.ephemeral.c_str());
	v = cache.get(name.c_str());
//	cout << " 6addTest cache size=" << cache.size() << " get vector size=" << (v ? v->size() : 0) << endl;
//	cout << "\t\t\texpected vs=2" << endl;
	BOOST_CHECK(v->size() == 2);
//	cout << "done.................................." << endl;
}
BOOST_FIXTURE_TEST_CASE( remove2 , F) {
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
//	cout << " addTest cache size=" << cache.size() << " get vector size=" << (v ? v->size() : 0) << " ref=" << &(*v) << endl;
	BOOST_CHECK((v ? v->size() : 0) == 2);

	cache.remove(r1);
	v = cache.get(name.c_str());
//	cout << " addTest cache size=" << cache.size() << " get vector size=" << (v ? v->size() : 0) << " ref=" << &(*v) << endl;
	BOOST_CHECK((v ? v->size() : 0) == 1);
	cache.remove(r2);
	v = cache.get(name.c_str());
//	cout << " addTest cache size=" << cache.size() << " get vector size=" << (v ? v->size() : 0) << " ref=" << &(*v) << endl;
	BOOST_CHECK((v ? v->size() : 0) == 0);

	cache.add(r1);
	cache.add(r1);
	cache.add(r2);

	cache.remove(r2);
	v = cache.get(name.c_str());
//	cout << " addTest cache size=" << cache.size() << " get vector size=" << (v ? v->size() : 0) << " ref=" << &(*v) << endl;
	BOOST_CHECK((v ? v->size() : 0) == 1);
	cache.remove(r1);
	v = cache.get(name.c_str());
//	cout << " addTest cache size=" << cache.size() << " get vector size=" << (v ? v->size() : 0) << " ref=" << &(*v) << endl;
	BOOST_CHECK((v ? v->size() : 0) == 0);
	BOOST_CHECK(true);
}
BOOST_FIXTURE_TEST_CASE( remove3 , F) {
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
//	cout << " addTest cache size=" << cache.size() << " get vector size=" << (v ? v->size() : 0) << endl;
//	cout << "\t\t\texpected 2" << endl;
	BOOST_CHECK((v ? v->size() : 0) == 2);
	cache.remove(name.c_str());
	v = cache.get(name.c_str());
//	cout << " addTest cache size=" << cache.size() << " get vector size=" << (v ? v->size() : 0) << endl;
//	cout << "\t\t\texpected 0" << endl;
	BOOST_CHECK((v ? v->size() : 0) == 0);
	cache.add(r1);
	cache.add(r1);
	cache.add(r2);

	cache.remove(name.c_str());
	v = cache.get(name.c_str());
//	cout << " addTest cache size=" << cache.size() << " get vector size=" << (v ? v->size() : 0) << endl;
////		cout << " addTest cache size=" << cache.size() << " get vector size=" << (v ? v->size() : -1) << endl;
//	cout << "\t\t\texpected 0" << endl;
	BOOST_CHECK((v ? v->size() : 0) == 0);
	BOOST_CHECK(true);
}

BOOST_FIXTURE_TEST_CASE( remove4 , F) {
	uint32_t time = 100;
	// can delete case
	Registry r = createRegistry();
	r.mtime = time;
//	cache.add(r);
//	cache.add(r);
//	BOOST_CHECK(cache.size() == 1);
//	cache.remove_overdue(time + 1);
	RVector* v = cache.get(r.name);
//	BOOST_CHECK((v ? v->size() : 0) == 0);
//	BOOST_CHECK(cache.size() == 0);
//	// multi can del case
//	cache.clear();
//	BOOST_CHECK(cache.size() == 0);
	int nameTotal = 20;
	for(int i =0; i< nameTotal;i++) {
		r = createRegistry();
		r.mtime = time;
		cache.add(r);
	}

	v = cache.get(r.name);
	BOOST_CHECK(cache.size() == 1);
	BOOST_CHECK((v ? v->size() : 0) == nameTotal);
	r = createRegistry();
	r.name = r.name + "x";
	r.mtime = time;
	cache.add(r);
	v = cache.get(r.name);
	BOOST_CHECK(cache.size() == 2);
	BOOST_CHECK((v ? v->size() : 0) == 1);

//	cout << "before commit remove_overdue "<< cache.dump() << endl;
	cache.remove_overdue(time + 1);
	v = cache.get(r.name);
	BOOST_CHECK((v ? v->size() : 0) == 0);
//	cout << "after remove_overdue sizeeeeeeeeeeeeeeeeeeeeeeee: "<< cache.size() << endl;
//	cout << cache.dump() << endl;
	BOOST_CHECK(cache.size() == 0);
	// cannot delete case
	cache.clear();
	r = createRegistry();
	r.mtime = time;
	cache.add(r);

	cache.remove_overdue(time - 1);
	v = cache.get(r.name);
	BOOST_CHECK((v ? v->size() : 0) == 1);

	// equal case
	cache.clear();
	r = createRegistry();
	r.mtime = time;
	cache.add(r);
	BOOST_CHECK(cache.size() == 1);
	v = cache.get(r.name);
	BOOST_CHECK((v ? v->size() : 0) == 1);

	cache.remove_overdue(time);
	v = cache.get(r.name);
	BOOST_CHECK((v ? v->size() : 0) == 1);

	BOOST_CHECK(true);
}

BOOST_FIXTURE_TEST_CASE( remove5_memory_leak_test , F) {
	uint32_t time = 100;
	int round = 100 * 1000; // 100 * 10000;
	for (int i = 0; i < round; i++) {
		Registry r = createRegistry();
		r.mtime = time;
		cache.add(r);
		cache.add(r);
		BOOST_CHECK(cache.size() == 1);

		cache.remove_overdue(time + 1);
		RVector* v = cache.get(r.name);
		BOOST_CHECK((v ? v->size() : 0) == 0);
	}

	BOOST_CHECK(true);
}

BOOST_FIXTURE_TEST_CASE( json_test, F) {
	Registry r;
	r.name = "/soa/testserivce";
	r.host = "127.0.0.1";
	r.port = 9999;
	r.ctime = time(NULL) / 1000;
//		_return = Registry::toJsonString(r);
	vector<Registry> v;
	v.push_back(r);
	v.push_back(Registry("/soa/testserivce", "localhost", 8888));

	string _return = Registry::serialize(v);
	string s =
			"[{\"name\":\"/soa/testserivce\",\"host\":\"127.0.0.1\",\"port\":9999,\"weight\":3600 },{\"name\":\"/soa/testserivce\",\"host\":\"localhost\",\"port\":8888,\"weight\":3600 }]";
	cout << _return << endl;
	BOOST_CHECK(_return.size() > 40);
}

BOOST_FIXTURE_TEST_CASE( load_remove_overdue, F) {

	cache.from_file("/tmp/frproxy.cache");
	// usleep(1000);
	// cache.remove_overdue(utils::now_in_ms());
	//BOOST_CHECK(cache.size() == 0 );
	BOOST_CHECK(true);
}
BOOST_FIXTURE_TEST_CASE( remove_middle_one, F) {
	string name = "name";
	int middle  = 100;

	Registry r = createRegistry();
	r.name = name + " begin";
	cache.add(r);
	for(int i=0;i< middle; i++) {
		r = createRegistry();
		stringstream ss;
		ss << name << i;
		r.name = ss.str();
		cache.add(r);
	}
	r = createRegistry();
		r.name = name + " end";
		cache.add(r);

	BOOST_CHECK(cache.size() == middle + 2);

	for(int i=middle-1;i>-1; i--) {
		stringstream ss;
		ss << name << i;
		cache.remove(ss.str());
	}
	BOOST_CHECK(cache.size() == 2);

}

BOOST_FIXTURE_TEST_CASE( remove_middle_one2, F) {
	string name = "name";
	int middle  = 100;

	Registry r = createRegistry();
	r.name = name + " begin";
	cache.add(r);
	Registry& m = r;
	for(int i=0;i< middle; i++) {
		r = createRegistry();
		stringstream ss;
		ss << name << i;
		r.name = ss.str();
		cache.add(r);
		m = r;
	}
	r = createRegistry();
		r.name = name + " end";
		cache.add(r);

	BOOST_CHECK(cache.size() == middle + 2);

	cache.remove(m);
//	for(int i=middle-1;i>-1; i--) {
//		stringstream ss;
//		ss << name << i;
//		cache.remove(ss.str());
//	}
	BOOST_CHECK(cache.size() == middle + 1);

}
BOOST_AUTO_TEST_SUITE_END()
