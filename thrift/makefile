LIB_PATH= /usr/local/lib \
	./lib
INCLUDES= \
	-I/usr/include \
	-I/usr/local/include \
	-I/usr/local/include/thrift \
	-I/usr/local/include/zookeeper \
	-I./includes


all: clean compile link
#client.cpp  proxy_constants.cpp  proxy_types.cpp  RegistryProxy.cpp	server.cpp
	echo 'done....................................'	

compile:
	g++ -Wall -I/usr/local/include/thrift -c proxy_constants.cpp -o proxy_constants.o
	g++ -Wall -I/usr/local/include/thrift -c proxy_types.cpp -o proxy_types.o	
	g++ -Wall -I/usr/local/include/thrift -c RegistryProxy.cpp -o RegistryProxy.o
	g++ -Wall -I/usr/local/include/thrift -c RegistryProxyHandler.cpp -o RegistryProxyHandler.o
	g++ -Wall -I/usr/local/include/thrift -c server.cpp -o server.o
	g++ -Wall -I/usr/local/include/thrift -c serverSimple.cpp -o serverSimple.o
	g++ -Wall -I/usr/local/include/thrift -I../includes -c client.cpp -o client.o
	g++ -Wall -I/usr/local/include/thrift -I../includes -c clientSimple.cpp -o clientSimple.o	
link:
	g++ -lthrift -lthriftnb -levent -lutil_time -L../lib -L/usr/local/lib -I../includes RegistryProxy.o proxy_constants.o proxy_types.o RegistryProxyHandler.o server.o -o server
	g++ -lthrift -lthriftnb -levent -lutil_time -L../lib -L/usr/local/lib -I../includes RegistryProxy.o proxy_constants.o proxy_types.o RegistryProxyHandler.o serverSimple.o -o sserver 
	g++ -lthrift -lthriftnb -levent -lutil_time -L../lib -L/usr/local/lib -I../includes RegistryProxy.o proxy_constants.o proxy_types.o client.o -o client 
	g++ -lthrift -lthriftnb -levent -lutil_time -L../lib -L/usr/local/lib -I../includes RegistryProxy.o proxy_constants.o proxy_types.o clientSimple.o -o sclient 

clean:
	rm -f *.o
	rm -f sserver sclient server client
