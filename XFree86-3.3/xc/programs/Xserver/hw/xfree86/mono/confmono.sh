#!/bin/sh

# $XConsortium: confmono.sh /main/3 1996/02/21 17:49:06 kaleb $
# $XFree86: xc/programs/Xserver/hw/xfree86/mono/confmono.sh,v 3.2 1996/12/23 06:47:43 dawes Exp $
#
# This script generates monoConf.c
#
# usage: confmono.sh driver1 driver2 ...
#

MONOCONF=./monoConf.c

cat > $MONOCONF <<EOF
/*
 * This file is generated automatically -- DO NOT EDIT
 */

#include "mono.h"

extern monoVideoChipRec
EOF
Args="`echo $* | tr '[a-z]' '[A-Z]'`"
set - $Args
while [ $# -gt 1 ]; do
  echo "        $1," >> $MONOCONF
  shift
done
echo "        $1;" >> $MONOCONF
cat >> $MONOCONF <<EOF

monoVideoChipPtr monoDrivers[] =
{
EOF
for i in $Args; do
  echo "        &$i," >> $MONOCONF
done
echo "        NULL" >> $MONOCONF
echo "};" >> $MONOCONF
