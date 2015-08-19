#!/bin/sh

echo -e "this script must be runned on device or emulator !!"

check_ret () {
    if [ $? -gt 0 ]; then
        print_error_and_exit "error in command \n"
    fi
}

print_error_and_exit()
{
    echo -e $1
    exit 1
}


if [ ! -d "dex" ]; then
    mkdir dex
    check_ret
fi
if [ ! -d "dex/opt" ]; then
    mkdir dex/opt
    check_ret
fi

if [ ! -f "classes.dex" ]; then
    print_error_and_exit "can't find classes.dex, please copy it to `pwd`\n"
fi

cp classes.dex dex/target.dex
check_ret

#creating log files
touch arm_arthook.log
touch arthook.log
touch adbi.log

chmod -R 777 *


echo -e "ALL DONE!! Now you can inject the arthook .so into the target process using the runhijack.sh script!!\n"

echo -e "Please, remember to chown the \"dex/opt\" dir with the \$UID of the target application!"
echo -e "run in this dir: chown \$UID:\$UID dex/opt\n"



