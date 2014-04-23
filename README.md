frproxy
==========================
----------------
intoduce：
----------------
&nbsp;&nbsp;&nbsp;&nbsp;
Proxy of Service Framework of Ganji RPC. Wrote in C++.

----------------
interface：
----------------
##use thrift interface

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
}


----------------
features：
----------------
1. busy avoid
2. nonblocking
3. base of zk callback
4. log via scribe
5. warm on start 

----------------
performance：
----------------
10000 request pre second use short connection. 
16000 requests pre second using long connection.
2000 connections asure successfully. 
0.09% failure on 5000 connections.
0.02ms cost server to handle whole biz when one host returned.
