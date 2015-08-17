#!/bin/bash

echo "checking if checkjni is enabled..."

RES=`adb shell 'getprop' | grep 'checkjni' | cut -d : -f 2`

if [ -z $RES ]; then
    echo "checkjni is mandatory on real-world devices..."
    echo "it will be auto-enabled for you..."
    adb shell stop
    adb shell setprop dalvik.vm.checkjni true
    adb shell start
    echo "checkjni activated!! "
fi

if [ -n $RES ]; then
    echo "ok, checkjni is already enabled!"
    echo "moving on.."
fi

adb wait-for-device

sleep 1

adb shell 'cd /data/local/tmp; sh init.sh;'

adb install -r examples/app-testing.apk

echo -e "starting testing app"


# -D for wait debugger, problem with classloader..
adb shell "am start -W -n org.sid.testingmalwareapp/.MainActivity"


MYPID=`adb shell ps | grep testing | awk '{print $2}'`


MYUID=`adb shell ps | grep testing | awk '{print $1}'`

echo "target appliation pid: $MYPID and $MYUID"

adb shell chown $MYUID:$MYUID /data/local/tmp/dex/opt

adb shell "cd /data/local/tmp; "

sleep 2 

echo -e "starting hooking...."

adb shell "cd /data/local/tmp; sh runhijack.sh -t $MYPID"
 

