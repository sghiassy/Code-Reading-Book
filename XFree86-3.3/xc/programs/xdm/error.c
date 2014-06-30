/* $XConsortium: error.c,v 1.16 94/04/17 20:03:38 gildea Exp $ */
/*

Copyright (c) 1988  X Consortium

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization
from the X Consortium.

*/

/*
 * xdm - display manager daemon
 * Author:  Keith Packard, MIT X Consortium
 *
 * error.c
 *
 * Log display manager errors to a file as
 * we generally do not have a terminal to talk to
 */

# include "dm.h"
# include <stdio.h>
#if NeedVarargsPrototypes
# include <stdarg.h>
#else
/* this type needs to be big enough to contain int or pointer */
typedef long Fmtarg_t;
#endif

/*VARARGS1*/
LogInfo(
#if NeedVarargsPrototypes
    char * fmt, ...)
#else
    fmt, arg1, arg2, arg3, arg4, arg5, arg6)
    char *fmt;
    Fmtarg_t arg1, arg2, arg3, arg4, arg5, arg6;
#endif
{
    fprintf (stderr, "xdm info (pid %d): ", getpid());
#if NeedVarargsPrototypes
    {
	va_list args;
	va_start(args, fmt);
	vfprintf (stderr, fmt, args);
	va_end(args);
    }
#else
    fprintf (stderr, fmt, arg1, arg2, arg3, arg4, arg5, arg6);
#endif
    fflush (stderr);
}

/*VARARGS1*/
LogError (
#if NeedVarargsPrototypes
    char * fmt, ...)
#else
    fmt, arg1, arg2, arg3, arg4, arg5, arg6)
    char *fmt;
    Fmtarg_t arg1, arg2, arg3, arg4, arg5, arg6;
#endif
{
    fprintf (stderr, "xdm error (pid %d): ", getpid());
#if NeedVarargsPrototypes
    {
	va_list args;
	va_start(args, fmt);
	vfprintf (stderr, fmt, args);
	va_end(args);
    }
#else
    fprintf (stderr, fmt, arg1, arg2, arg3, arg4, arg5, arg6);
#endif
    fflush (stderr);
}

/*VARARGS1*/
LogPanic (
#if NeedVarargsPrototypes
    char * fmt, ...)
#else
    fmt, arg1, arg2, arg3, arg4, arg5, arg6)
    char *fmt;
    Fmtarg_t arg1, arg2, arg3, arg4, arg5, arg6;
#endif
{
    fprintf (stderr, "xdm panic (pid %d): ", getpid());
#if NeedVarargsPrototypes
    {
	va_list args;
	va_start(args, fmt);
	vfprintf (stderr, fmt, args);
	va_end(args);
    }
#else
    fprintf (fmt, arg1, arg2, arg3, arg4, arg5, arg6);
#endif
    fflush (stderr);
    exit (1);
}

/*VARARGS1*/
LogOutOfMem (
#if NeedVarargsPrototypes
    char * fmt, ...)
#else
    fmt, arg1, arg2, arg3, arg4, arg5, arg6)
    char *fmt;
    Fmtarg_t arg1, arg2, arg3, arg4, arg5, arg6;
#endif
{
    fprintf (stderr, "xdm: out of memory in routine ");
#if NeedVarargsPrototypes
    {
	va_list args;
	va_start(args, fmt);
	vfprintf (stderr, fmt, args);
	va_end(args);
    }
#else
    fprintf (stderr, fmt, arg1, arg2, arg3, arg4, arg5, arg6);
#endif
    fflush (stderr);
}

Panic (mesg)
char	*mesg;
{
    int	i;

    i = creat ("/dev/console", 0666);
    write (i, "panic: ", 7);
    write (i, mesg, strlen (mesg));
    exit (1);
}


/*VARARGS1*/
Debug (
#if NeedVarargsPrototypes
    char * fmt, ...)
#else
    fmt, arg1, arg2, arg3, arg4, arg5, arg6)
    char *fmt;
    Fmtarg_t arg1, arg2, arg3, arg4, arg5, arg6;
#endif
{
    if (debugLevel > 0)
    {
#if NeedVarargsPrototypes
	va_list args;
	va_start(args, fmt);
	vprintf (fmt, args);
	va_end(args);
#else
	printf (fmt, arg1, arg2, arg3, arg4, arg5, arg6);
#endif
	fflush (stdout);
    }
}

InitErrorLog ()
{
	int	i;
	if (errorLogFile[0]) {
		i = creat (errorLogFile, 0666);
		if (i != -1) {
			if (i != 2) {
				dup2 (i, 2);
				close (i);
			}
		} else
			LogError ("Cannot open errorLogFile %s\n", errorLogFile);
	}
}
