/* $XConsortium: xdmshell.c,v 1.16 95/01/05 20:57:57 kaleb Exp $ */
/*
 * xdmshell - simple program for running xdm from login
 *
 * 
Copyright (c) 1988  X Consortium

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from the X Consortium.
 * *
 * Author:  Jim Fulton, MIT X Consortium
 *
 * This program should probably be setuid to root.  On the macII, it must be
 * run from the console so that getty doesn't get confused about zero-length
 * reads.
 *
 * WARNING:  Make sure that you tailor your Xresources file to have a
 * way of invoking the abort-display() action.  Otherwise, you won't be able
 * bring down X when you are finished.
 */

/* $XFree86: xc/programs/xdm/xdmshell.c,v 3.2 1995/01/28 16:16:58 dawes Exp $ */

#include <stdio.h>
#include "dm.h"
#include <errno.h>
#ifdef X_NOT_STDC_ENV
extern int errno;
#endif

#ifdef macII
#define ON_CONSOLE_ONLY
#endif

#ifdef ON_CONSOLE_ONLY
#include <sys/ioctl.h>
#endif

#ifndef BINDIR
#define BINDIR "/usr/bin/X11"
#endif

/*
 * HP-UX does have vfork, but A/UX doesn't
 */
#ifndef HAS_VFORK
#define vfork() fork()
#endif

char *ProgramName;

static int exec_args (filename, args)
    char *filename;
    char **args;
{
    int pid;
    waitType status;

    if (!filename) return -1;

    if (filename[0] != '/') {
	fprintf (stderr, 
	       "%s:  attempt to execute program with relative pathname:  %s\n",
		 ProgramName, filename);
	return -1;
    }

    if (access (filename, X_OK) != 0) return -1;

    switch (pid = vfork ()) {
      case -1:						/* error */
	return -1;
      case 0:    					/* child */
	execv (filename, args);
	_exit (1);
	/* NOTREACHED */
      default:						/* parent */
	while (wait (&status) != pid) ;
    }
    return waitCode (status);
}

static int exec_one_arg (filename, arg)
    char    *filename;
    char    *arg;
{
    char    *argv[3];

    argv[0] = filename;
    argv[1] = arg;
    argv[2] = NULL;
    return exec_args (filename, argv);
}

main (argc, argv)
    int argc;
    char *argv[];
{
    int ttyfd;
    char cmdbuf[256];
    char *args[10];
#ifdef ON_CONSOLE_ONLY
    int consfd;
    int ttypgrp, conspgrp;
    char *ttyName;
    extern char *ttyname();
#endif

    ProgramName = argv[0];

    if (argc > 1) {
	fprintf (stderr, "usage:  %s\r\n", ProgramName);
	exit (1);
    }

    ttyfd = open ("/dev/tty", O_RDWR, 0);
    if (ttyfd < 3) {			/* stdin = 0, stdout = 1, stderr = 2 */
	fprintf (stderr, 
		 "%s:  must be run directly from the console.\r\n",
		 ProgramName);
	exit (1);
    }
#ifdef ON_CONSOLE_ONLY
    if (ioctl (ttyfd, TIOCGPGRP, (char *)&ttypgrp) != 0) {
	fprintf (stderr, "%s:  unable to get process group of /dev/tty\r\n",
		 ProgramName);
	(void) close (ttyfd);
	exit (1);
    }
#endif
    (void) close (ttyfd);
    
#ifdef ON_CONSOLE_ONLY
    ttyName = ttyname (0);
    if (!ttyName || strcmp (ttyName, "/dev/console") != 0) {
	fprintf (stderr, "%s:  must login on /dev/console instead of %s\r\n",
		 ProgramName, ttyName ? ttyName : "non-terminal device");
	exit (1);
    }

    consfd = open ("/dev/console", O_RDWR, 0);
    if (consfd < 3) {			/* stdin = 0, stdout = 1, stderr = 2 */
	fprintf (stderr, "%s:  unable to open /dev/console\r\n",
		 ProgramName);
	exit (1);
    }

    if (ioctl (consfd, TIOCGPGRP, (char *)&conspgrp) != 0) {
	fprintf (stderr,
		 "%s:  unable to get process group of /dev/console\r\n",
		 ProgramName);
	(void) close (consfd);
	exit (1);
    }
    (void) close (consfd);

    if (ttypgrp != conspgrp) {
	fprintf (stderr, "%s:  must be run from /dev/console\r\n", 
		 ProgramName);
	exit (1);
    }
#endif

    /* make xdm run in a non-setuid environment */
    setuid (geteuid());

    /*
     * exec /usr/bin/X11/xdm -nodaemon -udpPort 0
     */
    strcpy (cmdbuf, BINDIR);
    strcat (cmdbuf, "/xdm");
    args[0] = cmdbuf;
    args[1] = "-nodaemon";
    args[2] = "-udpPort";
    args[3] = "0";
    args[4] = NULL;
    if (exec_args (cmdbuf, args) == -1) {
	fprintf (stderr, "%s:  unable to execute %s (error %d, %s)\r\n",
		 ProgramName, cmdbuf, errno, strerror(errno));
	exit (1);
    }

#ifdef macII
    strcpy (cmdbuf, BINDIR);
    strcat (cmdbuf, "/Xrepair");
    (void) exec_one_arg (cmdbuf, NULL);
    (void) exec_one_arg ("/usr/bin/screenrestore", NULL);
#endif

#ifdef sun
    strcpy (cmdbuf, BINDIR);
    strcat (cmdbuf, "/kbd_mode");
    (void) exec_one_arg (cmdbuf, "-a");
#endif

    exit (0);
    /*NOTREACHED*/
}

