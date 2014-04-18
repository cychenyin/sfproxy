MODNAME=frproxy

CP=/bin/cp -f
RM=/bin/rm -f
MV=/bin/mv -f
MKDIR=/bin/mkdir -p
#CC=g++ -O2
CC=g++ -ggdb
# -fPIC -DPIC

BIN_PATH=./bin
INSTALL_PATH=/usr/local/bin
INCLUDE_PATH=.
LIB_PATH= \
	./lib/ganji \
	./lib/zookeeper \
	/usr/local/lib 
	#/home/asdf/downloads/gtest-1.7.0/lib \
	#/home/asdf/workspace/cc_dev/trunk/lib
	

INCLUDES= \
	-I./include \
	-I./include/zookeeper \
	-I./include/ganji \
	-I/usr/include \
	-I/usr/local/include \
	-I/usr/local/include/thrift 
	
	#-I/usr/local/include/zookeeper \
	#-I/home/asdf/workspace/cc_dev/trunk/include

#	-I/home/asdf/downloads/gtest-1.7.0/include \
	#-I/usr/include/c++/4.4.4 \
	-I/usr/include/c++/4.4.4/backward \
	-I/usr/include/c++/4.4.4/x86_64-redhat-linux \
	-I/usr/lib/gcc/x86_64-redhat-linux/4.4.4/include \

LIBS    := thrift thriftnb event pthread zookeeper_mt util_time util_config ganji_util_log ganji_util_thread ganji_util_thread_log
# thriftnb
#THREADED used in zk cli_mt
CFLAGS= -D_LINUX_ -D_GNU_SOURCE -DTHREADED 
#-Wall #c++11# -std=c++0x

TARGET1=frproxy
TARGET2=testproxy

all: clean preexec $(TARGET1) $(TARGET2) afterexec
	echo '---------------all done ---------------'
$(TARGET1): $(TARGET1).o
	test -z $(BIN_PATH) || $(MKDIR) -- $(BIN_PATH)
	
	$(CC) $(CFLAGS) $(CPPFLAGS) $(addprefix -l,$(LIBS)) $(addprefix -L,$(LIB_PATH)) $(LDFLAGS) \
		proxy_constants.o \
		proxy_types.o \
		RegistryProxy.o \
		Registry.o \
		RegistryCache.o \
		ZkClient.o \
		RequestPool.o \
		ClientPool.o \
		ServerHandler.o \
		frproxy.o \
		-o $(BIN_PATH)/$(TARGET1)
		
	#g++ -lthrift -L/usr/local/lib server.o RegistryProxy.o proxy_constants.o proxy_types.o -o server 
	echo '---------------link done ---------------'
		 
$(TARGET1).o: #$(OBJS) 
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDES) -c thrift/proxy_constants.cpp -o proxy_constants.o 
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDES) -c thrift/proxy_types.cpp -o proxy_types.o	
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDES) -c thrift/RegistryProxy.cpp -o RegistryProxy.o

	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDES) -c core/Registry.cpp -o Registry.o
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDES) -c core/RegistryCache.cpp -o RegistryCache.o
	
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDES) -c core/ZkClient.cpp -o ZkClient.o
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDES) -c core/RequestPool.cpp -o RequestPool.o
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDES) -c core/ClientPool.cpp -o ClientPool.o
	
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDES) -c core/ServerHandler.cpp -o ServerHandler.o 
	
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDES) -c test/main.cpp -o test.o

	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDES) -c main.cpp -o frproxy.o
	echo ---------------compile done ---------------
	
$(TARGET2): #$(OBJS)
#	ar r $(TARGET2) client/frproxy_client.o
#	ranlib $(TARGET2)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(addprefix -l,$(LIBS)) $(addprefix -L,$(LIB_PATH)) $(LDFLAGS) \
		proxy_constants.o \
		proxy_types.o \
		RegistryProxy.o \
		Registry.o \
		RegistryCache.o \
		ZkClient.o \
		RequestPool.o \
		ClientPool.o \
		ServerHandler.o \
		test.o \
		-o $(BIN_PATH)/$(TARGET2)
		
	#g++ -lthrift -L/usr/local/lib server.o RegistryProxy.o proxy_constants.o proxy_types.o -o server 
	echo '---------------link done ---------------'


.SUFFIXES: .o .cc
.cc.o:
	$(CC) -c $(INCLUDES) $< -o $@

.PHONY: preexec
preexec:
# gen thrift file
#	make -C thrift
#	$(MKDIR) $(INCLUDE_PATH)/ganji/$(MODNAME)
#	$(MKDIR) $(BIN_PATH)/$(MODNAME)
#	$(CP)  *.h client/*.h $(INCLUDE_PATH)/ganji/$(MODNAME)

.PHONY: afterexec
afterexec:
	$(RM) *~ *.swp

.PHONY: thrift 
thrift:
	make -C thrift
	
.PHONY: clean
clean:
	#make clean -C thrift
	$(RM) *.o $(BIN_PATH)/$(TARGET1) $(BIN_PATH)/$(TARGET2)
	
install:
	#echo "not support now."
	test -z $(INSTALL_PATH) || test -d $(INSTALL_PATH) || $(MKDIR) -- $(INSTALL_PATH)
	install -m 755 $(BIN_PATH)/frproxy $(INSTALL_PATH)/frproxy
	
