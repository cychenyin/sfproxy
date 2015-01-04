namespace java com.ganji.arch.sf.thrift
namespace php RegistryProxy
namespace py RegistryProxy
namespace cpp FinagleRegistryProxy

#zookeeper registry proxy interface for php client
service RegistryProxy {
	#get enty of serviceName, serviceName is path info in zookeeper
	string get(1: required string serviceName);
	#reserved method, not supportted currently
	string remove(1: required string serviceName, 2: optional string host, 3: optional i32 port);
	#dump server state
	string dump();
    string reset();
    i32 status();
}
