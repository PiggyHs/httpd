#!/bin/bash

ROOT=$(pwd)

if [ -z $LD_LIBRARY_PATH ];then
	export LD_LIBRARY_PATH=$ROOT/lib/lib
fi

./httpd 0 80
