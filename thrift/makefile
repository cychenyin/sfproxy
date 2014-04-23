CP=/bin/cp -f
RM=/bin/rm -f
MV=/bin/mv -f
MKDIR=/bin/mkdir -p
#CC=g++ -O2
CC=g++ -ggdb
CFLAGS=-D_LINUX_ -D_GNU_SOURCE -DTHREADED
#c++11# -std=c++0x  -Wall

LIBS    := thrift thriftnb event
LIB_PATH= ../lib/ganji /usr/local/lib 
BIN_PATH= ../bin	
INCLUDES= ../include /usr/include /usr/local/include /usr/local/include/thrift

MOD_NAME=client
MOD_NAME2=server
MOD_NAME3=sclient
MOD_NAME4=sserver

COMMON_CXXFILES=proxy_constants.cpp RegistryProxy.cpp proxy_types.cpp RegistryProxyHandler.cpp 
MOD_NAME_CXXFILES=$(COMMON_CXXFILES) client.cpp
MOD_NAME2_CXXFILES=$(COMMON_CXXFILES) server.cpp
MOD_NAME3_CXXFILES=$(COMMON_CXXFILES) clientSimple.cpp
MOD_NAME4_CXXFILES=$(COMMON_CXXFILES) serverSimple.cpp

OBJS=$(TARGET_CXXFILES:.cpp=.o)
	
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
	#ar r $(TARGET) $(OBJS) 
    #ranlib $(TARGET)

install:
	echo "not support now"	
clean:
	rm -f *.o
	rm -f sserver sclient server client
