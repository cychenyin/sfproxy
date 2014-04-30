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

#include "frproxy.h"
#include "test/RegistryCacheTest.h"
#include "test/ZkClientTest.h"

#include "core/ServerHandler.cpp"
#include "log/logger.h"

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
// using namespace ganji::util::log::ThriftLog;

int testmain(int argc, char** argv) {
	cout << "welcome to finagle regsitry proxy" << endl;
	ut::RegistryCacheTest a;
//	a.getTest();
	a.addTest();
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
	logger::warn("frproxy nonblocking server starting at port %d. zk server %s", port, zkhosts.c_str());
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
	uint64_t start = utils::now_in_us();
#endif
	handler.get()->warm();

#ifdef DEBUG_
	uint64_t elapse = utils::now_in_us() - start;
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
	logger::warn("frproxy pool server starting at port %d. zk server %s", port, zkhosts.c_str());

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
	logger::warn("frproxy threaded server starting at port %d. zk server %s", port, zkhosts.c_str());
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
	cout << "Proxy Server of Service Framework of Ganji RPC." << endl;
	cout << "Usage: frproxy [options [option value]]" << endl;
	cout << "	" << "-d,  --debug:\t\trun test main for debugging only" << endl;
	cout << "	" << "-p,  --port:\t\tuse definited port. default 9009. eg. -p 9009" << endl;
	cout << "	" << "-st, --threaded:\trun server as TTreadedServer" << endl;
	cout << "	" << "-sp, --threadpool:\trun server as TTreadPoolServer" << endl;
	cout << "	" << "-sn, --nonblocking:\trun server as TNonblockingServer. default server type" << endl;
	cout << "	"
			<< "-t,  --thread_count:\tthread pool size or max thread count. used in NoblockingServer & ThreadpoolServer. default 16. eg. -t 20"
			<< endl;
	cout << "	" << "-h,  --help:\t\tshow help" << endl;
	cout << "	" << "-v,  --version:\t\tshow version" << endl;
	cout << "	" << "-z,  --zkhosts:\t\tzookeeper hosts. default 127.0.0.1:2181. eg. -z 192.168.2.202:2181" << endl;
	cout << "	" << "-lh, --scribehost:\t\tscribe hosts. default 127.0.0.1. eg. -lg localhost" << endl;
	cout << "	" << "-lp, --scribeport:\t\tscribe port. default 11463. eg. -lp 11463" << endl;
	cout << "	" << "-l,  --enable:\t\tenable scribe log" << endl;
}

void version() {
	cout << "service frameword registry proxy server 1.0.0" << endl;
}
// return index if exists, or return 0
int option_exists(int argc, char **argv, const char *option) {
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], option) == 0)
			return i;
	}
	return 0;
}

char* option_value(int argc, char **argv, const char *option, const char* long_name_option, const char *default_value) {
	int index = option_exists(argc, argv, option);
	//char *ret = default_value;
	char *ret = const_cast<char *>(default_value);
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

int option_value(int argc, char **argv, const char *option, const char *long_name_option, int default_value) {
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
//		cout << "end..." << endl;
		return 0;
	}

	int port = option_value(argc, argv, "-p", "--port", 9009);
	int thread_count = option_value(argc, argv, "-t", "--thread_count", 16);
	string zkhosts = option_value(argc, argv, "-z", "--zkhosts", "localhost:2181");

	if (option_exists(argc, argv, "-l") || option_exists(argc, argv, "--enablelog")) {
		logger::enable();
	}
	char* log_host = option_value(argc, argv, "-lh", "--scribehost", "127.0.0.1");
	int log_port = option_value(argc, argv, "-lp", "--scribeport", 11463);
	logger::init(log_host, log_port, 999999);

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
	if (type > 0) {
		cout << "port " << port << endl;
		cout << "thread count " << thread_count << endl;
		cout << "zkhosts " << zkhosts << endl;
		cout << "server type  " << type << endl;
	}
#endif

	switch (type) {
	case 1:
		poolServer(zkhosts, port, thread_count);
		logger::destory();
		break;
	case 2:
		nonblockingServer(zkhosts, port, thread_count);
		logger::destory();
		break;
	case 3:
		threadedServer(zkhosts, port);
		logger::destory();
		break;
	case 0:
		usage();
		break;
	default:
		break;
	}
	return 0;
}
