#include <iostream>

#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>

#include <concurrency/PosixThreadFactory.h>
#include <concurrency/Thread.h>
#include <concurrency/ThreadManager.h>
#include <concurrency/Mutex.h>

#include "RegistryProxy.h"  // As an example
#include "../frproxy.h"
#include "../core/ClientPool.h"
#include "../core/ZkClient.h"
#include "../utils/ArgsParser.h"
#include "../log/logger.h"

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::concurrency;
using boost::shared_ptr;

using namespace FinagleRegistryProxy;

class Cursor {
public:
	Cursor(int seed) :
			seed(seed) {
	}
	~Cursor() {
	}
private:
	int seed;
	apache::thrift::concurrency::Mutex locker;
public:
	void add() {
		locker.lock();
		seed++;
		locker.unlock();
	}
	int get() {
		return seed;
	}
};

class ClientTask: public Runnable {
private:
	string host;
	int port;
	string name;
	int count;
	Cursor *cursor;
public:
	ClientTask(string host = "127.0.0.1", int port = 9009, string name = "testservice", int count = 0, Cursor *cursor = 0) :
			host(host), port(port), name(name), count(count), cursor(cursor) {
	}

	~ClientTask() {
	}

	void dump() {
		boost::shared_ptr<TSocket> socket(new TSocket(host, port));
		boost::shared_ptr<TTransport> transport(new TFramedTransport(socket));
		boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));

		RegistryProxyClient client(protocol);
		int i;
		try {
			transport->open();
			std::string ret;
			client.dump(ret);
			cout << ret << endl;
		} catch (const apache::thrift::transport::TTransportException& ex) {
			//transport->close();
			cout << ex.what() << endl;
		} catch (const std::exception& ex) {
			cout << ex.what() << endl;
		}
		transport->close();
	}

	void get_once() {
		boost::shared_ptr<TSocket> socket(new TSocket(host, port));
		boost::shared_ptr<TTransport> transport(new TFramedTransport(socket));
		boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));

		RegistryProxyClient client(protocol);
		int i;
		try {
			long start = utils::now_in_us();
			transport->open();
			long open = utils::now_in_us();
			std::string ret;
			client.get(ret, name);
			long done = utils::now_in_us();
			cout << "client get total=" << (double) (done - start) / 1000 << "ms. open cost=" << (double) (open - start) / 1000
					<< "ms. get cost=" << (double) (done - open) / 1000 << endl;
			cout << "result:	" << ret << endl;
		} catch (const apache::thrift::transport::TTransportException& ex) {
			//transport->close();
			cout << "client get excepiton: " << ex.what() << endl;
		} catch (const std::exception& ex) {
			cout << ex.what() << endl;
		}
		transport->close();
	}

	void status() {
		boost::shared_ptr<TSocket> socket(new TSocket(host, port));
		boost::shared_ptr<TTransport> transport(new TFramedTransport(socket));
		boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));

		RegistryProxyClient client(protocol);
		int i;
		try {
			transport->open();
			int status = client.status();
			cout << "status : " << status << endl;
			cout
					<< "	tips: get 0 is fine; 1 means zk conn , 2 means watcher, 3 means cache, 4 means that thread has some problem."
					<< endl;

		} catch (const apache::thrift::transport::TTransportException& ex) {
			//transport->close();
			cout << ex.what() << endl;
		} catch (const std::exception& ex) {
			cout << ex.what() << endl;
		}
		transport->close();
	}
	void reset() {
		boost::shared_ptr<TSocket> socket(new TSocket(host, port));
		boost::shared_ptr<TTransport> transport(new TFramedTransport(socket));
		boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));

		RegistryProxyClient client(protocol);
		int i;
		try {
			transport->open();
			string ret = "";
			client.reset(ret);
			cout << "reset result : " << ret << endl;
			cout << "	tips: empty result is fine." << endl;

		} catch (const apache::thrift::transport::TTransportException& ex) {
			//transport->close();
			cout << ex.what() << endl;
		} catch (const std::exception& ex) {
			cout << ex.what() << endl;
		}
		transport->close();
	}

	void run() {
		int c = 0;
		// cout << "count = " << count << endl;
		while (c++ < count || count == 0) {
			// cout << "thread[" << pthread_self() << "]=" << c << endl;
			get_once();
		}
		// cout << "thread[" << pthread_self() << "]=" << c << endl;
		if (cursor) {
			cursor->add();
		}
	}

};

