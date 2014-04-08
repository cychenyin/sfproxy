/*
 * ZkClientTest.h
 *
 *  Created on: Mar 19, 2014
 *      Author: asdf
 */

#ifndef ZKCLIENTTEST_H_
#define ZKCLIENTTEST_H_

#include <iostream>

//#include "../core/ClientPool.h"
#include "../core/ZkClient.h"

namespace ut {

using namespace FinagleRegistryProxy;

class ZkClientTest {
private:
	RegistryCache *cache;
	ClientPool *pool;
	string *root;

public:
	ZkClientTest() {
		root = new string("/soa/services");
		cache = new RegistryCache();
		pool = new ClientPool(new ZkClientFactory("yz-cdc-wrk-02.dns.ganji.com:2181", cache));
	}

	virtual ~ZkClientTest() {
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
	}

	void clientDestoryOnDisconnectTest() {

		ZkClient *c = (ZkClient *) pool->open();

		cout << " pool total=" << pool->size() << " used=" << pool->used() << " idle=" << pool->idle() << " client id="
				<< c->id() << endl;
		// before uncomment following code, you should set_connction method from protected to public firstly.
		// c->set_connected(false);

		cout << " pool total=" << pool->size() << " used=" << pool->used() << " idle=" << pool->idle() << " client id="
				<< c->id() << endl;
		if (c == 0) {
			cout << " crazy, client  destroied " << endl;
		}
		c->close();

		cout << " done......." << endl;
	}

	void poolMemReleaseTest() {
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
		cout << "delete done." << endl;
	}
	void poolTimeoutTest() {
		ZkClient *c = (ZkClient *) pool->open();
		c->get_children("/soa/services/testservice");
		int i = 30;
		while (--i > 0) {
			//usleep(20  * 1000 * 000); // us
			sleep(2);
			// c->get_children("/soa/services/testservice");
			cout << i << endl;
			cache->dump();
		}
		cout << " before close" << endl;
		c->close();
		cout << " closed" << endl;
	}
	void poolIncTest() {

		ZkClient *client = (ZkClient *) pool->open();
		cout << " pool total=" << pool->size() << " used=" << pool->used() << " idle=" << pool->idle() << " client id="
				<< client->id() << endl;

		ZkClient *c1 = (ZkClient *) pool->open();
		ZkClient *c2 = (ZkClient *) pool->open();
		ZkClient *c3 = (ZkClient *) pool->open();
		ZkClient *c4 = (ZkClient *) pool->open();
		cout << " 1pool total=" << pool->size() << " used=" << pool->used() << " idle=" << pool->idle() << " client id="
				<< client->id() << endl;
		cout << " 2pool total=" << pool->size() << " used=" << pool->used() << " idle=" << pool->idle() << " client id="
				<< c1->id() << endl;
		// use to test pool exhaust
		// cout << "c2 should be 0; =" << (c2 == 0) << endl;
		cout << " pool total=" << pool->size() << " used=" << pool->used() << " idle=" << pool->idle() << " client id="
				<< c2->id() << endl;
//		cout << " pool total=" << pool->size() << " used=" << pool->used() << " idle=" << pool->idle() << " client id=" << c3->id() << endl;
//		cout << " pool total=" << pool->size() << " used=" << pool->used() << " idle=" << pool->idle() << " client id=" << c4->id() << endl;

		client->close();
		cout << " 3pool total=" << pool->size() << " used=" << pool->used() << " idle=" << pool->idle() << " client id="
				<< client->id() << endl;
		c1->close();
		c2->close();
		cout << " 4pool total=" << pool->size() << " used=" << pool->used() << " idle=" << pool->idle() << " client id="
				<< client->id() << endl;
		c3->close();
		c4->close();
		cout << " 5pool total=" << pool->size() << " used=" << pool->used() << " idle=" << pool->idle() << " client id="
				<< client->id() << endl;
		cout << "aaaaaaaaaaaaaaaaaaaaaaaaaaaa" << endl;

	}
	void ConnecionTest() {
		// "192.168.117.19"
		ZkClient client("yz-cdc-wrk-02.dns.ganji.com:2181", 0);
		cout << " before conn" << endl;
		client.connect_zk();
		cout << " after conn" << endl;
		// 	client.Update("/aha/services/testservice/"); // bad arguments
		//	client.Update("/aha");
		//	client.Update("/aha/services");
		client.get_children("/soa/services/testservice");
		//	client.Update("/aha/services/testservice/member_0000000084");
	}

	void RegistryEqualsTest() {
		Registry r = Registry("a", "b", 2);

		cout << Registry::to_json_string(r) << endl;
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
	}
	;
};
} /* namespace ut */

#endif /* ZKCLIENTTEST_H_ */
