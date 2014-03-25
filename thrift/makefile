all: clean compile link
#client.cpp  proxy_constants.cpp  proxy_types.cpp  RegistryProxy.cpp	server.cpp
	

compile:
	g++ -Wall -I/usr/local/include/thrift -c proxy_constants.cpp -o proxy_constants.o
	g++ -Wall -I/usr/local/include/thrift -c proxy_types.cpp -o proxy_types.o	
	g++ -Wall -I/usr/local/include/thrift -c RegistryProxy.cpp -o RegistryProxy.o
	g++ -Wall -I/usr/local/include/thrift -c RegistryProxyHandler.cpp -o RegistryProxyHandler.o
	g++ -Wall -I/usr/local/include/thrift -c server.cpp -o server.o
	g++ -Wall -I/usr/local/include/thrift -c serverSimple.cpp -o serverSimple.o
	g++ -Wall -I/usr/local/include/thrift -c client.cpp -o client.o
	g++ -Wall -I/usr/local/include/thrift -c clientSimple.cpp -o clientSimple.o	
link:
	g++ -lthrift-0.9.1 -lthriftnb-0.9.1 -levent -L/usr/local/lib RegistryProxy.o proxy_constants.o proxy_types.o RegistryProxyHandler.o server.o -o server
	g++ -lthrift-0.9.1 -lthriftnb-0.9.1 -levent -L/usr/local/lib RegistryProxy.o proxy_constants.o proxy_types.o RegistryProxyHandler.o serverSimple.o -o sserver 
	g++ -lthrift-0.9.1 -lthriftnb-0.9.1 -levent -L/usr/local/lib RegistryProxy.o proxy_constants.o proxy_types.o client.o -o client 
	g++ -lthrift-0.9.1 -lthriftnb-0.9.1 -levent -L/usr/local/lib RegistryProxy.o proxy_constants.o proxy_types.o clientSimple.o -o sclient 

clean:
	rm -f *.o
	rm -f sserver sclient server client
