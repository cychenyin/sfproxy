#include <iostream>
#include "RegistryProxy.h"  // As an example
#include "../frproxy.h"

#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

using namespace FinagleRegistryProxy;

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

	int port = option_value(argc, argv, "-p", "--port", 9009);;
	string service_name = option_value(argc, argv, "-s", "--service", "testservice");
	cout << "conn to port=" << port << endl;
	cout << "service name=" << service_name << endl;

	boost::shared_ptr<TSocket> socket(new TSocket("127.0.0.1", port));

	boost::shared_ptr<TTransport> transport(new TFramedTransport(socket));
	boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));

	RegistryProxyClient client(protocol);
	int i;
	long start = utils::now_in_us();
	transport->open();
	long open = utils::now_in_us();

	try {
		for (i = 0; i < 1; i++) {
			// std::string serverName = "/soa/services/testservice";
			std::string ret;
			client.get(ret, service_name);
			long done = utils::now_in_us();
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
