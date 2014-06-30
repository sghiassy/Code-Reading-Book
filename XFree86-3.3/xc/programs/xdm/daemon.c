/* $XConsortium: daemon.c,v 1.16 94/12/01 17:10:49 kaleb Exp $ */
/* $XFree86: xc/programs/xdm/daemon.c,v 3.6 1996/06/29 09:10:31 dawes Exp $ */
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
 */

#include <X11/Xos.h>

#if defined(SVR4) || defined(USG)
#include <termios.h>
#else
#include <sys/ioctl.h>
#endif
#if defined(__osf__) || defined(linux) || defined(MINIX)
#define setpgrp setpgid
#endif
#ifdef hpux
#include <sys/ptyio.h>
#endif
#include <errno.h>
#ifdef X_NOT_STDC_ENV
extern int errno;
#endif
#include <sys/types.h>
#ifdef X_NOT_POSIX
#define Pid_t int
#else
#define Pid_t pid_t
#endif

extern void exit ();

BecomeOrphan ()
{
    Pid_t child_id;
    int stat;

    /*
     * fork so that the process goes into the background automatically. Also
     * has a nice side effect of having the child process get inherited by
     * init (pid 1).
     * Separate the child into its own process group before the parent
     * exits.  This eliminates the possibility that the child might get
     * killed when the init script that's running xdm exits.
     */

    child_id = fork();
    switch (child_id) {
    case 0:
	/* child */
	break;
    case -1:
	/* error */
	LogError("daemon fork failed, errno = %d\n", errno);
	break;

    default:
	/* parent */

#ifndef CSRG_BASED
#if defined(SVR4)
	stat = setpgid(child_id, child_id);
	/* This gets error EPERM.  Why? */
#else
#if defined(SYSV)
	stat = 0;	/* don't know how to set child's process group */
#else
	stat = setpgrp(child_id, child_id);
#ifndef MINIX
	if (stat != 0)
	    LogError("setting process grp for daemon failed, errno = %d\n",
		     errno);
#endif /* MINIX */
#endif
#endif
#endif /* !CSRG_BASED */
	exit (0);
    }
}

BecomeDaemon ()
{
    register int i;

    /*
     * Close standard file descriptors and get rid of controlling tty
     */

#ifdef CSRG_BASED
    daemon (0, 0);
#else
#if defined(SYSV) || defined(SVR4)
    setpgrp ();
#else
    setpgrp (0, getpid());
#endif

    close (0); 
    close (1);
    close (2);

#ifndef __EMX__
#ifdef MINIX
#if 0
    /* Use setsid() to get rid of our controlling tty, this requires an extra
     * fork though.
     */
    setsid();
    if (fork() > 0)
    	_exit(0);
#endif
#else /* !MINIX */
#if !((defined(SYSV) || defined(SVR4)) && defined(i386))
    if ((i = open ("/dev/tty", O_RDWR)) >= 0) {	/* did open succeed? */
#if defined(USG) && defined(TCCLRCTTY)
	int zero = 0;
	(void) ioctl (i, TCCLRCTTY, &zero);
#else
#if (defined(SYSV) || defined(SVR4)) && defined(TIOCTTY)
	int zero = 0;
	(void) ioctl (i, TIOCTTY, &zero);
#else
	(void) ioctl (i, TIOCNOTTY, (char *) 0);    /* detach, BSD style */
#endif
#endif
	(void) close (i);
    }
#endif /* !((SYSV || SVR4) && i386) */
#endif /* MINIX */
#endif /* !__EMX__ */

    /*
     * Set up the standard file descriptors.
     */
    (void) open ("/", O_RDONLY);	/* root inode already in core */
    (void) dup2 (0, 1);
    (void) dup2 (0, 2);
#endif /* CSRG_BASED */
}
