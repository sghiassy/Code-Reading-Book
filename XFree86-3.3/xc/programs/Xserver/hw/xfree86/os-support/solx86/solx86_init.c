/* $XFree86: xc/programs/Xserver/hw/xfree86/os-support/solx86/solx86_init.c,v 3.4 1996/12/23 06:51:17 dawes Exp $ */
/*
 * Copyright 1990,91 by Thomas Roell, Dinkelscherben, Germany
 * Copyright 1993 by David Wexelblat <dwex@goblin.org>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Thomas Roell and David Wexelblat 
 * not be used in advertising or publicity pertaining to distribution of 
 * the software without specific, written prior permission.  Thomas Roell and
 * David Wexelblat makes no representations about the suitability of this 
 * software for any purpose.  It is provided "as is" without express or 
 * implied warranty.
 *
 * THOMAS ROELL AND DAVID WEXELBLAT DISCLAIMS ALL WARRANTIES WITH REGARD TO 
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS, IN NO EVENT SHALL THOMAS ROELL OR DAVID WEXELBLAT BE LIABLE FOR 
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER 
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF 
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */
/* $XConsortium: solx86_init.c /main/4 1996/02/21 17:54:10 kaleb $ */

#include <signal.h>
#include <sys/time.h>
#include <unistd.h>

#include "X.h"
#include "Xmd.h"
#include "input.h"
#include "scrnintstr.h"

#include "compiler.h"

#include "xf86.h"
#include "xf86Procs.h"
#include "xf86_OSlib.h"

static Bool KeepTty = FALSE;
#ifdef SVR4
static Bool Protect0 = FALSE;
#endif
static int VTnum = -1;
static int xf86StartVT = -1;

#define MAX_SECONDS	60	
#define USEC_IN_SEC     (unsigned long)1000000

int xf86_solx86usleep(
#if NeedFunctionPrototypes
unsigned long
#endif
);
static void xf86_solx86sleep(
#if NeedFunctionPrototypes
int
#endif
);

extern void xf86VTRequest(
#if NeedFunctionPrototypes
	int
#endif
);

