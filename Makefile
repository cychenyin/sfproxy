MODNAME=frproxy

BIN_PATH=./bin
INCLUDE_PATH=.

LIB_PATH= /usr/local/lib \
	/home/asdf/downloads/gtest-1.7.0/lib
	
#LIB_GTEST= -lpthread

CP=/bin/cp -f
RM=/bin/rm -f
MV=/bin/mv -f
MKDIR=/bin/mkdir -p
#CC=g++ -O2
CC=g++ -ggdb

INCLUDES= \
	-I/usr/include \
	-I/usr/local/include \
	-I/usr/local/include/thrift \
	-I/usr/local/include/zookeeper \
	-I/usr/local/include/jsoncpp \
	-I/home/asdf/downloads/gtest-1.7.0/include
	
	#-I/usr/include/c++/4.4.4 \
	-I/usr/include/c++/4.4.4/backward \
	-I/usr/include/c++/4.4.4/x86_64-redhat-linux \
	-I/usr/lib/gcc/x86_64-redhat-linux/4.4.4/include \

LIBS    := thrift pthread zookeeper_mt json_linux-gcc-4.4.7_libmt
# thriftnb
CFLAGS=-D_LINUX_ -DTHREADED

TARGET1=frproxy
TARGET2=testproxy

all: clean preexec $(TARGET1) afterexec
	echo '---------------all done ---------------'
$(TARGET1): $(TARGET1).o
	test -z $(BIN_PATH) || $(MKDIR) -- $(BIN_PATH)
	
	$(CC) $(CFLAGS) $(addprefix -l,$(LIBS)) $(addprefix -L,$(LIB_PATH)) \
		frproxy.o \
		Registry.o \
		RegistryCache.o \
		ZkClient.o \
		RegistryCacheTest.o \
		ZkClientTest.o \
		-o $(BIN_PATH)/$(TARGET1)
		
	#g++ -lthrift -L/usr/local/lib server.o RegistryProxy.o proxy_constants.o proxy_types.o -o server 
	echo '---------------link done ---------------'
		 
$(TARGET1).o: $(OBJS) 
	$(CC) -Wall $(INCLUDES) -c core/Registry.cpp -o Registry.o
	$(CC) -Wall $(INCLUDES) -c core/RegistryCache.cpp -o RegistryCache.o
	$(CC) -Wall $(INCLUDES) -c core/ZkClient.cpp -o ZkClient.o
	$(CC) -Wall $(INCLUDES) -c test/RegistryCacheTest.cpp -o RegistryCacheTest.o
	$(CC) -Wall $(INCLUDES) -c test/ZkClientTest.cpp -o ZkClientTest.o

	$(CC) -Wall $(INCLUDES) -c main.cpp -o frproxy.o
	echo ---------------compile done ---------------
	
$(TARGET2): $(OBJS)
#	ar r $(TARGET2) client/frproxy_client.o
#	ranlib $(TARGET2)

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
	make clean -C thrift
	$(RM) *.o $(BIN_PATH)/$(TARGET1) $(BIN_PATH)/$(TARGET2)
	
