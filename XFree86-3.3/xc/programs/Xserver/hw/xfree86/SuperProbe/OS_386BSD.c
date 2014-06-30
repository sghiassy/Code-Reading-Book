/* $XFree86: xc/programs/Xserver/hw/xfree86/SuperProbe/OS_386BSD.c,v 3.10.2.2 1997/05/06 13:24:12 dawes Exp $ */
/*
 * (c) Copyright 1993,1994 by David Dawes <dawes@xfree86.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL 
 * DAVID DAWES BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF 
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
 * SOFTWARE.
 * 
 * Except as contained in this notice, the name of David Dawes shall not be
 * used in advertising or otherwise to promote the sale, use or other dealings
 * in this Software without prior written authorization from David Dawes.
 *
 */
/*
 * Copyright 1997
 * Digital Equipment Corporation. All rights reserved.
 * This software is furnished under license and may be used and copied only in 
 * accordance with the following terms and conditions.  Subject to these 
 * conditions, you may download, copy, install, use, modify and distribute 
 * this software in source and/or binary form. No title or ownership is 
 * transferred hereby.
 * 1) Any source code used, modified or distributed must reproduce and retain 
 *    this copyright notice and list of conditions as they appear in the 
 *    source file.
 *
 * 2) No right is granted to use any trade name, trademark, or logo of Digital 
 *    Equipment Corporation. Neither the "Digital Equipment Corporation" name 
 *    nor any trademark or logo of Digital Equipment Corporation may be used 
 *    to endorse or promote products derived from this software without the 
 *    prior written permission of Digital Equipment Corporation.
 *
 * 3) This software is provided "AS-IS" and any express or implied warranties,
 *    including but not limited to, any implied warranties of merchantability,
 *    fitness for a particular purpose, or non-infringement are disclaimed. In
 *    no event shall DIGITAL be liable for any damages whatsoever, and in 
 *    particular, DIGITAL shall not be liable for special, indirect, 
 *    consequential, or incidental damages or damages for lost profits, loss 
 *    of revenue or loss of use, whether such damages arise in contract, 
 *    negligence, tort, under statute, in equity, at law or otherwise, even if
 *    advised of the possibility of such damage. 
 */
/* $XConsortium: OS_386BSD.c /main/11 1996/10/27 11:04:03 kaleb $ */

#include "Probe.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <sys/mman.h>

#ifdef __bsdi__
# include <i386/isa/pcconsioctl.h>
# define CONSOLE_X_MODE_ON PCCONIOCRAW
# define CONSOLE_X_MODE_OFF PCCONIOCCOOK
#else
# if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
#  ifdef CODRV_SUPPORT
    /* This header is part of codrv */
#   include <machine/ioctl_pc.h>
#  endif
#  if defined(PCVT_SUPPORT) && !defined(SYSCONS_SUPPORT)
#   include <machine/pcvt_ioctl.h>
#  endif
#  ifdef SYSCONS_SUPPORT
    /* both, Free and NetBSD have syscons */
#   include <machine/console.h>
#  endif
# else
#  ifdef CODRV_SUPPORT
    /* This header is part of codrv */
#   include <sys/ioctl_pc.h>
#  endif
# endif
# undef CONSOLE_X_MODE_ON
# define CONSOLE_X_MODE_ON _IO('t',121)
# undef CONSOLE_X_MODE_OFF
# define CONSOLE_X_MODE_OFF _IO('t',122)
#endif
#if defined(__NetBSD__) && !defined(MAP_FILE)
#define MAP_FILE 0
#endif

#if defined(__NetBSD__) && !defined(__OpenBSD__)
#  include <sys/param.h>
#  ifdef NetBSD1_1
#    include <machine/sysarch.h>
#  endif
#endif

#if defined(__OpenBSD__)
#  include <machine/sysarch.h>
#endif

#if !defined(__OpenBSD__)
#  define CONSOLE_DEVICE "/dev/ttyv0"
#else
#  define CONSOLE_DEVICE "/dev/ttyC0"
#endif

#ifdef USE_ARM32_MMAP
#define	DEV_MEM_IOBASE	0x43000000
extern unsigned int IOPortBase;
static int DEVMEM_fd = -1;
#endif

static int CONS_fd = -1;
static int BIOS_fd = -1;

static Bool HasCodrv = FALSE;
static Bool HasUslVt = FALSE;

/*
 * OpenVideo --
 *
 * Enable access to the installed video hardware.  For 386BSD, we disable
 * IO protection, since this is currently the only way to access any
 * IO registers.
 */
