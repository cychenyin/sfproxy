/*
 * ZkClientTest.cpp
 *
 *  Created on: Mar 19, 2014
 *      Author: asdf
 */

#include "ZkClientTest.h"

namespace ut {

ZkClientTest::ZkClientTest() {
	// TODO Auto-generated constructor stub

}

ZkClientTest::~ZkClientTest() {
	// TODO Auto-generated destructor stub
}


void ZkClientTest::ConnecionTest(){
	// "192.168.117.19"
	ZkClient client = ZkClient("yz-cdc-wrk-02.dns.ganji.com:2181");
	client.ConnectZK();

// 	client.Update("/aha/services/testservice/"); // bad arguments
//	client.Update("/aha");
//	client.Update("/aha/services");
	client.UpdateServices("/aha/services/testservice");
//	client.Update("/aha/services/testservice/member_0000000084");
}






} /* namespace ut */
