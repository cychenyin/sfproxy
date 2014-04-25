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
testing env:
	24 log processors Intel(R) Xeon(R) CPU E5-2620 0 @ 2.00GHz
	Linux 2.6.18-194.el5 #1 SMP Tue Mar 16 21:52:39 EDT 2010 x86_64 x86_64 x86_64 GNU/Linux
	tester & server run in same host to simulate online situation.		
10000 request pre second use short connection. 
16000 requests pre second using long connection.
2000 connections asure successfully. 
0.09% failure on 5000 connections.
0.02ms cost server to handle whole biz when one host returned.
	

----------------
deployment：
----------------
deploy on every php server. work with finagle-php & fingle raw in java services.


----------------
usage：
----------------
Proxy Server of Service Framework of Ganji RPC.
Usage: frproxy [options [option value]]
	-d,  --debug:		run test main for debugging only
	-p,  --port:		use definited port. default 9091. eg. -p 9090
	-st, --threaded:	run server as TTreadedServer
	-sp, --threadpool:	run server as TTreadPoolServer
	-sn, --nonblocking:	run server as TNonblockingServer. default server type
	-t,  --thread_count:	thread pool size or max thread count. used in NoblockingServer & ThreadpoolServer. default 16. eg. -t 20
	-h,  --help:		show help
	-v,  --version:		show version
	-z,  --zkhosts:		zookeeper hosts. default 127.0.0.1. eg. -z 192.168.2.202
	-lh, --scribehost:		scribe hosts. default 127.0.0.1. eg. -lg localhost
	-lp, --scribeport:		scribe port. default 11463. eg. -lp 11463
	-l,  --enable:		enable scribe log
eg. ./frproxy -l -sn -t 20
 
