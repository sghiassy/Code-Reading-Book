XCOMM!/bin/sh

XCOMM $XConsortium: mkmf.cpp /main/4 1996/02/21 17:08:38 kaleb $
XCOMM $XFree86: xc/programs/Xserver/hw/xfree86/LinkKit/mkmf.cpp,v 3.3 1996/12/23 06:30:46 dawes Exp $

XCOMM
XCOMM Build Makefile for LinkKit
XCOMM

if [ ! -x ./mkmf ]; then
    echo "mkmf cannot be executed from this directory"
    exit 1
fi

if [ -d ./config ]; then
    CONFIG_DIR_SPEC=-I./config/cf
    IMAKE_COMMAND=./config/imake/imake
elif [ x"$XWINHOME" != x ]; then
    CONFIG_DIR_SPEC=-I$XWINHOME/lib/X11/config
    IMAKE_COMMAND="imake -DUseInstalled"
else
    CONFIG_DIR_SPEC=CONFIGDIRSPEC
    IMAKE_COMMAND="imake -DUseInstalled"
fi

if [ -f Makefile ]; then
  (set -x
    rm -f Makefile.bak
    mv Makefile Makefile.bak
  )
fi
rm -f Makefile
(set -x
  $IMAKE_COMMAND -I. $CONFIG_DIR_SPEC -DXF86LinkKit=1 -DTOPDIR=. -DCURDIR=.
  make Makefiles
XCOMM make clean
  make depend
)

