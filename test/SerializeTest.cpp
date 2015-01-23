#define BOOST_TEST_NO_MAIN

#include <iostream>
#include <sstream>
#include <boost/test/unit_test.hpp>
#include "../core/RegistryCache.h"
#include "../core/FileCache.h"

using namespace FinagleRegistryProxy;
using namespace std;

BOOST_AUTO_TEST_SUITE( serialize_suite )

RegistryCache* p;
const string filename = "/tmp/serialize.test";
const int count = 2;

Registry createReg(int i=0) {
	Registry r;
	stringstream ss;
	ss << "servier" << i;
	r.name = ss.str();
	r.host = "localhost";
	r.port = i;
	r.ephemeral = "e";
	r.ctime = 1232344;
	return r;
}

void setup() {
	p = new RegistryCache();
	for (int i = 0; i < count; i++) {
		Registry r = createReg(i);
		p->add(r);
	}
}

void teardown() {
	if (p) {
		delete p;
		p = NULL;
	}
	// // cout << "-------------------------------------" << endl;
}

string to_string(RegistryCache* up) {
	if (up) {
		RVector v;
		for (RMap::iterator mit = up->cache.begin(); mit != up->cache.end(); ++mit) {
			RVector& pv = mit->second;
			for (RVector::iterator vit = pv.begin(); vit != pv.end(); ++vit) {
				v.push_back(*vit);
			}
		}
		string json = Registry::serialize(v);
		return json;
	}
	return "";
}

BOOST_AUTO_TEST_CASE( empty ) {
//	// cout << "-------------------------------------empty" << endl;
	setup();
	BOOST_CHECK(true);
	teardown();
}

BOOST_AUTO_TEST_CASE( serialize ) {
//	// cout << "-------------------------------------serialize" << endl;
	setup();

	p->save(filename);
	BOOST_CHECK(true);

	teardown();
}

BOOST_AUTO_TEST_CASE( unserialize0 ) {
//	// cout << "-------------------------------------unserialize0" << endl;
	const string content = "[{\"name\":\"xxxx\" , \"host\":\"localhost\" , \"port\":999 , \"weight\":3600 }]";
	RVector v = Registry::unserialize(content);
	BOOST_CHECK_EQUAL(v.size(), 1);
//	// cout << content << endl;
//	// cout << v.front().host << v.front().port << v.front().name << endl;

	BOOST_CHECK_EQUAL(v.front().host, "localhost");
	BOOST_CHECK_EQUAL(v.front().port, 999);
	BOOST_CHECK_EQUAL(v.front().name, "xxxx");
}

BOOST_AUTO_TEST_CASE( unserialize1 ) {
//	// cout << "-------------------------------------unserialize1" << endl;

	const string content =
			"[{\"name\":\"servier0\",\"host\":\"localhost\",\"port\":0,\"weight\":3600 },{\"name\":\"servier1\",\"host\":\"localhost\",\"port\":1,\"weight\":3600 }]";
	RVector v = Registry::unserialize(content);
	BOOST_CHECK_EQUAL(v.size(), 2);
//	// cout << content << endl;
//	// cout << v[0].host << v[0].port << v[0].name << endl;
//	// cout << v[1].host << v[1].port << v[1].name << endl;

	Registry& l = v[0];

	BOOST_CHECK_EQUAL(l.name, "servier0");
	BOOST_CHECK_EQUAL(l.host, "localhost");
	BOOST_CHECK_EQUAL(l.port, 0);

	Registry& r = v[1];
	BOOST_CHECK_EQUAL(r.name, "servier1");
	BOOST_CHECK_EQUAL(r.host, "localhost");
	BOOST_CHECK_EQUAL(r.port, 1);

	// teardown();
}

BOOST_AUTO_TEST_CASE( unserialize2 ) {
	// cout << "-------------------------------------unserialize2" << endl;
	setup();
	string orginal = to_string(p);
	// cout << orginal << endl;
	RegistryCache* up = new RegistryCache();
	RVector v = Registry::unserialize(orginal);
	string now = Registry::serialize(v);
	// cout<< now << endl;
	BOOST_CHECK_EQUAL(p->size(), v.size());
}