void usage(int egg = 0) {
	cout << "client version: " << FinagleRegistryProxy::FRPROXY_VERSION << endl;
	cout << "Usage: client [option [option_value]]" << endl;
	cout << "Options:" << endl;
	cout << "	" << "-h, --help:		print usage. " << endl;
	cout << "	" << "-s, --server:		server of frproxy :	default 127.0.0.1. eg. -s localhost" << endl;
	cout << "	" << "-p, --port:		default 9009. eg. -p 9090" << endl;
	cout << "	" << "-n, --name:		name of service:	default testservice. eg. -n rpc.counter.thrift" << endl;
	cout << "	" << "-t, --threadcount:default 0, single thread. eg. -t 2" << endl;
	cout << "	" << "-c, --count:		default 1 when single thread. or 1. eg. -c 9999" << endl;
	cout << "	" << "-m, --method:		candidatation include get, remove, dump. default get" << endl;
	cout << "	" << "-l, --list:		list frproxy server" << endl;
	cout << "	"
			<< "-a, --status:		list frproxy working status. get 0 is fine; 1 means zk conn , 2 means watcher, 3 means cache, 4 means that thread has some problem. "
			<< endl;
	cout << "	" << "-z,  --zkhosts:	zookeeper hosts. default 127.0.0.1:2181. eg. -z 192.168.2.202:2181" << endl;
	if (egg > 0) {
		cout << "	" << "-r,  --reset:\t\t server running health status" << endl;
	}

	cout << "eg. " << endl;
	cout << "	" << "./client " << endl;
	cout << "	" << "./client -c 999" << endl;
	cout << "	" << "./client -c 999 -t 2" << endl;
	cout << "	" << "./client -c 999 -t 2 -s 192.168.1.111 -p 9009" << endl;
	cout << "	" << "./client -m dump" << endl;
	cout << "	" << "./client -l -z 192.168.113.212:2181" << endl;
	cout << "	" << "./client -n rpc.counter.thrift" << endl;
}

void listServer(string zkhosts) {
	ClientPool pool = ClientPool(new ZkClientFactory(zkhosts, 0));

	ZkClient* client = (ZkClient*) pool.open();
	string root = "/soa/proxies";
	client->get_all_services(root);
	vector<string> v = client->get_children(root);
	cout << "frproxy servers regeistered in zk: " << zkhosts << " total:" << v.size() << endl;
	for (vector<string>::iterator it = v.begin(); it != v.end(); it++) {
		cout << "	" << *it << endl;
	}
}

int main(int argc, char **argv) {
	string host = option_value(argc, argv, "-s", "--server", "127.0.0.1");
	int port = option_value(argc, argv, "-p", "--port", 9009);
	string service_name = option_value(argc, argv, "-n", "--name", "testservice");
	string zkhosts = option_value(argc, argv, "-z", "--zkhosts", "localhost:2181");

	int pool_size = option_value(argc, argv, "-t", "--threadcount", 0);
	int count = option_value(argc, argv, "-c", "--count", pool_size > 0 ? 0 : 1);

	string method = option_value(argc, argv, "-m", "--method", "");

	if (option_exists(argc, argv, "-h") || option_exists(argc, argv, "--help")) {
		usage();
		return 0;
	}
	if (option_exists(argc, argv, "-l") || option_exists(argc, argv, "--list")) {
		listServer(zkhosts);
		return 0;
	}
	if (option_exists(argc, argv, "-a") || option_exists(argc, argv, "--status")) {
		ClientTask task(host, port, service_name, count);
		task.status();
		return 0;
	}
	if (option_exists(argc, argv, "-r") || option_exists(argc, argv, "--reset")) {
		ClientTask task(host, port, service_name, count);
		task.reset();
		return 0;
	}

	// cout << "connecting to port=" << port << endl;
	if (method == "dump") {
		ClientTask task(host, port, service_name, count);
		task.dump();
		return 0;
	}

	// cout << "service name=" << service_name << endl;
	if (pool_size > 0) {
		// cout << "multi thread modeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee;" << endl;
		Cursor *cursor = new Cursor(pool_size);
		shared_ptr<ThreadManager> threadManager = ThreadManager::newSimpleThreadManager(pool_size);
		shared_ptr<PosixThreadFactory> threadFactory = shared_ptr<PosixThreadFactory>(new PosixThreadFactory());
		threadManager->threadFactory(threadFactory);
		threadManager->start();
		for (int i = 0; i < pool_size; i++) {
			int batch = count / pool_size == 0 ? (count == 0 ? 0 : 1) : count / pool_size;
			threadManager->add(shared_ptr<ClientTask>(new ClientTask(host, port, service_name, batch, cursor)), 0, 0);
		}
		while (threadManager->idleWorkerCount() < pool_size) {
			usleep(1000);
		}
		cout << "idle thread count " << threadManager->idleWorkerCount() << " pool_size=" << pool_size << " count=" << count
				<< endl;
//		while(cursor.get() < pool_size){
//			usleep(1000);
//		}
		usleep(100000);
		threadManager->stop();
		delete cursor;
		cursor = 0;
	} else {
		// cout << "single threadddddddddddddddddddddddddddddddddddd mode;" << endl;
		ClientTask task(host, port, service_name, count);
		task.run();
	}

	return 0;
}
