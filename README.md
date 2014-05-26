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
 
----------------
release：
----------------
#### version 1.1.1 release 9
*   1. package frproxy.init.d.sh into rpm
#### version 1.1.0 release 7
*    1. add skip strategy
*    2. change conn pool from delete to retry when session stuff occured
*    3. add info type supported in logger
*    4. move warm to asyc mode
*    5. add self register
*    6. add service scripts for init.d
*    7. move package stuff into releases folder
*    8. add dump interface
*    9. add py tester
*    10. add c++ multiple thread tester
    
#### version 1.0.1 release 6
    1. package client into rpm 
    2. behavior of log change from overwrite to append
#### version 1.0.1 release 5
    1. fix -v --version start server bug
    2. 
#### version 1.0.1 release 4
    1. fix scribe_log make cpu keep load high bug.
    2. update verion to 1.0.1 

#### version 1.0.0 release 3
    1. update default port from 9091 to 9009
#### version 1.0.0 release 2
    1. zoo_set_debug_level change from ZOO_LOG_LEVEL_WARN to ZOO_LOG_LEVEL_ERROR
    2. proxy.sh run frproxy in background, and write stdout in file.
    3. fix usage zk eg bug about loss port
#### version 1.0.0
    init release.
