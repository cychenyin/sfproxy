MODNAME=frproxy

CP=/bin/cp -f
RM=/bin/rm -f
MV=/bin/mv -f
MKDIR=/bin/mkdir -p
THRIFT=/usr/local/bin/thrift
#CC=g++ -O2
CC=g++ -ggdb -g
# -fPIC -DPIC
CPP_OPTS=-D_LINUX_ -D_GNU_SOURCE -DTHREADED -Wl,-rpath=.:/usr/local/frproxy
#-Wall #c++11# -std=c++0x
#THREADED used in zk cli_mt
#CFLAGS= -D_LINUX_ -D_GNU_SOURCE -DTHREADED -static

BIN_PATH=./bin
INSTALL_PATH=/usr/local/webserver/frproxy
LIB_PATH= ./lib  
	#/home/asdf/downloads/gtest-1.7.0/lib \
	#/usr/local/lib

INCLUDES= \
	-I./include \
	-I./include/zookeeper \
	-I/usr/include \
	-I/usr/local/include \
	-I/usr/local/include/thrift 
	
LIBS		:= thrift thriftnb event pthread zookeeper_mt rt boost_system boost_filesystem
STATIC_LibS :=  pthread rt
#thrift thriftnb event zookeeper_mt 
EMBED_STATIC_LIBS= lib/thrift/libthrift.a \
	lib/thrift/libthriftnb.a \
	lib/libevent/libevent.a \
	lib/zookeeper/libzookeeper_mt.a

TARGET=frproxy
#TARGET2=testproxy
LOG_CXXFILES=log/logger.cpp log/scribe_log.cpp log/gen-cpp/fb303_constants.cpp log/gen-cpp/fb303_types.cpp log/gen-cpp/scribe_constants.cpp log/gen-cpp/scribe_types.cpp log/gen-cpp/scribe.cpp log/gen-cpp/FacebookService.cpp log/LoggerStream.cpp
#TARGET_CXXFILES=$(LOG_CXXFILES) thrift/proxy_constants.cpp thrift/proxy_types.cpp thrift/RegistryProxy.cpp core/Registry.cpp core/JsonUtil.cpp core/FileCache.cpp core/RegistryCache.cpp core/ZkClient.cpp core/ClientPool.cpp core/ServerHandler.cpp \

TARGET_CXXFILES=$(LOG_CXXFILES) thrift/proxy_constants.cpp thrift/proxy_types.cpp thrift/RegistryProxy.cpp core/Registry.cpp core/JsonUtil.cpp core/FileCache.cpp core/RegistryCache.cpp core/ZkClient.cpp core/ClientPool.cpp core/ServerHandler.cpp \
 main.cpp
 
# test/main.cpp

OBJS=$(TARGET_CXXFILES:.cpp=.o)

all: clean preexec $(TARGET) afterexec client
	@echo '---------------all done. package can be started now ---------------'

.PHONY: debug
debug: clean preexecForDebug $(TARGET) afterexec
	@echo '---------------all debug make done ---------------'

$(TARGET): $(OBJS)
#static link
#g++ -ggdb -o ./bin/frproxy  -D_LINUX_ -D_GNU_SOURCE -DTHREADED  -static-libgcc \
-lrt -lpthread \
-L./lib/zookeeper -L/usr/local/lib  log/logger.o log/scribe_log.o log/gen-cpp/fb303_constants.o log/gen-cpp/fb303_types.o log/gen-cpp/scribe_constants.o log/gen-cpp/scribe_types.o log/gen-cpp/scribe.o log/gen-cpp/FacebookService.o thrift/proxy_constants.o thrift/proxy_types.o thrift/RegistryProxy.o core/Registry.o core/RegistryCache.o core/ZkClient.o core/ClientPool.o core/ServerHandler.o main.o \
lib/thrift/libthrift.a \
lib/thrift/libthriftnb.a \
lib/libevent/libevent.a \
lib/zookeeper/libzookeeper_mt.a
# -static-libgcc -gdwarf-2 -gstrict-dwarf 
# -static-libstdc++ 
#	$(CC) -lmaplec -Wl,--no-as-needed -static -o $(BIN_PATH)/$(TARGET) $(CFLAGS) $(CPP_OPTS) $(CPPFLAGS) $(addprefix -l,$(STATIC_LibS)) $(addprefix -L,$(LIB_PATH)) $(LDFLAGS) \
		$(OBJS) \
		$(EMBED_STATIC_LIBS) \
		/usr/lib64/libpthread.a
#shared link
#	$(CC) -o $(BIN_PATH)/$(TARGET) $(CFLAGS) $(CPP_OPTS) $(CPPFLAGS) $(LDFLAGS) $(addprefix -l,$(LIBS)) $(addprefix -L,$(LIB_PATH)) $(OBJS) 
	$(CC) -o $(BIN_PATH)/$(TARGET) $(CFLAGS) $(CPP_OPTS) $(CPPFLAGS) $(LDFLAGS) $(addprefix -l,$(LIBS)) $(addprefix -L,$(LIB_PATH)) $(OBJS) 

.SUFFIXES: .o .cpp
.cpp.o:
	$(CC) $(CFLAGS) $(CPP_OPTS) $(CPPFLAGS) -c $(INCLUDES) $< -o $@

.PHONY: preexecForDebug
preexecForDebug: preexec
#	CFLAGS=-D_LINUX_ -D_GNU_SOURCE -DTHREADED -DDEBUG_ 
		
.PHONY: preexec
preexec:
	test -z $(BIN_PATH) || $(MKDIR) -- $(BIN_PATH)
	
.PHONY: afterexec
afterexec:
	$(RM) *~ *.swp
	$(CP) ./lib/* $(BIN_PATH)/
	cp -f proxy.sh $(BIN_PATH)/
	cp -f proxy_client.sh $(BIN_PATH)/

.PHONY: client
client:
	make -C thrift

.PHONY: test
test: btest
	make -C test
	
.PHONY: clean
clean:
	$(RM) *.o log/*.o ./core/*.o ./thrift/*.o t
	test -z $(TARGET) || $(RM) $(BIN_PATH)/$(TARGET) 
	test -z $(TARGET1) || $(RM) $(BIN_PATH)/$(TARGET2)
	test -z $(TARGET2) || $(RM) $(BIN_PATH)/$(TARGET2)
	
install:
#	ar r $(TARGET) $(OBJS) 
#	ranlib $(TARGET)

	#echo "not support now."
	test -z $(INSTALL_PATH) || $(MKDIR) -- $(INSTALL_PATH)
	install -m 777 $(BIN_PATH)/frproxy $(INSTALL_PATH)/frproxy
	install -m 777 $(BIN_PATH)/proxy.sh $(INSTALL_PATH)/proxy.sh
	install -m 777 $(BIN_PATH)/proxy_client.sh $(INSTALL_PATH)/proxy_client.sh
	install lib/* $(INSTALL_PATH)/
	
.PHONY: gen
gen:
# gen thrift file
	test -d $(THRIFT) && test -d log/fb303_types.h || $(THRIFT) -o ./log/ --gen cpp log/fb303.thrift
	test -d $(THRIFT) && test -d log/scribe_types.h || $(THRIFT) -o ./log/ --gen cpp log/scribe.thrift
	
