XCOMM!/bin/sh

XCOMM $XConsortium: startx.cpp,v 1.4 91/08/22 11:41:29 rws Exp $
XCOMM $XFree86: xc/programs/xinit/startx.cpp,v 3.0 1994/05/22 00:02:28 dawes Exp $
XCOMM 
XCOMM This is just a sample implementation of a slightly less primitive 
XCOMM interface than xinit.  It looks for user .xinitrc and .xserverrc
XCOMM files, then system xinitrc and xserverrc files, else lets xinit choose
XCOMM its default.  The system xinitrc should probably do things like check
XCOMM for .Xresources files and merge them in, startup up a window manager,
XCOMM and pop a clock and serveral xterms.
XCOMM
XCOMM Site administrators are STRONGLY urged to write nicer versions.
XCOMM 

#ifdef SCO

XCOMM Check for /usr/bin/X11 and BINDIR in the path, if not add them.
XCOMM This allows startx to be placed in a place like /usr/bin or /usr/local/bin
XCOMM and people may use X without changing their PATH

XCOMM First our compiled path

bindir=BINDIR
if expr $PATH : ".*`echo $bindir | sed 's?/?\\/?g'`.*" > /dev/null 2>&1; then
	:
else
	PATH=$PATH:BINDIR
fi

XCOMM Now the "SCO" compiled path

if expr $PATH : '.*\/usr\/bin\/X11.*' > /dev/null 2>&1; then
	:
else
	PATH=$PATH:/usr/bin/X11
fi

XCOMM Set up the XMERGE env var so that dos merge is happy under X

if [ -f /usr/lib/merge/xmergeset.sh ]; then
	. /usr/lib/merge/xmergeset.sh
else if [ -f /usr/lib/merge/console.disp ]; then
	XMERGE=`cat /usr/lib/merge/console.disp`
	export XMERGE
fi
fi

scoclientrc=$HOME/.startxrc
#endif

userclientrc=$HOME/.xinitrc
userserverrc=$HOME/.xserverrc
sysclientrc=XINITDIR/xinitrc
sysserverrc=XINITDIR/xserverrc
clientargs=""
serverargs=""

#ifdef SCO
if [ -f $scoclientrc ]; then
    clientargs=$scoclientrc
else
#endif
if [ -f $userclientrc ]; then
    clientargs=$userclientrc
else if [ -f $sysclientrc ]; then
    clientargs=$sysclientrc
fi
fi
#ifdef SCO
fi
#endif

if [ -f $userserverrc ]; then
    serverargs=$userserverrc
else if [ -f $sysserverrc ]; then
    serverargs=$sysserverrc
fi
fi

whoseargs="client"
while [ "x$1" != "x" ]; do
    case "$1" in
	/''*|\.*)	if [ "$whoseargs" = "client" ]; then
		    clientargs="$1"
		else
		    serverargs="$1"
		fi ;;
	--)	whoseargs="server" ;;
	*)	if [ "$whoseargs" = "client" ]; then
		    clientargs="$clientargs $1"
		else
		    serverargs="$serverargs $1"
		fi ;;
    esac
    shift
done

xinit $clientargs -- $serverargs

/*
 * various machines need special cleaning up
 */
#ifdef macII
Xrepair
screenrestore
#endif

#ifdef sun
kbd_mode -a
#endif
