#!/usr/bin/env python

#import os
#import sys
#from urlparse import urlparse
from thrift.transport import TTransport
from thrift.transport import TSocket
from thrift.transport import THttpClient
from thrift.protocol import TBinaryProtocol
import RegistryProxy
#from ttypes import *

servers=["127.0.0.1"]
port = 9009
http = False
framed = True
timeout = 1000

code = 0
for host in servers:
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
	try:
		transport.open()
		#pp.pprint(client.get("rta.counter.thrift"))
		client.get("rta.counter.thrift")
	except TApplicationException, e:
		#print "TApplicationException {} {}".format(host, e.message)
		code = 1
	except TException, e:
		#print "TException {} {}".format(host, e.message)
		code = 1
	except Exception, e:
		#print "Exception {} {}".format(host, e) 
		code = 1
	except:
		#print "unknown exception occured. {}".format(host)
		code = 2
	finally:
		transport.close()
#exit(code)
print code
