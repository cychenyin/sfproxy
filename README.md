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
$ ./frproxy -h
Proxy Server of Service Framework of Ganji RPC.
Usage: frproxy [options [option value]]
	-d,  --debug:		run test main for debugging only
	-p,  --port:		use definited port. default 9009. eg. -p 9009
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

$ ./client --help
client version:1.0.1
Usage: client [option [option_value]]
Options:
	-h, --help:			print usage. 
	-s, --server:		server of frproxy :	default 127.0.0.1. eg. -s localhost
	-p, --port:			default 9009. eg. -p 9090
	-n, --name:			name of service:	default testservice. eg. -n rpc.counter.thrift
	-t, --threadcount:	default 0, single thread. eg. -t 2
	-c, --count:		default 1 when single thread. or 1. eg. -c 9999
	-m, --method:		candidatation include get, remove, dump. default get
	-l, --list:	list 	frproxy server
	-z,  --zkhosts:		zookeeper hosts. default 127.0.0.1:2181. eg. -z 192.168.2.202:2181
eg. 
	./client 
	./client -c 999
	./client -c 999 -t 2
	./client -c 999 -t 2 -i 192.168.1.111 -p 9009
	./client -m dump
	./client -l -z 192.168.113.212:2181
	./client -n rpc.counter.thrift

----------------
package & install：
----------------
	Binary rpm package provided only.
	Rpmbuilder required when packaging. And sh & spec files are wroten for centos6.5. If package folder is in /usr/linux, then you sould mod path to be match in file package.sh.
$ cd releases
$ ./package.sh
$ cd tmp
	Installation is simple. You can do it throught raw rpm file or yum tool.
$ rpm -ivh --force frproxy-v1.1.1-9-x86-64.rpm
	or 
$ yum install
	Obviously, rpm package should be deplyed into rpm server firstly.
	 
----------------
release：
----------------
#### version 1.1.7 release 20
#   1. 简化了连接池中连接状态与使用状态的控制
#   2. 回调中不再复用原连接
#   3. 保证先监听端口, 启动的过程中热机和自注册不再阻塞, 避免在zk不可用情况下表现为假死
#   4. 改进了一些提示
#   5. 按上面的状态图增加销毁队列, 
#   6. 将使用状态改成计数器, 
#   7. 在加入销毁队列的时候讲状态的client.id设置为0, 
#   8. 在restore_status的时候恢复client.id=0的状态, 
#   9. 改进了watcher的实现,  避免静态函数中访问受保护的成员; 
#   10. 改进了client中状态恢复的机制, 10分钟(检查周期，可修改)后通过check自动恢复监视
#   11. 增加了当client中队列满的时候的容错机制

#### version 1.1.6 release 19
#   1. add log error when save cache; 
#   2. fix serverhandler status calculate bug. 
#   3. add try catch to surround all zk access action,specifically, wake, check, reset etc.

#### version 1.1.6 release 17
#   1. add status method supported in frproxy.init.d.sh 
#   2. frproxy 增加zk自动刷新
#   3. frproxy增加配置落地自动加载
#   4. frproxy 增加远程手工刷新的接口, 不对外公开
#   5. frproxy线上自注册有服务器丢失的问题,zk连接bug
#   6. frproxy zk node 增加校验
#   7. frproxy 增加自守护
#   8. frproxy中zk连接周期性自动断开


#### version 1.1.6 release 16
#   1. explict that loading *.so in current folder; make more simple on deployment
#   2. move install path from /usr/local/webserver/frproxy to /usr/local/frproxy

#### version 1.1.5 release 15
*   3. add zkhosts in dump result
	
#### version 1.1.5 release 14
*   1. fix root watch bug
*   2. add more comments in source code
*   3. some refactor in zkclient

#### version 1.1.4 release 13
*   1. remove some cout
	
#### version 1.1.3 release 12
*   1. forbidden zk log
*	2. little improve in main function

#### version 1.1.2 release 10
*   1. add help support in client
*	2. add dump support in client

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
