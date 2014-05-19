#!/bin/bash
##This file should be used to construct scripts to be placed in /etc/init.d

PROG="frproxy"
LOG_PATH=/data/log/frproxy
LOG_PATH=/home/asdf/temp
PROG_PATH="/usr/local/webserver/frproxy"
PROG_ARGS="-l" 

rawstart() {
	cd $PROG_PATH
        export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.
        $PROG_PATH/$PROG $PROG_ARGS >>${LOG_PATH}/frproxy.stdout 2>&1 &
	sleep 3

	found=0
	pids=$(ps -ef | grep frproxy | grep -v grep | awk '{print $2}' )
	for pid in $pids;
	do
	    if [[ -f "/proc/$pid/exe" ]]; then
		fullpath=$(ls -l "/proc/$pid/exe" | awk -F" -> " '{print $2}')
		if [[ "/usr/local/webserver/frproxy/frproxy" == $fullpath ]]; then
		    found=1
		    echo "$pid" 1>&2
		    break
		fi;
	    fi
	done;
	if [[ $found -eq 1 ]]; then
	    echo "$PROG started. pid is $pid" 1>&2
	    touch "$LOG_PATH/$PROG.pid"
	else
	    echo "fail to start $PROG" 1>&2
	    exit 1
	fi

}

start() {
    if [ -e "$LOG_PATH/$PROG.pid" ]; then
        ## Program is running, exit with error.
        echo "Error! $PROG is currently running!" 1>&2
        exit 1
    else
	rawstart
    fi
}

stop() {
    if [ -e "$LOG_PATH/$PROG.pid" ]; then
        ## Program is running, so stop it
        killall -q -w $PROG_PATH/$PROG
        rm -f "$LOG_PATH/$PROG.pid"
        
        echo "$PROG stopped"
    else
        ## Program is not running, exit with error.
        echo "Error: $PROG not started by /etc/init.d/$PROG!" 1>&2
        exit 1
    fi
}

restart() {
    killall -w -q $PROG_PATH/$PROG
    if [ -e "$LOG_PATH/$PROG.pid" ]; then
        rm -f "$LOG_PATH/$PROG.pid"
        echo "$PROG stopped"
    else
	echo "no $PROG running"
    fi

    if [ -e "$LOG_PATH/$PROG.pid" ]; then
        ## Program is running, exit with error.
        echo "Error: $PROG is currently running!" 1>&2
    else
	rawstart
    fi
}

## Check to see if we are running as root first.
#if [ "$(id -u)" != "0" ]; then
#    echo "This script must be run as root" 1>&2
#    exit 1
#fi

case "$1" in
    start)
        start
        exit 0
    ;;
    stop)
        stop
        exit 0
    ;;
    reload|restart|force-reload)
        restart
        exit 0
    ;;
    **)
        echo "Usage: $0 {start|stop|restart}" 1>&2
        exit 3
    ;;
esac
exit 0
