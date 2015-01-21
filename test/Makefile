MODNAME=frproxy_unittest

CP=/bin/cp -f
RM=/bin/rm -f
MV=/bin/mv -f
MKDIR=/bin/mkdir -p
THRIFT=/usr/local/bin/thrift
#CC=g++ -O2
CC=g++ -ggdb -g
# -fPIC -DPIC
CPP_OPTS=-D_LINUX_ -D_GNU_SOURCE -DTHREADED -Wl,-rpath=.:/usr/local/frproxy:../lib:../bin:/usr/local/lib

BIN_PATH=.
INSTALL_PATH=/usr/local/webserver/frproxy
LIB_PATH=../lib /usr/local/lib

INCLUDES= \
	-I../include \
	-I../include/zookeeper \
	-I/usr/include \
	-I/usr/local/include \
	-I/usr/local/include/thrift 

LIBS		:= thrift thriftnb event pthread zookeeper_mt rt boost_system boost_filesystem boost_unit_test_framework boost_unit_test_framework-mt
STATIC_LibS :=  pthread rt
EMBED_STATIC_LIBS= ../lib/thrift/libthrift.a \
	../lib/thrift/libthriftnb.a \
	../lib/libevent/libevent.a \
	../lib/zookeeper/libzookeeper_mt.a

TARGET=$(MODNAME)
#TARGET2=testproxy
LOG_CXXFILES= ../log/logger.cpp ../log/scribe_log.cpp ../log/gen-cpp/fb303_constants.cpp ../log/gen-cpp/fb303_types.cpp ../log/gen-cpp/scribe_constants.cpp ../log/gen-cpp/scribe_types.cpp ../log/gen-cpp/scribe.cpp ../log/gen-cpp/FacebookService.cpp ../log/LoggerStream.cpp
#TARGET_CXXFILES=$(LOG_CXXFILES) thrift/proxy_constants.cpp thrift/proxy_types.cpp thrift/RegistryProxy.cpp core/Registry.cpp core/JsonUtil.cpp core/FileCache.cpp core/RegistryCache.cpp core/ZkClient.cpp core/ClientPool.cpp core/ServerHandler.cpp \


SRC_CXXFILE =$(LOG_CXXFILES) ../thrift/proxy_constants.cpp ../thrift/proxy_types.cpp ../thrift/RegistryProxy.cpp \
	../core/Registry.cpp ../core/JsonUtil.cpp ../core/FileCache.cpp ../core/RegistryCache.cpp ../core/ZkClient.cpp ../core/ClientPool.cpp \
	../core/ServerHandler.cpp 
	
TARGET_CXXFILES=$(SRC_CXXFILE) entry.cpp SerializeTest.cpp # ZkPoolTest.cpp # UtilsTest.cpp # RegistryCacheTest.cpp # SmartpointTest.cpp #Polymorphism.cpp # TaskTest.cpp SerializeFixtureTest.cpp FileCacheTest.cpp SchedulerTest.cpp 

OBJS=$(TARGET_CXXFILES:.cpp=.o)

all: preexec $(TARGET) afterexec
	@echo '---------------all done. package can be started now ---------------'

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(CPP_OPTS) $(CPPFLAGS) $(LDFLAGS) $(addprefix -l,$(LIBS)) $(addprefix -L,$(LIB_PATH)) $(OBJS) -o $(BIN_PATH)/$(TARGET) 

.PHONY:re
re: clean all
	@echo '---------------clean  *.o in all source code dir ---------------'

.SUFFIXES: .o .cpp
.cpp.o:
	$(CC) $(CFLAGS) $(CPP_OPTS) $(CPPFLAGS) -c $(INCLUDES) $< -o $@
	
.PHONY: preexec
preexec:
	
	
.PHONY: afterexec
afterexec:
	@echo '=================================================================='
	test -f $(BIN_PATH)/$(TARGET) && $(BIN_PATH)/$(TARGET) --log_level=message
	
.PHONY: test
test:
	
.PHONY: clean
clean:
	$(RM) *.o ../log/*.o ../core/*.o ../thrift/*.o
	test -z $(BIN_PATH)/$(TARGET) || $(RM) $(BIN_PATH)/$(TARGET) 
	
install:
