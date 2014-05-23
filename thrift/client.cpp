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
#include "../utils/ArgsParser.h"

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::concurrency;
using boost::shared_ptr;

using namespace FinagleRegistryProxy;


class Cursor{
public:
	Cursor(int seed) : seed(seed){}
	~Cursor(){}
private:
	int seed;
	apache::thrift::concurrency::Mutex locker;
public:
	void add(){
		locker.lock();
		seed ++;
		locker.unlock();
	}
	int get(){
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
	ClientTask(string host="127.0.0.1", int port=9009, string name="testservice", int count=0, Cursor *cursor = 0) :
			host(host), port(port), name(name), count(count) , cursor(cursor){
	}

	~ClientTask() {
	}

	void once() {
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
			cout << "client get total=" << (double) (done - start) / 1000 << "ms. open cost="
					<< (double) (open - start) / 1000 << "ms. get cost=" << (double) (done - open) / 1000 << endl;
			cout << "result:	" << ret << endl;
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
		cout << "count = "<< count << endl;
		while (c++ < count || count == 0) {
			cout << "thread[" << pthread_self() << "]=" << c << endl;
			once();
		}
		// cout << "thread[" << pthread_self() << "]=" << c << endl;
		if(cursor){
			cursor->add();
		}
	}

};


int main(int argc, char **argv) {
	string host = option_value(argc, argv, "-h", "--host", "127.0.0.1");
	int port = option_value(argc, argv, "-p", "--port", 9009);
	string service_name = option_value(argc, argv, "-s", "--service", "testservice");

	int pool_size = option_value(argc, argv, "-t", "--threadcount", 0);
	int count = option_value(argc, argv, "-c", "--count", pool_size > 0 ? 0 : 1);

	cout << "conn to port=" << port << endl;
	cout << "service name=" << service_name << endl;

	if (pool_size > 0) {
		cout << "multi threadddddddddddddddddddddddddddddddddddd mode;" << endl;
		Cursor *cursor = new Cursor(pool_size);
		shared_ptr<ThreadManager> threadManager = ThreadManager::newSimpleThreadManager(pool_size);
		shared_ptr<PosixThreadFactory> threadFactory = shared_ptr<PosixThreadFactory>(new PosixThreadFactory());
		threadManager->threadFactory(threadFactory);
		threadManager->start();
		for(int i=0;i<pool_size; i++){
			int batch = count / pool_size == 0 ? (count == 0 ? 0 : 1) : count / pool_size;
			threadManager->add(shared_ptr<ClientTask>(new ClientTask (host, port, service_name, batch, cursor)), 0, 0);
		}
		while(threadManager->idleWorkerCount() < pool_size){
			usleep(1000);
		}
		cout << "idle thread count " << threadManager->idleWorkerCount() << " pool_size=" << pool_size << " count=" << count <<endl;
//		while(cursor.get() < pool_size){
//			usleep(1000);
//		}
		usleep(100000);
		threadManager->stop();
		delete cursor;
		cursor = 0;
	} else {
		cout << "single threadddddddddddddddddddddddddddddddddddd mode;" << endl;
		ClientTask task(host, port, service_name, count);
		task.run();
	}

	return 0;
}
