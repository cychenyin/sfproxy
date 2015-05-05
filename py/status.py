#!/usr/bin/env python

import sys
import pprint
from urlparse import urlparse
from thrift.transport import TTransport
from thrift.transport import TSocket
from thrift.transport import THttpClient
from thrift.protocol import TBinaryProtocol

import RegistryProxy
#from RegistryProxy import *
from ttypes import *

class ClientX:
    def __init__(self, host_=None, port_=None):
        self.host = host_ if host_ else "localhost"
        self.port = port_ if port_ else 9009

    def status(self):
        try:
            socket = TSocket.TSocket(self.host, self.port)
            transport = TTransport.TFramedTransport(socket)
            protocol = TBinaryProtocol.TBinaryProtocol(transport)
            client = RegistryProxy.Client(protocol)
            transport.open()
            status = client.status()
            if status & 1 == 1:
                return False
        except Exception as e:
            return False
        except:
            print 'exception'
            return False
        finally:
            if transport:
                transport.close()
        return True



if __name__ == '__main__':
    c = ClientX()
    print c.status();


