// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

#include <iostream>
#include <protocol/TBinaryProtocol.h>
#include <server/TSimpleServer.h>
#include <transport/TServerSocket.h>
#include <transport/TBufferTransports.h>

#include "../thrift/RegistryProxy.h"

#include "ZkClient.h"
//#include "RegistryCache.h"
#include "RequestPool.h"

using namespace std;
using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;
using boost::shared_ptr;
//using namespace FinagleRegistryProxy;

namespace FinagleRegistryProxy {


class ServerHandler: virtual public RegistryProxyIf {

private:
	RegistryCache *cache;
	ZkClient *client;
public:
	ServerHandler(string zkhosts) {
		cache = &RegistryCache();
		client = &ZkClient(zkhosts, cache);
	}

//	RequestPool rpool;
//	ClientPool cpool;

	void get(std::string& _return, const std::string& serviceName) {
		vector<Registry>* pvector = cache->get(serviceName);
		if (pvector == 0 || pvector->size() == 0) {
			client->UpdateServices(serviceName);
			pvector = cache->get(serviceName);
		}
		if (pvector && pvector->size() > 0) {
			_return = Registry::toJsonStringx(*pvector);
		} else
			_return = "";
	}

	void remove(std::string& _return, const std::string& serviceName, const std::string& host, const int32_t port) {
		// Your implementation goes here
		// printf("remove\n");
		cout << "remove called" << endl;
	}
};

} /* namespace FinagleRegistryProxy  */