int OpenVideo()
{
	if (geteuid() != 0)
	{
		fprintf(stderr, 
			"%s: Must be run as root\n", 
			MyName);
		return(-1);
	}
	/*
	 * Attempt to open /dev/kbd.  If this fails, the driver is either
	 * pccons, or it is codrv and something else has it open.  errno
	 * will tell us which case it is.
	 */
	if ((CONS_fd = open("/dev/kbd", O_RDONLY|O_NDELAY, 0)) < 0)
	{
		if (errno == EBUSY)
		{
			/*
			 * Codrv, but something else is using the console
			 * in "X" mode
			 */
			fprintf(stderr,
			    "%s: Cannot be run while an X server is running\n",
			    MyName);
			return(-1);
		}
#ifndef __bsdi__
		/*
		 * pccons.  To guess if we're being run from within an
		 * X session, check $DISPLAY, and fail if it is set.
		 * This check is not foolproof -- just a guide.
		 */
		if (getenv("DISPLAY"))
		{
			fprintf(stderr,
			    "%s: Cannot be run while an X server is running\n",
			    MyName);
			fprintf(stderr,
			    "%s: If an X server is not running, unset $DISPLAY",
			    MyName);
			fprintf(stderr,
			    " and try again\n");
			return(-1);
		}
		if ((CONS_fd = open("/dev/vga", O_RDWR, 0)) < 0
		    && (CONS_fd = open(CONSOLE_DEVICE, O_RDWR, 0)) < 0)
		{
			fprintf(stderr,
				"%s: Cannot open /dev/vga nor %s\n", 
				MyName, CONSOLE_DEVICE);
			return(-1);
		}
#endif
	}
	else
	{
#ifndef __bsdi__
#ifdef CODRV_SUPPORT
		struct oldconsinfo tmp;

		if (ioctl(CONS_fd, OLDCONSGINFO, &tmp) < 0)
		{
			fprintf(stderr, "%s: Unknown console driver\n",
				MyName);
			return(-1);
		}
		HasCodrv = TRUE;
#endif
#endif
	}
#ifndef __bsdi__
	if (HasCodrv)
	{
#ifdef CODRV_SUPPORT
		int onoff = X_MODE_ON;

		if (ioctl(CONS_fd, CONSOLE_X_MODE, &onoff) < 0)
		{
			fprintf(stderr, "%s: CONSOLE_X_MODE ON failed\n",
				MyName);
			return(-1);
		}
		ioctl(CONS_fd, VGATAKECTRL, 0);
#endif
	}
	else
	{
#if defined(SYSCONS_SUPPORT) || defined(PCVT_SUPPORT)
		/*
		 * not codrv and not BSDI; first look if we have a console
		 * driver that understands USL-style VT commands
		 */
		int vt_num;
		
		if (ioctl(CONS_fd, VT_GETACTIVE, &vt_num) == 0)
		{
			/* yes, so we don't use the old stuff */
			HasUslVt = TRUE;
			if (ioctl(CONS_fd, KDENABIO, 0) < 0)
			{
				fprintf(stderr, "%s: KDENABIO failed\n",
					MyName);
				return(-1);
			}
		}
		else
#endif
#endif
		if (ioctl(CONS_fd, CONSOLE_X_MODE_ON, 0) < 0)
		{
			fprintf(stderr, "%s: CONSOLE_X_MODE_ON failed\n",
				MyName);
			return(-1);
		}
#ifdef __bsdi__
	ioctl(CONS_fd, PCCONENABIOPL, 0);
#else
	}
#endif
	return(CONS_fd);
}

/*
 * CloseVideo --
 *
 * Disable access to the video hardware.  For 386BSD, we re-enable
 * IO protection.
 */
void CloseVideo()
{
	if (CONS_fd != -1)
	{
#ifndef __bsdi__
#ifdef CODRV_SUPPORT
		int onoff = X_MODE_OFF;
#endif

		if (HasCodrv)
		{
#ifdef CODRV_SUPPORT
			ioctl(CONS_fd, VGAGIVECTRL, 0);
			ioctl(CONS_fd, CONSOLE_X_MODE, &onoff);
#endif
		}
#if defined(SYSCONS_SUPPORT) || defined(PCVT_SUPPORT)
		else if(HasUslVt)
			ioctl(CONS_fd, KDDISABIO, 0);
#endif
		else
#endif
			ioctl(CONS_fd, CONSOLE_X_MODE_OFF, 0);
	}
#ifdef __bsdi__
	ioctl(CONS_fd, PCCONDISABIOPL, 0);
#endif
	if (CONS_fd > 0)
	{
		close(CONS_fd);
	}
}

/*
 * MapVGA --
 *
 * Map the VGA memory window (0xA0000-0xAFFFF) as read/write memory for
 * the process for use in probing memory.
 */
