/*
 * ZkClientTest.h
 *
 *  Created on: Mar 19, 2014
 *      Author: asdf
 */

#ifndef ZKCLIENTTEST_H_
#define ZKCLIENTTEST_H_

#include <iostream>

#include "../core/ZkClient.h"

namespace ut {

using namespace FinagleRegistryProxy;

class ZkClientTest {
public:
	ZkClientTest();
	virtual ~ZkClientTest();

	void ConnecionTest(){
		// "192.168.117.19"
		ZkClient client("yz-cdc-wrk-02.dns.ganji.com:2181", 0);
		client.connect_zk();

	// 	client.Update("/aha/services/testservice/"); // bad arguments
	//	client.Update("/aha");
	//	client.Update("/aha/services");
		client.get_children("/aha/services/testservice");
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
		r.name  = "a";
		cout << (l == r) << endl;
		cout << (&l == &r) << endl;
	};
};
} /* namespace ut */

#endif /* ZKCLIENTTEST_H_ */
