#!/bin/bash

OLDDIR=`pwd`

cd ../

DIR=`pwd`

HIJACK="$DIR/adbi/hijack/libs/armeabi/hijack"
ARTHOOK="$DIR/examples/arthook_demo/libs/armeabi/libarthookdemo.so"
DEXFILE="$DIR/examples/classes.dex"
SCRIPTDIR="$DIR/scripts/device"

DESTDIR="/data/local/tmp"

fail_msg () {
	echo ""
	echo "maybe lunching sh build.sh will solve the problems?"
	echo ""
	exit 1
}

# check if file exists
check_file () {
	if [ ! -f $HIJACK ]; then
		echo >&2 "$HIJACK not exist.  Aborting.";
		fail_msg
	fi
	if [ ! -f $ARTHOOK ]; then
		echo >&2 "$ARTHOOK not exist.  Aborting";
		fail_msg
	fi
	if [ ! -f $DEXFILE ]; then
		echo >&2 "$DEXFILE not exist . aborting";
		fail_msg;
	fi
}

check_ret () {
	if [ $? -eq 1 ]; then
		echo " error in adb command!!";
		exit 1
	fi
}
adb_push_files (){
	adb push $HIJACK $DESTDIR
	check_ret
	command adb  push $ARTHOOK $DESTDIR
	check_ret
	command adb  push $DEXFILE $DESTDIR
	check_ret

    for file in `ls $SCRIPTDIR/*`;
    do
        command adb push $file $DESTDIR
    done
}

check_adb () {
	type adb >/dev/null 2>&1 || { echo >&2 "I require adb but it's not in PATH.  Aborting."; exit 1; }
}



check_adb
check_file
adb_push_files

echo -e "\n"
echo -e "all files has been pushed to the device!"
echo -e "now connect to the device/emu, \"cd /data/local/tmp\" and \"sh ./init.sh\" \n"

cd $OLDDIR
