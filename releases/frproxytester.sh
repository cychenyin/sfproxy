#!/bin/bash 

echo "==============start stop"
./frproxy.sh start
echo $?
./frproxy.sh stop
echo $?
echo "==============start restart stop"
./frproxy.sh start
echo $?
./frproxy.sh restart
echo $?
./frproxy.sh stop
echo $?
echo "==============restart start stop"
./frproxy.sh restart
echo $?
./frproxy.sh start
echo $?
./frproxy.sh stop
echo $?
echo "==============restart stop "
./frproxy.sh restart
echo $?
./frproxy.sh stop
echo $?
echo "==============start start stop"
./frproxy.sh start
echo $?
./frproxy.sh start
echo $?
./frproxy.sh stop
echo $?

echo "==============stop stop" 
./frproxy.sh stop 
echo $?
./frproxy.sh stop
echo $?
echo "==============stop start stop" 
./frproxy.sh stop 
echo $?
./frproxy.sh start
echo $?
./frproxy.sh stop
echo $?


