#!/usr/bin/env python
#
# Autogenerated by Thrift
#
# DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
#

import os
import sys
import pprint
from urlparse import urlparse
from thrift.transport import TTransport
from thrift.transport import TSocket
from thrift.transport import THttpClient
from thrift.protocol import TBinaryProtocol
import time
import RegistryProxy
from ttypes import *
from timeutils import *

class Stat:
	def __init__(self, host):
		self.host = host;
		self.failCounter = float(0);
		self.successCounter = float(0);
		self.costAcc = float(0);
	def getHost(self):
		return self.host;

	def success(self):
		self.successCounter = self.successCounter + 1
		return self.successCounter

	def fail(self):
		self.failCounter = self.failCounter + 1
		return self.failCounter

	def successRate(self):
		if self.successCounter + self.failCounter == 0:
			return 1
		return self.successCounter / (self.successCounter + self.failCounter);
	def cost(self, val):
	    self.costAcc = self.costAcc + val
	def avg(self):
	    total = self.successCounter + self.failCounter
	    if total == 0:
		return 0
	    return self.costAcc * 1000 / total

	def toString(self):
		#return "host	{}	total	{}	success	{}".format(self.host, self.successCounter + self.failCounter, self.successRate())
		#return "host	%s	total	%d	success	%f	fail	%f" % (self.host, self.successCounter + self.failCounter, self.successRate(), float(1)-self.successRate())
		return "host	%s	total	%d	success	%f	avg %f" % (self.host, self.successCounter + self.failCounter, self.successRate(), self.avg())
	def printt(self):
		print self.toString();


servers = [
"g1-fang-web-01",
"g1-fang-web-02",
"g1-fang-web-03",
"g1-fang-web-05",
"g1-fang-web-06",
"g1-fang-web-07",
"g1-fang-web-08",
"g1-fang-web-09",
"g1-fang-web-10",
"g1-mob-app-01",
"g1-mob-app-02",
"g1-mob-app-03",
"g1-mob-app-04",
"g1-mob-app-05",
"g1-mob-app-06",
"g1-mob-app-07",
"g1-mob-app-08",
"g1-mob-app-09",
"g1-mob-app-10",
"g1-mob-app-11",
"g1-mob-app-12",
"g1-mob-app-13",
"g1-mob-wap-01",
"g1-mob-wap-02",
"g1-mob-wap-03",
"g1-mob-wap-04",
"g1-mob-wap-05",
"g1-ms-sta-05",
"g1-ms-sta-06",
"g1-ms-sta-07",
"g1-ms-sta-08",
"g1-ms-web-01",
"g1-ms-web-02",
"g1-ms-web-04",
"g1-ms-web-05",
"g1-ms-web-08",
"g1-ms-web-09",
"g1-sec-web-01",
"g1-sec-web-02",
"g1-sec-web-03",
"g1-sec-web-04",
"g1-zp-web-01",
"g1-zp-web-02",
"g1-zp-web-03",
"g1-zp-web-05",
"g1-zp-web-07",
"g1-zp-web-08",
"jxq-ms-sta-01",
"jxq-ms-sta-02",
"jxq-ms-sta-03",
"jxq-ms-sta-04",
"jxq-ms-sta-05",
"jxq-ms-sta-06",
"yz-adm-web-01",
"yz-adm-web-02",
"yz-fang-web-02",
"yz-fang-web-03",
"yz-fang-web-04",
"yz-fang-web-05",
"yz-fang-web-06",
"yz-fang-web-08",
"yz-fang-web-09",
"yz-fang-web-10",
"yz-fang-web-11",
"yz-fang-web-12",
"yz-fang-web-13",
"yz-fang-web-14",
"yz-im-web-01",
"yz-im-web-02",
"yz-im-web-03",
"yz-im-web-04",
"yz-im-web-05",
"yz-im-web-06",
"yz-im-web-07",
"yz-im-web-08",
"yz-im-web-09",
"yz-im-web-10",
"yz-im-web-11",
"yz-im-web-12",
"yz-im-web-14",
"yz-im-web-15",
"yz-im-web-16",
"yz-mob-app-01",
"yz-mob-app-02",
"yz-mob-app-04",
"yz-mob-app-06",
"yz-mob-app-07",
"yz-mob-app-08",
"yz-mob-app-09",
"yz-mob-app-10",
"yz-mob-app-11",
"yz-mob-app-12",
"yz-mob-app-13",
"yz-mob-wap-01",
"yz-mob-wap-02",
"yz-mob-wap-03",
"yz-mob-wap-04",
"yz-mob-wap-05",
"yz-mob-wap-07",
"yz-mob-wap-08",
"yz-mob-wap-09",
"yz-mob-wap-10",
"yz-mob-wap-11",
"yz-mob-wap-12",
"yz-mob-wap-13",
"yz-mob-wap-14",
"yz-mob-wap-15",
"yz-mob-wap-16",
"yz-ms-web-01",
"yz-ms-web-03",
"yz-ms-web-08",
"yz-ms-web-09",
"yz-ms-web-10",
"yz-ms-web-18",
"yz-ms-web-19",
"yz-ms-web-24",
"yz-ms-web-31",
"yz-ms-web-a06",
"yz-ms-web-a07",
"yz-ms-web-a09",
"yz-pay-web-01",
"yz-pay-web-02",
"yz-pay-web-03",
"yz-pay-web-03",
"yz-psp-web-01",
"yz-psp-web-02",
"yz-psp-web-03",
"yz-sec-web-01",
"yz-sec-web-02",
"yz-sec-web-03",
"yz-sec-web-04",
"yz-sec-web-05",
"yz-sec-web-06",
"yz-sec-web-07",
"yz-sec-web-09",
"yz-tg-web-01",
"yz-tg-web-02",
"yz-tg-web-03",
"yz-tg-web-04",
"yz-tg-web-05",
"yz-tg-web-06",
"yz-tg-web-07",
"yz-tg-web-08",
"yz-tg-web-09",
"yz-zp-web-01",
"yz-zp-web-02",
"yz-zp-web-03",
"yz-zp-web-04",
"yz-zp-web-05",
"yz-zp-web-06",
"yz-zp-web-07",
"yz-zp-web-08",
"yz-zp-web-09",
"yz-zp-web-10",
"yz-zp-web-11",
"yz-zp-web-12",
"yz-fw-web-01",
"yz-fw-web-02",
"yz-fw-web-03",
"yz-fw-web-04",
"yz-fw-web-05",
"yz-fw-web-06",
"yz-fw-web-07",

]
servers=["127.0.0.1"]
stats = {host:Stat(host) for host in servers  }

