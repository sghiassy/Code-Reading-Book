/* $XFree86: xc/programs/Xserver/hw/xfree86/etc/ld-wrapper.c,v 3.3 1996/12/23 06:47:15 dawes Exp $ */
/*
 * Copyright 1993 by Thomas Mueller
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Thomas Mueller not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Thomas Mueller makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * THOMAS MUELLER DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THOMAS MUELLER BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */

/* $XConsortium: ld-wrapper.c /main/4 1996/02/21 17:47:57 kaleb $ */

/* LynxOS V2.2.x /bin/ld wrapper to emulate -L option */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <limits.h>

extern char *strdup();

/* check for '-L dir' arguments and collect them...	*/
/* check for '-llib' and check if these libs are in	*/
/* the so far collected '-L dir' places... 		*/
/* if found, replace -llib with the complete path   	*/

#define MAX_L	32		/* maximum of -L options...	*/
#define MAX_LIB	512		/* maximum of -l options...	*/

#ifdef DEBUG
#define		dbg(x)	x
#else
#define		dbg(x)
#endif

main(argc, argv)
int argc;
char **argv;
{
    int i, j, l;
    char buf[MAXPATHLEN];

    char *L[MAX_L];
    int L_num = 0;

    char *lib[MAX_LIB];
    int lib_num = 0;
    int verbose = 0;

    char *opt;

    char **myargv;

    for (i = 1; i < argc; i++)
    {
	if (!strncmp(argv[i], "-v", 2))
	{
	    ++verbose;
	    continue;
	}
    }

    myargv = (char **) calloc(argc + 10, sizeof(char *));
    if (!myargv)
    {
	fprintf(stderr, "out of memory, can't alloc %d char pointers\n", argc + 10);
	exit(1);
    }

    myargv[0] = "/bin/ld.org";
    if (argc > 1)
	for (i = j = 1; i < argc;)
	{
	    dbg((printf("argc %d  = \"%s\"\n", i, argv[i])));
	    if (*argv[i] != '-')
	    {
		dbg((printf("copy: myargc %d  = \"%s\"\n", j, argv[i])));
		myargv[j++] = argv[i++];
	    }
	    else
	    {
		int found;

		switch (argv[i][1])
		{
		case 'L':
		    dbg((printf("got -L")));
		    if (strlen(argv[i]) == 2)
		    {
			dbg((printf("(with argument)")));
			++i;	/* skip argument to -L	*/
			L[L_num++] = strdup(argv[i]);
		    }
		    else
			L[L_num++] = strdup(&argv[i][2]);
		    ++i;
		    dbg((printf("-L directory %s\n", L[L_num - 1])));
		    break;

		case 'l':
		    dbg((printf("got -l >%s<", argv[i])));
		    if (strlen(argv[i]) == 2)
		    {
			fprintf(stderr, "spurious -l option\n");
			++i;
			break;
		    }

		    /* do the job....	*/
		    for (l = found = 0; !found && l < L_num; l++)
		    {
			sprintf(buf, "%s/lib%s.a", L[l], &argv[i][2]);
			dbg((printf("..checking >%s< ", buf)));
			if (access(buf, R_OK) == 0)
			{
			    dbg((printf("FOUND!!\nmyargc = %d = \"%s\"\n", j, buf)));
			    /* gotcha! */
			    myargv[j++] = strdup(buf);
			    found = 1;
			    i++;/* next argument */
			}
			else
			    dbg((printf("not found\n")));
		    }
		    if (!found)
		    {
			dbg((printf("%s not found in -L list, copying\n", argv[i])));
			dbg((printf("copy: myargc %d  = \"%s\"\n", j, argv[i])));
			myargv[j++] = argv[i++];
		    }
		    break;
		default:
		    dbg((printf("copy: myargc %d  = \"%s\"\n", j, argv[i])));
		    myargv[j++] = argv[i++];
		    break;
		}
	    }
	}

    if (verbose)
    {
	for (i = 0; i < j; i++)
	    printf("%d\t%s\n", i, myargv[i] ? myargv[i] : "(null)");
	fflush(stdout);
    }

    if (execvp("/bin/ld.org", myargv) < 0)
    {
	perror("execvp");
	exit(1);
    }
}
