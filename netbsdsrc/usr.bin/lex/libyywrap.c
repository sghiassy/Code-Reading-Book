/* libyywrap - flex run-time support library "yywrap" function */

/* $Header: /src/kona.cvs/netbsd/usr.bin/lex/libyywrap.c,v 1.1.1.2 1997/12/22 22:18:31 cgd Exp $ */

#include <sys/cdefs.h>

int yywrap __P((void));

int
yywrap()
	{
	return 1;
	}
