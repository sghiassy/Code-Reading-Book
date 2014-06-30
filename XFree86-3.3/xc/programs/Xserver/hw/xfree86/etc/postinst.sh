#!/bin/sh

# $XFree86: xc/programs/Xserver/hw/xfree86/etc/postinst.sh,v 3.13.2.3 1997/05/27 09:13:04 dawes Exp $
#
# postinst.sh (for XFree86 3.3)
#
# This script should be run after installing a new version of XFree86.
#

RUNDIR=/usr/X11R6

if [ ! -d $RUNDIR/. ]; then
	echo $RUNDIR does not exist
	exit 1
fi

# Since the misc fonts are distributed in two parts, make sure that the
# fonts.dir file is correct if only one part has been installed.
if [ -d $RUNDIR/lib/X11/fonts/misc ]; then
	echo ""
	echo "Updating the fonts.dir file in $RUNDIR/lib/X11/fonts/misc"
	echo "This might take a while ..."
	$RUNDIR/bin/mkfontdir $RUNDIR/lib/X11/fonts/misc
fi

# Check if the system has a termcap file
TERMCAP1DIR=/usr/share
TERMCAP2=/etc/termcap
if [ -d $TERMCAP1DIR ]; then
	TERMCAP1=`find $TERMCAP1DIR -type f -name termcap -print 2> /dev/null`
	if [ x"$TERMCAP1" != x ]; then
		TERMCAPFILE="$TERMCAP1"
	fi
fi
if [ x"$TERMCAPFILE" = x ]; then
	if [ -f $TERMCAP2 ]; then
		TERMCAPFILE="$TERMCAP2"
	fi
fi
if [ x"$TERMCAPFILE" != x ]; then
	echo ""
	echo "You appear to have a termcap file: $TERMCAPFILE"
	echo "This should be edited manually to replace the xterm entries"
	echo "with those in $RUNDIR/lib/X11/etc/xterm.termcap"
	echo ""
	echo "Note: the new xterm entries are required to take full advantage"
	echo "of new features, but they may cause problems when used with"
	echo "older versions of xterm.  A terminal type 'xterm-r6' is included"
	echo "for compatibility with the standard X11R6 version of xterm."
fi

# Check for terminfo, and update the xterm entry
TINFODIR=/usr/lib/terminfo
OLDTINFO=" \
	x/xterm \
	x/xterms \
	x/xterm-24 \
	x/xterm-vi \
	x/xterm-65 \
	x/xterm-bold \
	x/xtermm \
	x/xterm-boldso \
	x/xterm-ic \
	x/xterm-r6 \
	x/xterm-old \
	x/xterm-r5 \
	v/vs100"
	
if [ -d $TINFODIR ]; then
	echo ""
	echo "You appear to have a terminfo directory: $TINFODIR"
	echo "New xterm terminfo entries can be installed now."
	echo ""
	echo "Note: the new xterm entries are required to take full advantage"
	echo "of new features, but they may cause problems when used with"
	echo "older versions of xterm.  A terminal type 'xterm-r6' is included"
	echo "for compatibility with the standard X11R6 version of xterm."
	echo ""
	echo "Do you wish to have the new xterm terminfo entries installed now (y/n)?"
	read Resp
	case "$Resp" in
	[yY]*)
		echo ""
		for t in $OLDTINFO; do
			if [ -f $TINFODIR/$t ]; then
				echo "Moving old terminfo file $TINFODIR/$t to $TINFODIR/$t.bak"
				rm -f $TINFODIR/$t.bak
				mv -f $TINFODIR/$t $TINFODIR/$t.bak
			fi
		done
		echo ""
		echo "Installing new terminfo entries for xterm."
		echo ""
		echo "On some systems you may get warnings from tic about 'meml'"
		echo "and 'memu'.  These warnings can safely be ignored."
		echo ""
		tic /usr/X11R6/lib/X11/etc/xterm.terminfo
		;;
	*)
		echo ""
		echo "Not installing new terminfo entries for xterm."
		echo "They can be installed later by running:"
		echo ""
		echo "  tic /usr/X11R6/lib/X11/etc/xterm.terminfo"
		;;
	esac
fi

if [ -f /usr/X11R6/bin/rstartd ]; then
	echo ""
	echo "If you are going to use rstart and /usr/X11R6/bin isn't in the"
	echo "default path for commands run remotely via rsh, you will need"
	echo "a link to rstartd installed in /usr/bin."
	echo ""
	echo "Do you wish to have this link installed (y/n)?"
	read Resp
	case "$Resp" in
	[yY]*)
		echo "Creating link from /usr/X11R6/bin/rstartd to /usr/bin/rstartd"
		rm -f /usr/bin/rstartd
		ln -s /usr/X11R6/bin/rstartd /usr/bin/rstartd
		;;
	esac
fi

case `uname` in
	FreeBSD|NetBSD|OpenBSD)
		echo ""
		echo "Running ldconfig"
		/sbin/ldconfig -m /usr/X11R6/lib
		;;
	Linux)
		echo ""
		echo "Running ldconfig"
		/sbin/ldconfig /usr/X11R6/lib
		;;
esac

exit 0
