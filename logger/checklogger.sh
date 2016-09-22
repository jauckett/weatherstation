#!/bin/bash

P=`ps ax | grep python | grep weather`

if [ -z "$P" ] ; then
    echo "Logger not running"
else
    echo "Logger is running"

fi

echo $P
