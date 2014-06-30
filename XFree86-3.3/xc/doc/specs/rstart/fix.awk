#! /bin/awk -f
# $XConsortium: fix.awk,v 1.2 94/02/06 16:37:59 mor Exp $
#

BEGIN {
	ignore = 1;
}

# following line starts /^L/
//	{
	print;
	ignore = 1;
	next;
}

/^$/ {
	if(ignore) next;
}

{
	ignore = 0;
	print;
}
