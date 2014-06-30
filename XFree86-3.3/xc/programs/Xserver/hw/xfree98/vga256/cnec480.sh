#!/bin/sh

# $XFree86: xc/programs/Xserver/hw/xfree98/vga256/cnec480.sh,v 3.2 1996/12/23 07:08:27 dawes Exp $
#
# This script generates cnec480Conf.c
#
# usage: cnec480.sh driver1 driver2 ...
#
# $XConsortium: cnec480.sh /main/3 1996/02/21 18:16:32 kaleb $

VGACONF=./nec480Conf.c

cat > $VGACONF <<EOF
/*
 * This file is generated automatically -- DO NOT EDIT
 */

#include "xf86.h"
#include "vga.h"

extern vgaVideoChipRec
EOF
Args="`echo $* | tr '[a-z]' '[A-Z]'`"
set - $Args
while [ $# -gt 1 ]; do
  echo "        $1," >> $VGACONF
  shift
done
echo "        $1;" >> $VGACONF
cat >> $VGACONF <<EOF

vgaVideoChipPtr vgaDrivers[] =
{
EOF
for i in $Args; do
  echo "        &$i," >> $VGACONF
done
echo "        NULL" >> $VGACONF
echo "};" >> $VGACONF