logFileName = "burn.log." + timestamp_fromunixtime2()
logMaxSize = 100 * 1024 * 1024;
flog = open(logFileName, "a+")

#print timestamp_fromunixtime2()

#print servers
#print len(stats)
#print "reparation is over. ready to burn ......"
#print os.path.getsize("rrsend.py")
#exit(0)


port = 9009
http = False
framed = True
timeout = 100
pp = pprint.PrettyPrinter(indent = 1)
round = 0
while(True):
	round = round + 1
	print "round %d" % round
	for host in servers:
		stat = stats[host]

		if http:
			transport = THttpClient.THttpClient(host, port, uri)
			transport.setTimeout(timeout)
		else:
			socket = TSocket.TSocket(host, port)
			socket.setTimeout(timeout)
		if framed:
			transport = TTransport.TFramedTransport(socket)
		else:
			transport = TTransport.TBufferedTransport(socket)
		protocol = TBinaryProtocol.TBinaryProtocol(transport)
		client = RegistryProxy.Client(protocol)
		start = time.clock()
		try:
			transport.open()
			#pp.pprint(client.get("rta.counter.thrift"))
			client.get("rta.counter.thrift")
			stat.success()
		except TApplicationException, e:
			print "{} TApplicationException {} {}".format(timestamp_fromunixtime(), host, e.message)
			stat.fail()
		except TException, e:
			print "{} TException {} {}".format(timestamp_fromunixtime(), host, e.message)
			stat.fail()
		except Exception, e:
			print "{} Exception {} {}".format(timestamp_fromunixtime(), host, e) 
			stat.fail()

		except IOError, e:
			print "{} IOError {} {}: {} {}".format(timestamp_fromunixtime(), host, e.errno, e.strerror, e)
			stat.fail()
		except:
			print "unknown exception occured. {}".format(host)
			stat.fail()
		finally:
			end = time.clock() - start
			stat.cost(end)
			transport.close()
			#print stat.toString()
			try:
				flog.write(stat.toString() + "\n")
			except:
				print "write log error."
	flog.flush()
	if os.path.getsize(logFileName) > logMaxSize:
		flog.close()
		logFileName = "burn.log." + timestamp_fromunixtime2()		
		flog = open(logFileName, "a+")
	#time.sleep(0.5)
flog.flush()
flog.close()


