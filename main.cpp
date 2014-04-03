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

#include "ganji/util/time/time.h"
//#include "ganji/util/thread/mutex.h"

#include <thrift/concurrency/PosixThreadFactory.h>
#include <thrift/concurrency/ThreadManager.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TNonblockingServer.h>
#include <thrift/server/TThreadPoolServer.h>

#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;
using namespace ::apache::thrift::concurrency;

using namespace std;
using namespace ut;
using boost::shared_ptr;
//using namespace ::ganji::util::thread;
//using namespace ganji::util::thread;

int testmain(int argc, char** argv) {
	cout << "welcome to finagle regsitry proxy" << endl;

	cout << "-----------------------" << endl;
	ut::RegistryCacheTest a;
//	a.getTest();
	cout << "before addTest-----------------------" << endl;
	a.addTest();
	cout << "after addTest-----------------------" << endl;
//	a.removeTest();
//	a.removeTest1();
//	a.removeTest2();

	//	ZkClientTest clientTest;
//	clientTest.ConnecionTest();
//	clientTest.RegistryEqualsTest();

	return 0;
}

int main(int argc, char **argv) {

//	cout << ganji::util::time::GetCurTimeUs() << endl;
//	cout << ganji::util::time::GetCurTimeMs() << endl;

#ifdef DEBUG_
	for (int i = 0; i < argc; i++) {
		cout << " arg[" << i << "]=" << argv[i];
	}
	cout << endl;
#endif

	if (argc > 1 && (strcmp(argv[1], "-t") == 0 || strcmp(argv[1], "--test") == 0)) {
		testmain(argc, argv);
		cout << "end..." << endl;
		return 0;
	}

	int port = 9091;
	if (argc > 2 && (strcmp(argv[2], "-d") == 0 || strcmp(argv[2], "--debug") == 0)) {
		port = 9090;
	}

	cout << "frproxy server starting at port " << port << endl;


	shared_ptr<ServerHandler> handler(new ServerHandler("yz-cdc-wrk-02.dns.ganji.com:2181"));
	shared_ptr<TProcessor> processor(new RegistryProxyProcessor(handler));

	shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
	shared_ptr<TTransportFactory> transportFactory(new TFramedTransportFactory());
	shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

	// using thread pool with maximum 15 threads to handle incoming requests
	shared_ptr<ThreadManager> threadManager = ThreadManager::newSimpleThreadManager(15);
	shared_ptr<PosixThreadFactory> threadFactory = shared_ptr<PosixThreadFactory>(new PosixThreadFactory());
	threadManager->threadFactory(threadFactory);
	threadManager->start();

	TNonblockingServer server(processor, protocolFactory, port, threadManager);
#ifdef DEBUG_
	long start = ganji::util::time::GetCurTimeUs();
#endif
	handler.get()->warm();

	#ifdef DEBUG_
	long elapse = ganji::util::time::GetCurTimeUs() - start;
	cout << "frproxy server warm cost " << (double)elapse / 1000 << "ms" << endl;
#endif
	server.serve();


	cout << "frproxy server exiting." << endl;
	threadManager->stop();
	cout << "frproxy server exited." << endl;
	return 0;
}

void threadServer() {
	//	shared_ptr<ServerHandler> handler(new ServerHandler("yz-cdc-wrk-02.dns.ganji.com:2181"));
	//	shared_ptr<TProcessor> processor(new RegistryProxyProcessor(handler));
	//
	//	shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());
	//
	//	shared_ptr<ThreadManager> threadManager = ThreadManager::newSimpleThreadManager(15);
	//	shared_ptr<PosixThreadFactory> threadFactory = shared_ptr<PosixThreadFactory > (new PosixThreadFactory());
	//	threadManager->threadFactory(threadFactory);
	//	threadManager->start();
	//	TThreadPoolServer server(processor, serverTransport,transportFactory, protocolFactory, threadManager);
	//	TNonblockingServer server(processor, protocolFactory, port, threadManager);
	////	TNonblockingServer server(processor, port);
	//	server.serve();
}
