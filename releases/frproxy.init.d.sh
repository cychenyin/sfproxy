#!/bin/bash
##This file should be used to construct scripts to be placed in /etc/init.d

PROG="frproxy"
LOG_PATH=/data/log/frproxy
#LOG_PATH=/home/asdf/temp
PROG_PATH="/usr/local/frproxy"
PROG_ARGS="-l" 
test -d $LOG_PATH || LOG_PATH=/tmp

rawstart() {
	cd $PROG_PATH
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.
    $PROG_PATH/$PROG $PROG_ARGS >>${LOG_PATH}/frproxy.stdout 2>&1 &
	sleep 3

	found=0
	pids=$(ps -ef | grep "$PROG" | grep -v "grep" | awk '{print $2}' )
	for pid in $pids;
	do
	    if [[ -f "/proc/$pid/exe" ]]; then
    		fullpath=$(ls -l "/proc/$pid/exe" | awk -F" -> " '{print $2}')
    		if [[ "$PROG_PATH/$PROG" == $fullpath ]]; then
    		    found=1
    		    #echo "$pid" 1>&2
    		    break
    		fi;
	    fi
	done;
	if [[ $found -eq 1 ]]; then
	    echo "$PROG started. pid is $pid" 1>&2
	    touch "$LOG_PATH/$PROG.pid"
	else
	    echo "fail to start $PROG" 1>&2
	    return 1
	fi
    return 0
}

stop() {
    pids=$(ps -ef | grep "$PROG" | grep -v "frproxy.init.d.sh" | grep -v "frproxytester.sh" | grep -v "grep" | awk '{print $2}' )
    for pid in $pids;
    do
		if [[ "$pid" != "$$" ]]; then
            kill -9 $pid >>/dev/null 2>&1 &
		fi
    done;

    if [ -e "$LOG_PATH/$PROG.pid" ]; then
        rm -f "$LOG_PATH/$PROG.pid"
        echo "$PROG stopped"
    else
        ## Program is not running, exit with error.
        echo "no /etc/init.d/$PROG running" 1>&2
        return 1
    fi
    return 0
}

status() {
	found=0
	pids=$(ps -ef | grep "$PROG" | grep -v "grep" | awk '{print $2}' )
    #set -x
	for pid in $pids;
	do
	    if [[ -f "/proc/$pid/exe" ]]; then
    		fullpath=$(ls -l "/proc/$pid/exe" | awk -F" -> " '{print $2}')
    		if [[ "$PROG_PATH/$PROG" == $fullpath ]]; then
    		    found=1
    		    break
    		fi;
	    fi
	done;

    if [ -e "$LOG_PATH/$PROG.pid" ]; then
        found=$(($found + 1))
    fi

    if [[ $found -eq 2 ]]; then
        return 0
    else
        return 1
    fi
}

start() {
    if [ -e "$LOG_PATH/$PROG.pid" ]; then
        ## Program is running, exit with error.
        echo "Error! $PROG is currently running!" 1>&2
        return 1
    else
	    rawstart
        return $?
    fi
}


## Check to see if we are running as root first.
#if [ "$(id -u)" != "0" ]; then
#    echo "This script must be run as root" 1>&2
#    exit 1
#fi
code=0
case "$1" in
    start)
        start
        code=$?
    ;;
    stop)
        stop
        code=$?
    ;;
    reload|restart|force-reload)
        stop
        start
        code=$?
    ;;
    status)
        status
        code=$?
    ;;
    **)
        echo "Usage: $0 {start|stop|restart}" 1>&2
        code=3 
    ;;
esac
exit $code