void xf86OpenConsole()
{
    int fd;
    struct vt_mode VT;
    struct vt_stat vtinfo;
    char vtname1[10];
    int i, FreeVTslot;

    if (serverGeneration == 1) 
    {
    	/* check if we're run with euid==0 */
    	if (geteuid() != 0)
	{
      	    FatalError("xf86OpenConsole: Server must be suid root\n");
	}

	/* Protect page 0 to help find NULL dereferencing */
	/* mprotect() doesn't seem to work */
	if (Protect0)
	{
	    int fd = -1;

	    if ((fd = open("/dev/zero", O_RDONLY, 0)) < 0)
	    {
		ErrorF("xf86OpenConsole: cannot open /dev/zero (%s)\n",
		       strerror(errno));
	    }
	    else
	    {
		if ((int)mmap(0, 0x1000, PROT_NONE,
			      MAP_FIXED | MAP_SHARED, fd, 0) == -1)
		{
		    ErrorF("xf86OpenConsole: failed to protect page 0 (%s)\n",
		       strerror(errno));
		}
		close(fd);
	    }
	}
    	/*
     	 * setup the virtual terminal manager
     	 */
    	if (VTnum != -1) 
	{
      	    xf86Info.vtno = VTnum;
    	}
    	else 
	{
      	    if ((fd = open("/dev/vt00",O_RDWR,0)) < 0) 
	    {
        	FatalError(
		    "xf86OpenConsole: Cannot open /dev/vt00 (%s)\n",
		    strerror(errno));
	    }
            if (ioctl(fd, VT_GETSTATE, &vtinfo) < 0)
            {
                FatalError("xf86OpenConsole: Cannot determine current VT\n");
            }
            xf86StartVT=vtinfo.v_active;

/* There is a SEVERE problem with x86's VT's the VT_OPENQRY ioctl()
 * will panic the entire system if all 8 (7 VT's+Console) terminals
 * are used.
 * The only other way I've found to determine if there is a free
 * is to try activating all the the available VT's and see if they
 * all succeed - if they do, there there is not a free VT, and
 * the Xserver cannot continue with out panic'ing the system.
 * (Its ugly, however, it seems to work)
 * Note there is a possible race condition here, btw.
 *
 * David Holland 2/23/94
 */

	    FreeVTslot = 0;
	    for(i=7; (i>=0) && (!FreeVTslot); i--)
		if (ioctl(fd, VT_ACTIVATE, i) != 0)
		    FreeVTslot = 1;

	    if(!FreeVTslot)
	    {
		FatalError("xf86OpenConsole: Cannot find a free VT\n");
	    }

      	    if ((ioctl(fd, VT_OPENQRY, &xf86Info.vtno) < 0) || 
		(xf86Info.vtno == -1))
	    {
        	FatalError("xf86OpenConsole: Cannot find a free VT\n");
	    }
           close(fd);
        }
	ErrorF("(using VT number %d)\n\n", xf86Info.vtno);

 	sprintf(vtname1,"/dev/vt%02d",xf86Info.vtno); /* Solaris 2.1  x86 */ 

	xf86Config(FALSE); /* Read XF86Config */

	if (!KeepTty)
    	{
    	    setpgrp();
	}

	if (((xf86Info.consoleFd = open(vtname1, O_RDWR | O_NDELAY, 0)) < 0))
	{
            FatalError("xf86OpenConsole: Cannot open %s (%s)\n",
		       vtname1, strerror(errno));
	}

	/* change ownership of the vt */
	chown(vtname1, getuid(), getgid());

	/*
	 * now get the VT
	 */
	if (ioctl(xf86Info.consoleFd, VT_ACTIVATE, xf86Info.vtno) != 0)
	{
    	    ErrorF("xf86OpenConsole: VT_ACTIVATE failed\n");
	}
	if (ioctl(xf86Info.consoleFd, VT_WAITACTIVE, xf86Info.vtno) != 0)
	{
	    ErrorF("xf86OpenConsole: VT_WAITACTIVE failed\n");
	}
	if (ioctl(xf86Info.consoleFd, VT_GETMODE, &VT) < 0) 
	{
	    FatalError("xf86OpenConsole: VT_GETMODE failed\n");
	}

	signal(SIGUSR1, xf86VTRequest);

	VT.mode = VT_PROCESS;
	VT.relsig = SIGUSR1;
	VT.acqsig = SIGUSR1;

	if (ioctl(xf86Info.consoleFd, VT_SETMODE, &VT) < 0) 
	{
	    FatalError("xf86OpenConsole: VT_SETMODE VT_PROCESS failed\n");
	}
	if (ioctl(xf86Info.consoleFd, KDSETMODE, KD_GRAPHICS) < 0)
	{
	    FatalError("xf86OpenConsole: KDSETMODE KD_GRAPHICS failed\n");
	}
    }
    else 
    {   
	/* serverGeneration != 1 */
	/*
	 * now get the VT
	 */
	if (ioctl(xf86Info.consoleFd, VT_ACTIVATE, xf86Info.vtno) != 0)
	{
	    ErrorF("xf86OpenConsole: VT_ACTIVATE failed\n");
	}
	if (ioctl(xf86Info.consoleFd, VT_WAITACTIVE, xf86Info.vtno) != 0)
	{
      	    ErrorF("xf86OpenConsole: VT_WAITACTIVE failed\n");
	}
	/*
	 * If the server doesn't have the VT when the reset occurs,
	 * this is to make sure we don't continue until the activate
	 * signal is received.
	 */
	if (!xf86VTSema)
	    sleep(5);
    }
    return;
}

void xf86CloseConsole()
{
    struct vt_mode   VT;
    char	     *console = "/dev/vt00";
    int		     console_fd;

    /*
     * Solaris 2.1 x86 doesnt seem to "switch" back to the console
     * when the VT is relinquished and its mode is reset to auto. 
     * Also, Solaris 2.1 also seems to associate vt00 with the
     * console so I've opened the "console" back up and made it
     * the active vt again in text mode and then closed it.
     * There must be a better hack for this but I'm not aware of
     * one at this time.
     *
     * Doug Anson 11/6/93
     * danson@lgc.com
     *
     * Fixed - 12/5/93 - David Holland - davidh@dorite.use.com
     * Did the whole thing similarly to the way linux does it
     */

    /* reset the display back to text mode */

    ioctl(xf86Info.consoleFd, KDSETMODE, KD_TEXT);  /* Back to text mode ... */
    if (ioctl(xf86Info.consoleFd, VT_GETMODE, &VT) != -1)
    {
	VT.mode = VT_AUTO;
	ioctl(xf86Info.consoleFd, VT_SETMODE, &VT); /* set dflt vt handling */
    }

    /* Activate the VT that X was started on */ 

    ioctl(xf86Info.consoleFd, VT_ACTIVATE, xf86StartVT);
    close(xf86Info.consoleFd);                 /* make the vt-manager happy */
    return;
}

