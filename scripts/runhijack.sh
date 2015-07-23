#!/system/xbin/bash

#esegue inject
#-p -l  -d -z -s 
BIN=/data/local/tmp/hijack 
PIDZYG=`ps | grep zygote  | awk '{print $2}'`
LIB=/data/local/tmp/libarthook.so
ZYG=true

function help ()
{
	echo 'HELP:'
	echo 'di default injecta nello zygote, per specificare il package name:'
	echo "$0 -s package_name"
	echo ""
	echo 'per injectare in uno specifico processo eseguire:'
	echo "$0 -t <pid>" 
	echo ""
}

function test
{
	"$@"
	local status=$?
	if [ $status -ne 0 ]; then
		echo "errore con $1" >&2
	fi
	return $status
}


if [ $# == 0 ]; then
	help
	exit 0
fi

while getopts t:s: option
do
	case "${option}"
		in
			t)
				ZYG=false
				PIDT=${OPTARG}
				;;
			s)
				PCKN=$OPTARG
				;;
	esac
done

#todo: check size
#if [ -e "strmon.log" ];
#then
#	echo "logrotate"
#	./logrotate.sh
#else
#	echo "creo file di log"
#	touch strmon.log; chmod 777 strmon.log;
#fi
#h collectinfo.sh
#echo "num ags: $#"
#for i in $*
#do
if [ "$ZYG" = true ] ; then
	echo "hijack su zygote, pck name: $PCKN"
	$BIN -p $PIDZYG -l $LIB  -z -s $PCKN &
	echo "eseguito"
else
	echo "hijack su pid: $PIDT"
	test $BIN -p $PIDT -l $LIB
	echo "eseguito"
fi
#done

