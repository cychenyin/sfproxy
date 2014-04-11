CP=/bin/cp -f
RM=/bin/rm -f
MV=/bin/mv -f
MKDIR=/bin/mkdir -p
#CC=g++ -O2
CC=g++ -ggdb
CFLAGS=-D_LINUX_ -D_GNU_SOURCE -DTHREADED -Wall
#c++11# -std=c++0x

LIBS    := thrift thriftnb event util_time
LIB_PATH= \
	../lib/ganji \
	/usr/local/lib 
	
INCLUDES= \
	../include \
	/usr/include \
	/usr/local/include \
	/usr/local/include/thrift
	
#	../include/ganji \
	
all: clean compile link
#client.cpp  proxy_constants.cpp  proxy_types.cpp  RegistryProxy.cpp	server.cpp
	echo 'done....................................'	

compile:
	g++ $(CFLAGS) $(CPPFLAGS) $(addprefix -I,$(INCLUDES)) -c proxy_constants.cpp -o proxy_constants.o
	g++ $(CFLAGS) $(CPPFLAGS) $(addprefix -I,$(INCLUDES)) -c proxy_types.cpp -o proxy_types.o	
	g++ $(CFLAGS) $(CPPFLAGS) $(addprefix -I,$(INCLUDES)) -c RegistryProxy.cpp -o RegistryProxy.o
	g++ $(CFLAGS) $(CPPFLAGS) $(addprefix -I,$(INCLUDES)) -c RegistryProxyHandler.cpp -o RegistryProxyHandler.o
	g++ $(CFLAGS) $(CPPFLAGS) $(addprefix -I,$(INCLUDES)) -c server.cpp -o server.o
	g++ $(CFLAGS) $(CPPFLAGS) $(addprefix -I,$(INCLUDES)) -c serverSimple.cpp -o serverSimple.o
	g++ $(CFLAGS) $(CPPFLAGS) $(addprefix -I,$(INCLUDES)) -c client.cpp -o client.o
	g++ $(CFLAGS) $(CPPFLAGS) $(addprefix -I,$(INCLUDES)) -c clientSimple.cpp -o clientSimple.o
#	g++ $(CFLAGS) $(CPPFLAGS) -I/usr/local/include/thrift -I../includes -c clientSimple.cpp -o clientSimple.o	
link:
	g++ $(CFLAGS) $(addprefix -l,$(LIBS)) $(addprefix -L,$(LIB_PATH)) $(LDFLAGS) RegistryProxy.o proxy_constants.o proxy_types.o RegistryProxyHandler.o server.o -o server
	g++ $(CFLAGS) $(addprefix -l,$(LIBS)) $(addprefix -L,$(LIB_PATH)) $(LDFLAGS) RegistryProxy.o proxy_constants.o proxy_types.o RegistryProxyHandler.o serverSimple.o -o sserver 
	g++ $(CFLAGS) $(addprefix -l,$(LIBS)) $(addprefix -L,$(LIB_PATH)) $(LDFLAGS) RegistryProxy.o proxy_constants.o proxy_types.o client.o -o client 
	g++ $(CFLAGS) $(addprefix -l,$(LIBS)) $(addprefix -L,$(LIB_PATH)) $(LDFLAGS) RegistryProxy.o proxy_constants.o proxy_types.o clientSimple.o -o sclient 

install:
	echo "not support now"	
clean:
	rm -f *.o
	rm -f sserver sclient server client
