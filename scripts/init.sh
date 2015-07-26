#!/bin/sh

echo "this script must be runned on device or emulator !!"

mkdir dex
mkdir dex/opt
cp classes.dex dex/target.dex


#creating log files
touch arm_arthook.log
touch arthook.log
touch adbi.log

chmod -R 777 *


echo "Please, remember to chown the \"dex/opt\" dir with the UID of the target application!"
