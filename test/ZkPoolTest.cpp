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
		BOOST_TEST_MESSAGE("*************** destroy test object" << ++ counter << "\n\n");
	}

};

//BOOST_FIXTURE_TEST_CASE( empty , F) {
//	BOOST_CHECK(true);
//}

//BOOST_FIXTURE_TEST_CASE( new_not_close, F) {
//	ZkClient *c = (ZkClient*) pool->open();
//	// cout << "using: " << pool->size() << " idle:" << pool->idle() << " total: " << pool->size() << endl;
//	// cout << pool->dump() << endl;
//	BOOST_CHECK(pool->size() == 1);
//	BOOST_CHECK(pool->used() == 1);
//	BOOST_CHECK(pool->idle() == 0);
//	BOOST_CHECK(pool->destroying() == 0);
//}
//
//BOOST_FIXTURE_TEST_CASE( new_close, F) {
//	ZkClient *c = (ZkClient*) pool->open();
//	BOOST_CHECK(pool->size() == 1);
//	BOOST_CHECK(pool->used() == 1);
//	BOOST_CHECK(pool->idle() == 0);
//	BOOST_CHECK(pool->destroying() == 0);
//
//	bool conn = c->get_connected();
//	//cout << pool->dump() << endl;
//	c->close();
//	if (conn) {
//		BOOST_CHECK(pool->size() == 1);
//		BOOST_CHECK(pool->used() == 0);
//		BOOST_CHECK(pool->idle() == 1);
//		BOOST_CHECK(pool->destroying() == 0);
//	} else {
//		BOOST_CHECK(pool->size() == 0);
//		BOOST_CHECK(pool->used() == 0);
//		BOOST_CHECK(pool->idle() == 0);
//		BOOST_CHECK(pool->destroying() == 1);
//	}
//}
//
//BOOST_FIXTURE_TEST_CASE( open_idle_close, F) {
//	ZkClient *c = (ZkClient*) pool->open();
//	BOOST_CHECK(pool->size() == 1);
//	BOOST_CHECK(pool->used() == 1);
//	BOOST_CHECK(pool->idle() == 0);
//	BOOST_CHECK(pool->destroying() == 0);
//
//	bool conn = c->get_connected();
//	c->close();
//	if (conn) {
//		BOOST_CHECK(pool->size() == 1);
//		BOOST_CHECK(pool->used() == 0);
//		BOOST_CHECK(pool->idle() == 1);
//		BOOST_CHECK(pool->destroying() == 0);
//	} else {
//		BOOST_CHECK(pool->size() == 0);
//		BOOST_CHECK(pool->used() == 0);
//		BOOST_CHECK(pool->idle() == 0);
//		BOOST_CHECK(pool->destroying() == 1);
//	}
//	c = (ZkClient*) pool->open();
//	BOOST_CHECK(pool->size() == 1);
//	BOOST_CHECK(pool->used() == 1);
//	BOOST_CHECK(pool->idle() == 0);
//	BOOST_CHECK(pool->destroying() == 0);
//	c->close();
//	if (conn) {
//		BOOST_CHECK(pool->size() == 1);
//		BOOST_CHECK(pool->used() == 0);
//		BOOST_CHECK(pool->idle() == 1);
//		BOOST_CHECK(pool->destroying() == 0);
//	} else {
//		BOOST_CHECK(pool->size() == 0);
//		BOOST_CHECK(pool->used() == 0);
//		BOOST_CHECK(pool->idle() == 0);
//		BOOST_CHECK(pool->destroying() == 1);
//	}
//
//}
//
//BOOST_FIXTURE_TEST_CASE( open2, F) {
//	int total = 2;
//	ZkClient *c = (ZkClient*) pool->open();
//	ZkClient *c1 = (ZkClient*) pool->open();
//	BOOST_CHECK( pool->size() == 2);
//	BOOST_CHECK( pool->used() == 2);
//	BOOST_CHECK( pool->idle() == 0);
//	BOOST_CHECK( c->get_in_using() == 1);
//	BOOST_CHECK( c1->get_in_using() == 1);
//
//	// cout << "opened using: " << pool->size() << " idle:" << pool->idle() << " total: " << pool->size() << endl;
//	int conn = 0;
//	conn += c->get_connected() ? 1: 0;
//	conn += c1->get_connected() ? 1 : 0;
//	c->close();
//	c1->close();
//	BOOST_CHECK( pool->size() == conn);
//	BOOST_CHECK( pool->used() == 0);
//	BOOST_CHECK( pool->idle() == conn);
//	BOOST_CHECK( pool->destroying() == total - conn);
//}
//
//BOOST_FIXTURE_TEST_CASE( open3, F) {
//	int total = 2;
//	int conn = 0;
//	ZkClient *c = (ZkClient*) pool->open();
//	ZkClient *c1 = (ZkClient*) pool->open();
//	BOOST_CHECK( pool->size() == 2);
//	BOOST_CHECK( pool->used() == 2);
//	BOOST_CHECK( pool->idle() == 0);
//	BOOST_CHECK( c->get_in_using() == 1);
//	BOOST_CHECK( c1->get_in_using() == 1);
//
//	conn += c->get_connected() ? 1: 0;
//	c->close();
//	BOOST_CHECK( pool->size() == 1 + conn);
//	BOOST_CHECK( pool->used() == 1);
//	BOOST_CHECK( pool->idle() == conn);
//
//	BOOST_CHECK( c->get_in_using() == 0);
//	BOOST_CHECK( c1->get_in_using() == 1);
//
//	c = (ZkClient*) pool->open();
//
//	BOOST_CHECK( pool->size() == total);
//	BOOST_CHECK( pool->used() == 1 + (c->get_connected() ? 1 : 0) );
//	BOOST_CHECK( pool->idle() == 0);
//	BOOST_CHECK( c->get_in_using() == 1);
//	conn = 0;
//	conn += c->get_connected() ? 1: 0;
//	conn += c1->get_connected() ? 1: 0;
//	c->close();
//	c1->close();
//
//	BOOST_CHECK( pool->size() == conn);
//	BOOST_CHECK( pool->used() == 0);
//	BOOST_CHECK( pool->idle() == conn);
//	BOOST_CHECK( pool->destroying() == total - conn);
//	BOOST_CHECK( c->get_in_using() == 0);
//	BOOST_CHECK( c1->get_in_using() == 0);
//
//}
//
//BOOST_FIXTURE_TEST_CASE( open4, F) {
//	int total = 100;
//	int u = 0;
//	int i = 0;
//	int d = 0;
//	bool conn = false;
//
//	ZkClient *c = (ZkClient*) pool->open();
//	conn = c->get_connected();
//	u += conn ? 1 : 0;
//
//	for(int m=0;m<total; m++) {
//		c->close();
//		u --;
//		if(conn) {
//			i ++;
//		} else
//			d ++;
//		c = (ZkClient*) pool->open();
//		if( d > 0) // connection client is used once, so do that
//			d --;
//		u += c->get_connected() ? 1 : 0;
//		if(i > 0)
//			i --;
//	}
//
//	BOOST_CHECK( pool->size() == u + i);
//	BOOST_CHECK( pool->used() == u);
//	BOOST_CHECK( pool->idle() == i);
//	BOOST_CHECK( pool->destroying() == d);
//
//	cout << pool->destroying() << endl;
//
//	ZkClient *c1 = (ZkClient*) pool->open();
//	for(int i=0;i<100; i++) {
//		c->close();
//		c = (ZkClient*) pool->open();
//	}
//	BOOST_CHECK( pool->size() == 2);
//	BOOST_CHECK( pool->used() == 2);
//	BOOST_CHECK( pool->idle() == 0);
//	c->close();
//	c1->close();
//	BOOST_CHECK( pool->size() == 2);
//	BOOST_CHECK( pool->used() == 0);
//	BOOST_CHECK( pool->idle() == 2);
//}
//
//BOOST_FIXTURE_TEST_CASE( open5, F) {
//	int count = 0;
//	ZkClient *c = 0;
//	int total = 2;
//
//	// ZkClient (*cs)[] = new ZkClient*[total]();
//
//	ZkClient* cs[total]; //  = new ZkClient*[total]();
//	pool->set_max_client(1000);
//	for(int i=0; i<total; i++) {
//		cs[i] = (ZkClient*) pool->open();
//		if(cs[i]) {
//			count ++;
//		}
//	}
//	cout << "closed using: " << pool->size() << " idle:" << pool->idle() << " total: " << pool->size() << " count: " << count << endl;
//	BOOST_CHECK( pool->size() == total);
//	BOOST_CHECK( pool->used() == total);
//	BOOST_CHECK( pool->idle() == 0);
//	for(int i=0; i<total; i++) {
//		if(cs[i]) {
//			cs[i]->close();
//			count --;
//		}
//	}
//	cout << "closed using: " << pool->size() << " idle:" << pool->idle() << " total: " << pool->size() << " count: " << count << endl;
//	BOOST_CHECK( pool->size() == total);
//	BOOST_CHECK( pool->used() == 0);
//	BOOST_CHECK( pool->idle() == total);
//
//}
//
//BOOST_FIXTURE_TEST_CASE( clear, F) {
////	cout << "=====================================clearrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr" << endl;
//	int count = 0;
//	ZkClient *c = 0;
//	int total = 3;
//	ZkClient* cs[total];
//	pool->set_max_client(1000);
//	for(int m = 0; m < 10;  m++) {
//		for(int i=0; i<total; i++) {
//			cs[i] = (ZkClient*) pool->open();
//			if(cs[i])
//				count ++;
//		}
//		pool->clear();
//		BOOST_CHECK( pool->size() == 0);
//		BOOST_CHECK( pool->used() == 0);
//		BOOST_CHECK( pool->idle() == 0);
//		count = 0;
//		for(int i=0; i<total; i++) {
//			cs[i] = (ZkClient*) pool->open();
//			if(cs[i])
//				count ++;
//		}
//		// cout << "closed using: " << pool->size() << " idle:" << pool->idle() << " total: " << pool->size() << " count: " << count << endl;
//		BOOST_CHECK( pool->size() == count);
//		BOOST_CHECK( pool->used() == count);
//		BOOST_CHECK( pool->idle() == 0);
//	}
//}
//
//BOOST_FIXTURE_TEST_CASE( conn, F) {
//	ZkClient * c = (ZkClient*) pool->open();
//	cout << "conn: " << c->get_connected() << endl;
//
//	BOOST_CHECK(true);
//}
//
//BOOST_FIXTURE_TEST_CASE( check , F) {
//	ZkClient * c = (ZkClient*) pool->open();
//	// need mock
////	BOOST_CHECK(c->get_connected());
//
////	BOOST_CHECK(c->check());
////	BOOST_CHECK(c->get_connected());
//	c->close();
////	BOOST_CHECK(c->get_connected());
//}
//
//BOOST_FIXTURE_TEST_CASE( dump , F) {
//
//	ZkClient * c = (ZkClient*) pool->open();
//	//BOOST_CHECK(c->get_connected());
//	pool->dump();
//	BOOST_CHECK(true);
//}
//
//// find segment fault in this case
//BOOST_FIXTURE_TEST_CASE( dump_with_state, F) {
//
//	ZkClient * c = (ZkClient*) pool->open();
//	string path = "/path";
//	string path2 = path + "/0";
//	path2 = path + "/1"; c->save_state(path2, 2, "data1");
//	path2 = path + "/2"; c->save_state(path2, 2, "data2");
//	path2 = path + "/3"; c->save_state(path2, 2, "data3");
//	path2 = path + "/4"; c->save_state(path2, 2, "data4");
//	path2 = path + "/5"; c->save_state(path2, 2, "data5");
//	path2 = path + "/6"; c->save_state(path2, 2, "data6");
//
//	path2 = path + "/6"; c->save_state(path2, 2, "data6xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
//
//	c->close();
//	//cout << pool->dump() << endl;
//	BOOST_CHECK(true);
//}
//
//BOOST_FIXTURE_TEST_CASE( restore_state, F) {
//
//	ZkClient * c = (ZkClient*) pool->open();
//	BOOST_CHECK(c->get_in_using() == 1);
////	cout << pool->dump() << endl;
//	c->close();
////	cout << pool->dump() << endl;
//	BOOST_CHECK(c->get_in_using() == 0);
//	c = (ZkClient*) pool->open();
//	BOOST_CHECK(pool->size() == 1);
//	BOOST_CHECK(c->get_in_using() == 1);
//		cout << pool->dump() << endl;
//
//	string path = "/path";
//	string path2 = path + "/0";
//	path2 = path + "/1"; c->save_state(path2, 2, "data1");
//	path2 = path + "/2"; c->save_state(path2, 2, "data2");
//	path2 = path + "/3"; c->save_state(path2, 2, "data3");
//	path2 = path + "/4"; c->save_state(path2, 2, "data4");
//	path2 = path + "/5"; c->save_state(path2, 2, "data5");
//	path2 = path + "/6"; c->save_state(path2, 2, "data6");
//
//	path2 = path + "/6"; c->save_state(path2, 2, "data6xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
//
//	ZkClient *c1 = (ZkClient*) pool->open();
//	BOOST_CHECK(pool->size() == 2);
//	BOOST_CHECK(c->get_in_using() == 1); // safe
//	c->close();
//	BOOST_CHECK(c->get_in_using() == 0); // safe
//	c1->close();
//
//	c1 = (ZkClient*) pool->open();
////	BOOST_CHECK(c->get_in_using() == 0); // in danger
//	cout << "restore_stateeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee 11111111" << endl;
////	cout << pool->dump() << endl;
//
//	c1->restore_states();
//
//	c1->close();
//	cout << "restore_stateeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee 22222222" << endl;
////	cout << pool->dump() << endl;
//	BOOST_CHECK(true);
//}
//
//BOOST_FIXTURE_TEST_CASE( sim_concurrency, F) {
//	ZkClient * c = (ZkClient*) pool->open();
//	c->close();
//	c = (ZkClient*) pool->open();
//
//	ZkClient *c1 = (ZkClient*) pool->open();
//	// break zookeeper, now
//	// ...
//	c->close();							// c1 move to idle queue, and be destroyed by pool when open claimed.
//	c1->close();
//
//	c1 = (ZkClient*) pool->open();
//	// BOOST_CHECK(c->get_in_using() == 0); // in danger // leading to crash; c is destroyed, unexpected memory address accessing.
//
//	c1->close();
//	BOOST_CHECK(true);
//
//}
void pool_full_internal(ClientPool *pool, int max) {
	pool->set_used_timeout(1000);
	int max_client = max;
	pool->set_max_client(max_client);

	for (int i = 0; i < max_client * 2; i++) {
		ZkClient * c = (ZkClient*) pool->open();
		// cout << "using: " << pool->size() << " idle:" << pool->idle() << " total: " << pool->size() << "*c=" << c << endl;
	}
	cout << "using: " << pool->size() << " idle:" << pool->idle() << " total: " << pool->size() << endl;

	usleep(1100 * 1000);
//	cout << "time out nowwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww" << endl;
	// time out now
	ZkClient * c = (ZkClient*) pool->open();
//	cout << "using: " << pool->size() << " idle:" << pool->idle() << " total: " << pool->size() << endl;
	BOOST_CHECK(c == 0);
	c = (ZkClient*) pool->open();
//	cout << "using: " << pool->size() << " idle:" << pool->idle() << " total: " << pool->size() << endl;
	BOOST_CHECK(c != 0);
	c->close();
	BOOST_CHECK(pool->size() == 1);

}

BOOST_FIXTURE_TEST_CASE( pool_full, F) {
	pool_full_internal(pool, 5);
	for (int i = 1; i < 20; i++) {
		pool_full_internal(pool, i);
	}
}


BOOST_AUTO_TEST_SUITE_END()
