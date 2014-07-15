#!/bin/bash 
./frproxy.sh stop
echo "==============start stop;"
./frproxy.sh start
echo "$? expect 0"
./frproxy.sh stop
echo "$? expect 0"
echo "==============start restart stop"
./frproxy.sh start
echo "$? expect 0"
./frproxy.sh restart
echo "$? expect 0"
./frproxy.sh stop
echo "$? expect 0"
echo "==============restart start stop"
./frproxy.sh restart
echo "$? expect 0"
./frproxy.sh start
echo "$? expect 1"
./frproxy.sh stop
echo "$? expect 0"
echo "==============restart stop "
./frproxy.sh restart
echo "$? expect 0"
./frproxy.sh stop
echo "$? expect 0"
echo "==============start start stop"
./frproxy.sh start
echo "$? expect 0"
./frproxy.sh start
echo "$? expect 1"
./frproxy.sh stop
echo "$? expect 0"

echo "==============stop stop" 
./frproxy.sh stop 
echo "$? expect 1"
./frproxy.sh stop
echo "$? expect 1"
echo "==============stop start stop" 
./frproxy.sh stop 
echo "$? expect 1"
./frproxy.sh start
echo "$? expect 0"
./frproxy.sh stop
echo "$? expect 1"


