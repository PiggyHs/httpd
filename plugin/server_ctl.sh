#!/bin/bash

ROOT=$(pwd)
CONF=$ROOT/conf/server.conf
bin=httpd
pid=''
ip=$(awk -F: '/^IP/{print $NF}' $CONF)
port=$(awk -F: '/^PORT/{print $NF}' $CONF)

if [ -z $LD_LIBRARY_PATH ];then
	export LD_LIBRARY_PATH=$ROOT/lib/lib
fi

proc=$(basename $0)
function usage(){
	printf "Usage:\n\t%s [start(-s)|stop(-t)|restart(-r)|status(-st)]\n\n" "$proc"
}

if [ $# -ne 1 ];then
	usage
	exit 1
fi

function is_exist(){
	pid=`pidof $bin`
	if [ -z "$pid" ];then
		return 1
	else
		return 0
	fi
}

function start_server(){
	if is_exist; then
		echo "server is running, pid : $pid"
	else
		./$bin $ip $port
		echo "server start ... done"
	fi
}

function stop_server(){
	if is_exist; then
		kill -9 $pid
		echo "server stop ... done"
	else
		echo "server is not running."
	fi
}

function restart_server(){
	stop_server
	start_server
}

function status_server(){
	if is_exist; then
		echo "server is ok, pid: $pid"
	else
		echo "server is not exist, please start frist."
	fi
}

case $1 in
	start | -s)
		start_server
	;;
	stop | -t )
		stop_server
	;;
	restart | -r )
		restart_server
	;;
	status | -st )
		status_server
	;;
	* )
	usage
	exit 2
	;;
esac


