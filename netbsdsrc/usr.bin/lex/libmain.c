/* libmain - flex run-time support library "main" function */

/* $Header: /src/kona.cvs/netbsd/usr.bin/lex/libmain.c,v 1.1.1.2 1997/12/22 22:18:30 cgd Exp $ */

#include <sys/cdefs.h>

int yylex __P((void));
int main __P((int, char **, char **));

int
main( argc, argv, envp )
int argc;
char *argv[];
char *envp[];
	{
	while ( yylex() != 0 )
		;

	return 0;
	}
