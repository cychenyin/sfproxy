/*
 * ZkClientTest.cpp
 *
 *  Created on: Mar 19, 2014
 *      Author: asdf
 */

#define BOOST_TEST_NO_MAIN

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <boost/test/unit_test.hpp>

#include "../frproxy.h"
#include "../core/ClientPool.h"
#include "../core/ZkClient.h"

using namespace std;
using namespace FinagleRegistryProxy;

BOOST_AUTO_TEST_SUITE( zkclient_suite )
static int counter = 0;
struct F {
	RegistryCache *cache;
	ClientPool *pool;
	string *root;
	StateBag* stateBag;
public:
	F() {
		root = new string("/soa/services");
		cache = new RegistryCache();
		pool = new ClientPool(new ZkClientFactory("127.0.0.1:2181", cache));
		stateBag = new StateBag();
	}
	~F() {
		if (root) {
			delete root;
			root = 0;
		}
		if (pool) {
			delete pool;
			pool = 0;
		}
		if (cache) {
			delete cache;
			cache = 0;
		}
		if (stateBag) {
			delete stateBag;
			stateBag = 0;
		}
		BOOST_TEST_MESSAGE("### destroy test object" << ++ counter << "\n\n");
	}

};

BOOST_FIXTURE_TEST_CASE( clientDestoryOnDisconnectTest, F) {
	ZkClient *c = (ZkClient *) pool->open();
	cout << " pool total=" << pool->size() << " u=" << pool->used() << " i=" << pool->idle() << " client id=" << c->id() << endl;
	// before uncomment following code, you should set_connction method from protected to public firstly.
	// c->set_connected(false);

	cout << " pool total=" << pool->size() << " u=" << pool->used() << " i=" << pool->idle() << " client id=" << c->id() << endl;
	if (c == 0) {
		cout << " crazy, client  destroied " << endl;
	}
	c->close();
	BOOST_CHECK(true);
	cout << "clientDestoryOnDisconnectTest done......." << endl;
}

BOOST_FIXTURE_TEST_CASE( poolMemReleaseTest, F) {
	cout << "poolMemReleaseTest ing" << endl;
	ZkClient *c = (ZkClient *) pool->open();
	c->get_children("/soa/services/testservice");

	sleep(2);
	cout << "waked. deleting" << endl;
	if (root) {
		delete root;
		root = 0;
	}
	if (pool) {
		delete pool;
		pool = 0;
	}
	if (cache) {
		delete cache;
		cache = 0;
	}
	BOOST_CHECK(true);
	cout << "poolMemReleaseTest done." << endl;
}

BOOST_FIXTURE_TEST_CASE( poolTimeoutTest, F) {
	cout << "poolTimeoutTest ing" << endl;
	ZkClient *c = (ZkClient *) pool->open();
	c->get_children("/soa/services/testservice");
//	int i = 30;
//	while (--i > 0) {
//		//usleep(20  * 1000 * 000); // us
//		usleep(200);
//		cout << i << endl;
//		cache->dump();
//	}
	cache->dump();

	cout << " before close" << endl;
	c->close();

	BOOST_CHECK(true);
	cout << "poolTimeoutTest done ......" << endl;
}

BOOST_FIXTURE_TEST_CASE( poolIncTest, F) {
	cout << "poolIncTest ing " << endl;
	ZkClient *client = (ZkClient *) pool->open();

	ZkClient *c1 = (ZkClient *) pool->open();
	ZkClient *c2 = (ZkClient *) pool->open();
	ZkClient *c3 = (ZkClient *) pool->open();
	ZkClient *c4 = (ZkClient *) pool->open();

	client->close();
	if (c1) c1->close();
	if (c2) c2->close();
	if (c3) c3->close();
	if (c4) c4->close();

	BOOST_CHECK(pool->used() == 0);
	BOOST_CHECK(true);
	cout << "poolIncTest done ......" << endl;
}

BOOST_FIXTURE_TEST_CASE(RegistryEqualsTest, F) {
	Registry r = Registry("a", "b", 2);

	cout << Registry::serialize(r) << endl;
	Registry &r1 = r;
	Registry *p = &r;

	cout << (r == r1) << endl;
	cout << (*p == r) << endl;

	r1.name = "aa";

	cout << (p == &r) << endl;
	cout << (p == &r1) << endl;
	cout << (&r == &r1) << endl;

	Registry l = Registry("a", "b", 2);
	cout << (l == r) << endl;
	r.name = "a";
	cout << (l == r) << endl;
	cout << (&l == &r) << endl;

	BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()