Byte *MapVGA()
{
	int fd;
	Byte *base;

	if ((fd = open("/dev/vga", O_RDWR)) < 0)
	{
		fprintf(stderr, "%s: Failed to open /dev/vga\n", MyName);
		return((Byte *)0);
	}
	base = (Byte *)mmap((caddr_t)0, 0x10000, PROT_READ|PROT_WRITE,
			    MAP_FILE, fd, 0);
	close(fd);
	if ((long)base == -1)
	{
		fprintf(stderr, "%s: Failed to mmap framebuffer\n", MyName);
		return((Byte *)0);
	}
	return(base);
}

Byte *MapMem(address,size)
	unsigned long address;
	unsigned long size;
{
	return((Byte*)0);
}

/*
 * UnMapVGA --
 *
 * Unmap the VGA memory window.
 */
void UnMapVGA(base)
Byte *base;
{
	munmap((caddr_t)base, 0x10000);
}

void UnMapMem(base,size)
	Byte *base;
	unsigned long size;
{
	return;
}

/*
 * ReadBIOS --
 *
 * Read 'Len' bytes from the video BIOS at address 'Bios_Base'+'Offset' into 
 * buffer 'Buffer'.
 */
int ReadBIOS(Offset, Buffer, Len)
unsigned Offset;
Byte *Buffer;
int Len;
{
	Word tmp;
	Byte *Base = Bios_Base + Offset;
	unsigned long bs = (unsigned long) Base;

#ifdef	__arm32__
	return(-1);
#endif

	if (BIOS_fd == -1)
	{
		if ((BIOS_fd = open("/dev/mem", O_RDONLY, 0)) < 0)
		{
			fprintf(stderr, "%s: cannot open /dev/mem\n", MyName);
			return(-1);
		}
	}
	if ((off_t)(bs & 0x7FFF) != (off_t)0)
	{
		/*
	 	 * Sanity check...
	 	 */
		(void)lseek(BIOS_fd, (off_t)(bs & 0xF8000), SEEK_SET);
		(void)read(BIOS_fd, &tmp, 2);
		if (tmp != (Word)0xAA55)
		{
			fprintf(stderr, 
				"%s: BIOS sanity check failed, addr=%lx\n",
				MyName, bs);
			return(-1);
		}
	}
	if (lseek(BIOS_fd, (off_t)bs, SEEK_SET) < 0)
	{
		fprintf(stderr, "%s: BIOS seek failed\n", MyName);
		return(-1);
	}
	if (read(BIOS_fd, Buffer, Len) != Len)
	{
		fprintf(stderr, "%s: BIOS read failed\n", MyName);
		return(-1);
	}
	return(Len);
}

/*
 * EnableIOPort --
 *
 * Enable access to 'NumPorts' IO ports listed in array 'Ports'.  For 386BSD, 
 * we've disabled IO protections so this is a no-op.
 */
static int IOEnabled = 0;
/*ARGSUSED*/
int EnableIOPorts(NumPorts, Ports)
CONST int NumPorts;
CONST Word *Ports;
{
#ifdef USE_I386_IOPL
    if (IOEnabled++ == 0) {
	i386_iopl(TRUE);
    }
#endif
#ifdef USE_ARM32_MMAP
    if (IOEnabled++ == 0) {
	if (DEVMEM_fd == -1 && (DEVMEM_fd = open("/dev/mem", O_RDWR, 0)) < 0)
	{
	    fprintf(stderr,
		    "%s: Cannot open /dev/mem to map IO ports\n", MyName);
	    exit(1);
	}

	IOPortBase = (unsigned int)mmap((caddr_t)0, 0x400, PROT_READ|PROT_WRITE,
					MAP_FILE, DEVMEM_fd, (off_t)DEV_MEM_IOBASE);

        if (IOPortBase == (unsigned int)-1)
	{	
	    fprintf(stderr,
		    "%s: Cannot mmap IO ports\n", MyName);
	    exit(1);
	}
    }
#endif
	return(0);
}

/*
 * DisableIOPort --
 *
 * Disable access to 'NumPorts' IO ports listed in array  'Ports'.  For 386BSD, 
 * we've disabled IO protections so this is a no-op.
 */
/*ARGSUSED*/
/*ARGSUSED*/
int DisableIOPorts(NumPorts, Port)
CONST int NumPorts;
CONST Word *Port;
{
#ifdef USE_I386_IOPL
    if (--IOEnabled == 0) {
	i386_iopl(FALSE);
    }
#endif
#ifdef USE_ARM32_MMAP
    if (--IOEnabled == 0) {
	munmap((caddr_t)IOPortBase, 0x400);
    }
#endif
	return(0);
}

/*
 * ShortSleep --
 *
 * Sleep for the number of milliseconds specified in 'Delay'.
 */
void ShortSleep(Delay)
int Delay;
{
	usleep(Delay * 1000);
}

