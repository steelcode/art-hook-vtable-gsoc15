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

# TODO: check if adb was started as root
# alla bruttodio

RES=`adb root`

status=$?


if [ $status -ne 0 ]; then
    echo ""
    exit 1
elif [[ $RES == *"disabled"* ]]; then
    echo "adb root disabled, error message: $RES"
    echo "please, this demo requires adb running as root!"
    exit 1
fi


adb wait-for-device

sleep 1

adb shell 'cd /data/local/tmp; sh init.sh;'

adb install -r ../examples/testing-app.apk

echo -e "starting testing app"


# -D for wait debugger, problem with classloader..
adb shell "am start -W -n org.sid.testingmalwareapp/.MainActivity"


MYPID=`adb shell ps | grep testing | awk '{print $2}'`


MYUID=`adb shell ps | grep testing | awk '{print $1}'`

echo "target appliation pid: $MYPID and $MYUID"

adb shell chown $MYUID:$MYUID /data/local/tmp/dex/opt


echo -e "starting hooking...."

adb shell "cd /data/local/tmp; sh runhijack.sh -t $MYPID"
 

