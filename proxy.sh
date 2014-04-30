#!/bin/bash
cd "$(dirname "$0")"
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.
logdir=/data/log/frproxy
#logdir=/home/asdf/log/frproxy
test -d ${logdir}/ || mkdir -p -- ${logdir}/

nohup ./frproxy $* >${logdir}/frproxy.stdout 2>&1 &

