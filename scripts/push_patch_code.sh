#!/bin/bash

TARGET_DIR=""

usage(){
cat << EOF

usage:
    -h "print this help"    
    -t "specify patch code to push on device [apk || dex ]"

EOF

print_error_and_exit "please, check arguments! \n"
}

check_format(){
    echo -e "check_format"
    if [ ${t: -4} == ".apk" ] ; then
        echo -e "apk file!!\n"
        push_to_device "apk"
    elif [ ${t: -4} == ".dex" ] ; then
        echo -e "dex file!!\n"
        push_to_device "dex"
    else
        print_error_and_exit "please, extension error. accepted file are: .apk and .dex "
    fi 
}

print_error_and_exit()
{
    echo -e $1
    exit 1
}



    
if [ $# -eq 0 ]; then
usage
fi

while getopts ":ht:" opzione
do
    case "$opzione" in 
        h )
            usage
            ;;
        t )
            echo -e "case t\n "
            t=${OPTARG}
            TARGET_DIR=${OPTARG}
            ;;
        * )
            echo "case all"
            usage
            ;;
    esac
done
shift $((OPTIND-1))

if [ -z "${t}" ]; then
    usage
    print_error_and_exit "please, specify a type \n"
fi



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


push_file()
{
    echo  -e "\t pushing: $1 \n"
    adb push $1 /data/local/tmp
    check_ret
    echo -e "\t ok..."
}

push_to_device()
{
    echo "pushing type: $1"
    echo -e "target file name: $t"
    echo -e "target file dir: $TARGET_DIR"

    case  "$1" in 
        apk )
            echo "pusho apk"
            # need to extract dex from apk
            rm -fr /tmp/asd
            mkdir /tmp/asd
            check_ret
            unzip -o -x $TARGET_DIR -d /tmp/asd
            check_ret
            push_file "/tmp/asd/classes.dex"
            ;;
        dex )
            echo "pusho dex"
            push_file $t
            ;;
        * ) 
            echo "error"
            ;;
    esac
}

finish()
{
    echo -e "ALL DONE !!!\n"
}

main()
{
    echo  -e "main \n"
    check_format

    finish
}


main
