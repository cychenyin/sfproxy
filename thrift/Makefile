CP=/bin/cp -f
RM=/bin/rm -f
MV=/bin/mv -f
MKDIR=/bin/mkdir -p
#CC=g++ -O2
CC=g++ -ggdb
CFLAGS=-D_LINUX_ -D_GNU_SOURCE -DTHREADED -Wl,-rpath=.:/usr/local/frproxy:/usr/local/lib
#c++11# -std=c++0x  -Wall

LIBS    := thrift thriftnb event pthread zookeeper_mt boost_system boost_filesystem
LIB_PATH=../lib /usr/local/lib
#~/opt/boost/lib  
BIN_PATH=../bin
INCLUDES=../include /usr/include /usr/local/include /usr/local/include/thrift ../include/zookeeper

MOD_NAME=client
MOD_NAME2=server
MOD_NAME3=sclient
MOD_NAME4=sserver

LOG_CXXFILES=../log/logger.cpp ../log/scribe_log.cpp ../log/gen-cpp/fb303_constants.cpp ../log/gen-cpp/fb303_types.cpp ../log/gen-cpp/scribe_constants.cpp ../log/gen-cpp/scribe_types.cpp ../log/gen-cpp/scribe.cpp ../log/gen-cpp/FacebookService.cpp
COMMON_CXXFILES=proxy_constants.cpp RegistryProxy.cpp proxy_types.cpp RegistryProxyHandler.cpp $(LOG_CXXFILES)
MOD_NAME_CXXFILES=$(COMMON_CXXFILES) client.cpp ../core/ClientPool.cpp ../core/ZkClient.cpp ../core/Registry.cpp ../core/JsonUtil.cpp ../core/FileCache.cpp ../core/RegistryCache.cpp
MOD_NAME2_CXXFILES=$(COMMON_CXXFILES) server.cpp
MOD_NAME3_CXXFILES=$(COMMON_CXXFILES) clientSimple.cpp
MOD_NAME4_CXXFILES=$(COMMON_CXXFILES) serverSimple.cpp

OBJS=$(MOD_NAME_CXXFILES:.cpp=.o)
OBJS2=$(MOD_NAME2_CXXFILES:.cpp=.o)
OBJS3=$(MOD_NAME3_CXXFILES:.cpp=.o)
OBJS4=$(MOD_NAME4_CXXFILES:.cpp=.o)

all: $(MOD_NAME) $(MOD_NAME2) $(MOD_NAME3) $(MOD_NAME4)
# $(MOD_NAME2) $(MOD_NAME3) $(MOD_NAME4)
#client.cpp  proxy_constants.cpp  proxy_types.cpp  RegistryProxy.cpp	server.cpp
	@echo 'make client done....................................'	

$(MOD_NAME): $(OBJS)
	$(CC) -o $(BIN_PATH)/$(MOD_NAME) $(OBJS) $(CFLAGS) $(CPPFLAGS) $(addprefix -l,$(LIBS)) $(addprefix -L,$(LIB_PATH)) $(LDFLAGS)

$(MOD_NAME2): $(OBJS2)
	$(CC) -o $(BIN_PATH)/$(MOD_NAME2) $(OBJS2) $(CFLAGS) $(CPPFLAGS) $(addprefix -l,$(LIBS)) $(addprefix -L,$(LIB_PATH)) $(LDFLAGS)

$(MOD_NAME3): $(OBJS3)
	$(CC) -o $(BIN_PATH)/$(MOD_NAME3) $(OBJS3) $(CFLAGS) $(CPPFLAGS) $(addprefix -l,$(LIBS)) $(addprefix -L,$(LIB_PATH)) $(LDFLAGS) 

$(MOD_NAME4): $(OBJS4)
	$(CC) -o $(BIN_PATH)/$(MOD_NAME4) $(OBJS4) $(CFLAGS) $(CPPFLAGS) $(addprefix -l,$(LIBS)) $(addprefix -L,$(LIB_PATH)) $(LDFLAGS) 

.SUFFIXES: .o .cpp
.cpp.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $(addprefix -I,$(INCLUDES)) $< -o $@

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
	echo $(MOD_NAME)
	$(RM) "$(BIN_PATH)/$(MOD_NAME)"
#	test -z $(MOD_NAME2) && $(RM) "$(BIN_PATH)/$(MOD_NAME2)"
#	test -z $(MOD_NAME3) && $(RM) "$(BIN_PATH)/$(MOD_NAME3)"
#	test -z $(MOD_NAME4) && $(RM) "$(BIN_PATH)/$(MOD_NAME4)"
