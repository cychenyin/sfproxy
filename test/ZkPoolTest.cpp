/*
 * testtemplate.cpp
 *
 *  Created on: Jan 14, 2015
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

BOOST_AUTO_TEST_SUITE( zkpool_suite )
static int counter = 0;
struct F {
	RegistryCache *cache;
	ClientPool *pool;
	string *root;

public:
	F() {
		root = new string("/soa/services");
		cache = new RegistryCache();
		pool = new ClientPool(new ZkClientFactory("127.0.0.1:2181", cache));
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
		BOOST_TEST_MESSAGE("============== end of test method " << ++ counter);
	}

};

BOOST_FIXTURE_TEST_CASE( empty , F) {
	BOOST_CHECK(true);
}

BOOST_FIXTURE_TEST_CASE( check , F) {
	ZkClient * c = (ZkClient*) pool->open();

	BOOST_CHECK(c->get_connected());

	BOOST_CHECK(c->check());
	BOOST_CHECK(c->get_connected());
	c->close();
	BOOST_CHECK(c->get_connected());

	BOOST_CHECK(true);
}


BOOST_AUTO_TEST_SUITE_END()
