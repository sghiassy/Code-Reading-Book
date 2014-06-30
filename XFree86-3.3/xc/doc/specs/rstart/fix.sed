#! /bin/sed -f
#
# $XConsortium: fix.sed,v 1.2 94/02/06 16:38:06 mor Exp $
#
s/o+/./g
s/|-/+/g
s/.//g
/FORMFEED\[Page/{
s/FORMFEED/        /
a\

}
