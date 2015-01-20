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

#include "concurrency/PosixThreadFactory.h"
#include "concurrency/ThreadManager.h"
#include "protocol/TBinaryProtocol.h"
#include "server/TNonblockingServer.h"
#include "server/TThreadPoolServer.h"
#include "server/TThreadedServer.h"

#include "transport/TServerSocket.h"
#include "transport/TBufferTransports.h"

#include "frproxy.h"
#include "core/ServerHandler.h"
#include "utils/ArgsParser.h"
#include "log/logger.h"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;
using namespace ::apache::thrift::concurrency;

//using namespace ut;
using namespace std;
using boost::shared_ptr;
using namespace FinagleRegistryProxy;

// using namespace ganji::util::log::ThriftLog;

int nonblockingServer(string zkhosts, int port, int threadCount) {
	cout << utils::time_stamp() << " frproxy nonblocking server starting at port " << port << endl;
	cout << utils::time_stamp() << " zk server " << zkhosts << endl;
	logger::warn("frproxy nonblocking server starting at port %d. zk server %s", port, zkhosts.c_str());

	shared_ptr<ServerHandler> handler(new ServerHandler(zkhosts, port));
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

	handler.get()->register_self();
	handler.get()->warm();
	handler.get()->start_scheduler();

	try {
		logger::warn("server is getting up.");
		server.serve();

	} catch (TException &ex) {
		logger::error("thrift error occured when trying to serve. check port which maybe used. message: %s", ex.what());
	}
	cout << "frproxy server exiting." << endl;
	threadManager->stop();
	cout << "frproxy server exited." << endl;
	exit(1);
}

int poolServer(string zkhosts, int port, int poolSize) {
	cout << utils::time_stamp() << " frproxy pool server starting at port " << port << endl;
	cout << utils::time_stamp() << " zk server " << zkhosts << endl;
	logger::warn("frproxy pool server starting at port %d. zk server %s", port, zkhosts.c_str());

	shared_ptr<ServerHandler> handler(new ServerHandler(zkhosts, port));
	shared_ptr<TProcessor> processor(new RegistryProxyProcessor(handler));
	shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
	shared_ptr<TTransportFactory> transportFactory(new TFramedTransportFactory());
	shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

	shared_ptr<ThreadManager> threadManager = ThreadManager::newSimpleThreadManager(poolSize);
	shared_ptr<PosixThreadFactory> threadFactory = shared_ptr<PosixThreadFactory>(new PosixThreadFactory());
	threadManager->threadFactory(threadFactory);
	threadManager->start();
	TThreadPoolServer server(processor, serverTransport, transportFactory, protocolFactory, threadManager);

	handler.get()->register_self();
	handler.get()->warm();
	try {
		server.serve();
	} catch (TException &ex) {
		logger::error("thrift error occured when trying to serve. check port which is not be used please. message: %s",
				ex.what());
		cout << "thrift error occured when trying to serve. check port which is not be used please. message: " << ex.what() << endl;
	}
	cout << "frproxy server exited." << endl;
	exit(1);
}

int threadedServer(string zkhosts, int port) {
	cout << utils::time_stamp() << " frproxy threaded server starting at port " << port << endl;
	cout << utils::time_stamp() << " zk server " << zkhosts << endl;
	logger::warn("frproxy threaded server starting at port %d. zk server %s", port, zkhosts.c_str());
	shared_ptr<ServerHandler> handler(new ServerHandler(zkhosts, port));
	shared_ptr<TProcessor> processor(new RegistryProxyProcessor(handler));

	shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
	shared_ptr<TTransportFactory> transportFactory(new TFramedTransportFactory());
	shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

	TThreadedServer server(processor, serverTransport, transportFactory, protocolFactory);

	handler.get()->register_self();
	handler.get()->warm();
	try {
		server.serve();
	} catch (TException &ex) {
		logger::error("thrift error occured when trying to serve. check port which is not be used please. message: %s",
				ex.what());
	}
	cout << "frproxy server exited." << endl;
	exit(1);
}

void version() {
	cout << "Proxy Server of Service Framework of Ganji RPC " << FinagleRegistryProxy::FRPROXY_VERSION << endl;
}

void usage() {
	version();
	cout << "Usage: frproxy [options [option value]]" << endl;
	cout << "	" << "-p,  --port:\t\tuse definited port. default 9009. eg. -p 9009" << endl;
	cout << "	" << "-st, --threaded:\trun server as TTreadedServer" << endl;
	cout << "	" << "-sp, --threadpool:\trun server as TTreadPoolServer" << endl;
	cout << "	" << "-sn, --nonblocking:\trun server as TNonblockingServer. default server type" << endl;
	cout << "	" << "-t,  --thread_count:\tthread pool size or max thread count. used in NoblockingServer & ThreadpoolServer. default 16. eg. -t 20" << endl;
	cout << "	" << "-h,  --help:\t\tshow help" << endl;
	cout << "	" << "-v,  --version:\t\tshow version" << endl;
	cout << "	" << "-z,  --zkhosts:\t\tzookeeper hosts. default 127.0.0.1:2181. eg. -z 192.168.2.202:2181" << endl;
	cout << "	" << "-lh, --scribehost:\t\tscribe hosts. default 127.0.0.1. eg. -lg localhost" << endl;
	cout << "	" << "-lp, --scribeport:\t\tscribe port. default 11463. eg. -lp 11463" << endl;
	cout << "	" << "-l,  --enablelog:\t\tenable scribe log" << endl;
	cout << "	" << "-a,  --autoreset:\t\tauto reset. default true; '-a true' same to '-a' eg. -a. eg. -a false. eg. -a true" << endl;
}

int main(int argc, char **argv) {
	// high priority args
	if (option_exists(argc, argv, "-h") || option_exists(argc, argv, "--help")) {
		usage();
		return 0;
	}
	if (option_exists(argc, argv, "-v") || option_exists(argc, argv, "--version")) {
		version();
		return 0;
	}

	int port = option_value(argc, argv, "-p", "--port", 9009);
	int thread_count = option_value(argc, argv, "-t", "--thread_count", 16);
	string zkhosts = option_value(argc, argv, "-z", "--zkhosts", "127.0.0.1:2181");
	if(zkhosts.find(':') < 0 ) {
		zkhosts = zkhosts + ":2181";
	}
	if (option_exists(argc, argv, "-l") || option_exists(argc, argv, "--enablelog")) {
		logger::enable();
	}
	char* log_host = option_value(argc, argv, "-lh", "--scribehost", "127.0.0.1");
	int log_port = option_value(argc, argv, "-lp", "--scribeport", 11463);
	logger::init(log_host, log_port, 999999);

	int type = 2; // nonblocking
	for (int i = 1; i < argc; i++) {

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
	try {
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
	} catch (const std::exception& ex) {
		cout << "fatal error occured! need to restart server. message:" << ex.what() << endl;
	} catch (char* ex) {
		logger::error("fatal error occured! need to restart server. message: %d", ex);
		cout << "fatal error occured! need to restart server. message:" << ex << endl;
		return 1;
	} catch (...) {
		cout << "what's the fucking is going on." << endl;
	}
	return 0;
}