BOOST_AUTO_TEST_CASE( ser_vs_unser ) {
// // cout << "-------------------------------------ser_vs_unser" << endl;
	assert(count > 0);

	setup();
	p->save(filename);

// // cout << to_string(p) << endl;

	RegistryCache* up = new RegistryCache();
	up->from_file(filename);

// // cout << to_string(up) << endl;

	BOOST_CHECK_EQUAL(up->size(), count);

	Registry left = createReg(1);
	RVector* v = p->get(left.name);
	BOOST_CHECK_EQUAL(v->size(), 1);
	Registry right = v->front();
	BOOST_CHECK_EQUAL(left.name, right.name);
	BOOST_CHECK_EQUAL(left.host, right.host);
// // cout << left.host << " = " << right.host << endl;
	BOOST_CHECK_EQUAL(left.port, right.port);

	left = createReg(count - 1);
	BOOST_CHECK_EQUAL(v->size(), 1);
	right = v->front();
	BOOST_CHECK_EQUAL(left.name, right.name);
	BOOST_CHECK_EQUAL(left.host, right.host);
// // cout << left.host << " = " << right.host << endl;
	BOOST_CHECK_EQUAL(left.port, right.port);

	delete up;
	up = NULL;

	teardown();
}

BOOST_AUTO_TEST_CASE( encode_test ) {
// // cout << "-------------------------------------encode" << endl;
	string file = "/tmp/frproxy.cache";
	RegistryCache* cache = new RegistryCache();
	cache->from_file(file);
	// cout << cache->dump() << endl;

	BOOST_CHECK(true);
}


BOOST_AUTO_TEST_CASE( large_data1 ) {
	string file = "/tmp/frproxy.cache";
	RegistryCache* cache = new RegistryCache();
	int count = 10;
	for(int i=0; i<count; i++) {
		Registry r = createReg(i);
		cache->add(r);
	}
	cache->save(file);

	delete cache;
	cache = NULL;

	cache = new RegistryCache();
	cache->from_file(file);
//	cout << cache->dump() << endl;
	BOOST_CHECK(cache->size() > 0);
	BOOST_CHECK(cache->size() == count);
}

BOOST_AUTO_TEST_CASE( large_data2 ) {
	string file = "/tmp/frproxy.cache";
	RegistryCache* cache = new RegistryCache();
	int count = 100;
	for(int i=0; i<count; i++) {
		Registry r = createReg(i);
		cache->add(r);
	}
	cache->save(file);

	delete cache;
	cache = NULL;

	cache = new RegistryCache();
	cache->from_file(file);
	// cout << cache->dump() << endl;
	BOOST_CHECK(cache->size() > 0);
	BOOST_CHECK(cache->size() == count);
}

BOOST_AUTO_TEST_CASE( large_data3 ) {
	string file = "/tmp/frproxy.cache";
	RegistryCache* cache = new RegistryCache();
	int count = 1000;
	for(int i=0; i<count; i++) {
		Registry r = createReg(i);
		cache->add(r);
	}
	cache->save(file);

	delete cache;
	cache = NULL;

	cache = new RegistryCache();
	cache->from_file(file);
	// cout << cache->dump() << endl;
	BOOST_CHECK(cache->size() > 0);
	BOOST_CHECK(cache->size() == count);
}

BOOST_AUTO_TEST_CASE( large_data4 ) {
	string file = "/tmp/frproxy.cache";
	RegistryCache* cache = new RegistryCache();
	int count = 10000;
	for(int i=0; i<count; i++) {
		Registry r = createReg(i);
		cache->add(r);
	}
	cache->save(file);

	delete cache;
	cache = NULL;

	cache = new RegistryCache();
	cache->from_file(file);
	// cout << cache->dump() << endl;
	BOOST_CHECK(cache->size() > 0);
	BOOST_CHECK(cache->size() == count);
}

BOOST_AUTO_TEST_SUITE_END()
