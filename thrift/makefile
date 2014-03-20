all: clean compile link
#client.cpp  proxy_constants.cpp  proxy_types.cpp  RegistryProxy.cpp	server.cpp
	

compile:
	g++ -Wall -I/usr/local/include/thrift -c client.cpp -o client.o	
	g++ -Wall -I/usr/local/include/thrift -c proxy_constants.cpp -o proxy_constants.o
	g++ -Wall -I/usr/local/include/thrift -c proxy_types.cpp -o proxy_types.o	
	g++ -Wall -I/usr/local/include/thrift -c RegistryProxy.cpp -o RegistryProxy.o
	g++ -Wall -I/usr/local/include/thrift -c server.cpp -o server.o
link:
	g++ -lthrift -L/usr/local/lib server.o RegistryProxy.o proxy_constants.o proxy_types.o -o server 
	g++ -lthrift -L/usr/local/lib client.o RegistryProxy.o proxy_constants.o proxy_types.o -o client 

clean:
	rm -f *.o
	rm -f server client

