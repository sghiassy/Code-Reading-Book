#!/bin/sh

# $XConsortium: mfbmap.sh /main/3 1996/02/21 17:50:10 kaleb $
# $XFree86: xc/programs/Xserver/hw/xfree86/mono/mfb.banked/mfbmap.sh,v 3.2 1996/12/23 06:48:39 dawes Exp $
#
# This skript recreates the mapping list that maps the mfb external
#  symbols * to mono_*
# This should only be rerun if there have been changes in the mfb code
#  that affect the external symbols.
#  It assumes that Xserver/mfb has been compiled.
# The output goes to stdout.
echo "/* mfbmap.h */"
echo ""
echo "#ifndef _MFBMAP_H"
echo "#define _MFBMAP_H"
echo ""
nm ../../../../mfb/*.o | awk "{ if ((\$2 == \"D\") || (\$2 == \"T\") || (\$2 == \"C\")) print \$3 }" | sed s/^_// | sort | awk "{ print \"#define \" \$1 \"  mono_\"\$1 }"
echo ""
echo "#endif"
