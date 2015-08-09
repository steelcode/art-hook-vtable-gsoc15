#!/bin/bash


DIR=`pwd`

IS_FIRST_RUN=false

ARTHOOK_SO="$DIR/hooking/libs/armeabi/libarthook.so"

HIJACK_BIN="$DIR/hijack/libs/armeabi/hijack"


usage(){
cat << EOF

usage:
EOF

}

check_ret () {
    if [ $? -eq 1 ]; then
        print_error_and_exit "error in adb command \n"
    fi
}

print_error_and_exit()
{
    echo $1
    exit 1
}

check_dependencies()
{
    printf "checking dependencies...\n"
    path_to_executable=$(which adb)
    if [ -x "$path_to_executable" ] ; then
        printf "It's here: $path_to_executable \n"
    else
        print_error_and_exit "please, add Android SDK to your PATH \n"
    fi
    path_to_ndk=$(which ndk-build)
    if [ -x "$path_to_ndk" ] ; then
        printf "ok, founded NDK at $path_to_ndk"
    else
        print_error_and_exit "please, add Android NDK to your PATH \n"
    fi
}

check_first_run()
{
    printf "check \n"
    if [ ! -f "$DIR/.first_run.no" ] ; then
        printf "this is the first run, adbi will be compiled too \n"
        touch "$DIR/.first_run.no"
        IS_FIRST_RUN=true
    else
        printf "this is not the first run! \n"
    fi
}

compile_all()
{
    cd $DIR
    if [ "$IS_FIRST_RUN" = true ] ; then
       cd adbi/
       #sh clean.sh
       sh build.sh
       IS_FIRST_RUN=false
    fi
    
    cd $DIR

    #sh clean.sh
    sh build.sh
    
    printf "\n ALL DONE ! \n"    
}

push_file()
{
    echo "\t pushing: $1 \n"
    adb push $1 /data/local/tmp
    check_ret
    echo "\t ok..."
}

push_to_device()
{
   if [ -f "$ARTHOOK_SO" ] ; then
      push_file $ARTHOOK_SO
      if [ -f "$HIJACK_BIN" ] ; then
          push_file $HIJACK_BIN
      else
          print_error_and_exit "can't found $HIJACK_BIN, please check adbi compilation results\n"
      fi
   else
      print_error_and_exit "can't found $ARTHOOK_SO, please check arthook ompilation results\n"
   fi 
    
}

main()
{
    check_dependencies
    check_first_run
    compile_all
    push_to_device

    sh pusher.sh
    
}

main
