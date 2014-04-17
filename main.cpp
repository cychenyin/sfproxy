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

#include "concurrency/PosixThreadFactory.h"
#include "concurrency/ThreadManager.h"
#include "protocol/TBinaryProtocol.h"
#include "server/TNonblockingServer.h"
#include "server/TThreadPoolServer.h"
#include "server/TThreadedServer.h"

#include "transport/TServerSocket.h"
#include "transport/TBufferTransports.h"

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

int nonblockingServer(string zkhosts, int port, int threadCount) {
	cout << "frproxy nonblocking server starting at port " << port << endl;
	cout << "zk server " << zkhosts << endl;

	// shared_ptr<ServerHandler> handler(new ServerHandler("yz-cdc-wrk-02.dns.ganji.com:2181"));
	shared_ptr<ServerHandler> handler(new ServerHandler(zkhosts));
	shared_ptr<TProcessor> processor(new RegistryProxyProcessor(handler));

	shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
	shared_ptr<TTransportFactory> transportFactory(new TFramedTransportFactory());
	shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

	// using thread pool with maximum 15 threads to handle incoming requests
	shared_ptr<ThreadManager> threadManager = ThreadManager::newSimpleThreadManager(threadCount);
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
	cout << "frproxy server warm cost " << (double) elapse / 1000 << "ms" << endl;
#endif
	server.serve();

	cout << "frproxy server exiting." << endl;
	threadManager->stop();
	cout << "frproxy server exited." << endl;
	return 0;
}

void poolServer(string zkhosts, int port, int poolSize) {
	cout << "frproxy pool server starting at port " << port << endl;
	cout << "zk server " << zkhosts << endl;

	shared_ptr<ServerHandler> handler(new ServerHandler(zkhosts));
	shared_ptr<TProcessor> processor(new RegistryProxyProcessor(handler));
	shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
	shared_ptr<TTransportFactory> transportFactory(new TFramedTransportFactory());
	shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

	shared_ptr<ThreadManager> threadManager = ThreadManager::newSimpleThreadManager(poolSize);
	shared_ptr<PosixThreadFactory> threadFactory = shared_ptr<PosixThreadFactory>(new PosixThreadFactory());
	threadManager->threadFactory(threadFactory);
	threadManager->start();
	TThreadPoolServer server(processor, serverTransport, transportFactory, protocolFactory, threadManager);
	server.serve();
}

int threadedServer(string zkhosts, int port) {
	cout << "frproxy threaded server starting at port " << port << endl;
	cout << "zk server " << zkhosts << endl;

	shared_ptr<ServerHandler> handler(new ServerHandler(zkhosts));
	shared_ptr<TProcessor> processor(new RegistryProxyProcessor(handler));

	shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
	shared_ptr<TTransportFactory> transportFactory(new TFramedTransportFactory());
	shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

	TThreadedServer server(processor, serverTransport, transportFactory, protocolFactory);
	handler.get()->warm();

	server.serve();
	cout << "frproxy server exited." << endl;
	return 0;
}

void usage() {
	cout << "service frameword registry proxy server." << endl;
	cout << "Usage: frproxy [options [option value]]" << endl;
	cout << "	" << "-d, --debug:\t\trun test main for debugging only" << endl;
	cout << "	" << "-p, --port:\t\tuse definited port. default 9091. eg. -p 9090" << endl;
	cout << "	" << "-st, --threaded:\trun server as TTreadedServer" << endl;
	cout << "	" << "-sp, --threadpool:\trun server as TTreadPoolServer" << endl;
	cout << "	" << "-sn, --nonblocking:\trun server as TNonblockingServer. default server type" << endl;
	cout << "	" << "-t, --thread_count:\tthread pool size or max thread count. default 16. eg. -t 20" << endl;
	cout << "	" << "-h, --help:\t\tshow help" << endl;
	cout << "	" << "-v, --version:\t\tshow version" << endl;
	cout << "	" << "-z, --zkhosts:\t\tzookeeper hosts. default 127.0.0.1. eg. -z 192.168.2.202" << endl;
}

void version() {
	cout << "service frameword registry proxy server 1.0.0" << endl;
}
// return index if exists, or return 0
int option_exists(int argc, char **argv, char *option) {
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], option) == 0)
			return i;
	}
	return 0;
}

char* option_value(int argc, char **argv, char *option, char* long_name_option, char *default_value) {
	int index = option_exists(argc, argv, option);
	char *ret = default_value;
	if (index && argc > index + 1) {
		ret = argv[index + 1];
	} else {
		if (long_name_option) {
			index = option_exists(argc, argv, long_name_option);
			if (index && argc > index + 1) {
				ret = argv[index + 1];
			}
		}
	}
	return ret;
}

int option_value(int argc, char **argv, char *option, char *long_name_option, int default_value) {
	int index = option_exists(argc, argv, option);
	int ret = default_value;
	if (index && argc > index + 1) {
		ret = atoi(argv[index + 1]);
	} else {
		if (long_name_option) {
			index = option_exists(argc, argv, long_name_option);
			if (index && argc > index + 1) {
				ret = atoi(argv[index + 1]);
			}
		}
	}
	return ret;
}


int main(int argc, char **argv) {
	if (option_exists(argc, argv, "-d") || option_exists(argc, argv, "--debug")) {
		testmain(argc, argv);
		cout << "end..." << endl;
		return 0;
	}

	int port = option_value(argc, argv, "-p", "--port", 9091);;
	int thread_count = option_value(argc, argv, "-t", "--thread_count", 16);
	string zkhosts = option_value(argc, argv, "-z", "--zkhosts", "192.168.2.202:2181");

	int type = 2; // nonblocking
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
			// theaded
			type = 0;
		}
		if (strcmp(argv[i], "-st") == 0 || strcmp(argv[i], "--threaded") == 0) {
			// theaded
			type = 3;
			break;
		} else if (strcmp(argv[i], "-sp") == 0 || strcmp(argv[i], "--threadpool") == 0) {
			// thread pool
			type = 1;
			break;
		} else if (strcmp(argv[i], "-sn") == 0 || strcmp(argv[i], "--nonblocking") == 0) {
			// nonblocking
			type = 2;
			break;
		}
	}
#ifdef DEBUG_
	cout << "port " << port << endl;
	cout << "thread count " << thread_count << endl;
	cout << "zkhosts " << zkhosts << endl;
	cout << "server type  " << type << endl;
#endif

	switch (type) {
	case 1:
		poolServer(zkhosts, port, thread_count);
		break;
	case 2:
		nonblockingServer(zkhosts, port, thread_count);
		break;
	case 3:
		threadedServer(zkhosts, port);
		break;
	case 0:
		usage();
		break;
	default:
		break;
	}
	return 0;
}
