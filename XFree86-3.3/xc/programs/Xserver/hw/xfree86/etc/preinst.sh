#!/bin/sh

# $XFree86: xc/programs/Xserver/hw/xfree86/etc/preinst.sh,v 3.8.2.2 1997/05/24 08:36:11 dawes Exp $
#
# preinst.sh  (for XFree86 3.3)
#
# This script should be run before installing a new version.
#
# It removes parts of an existing installation that can cause problems
# when extracting the new version.  This includes symbolic links to old
# beta versions, shared lib symlinks, and old files.
#
# $XConsortium: preinst.sh /main/5 1996/10/28 05:43:40 kaleb $
#

RUNDIR=/usr/X11R6
LIBLIST=" \
	libICE.so \
	libPEX5.so \
	libSM.so \
	libX11.so \
	libXIE.so \
	libXaw.so \
	libXext.so \
	libXi.so \
	libXmu.so \
	libXp.so \
	libXt.so \
	libXtst.so \
	liboldX.so \
	libICE.so.6 \
	libPEX5.so.6 \
	libSM.so.6 \
	libX11.so.6 \
	libXIE.so.6 \
	libXaw.so.6 \
	libXext.so.6 \
	libXi.so.6 \
	libXmu.so.6 \
	libXp.so.6 \
	libXt.so.6 \
	libXtst.so.6 \
	liboldX.so.6 \
	"

OLDFILES=" \
	lib/X11/doc/LbxproxyOnly \
	lib/X11/xkb/keycodes/sgi \
	lib/X11/xkb/symbols/de_nodead \
	"

# First, do some checks for Linux/ELF

if [ "`uname`" = Linux ]; then
	if file -L /bin/sh | grep ELF >/dev/null 2>&1; then
		echo ""
		echo "You appear to have an ELF system."
		echo "Make sure you are installing the ELF binary dist"
		# Check ldconfig
		LDSO=`/sbin/ldconfig -v -n | awk '{ print $3 }'`
		LDSOMIN=`echo $LDSO | awk -F. '{ print $3 }'`
		LDSOMID=`echo $LDSO | awk -F. '{ print $2 }'`
		LDSOMAJ=`echo $LDSO | awk -F. '{ print $1 }'`
		if [ "$LDSOMAJ" -gt 1 ]; then
			: OK
		else
			if [ "$LDSOMID" -gt 7 ]; then
				: OK
			else
				if [ "$LDSOMIN" -ge 14 ]; then
					: OK
				else
					echo ""
					echo "Before continuing you will need to get a current version of ld.so."
					echo "Version 1.7.14 or newer will do."
					NEEDSOMETHING=YES
				fi
			fi
		fi
	else
		case "`arch`" in
		i*86)
			echo ""
			echo "You appear to have an a.out system."
			echo "a.out binaries are not available for this release"
			exit 1
			;;
		esac
	fi
fi

if [ X"$NEEDSOMETHING" != X ]; then
	echo ""
	echo "When you've made the required updates, re-run this script"
	echo "before continuing with the installation"
	exit 1
fi


# If there is no previous installation, there is nothing more to do

if [ ! -d $RUNDIR/. ]; then
	echo ""
	echo Done
	exit 0
fi

echo ""
echo "You are strongly advised to backup your /usr/X11R6 directory before"
echo "proceeding with this installation.  This installation will overwrite"
echo "existing files."
echo ""
echo "Do you want to continue? (y/n) "
read response
case "$response" in
	[yY]*)
		;;
	*)
		echo Aborting
		exit 1
		;;
esac

for i in $LIBLIST; do
	if [ -h $RUNDIR/lib/$i ]; then
		echo Removing old library link $RUNDIR/lib/$i
		rm -f $RUNDIR/lib/$i
	fi
done

for i in $OLDFILES; do
	if [ -f $RUNDIR/$i ]; then
		echo Removing old file $RUNDIR/$i
		rm -f $RUNDIR/$i
	fi
done

echo ""
echo Done

exit 0
