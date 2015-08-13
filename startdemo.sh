#!/bin/bash


adb shell 'cd /data/local/tmp; sh init.sh;'

adb install -r app.apk

echo -e "starting testing app"

adb shell "am start -W -n org.sid.testingmalwareapp/.MainActivity"


MYPID=`adb shell ps | grep testing | awk '{print $2}'`


MYUID=`adb shell ps | grep testing | awk '{print $1}'`

echo "target appliation pid: $MYPID and $MYUID"

adb shell chown $MYUID:$MYUID /data/local/tmp/dex/opt

adb shell "cd /data/local/tmp; "

sleep 5

echo -e "starting hooking...."

adb shell "cd /data/local/tmp; sh runhijack.sh -t $MYPID"
 

