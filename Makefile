MODNAME=frproxy

CP=/bin/cp -f
RM=/bin/rm -f
MV=/bin/mv -f
MKDIR=/bin/mkdir -p
THRIFT=/usr/local/bin/thrift
#CC=g++ -O2
CC=g++ -ggdb
# -fPIC -DPIC
CFLAGS= -D_LINUX_ -D_GNU_SOURCE -DTHREADED 
#-Wall #c++11# -std=c++0x
#THREADED used in zk cli_mt

BIN_PATH=./bin
INSTALL_PATH=/usr/local/bin
LIB_PATH= \
	./lib/zookeeper \
	/usr/local/lib 
	#/home/asdf/downloads/gtest-1.7.0/lib \

INCLUDES= \
	-I./include \
	-I./include/zookeeper \
	-I/usr/include \
	-I/usr/local/include \
	-I/usr/local/include/thrift 
	
LIBS    := thrift thriftnb event pthread zookeeper_mt   
# thriftnb ganji_util_thread util_config util_time ganji_util_scribe_log

#TARGET=$(LIB_PATH)/$(MODNAME).a
#TARGET=$(BIN_PATH)/frproxy
TARGET=frproxy
#TARGET2=testproxy
LOG_CXXFILES=./log/scribe_log.cpp ./log/gen-cpp/fb303_constants.cpp ./log/gen-cpp/fb303_types.cpp ./log/gen-cpp/scribe_constants.cpp ./log/gen-cpp/scribe_types.cpp ./log/gen-cpp/scribe.cpp ./log/gen-cpp/FacebookService.cpp
TARGET_CXXFILES=thrift/proxy_constants.cpp thrift/proxy_types.cpp thrift/RegistryProxy.cpp core/Registry.cpp core/RegistryCache.cpp core/ZkClient.cpp core/ClientPool.cpp core/ServerHandler.cpp main.cpp
# test/main.cpp
TEST_CXXFILES=$(LOG_CXXFILES) a.xxxxxxxxxxxxxxxxxxxxxxxxxxxx

OBJS=$(TARGET_CXXFILES:.cpp=.o)


#all: clean preexec $(TARGET) $(TARGET2) afterexec
all: clean preexec $(TARGET) afterexec
	echo '---------------all done ---------------'

$(TARGET): $(OBJS)
	$(CC) -o $(BIN_PATH)/$(TARGET) $(CFLAGS) $(CPPFLAGS) $(addprefix -l,$(LIBS)) $(addprefix -L,$(LIB_PATH)) $(LDFLAGS) $(OBJS)
	$(CP) ./lib/zookeeper/libzookeeper_mt.so.2 ./lib/zookeeper/libzookeeper_mt.so $(BIN_PATH)/
#	ar r $(TARGET) $(OBJS) 
#	ranlib $(TARGET)

.SUFFIXES: .o .cpp
.cpp.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $(INCLUDES) $< -o $@

.PHONY: preexec
preexec:
	echo $(TARGET_CXXFILES)
# gen thrift file
#	make -C thrift
#	$(MKDIR) $(BIN_PATH)/$(MODNAME)
	test -d ./log/gen-cpp/ || $(THRIFT) -o ./log --gen cpp ./log/fb303.thrift && $(THRIFT) -o ./log --gen cpp ./log/scribe.thrift
	
.PHONY: afterexec
afterexec:
	$(RM) *~ *.swp

.PHONY: client
client:
	make -C thrift

.PHONY: test
test:
	#make -C test
	
.PHONY: clean
clean:
	$(RM) *.o ./log/*.o ./core/*.o ./thrift/*.o t
	test -z $(TARGET) || $(RM) $(BIN_PATH)/$(TARGET) 
	test -z $(TARGET1) || $(RM) $(BIN_PATH)/$(TARGET2)
	test -z $(TARGET2) || $(RM) $(BIN_PATH)/$(TARGET2)
	
install:
	#echo "not support now."
	test -z $(INSTALL_PATH) || test -d $(INSTALL_PATH) || $(MKDIR) -- $(INSTALL_PATH)
	install -m 755 $(BIN_PATH)/frproxy $(INSTALL_PATH)/frproxy
	
.PHONY: gen
gen:
	$(THRIFT) -o ./log --gen cpp fb303.thrift
	$(THRIFT) -o ./log --gen cpp scribe.thrift
	