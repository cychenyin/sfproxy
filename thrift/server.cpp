// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

#include <iostream>
#include "RegistryProxy.h"
#include <protocol/TBinaryProtocol.h>
#include <server/TSimpleServer.h>
#include <transport/TServerSocket.h>
#include <transport/TBufferTransports.h>


using namespace std;
using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace FinagleRegistryProxy;

class RegistryProxyHandler : virtual public RegistryProxyIf {
 public:
  RegistryProxyHandler() {
    // Your initialization goes here
  }

  void get(std::string& _return, const std::string& serviceName) {
    // Your implementation goes here
    _return = serviceName;
    // printf("get\n");
	cout << "get req" << endl;
  }

  void remove(std::string& _return, const std::string& serviceName, const std::string& host, const int32_t port) {
    // Your implementation goes here
    // printf("remove\n");
	cout << "remove called" << endl;
  }

};

int main(int argc, char **argv) {
  int port = 9091;
  shared_ptr<RegistryProxyHandler> handler(new RegistryProxyHandler());
  shared_ptr<TProcessor> processor(new RegistryProxyProcessor(handler));
  shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
  server.serve();
  return 0;
}

