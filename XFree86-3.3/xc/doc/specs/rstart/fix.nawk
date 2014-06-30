#! /bin/nawk -f
# $XConsortium: fix.nawk,v 1.2 94/02/06 16:38:05 mor Exp $
#

BEGIN {
	ignore = 1;
}

/FORMFEED\[Page/	{
	sub("FORMFEED", "        ");
	print;
	print "";
	ignore = 1;
	next;
}

$0 == "" {
	if(ignore) next;
}

{
	ignore = 0;
	print;
}
