#!/bin/sh
# $XFree86: xc/programs/Xserver/hw/xfree86/accel/s3_virge/confS3V.sh,v 3.1 1996/12/27 07:02:13 dawes Exp $
#
# This script generates s3vConf.c
#
# usage: confS3V.sh driver1 driver2 ...
#
# $XConsortium: confS3V.sh /main/2 1996/10/25 11:33:48 kaleb $

S3CONF=./s3vConf.c

cat > $S3CONF <<EOF
/*
 * This file is generated automatically -- DO NOT EDIT
 */

#include "s3v.h"

extern s3VideoChipRec
EOF
Args="`echo $* | tr '[a-z]' '[A-Z]'`"
set - $Args
while [ $# -gt 1 ]; do
  echo "        $1," >> $S3CONF
  shift
done
echo "        $1;" >> $S3CONF
cat >> $S3CONF <<EOF

s3VideoChipPtr s3Drivers[] =
{
EOF
for i in $Args; do
  echo "        &$i," >> $S3CONF
done
echo "        NULL" >> $S3CONF
echo "};" >> $S3CONF
