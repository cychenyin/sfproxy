/*
 ============================================================================
 Name        : finagle-registry-proxy.cpp
 Author      : chenyin
 Version     :
 Copyright   : Your copyright reserved by ganji @2014
 Description : Hello World in C++,
 ============================================================================
 */

#include <iostream>

#include "frpoxy.h"

#include "test/RegistryCacheTest.h"
#include "test/ZkClientTest.h"

#include "core/ServerHandler.cpp"

using namespace std;
using namespace ut;

//int main2(int argc, char** argv) {
//	cout << "welcome to finagle regsitry proxy" << endl;
//
//	ut::RegistryCacheTest a;
////	a.addTest();
//
//	ZkClientTest clientTest;
//	clientTest.ConnecionTest();
//	clientTest.RegistryEqualsTest();
//
//	return 0;
//}

int main(int argc, char **argv) {
	int port = 9090;
	shared_ptr<ServerHandler> handler(new ServerHandler("yz-cdc-wrk-02.dns.ganji.com:2181"));
	shared_ptr<TProcessor> processor(new RegistryProxyProcessor(handler));
	shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
	shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
	shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

	TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
	server.serve();
	return 0;
}
