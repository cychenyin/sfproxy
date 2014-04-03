#include <iostream>
#include "RegistryProxy.h"  // As an example

#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

using namespace FinagleRegistryProxy;

int main(int argc, char **argv) {
	// 9090 for test
	// 9091 for debug
	int port = 9091;
	cout << "conn to port=" << port << endl;
	boost::shared_ptr<TSocket> socket(new TSocket("localhost", port));
	boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
	boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));

	RegistryProxyClient client(protocol);
	int i;
	transport->open();
	for (i = 0; i < 100; i++) {
		std::string serverName = "/soa/service/testservice";
		std::string ret;
		client.get(ret, serverName);
		cout << "result:" << endl;
		cout << "	" << ret << endl;
	}
	transport->close();

	return 0;
}
