#!/bin/bash
cd "$(dirname "$0")"
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.
logdir=/data/log/frproxy
logdir=/tmp
test -d ${logdir}/ || mkdir -p -- ${logdir}/
if [[ "$*" == "-h" || "$*" == "--help" ]] ; then
	./frproxy -h
else
	nohup ./frproxy $* >>${logdir}/frproxy.stdout 2>&1 &
fi

