#!/bin/sh
# $XConsortium: xon.sh,v 1.10 94/12/09 22:53:55 gildea Exp $
# start up xterm (or any other X command) on the specified host
# Usage: xon host [arguments] [command]
case $# in
0)
	echo "Usage: $0 <hostname> [-user user] [-name window-name] [-debug]"
	echo "[-screen screen-number] [command ...]"
	exit 1
	;;
esac
target=$1
shift
label=$target
resource=xterm-$label
if [ -f /usr/bin/remsh ]; then
    rsh=/usr/bin/remsh
elif [ -f /usr/bin/rcmd ]; then
    rsh=/usr/bin/rcmd
else
    rsh=rsh
fi
rcmd="$rsh $target -n"
case $DISPLAY in
unix:*)
	DISPLAY=`echo $DISPLAY | sed 's/unix//'`
	;;
esac
case $DISPLAY in
:*)
	case `uname` in
	Linux*)
		fullname=`hostname -f`
		;;
	*)
		fullname=`hostname`
		;;
	esac
	hostname=`echo $fullname | sed 's/\..*$//'`
	if [ $hostname = $target ] || [ $fullname = $target ]; then
		DISPLAY=$DISPLAY
		rcmd="sh -c"
	else
		DISPLAY=$fullname$DISPLAY
	fi
	;;
esac
username=
sess_mangr=
xauth=
case x$XUSERFILESEARCHPATH in
x)
	xpath='HOME=${HOME-`pwd`} '
	;;
*)
	xpath='HOME=${HOME-`pwd`} XUSERFILESEARCHPATH=${XUSERFILESEARCHPATH-"'"$XUSERFILESEARCHPATH"'"} '
	;;
esac
redirect=" < /dev/null > /dev/null 2>&1 &"
command=
ls=-ls
continue=:
while $continue; do
	case $1 in
	-user)
		shift
		username="-l $1"
		label="$target $1"
		rcmd="$rsh $target $username -n"
		shift
		case x$XAUTHORITY in
		x)
			XAUTHORITY="$HOME/.Xauthority"
			;;
		esac
		case x$XUSERFILESEARCHPATH in
		x)
			;;
		*)
			xpath="XUSERFILESEARCHPATH=$XUSERFILESEARCHPATH "
			;;
		esac
		;;
	-access)
		shift
		xhost +$target
		;;
	-name)
		shift
		label="$1"
		resource="$1"
		shift
		;;
	-nols)
		shift
		ls=
		;;
	-debug)
		shift
		redirect=
		;;
	-screen)
		shift
		DISPLAY=`echo $DISPLAY | sed 's/:\\([0-9][0-9]*\\)\\.[0-9]/:\1/'`.$1
		shift
		;;
	*)
		continue=false
		;;
	esac
done
case x$XAUTHORITY in
x)
	;;
x*)
	xauth="XAUTHORITY=$XAUTHORITY "
	;;
esac
case x$SESSION_MANAGER in
x)
	;;
x*)
	sess_mangr="SESSION_MANAGER=$SESSION_MANAGER "
	;;
esac
vars="$xpath$xauth$sess_mangr"DISPLAY="$DISPLAY"
case $# in
0)
	$rcmd 'sh -c '"'$vars"' xterm '$ls' -name "'"$resource"'" -T "'"$label"'" -n "'"$label"'" '"$redirect'"
	;;
*)
	$rcmd 'sh -c '"'$vars"' '"$*$redirect'"
	;;
esac
