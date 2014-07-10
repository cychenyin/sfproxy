#!/bin/bash
cd "$(dirname "$0")"
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.
logdir=/data/log/frproxy
test -d ${logdir}/ || mkdir -p -- ${logdir}/
test -d ${logdir}/ || logdir=/tmp
echo $logdir

if [[ "$*" == "-h" || "$*" == "--help" ]] ; then
	./frproxy -h
else 
    if [[ "$*" == "-v" || "$*" == "--version" ]] ; then
	./frproxy -v 
    else
	nohup ./frproxy $* >>${logdir}/frproxy.stdout 2>&1 &
    fi
fi