int xf86ProcessArgument(argc, argv, i)
int argc;
char *argv[];
int i;
{
	/*
	 * Keep server from detaching from controlling tty.  This is useful 
	 * when debugging (so the server can receive keyboard signals.
	 */
	if (!strcmp(argv[i], "-keeptty"))
	{
		KeepTty = TRUE;
		return(1);
	}
	/*
	 * Undocumented flag to protect page 0 from read/write to help
	 * catch NULL pointer dereferences.  This is purely a debugging
	 * flag.
	 */
	if (!strcmp(argv[i], "-protect0"))
	{
		Protect0 = TRUE;
		return(1);
	}
	if ((argv[i][0] == 'v') && (argv[i][1] == 't'))
	{
		if (sscanf(argv[i], "vt%2d", &VTnum) == 0)
		{
			UseMsg();
			VTnum = -1;
			return(0);
		}
		return(1);
	}
	return(0);
}

void xf86UseMsg()
{
	ErrorF("vtXX                   use the specified VT number\n");
	ErrorF("-keeptty               ");
	ErrorF("don't detach controlling tty (for debugging only)\n");
	return;
}

/*
 * xf86_solx86usleep() - Solaris 2.1 x86 does not have a suitable
 * 			 replacement (SYSV) for usleep. Although
 *			 usleep exists in the BSD compatiblity libs
 *			 I dont want to use those libs if possible.
 *
 *			 Doug Anson
 *			 danson@lgc.com
 */
int xf86_solx86usleep(unsigned long usec)
{
    int		      retval = 0;
    struct itimerval  naptime;
    struct itimerval  savetime;
    unsigned long     useconds = 0;
    unsigned long     seconds = 0;
    int		      i;
    unsigned long     tmp;
	
/*
 * WHY DOESN'T THIS SIMPLY DO A select() WITH NO FILE DESCRIPTORS?
 */

    /* this time will allow a max of MAX_SECONDS seconds sleeping */
    for(i=MAX_SECONDS;i>=0;--i)
    {
	tmp = (unsigned long)((unsigned long)(i)*USEC_IN_SEC);
	if (tmp <= usec)
	{
	    seconds = i;
	    if (i == MAX_SECONDS)
		useconds = 0;
	    else
	        useconds = (unsigned long)(usec - tmp);
	    i = -1;
	}
    }

    /* get the current time */
    if ((retval=getitimer(ITIMER_REAL,&savetime)) == 0)
    {
    	/* set the itimer to reflect requested time to sleep */
	naptime.it_value.tv_sec = savetime.it_value.tv_sec + seconds;
    	naptime.it_value.tv_usec = savetime.it_value.tv_usec + useconds;
    
    	/* specify a one-shot clock */
    	naptime.it_interval.tv_usec = 0;
    	naptime.it_interval.tv_sec = 0;

	/* redisposition SIGALRM */
	signal(SIGALRM,xf86_solx86sleep);

    	/* use SIGLARM */
    	if ((retval=setitimer(ITIMER_REAL,&naptime,NULL)) == 0)
		/* now just pause */
		retval = pause();

	/* restore the timer */
	retval = setitimer(ITIMER_REAL,&savetime,NULL);

	/* restore the SIGALRM disposition */
	signal(SIGALRM,SIG_DFL);
    }

    /* return the return value */
    return retval;
}

/*
 * xf86_solx86sleep() - This function is a NOP disposition for 
 *			the SIGALRM that is used to implement 
 *			usleep() in Solaris 2.1 x86. 
 *
 *			Doug Anson
 *			danson@lgc.com
 */
static void xf86_solx86sleep(int signo)
{
    /* do nothing */
    return;
}
