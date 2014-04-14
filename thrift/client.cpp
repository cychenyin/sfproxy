#include <iostream>
#include "RegistryProxy.h"  // As an example

#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>

#include "ganji/util/time/time.h"

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

using namespace FinagleRegistryProxy;

int main(int argc, char **argv) {
	// 9090 for test
	// 9091 for debug
	int port = 9091;
	if (argc > 1 && (strcmp(argv[1], "-d") == 0 || strcmp(argv[1], "--debug") == 0)) {
		port = 9090;
	}
	cout << "conn to port=" << port << endl;
	boost::shared_ptr<TSocket> socket(new TSocket("localhost", port));

	boost::shared_ptr<TTransport> transport(new TFramedTransport(socket));
	boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));

	RegistryProxyClient client(protocol);
	int i;
	long start = ganji::util::time::GetCurTimeUs();
	transport->open();
	long open = ganji::util::time::GetCurTimeUs();

	try {
		for (i = 0; i < 1; i++) {
			// std::string serverName = "/soa/services/testservice";
			std::string serverName = "testservice";
			std::string ret;
			client.get(ret, serverName);
			long done = ganji::util::time::GetCurTimeUs();
			cout << "client get total=" << (double) (done - start) / 1000 << "ms. open cost="
					<< (double) (open - start) / 1000 << "ms. get cost=" << (double) (done - open) / 1000 << endl;
			cout << "result:	" << ret << endl;
		}
	} catch (const apache::thrift::transport::TTransportException& ex) {
		//transport->close();
		cout << ex.what() << endl;
	} catch (const std::exception& ex) {
		cout << ex.what() << endl;
	}

	transport->close();

	return 0;
}
