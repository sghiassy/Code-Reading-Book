/* $XFree86: xc/programs/Xserver/hw/xfree86/SuperProbe/OS_SYSV.c,v 3.15.2.1 1997/05/06 13:24:27 dawes Exp $ */
/*
 * (c) Copyright 1993,1994 by David Wexelblat <dwex@xfree86.org>
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
 * DAVID WEXELBLAT BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF 
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
 * SOFTWARE.
 * 
 * Except as contained in this notice, the name of David Wexelblat shall not be
 * used in advertising or otherwise to promote the sale, use or other dealings
 * in this Software without prior written authorization from David Wexelblat.
 *
 */
/* $XConsortium: OS_SYSV.c /main/13 1996/10/25 11:33:20 kaleb $ */

#if defined(sun)
/* Fix for Solaris */
#define __EXTENSIONS__
#endif

#include "Probe.h"

#include <fcntl.h>
#if defined(SCO) || defined(ESIX)
# include <sys/types.h>
#endif
#ifdef SYSV
# if !defined(O_NDELAY) && defined(O_NONBLOCK)
#  define O_NDELAY O_NONBLOCK
# endif
# ifndef SCO
#  include <sys/at_ansi.h>
# endif
# include <sys/param.h>
#endif
#ifdef SCO
# include <sys/vtkd.h>
# include <sys/console.h>
#ifdef SCO
#undef _status
#endif
#else
# include <sys/kd.h>
# if !defined(sun)
#  include <sys/vt.h>
# endif
#endif
#include <sys/immu.h>
#if !defined(sun)
# include <sys/region.h>
#endif
#include <sys/proc.h>
#include <sys/tss.h>
#ifdef NCR
/* broken NCR <sys/sysi86.h> */
#define __STDC
#include <sys/sysi86.h>
#undef __STDC
#else
#include <sys/sysi86.h>
#endif
#ifdef SVR4
# if !defined(sun)
#  include <sys/seg.h>
# endif
#endif
#include <sys/v86.h>
#if defined(sun)
# include <sys/psw.h>
#endif

#ifdef __STDC__
int sysi86(int, ...);
int syscall(int, ...);
#ifndef SCO
int munmap(caddr_t, size_t);
#endif
#else
int sysi86();
int syscall();
#ifndef SCO
int munmap();
#endif
#endif

#ifdef SVR4
# include <sys/mman.h>
# if defined(sun)
#  define DEV_MEM	"/dev/fb"
# else
#  ifdef SCO325
#   define DEV_MEM	"/dev/mem"
#  else
#   define DEV_MEM 	"/dev/pmem"
#  endif
# endif
#else
# define DEV_MEM	"/dev/mem"
#endif

#ifndef SI86IOPL
#define SET_IOPL() sysi86(SI86V86,V86SC_IOPL,PS_IOPL)
#define RESET_IOPL() sysi86(SI86V86,V86SC_IOPL,0)
#else
#define SET_IOPL() sysi86(SI86IOPL,3)
#define RESET_IOPL() sysi86(SI86IOPL,0)
#endif

static int VT_fd = -1;
static int VT_num = -1;
static int BIOS_fd = -1;

/*
 * OpenVideo --
 *
 * Enable access to the installed video hardware.  For SYSV, open a new
 * VT if not running on a VT.  Also, disable IO protection, since we may
 * need to get at extended registers.
 */
int OpenVideo()
{
	int fd;
	char fn[20];

	if (geteuid() != 0)
	{
		fprintf(stderr,
			"%s: Must be run as root\n",
			MyName);
		return(-1);
	}
	if (ioctl(0, KIOCINFO, 0) == (('k'<<8)|'d'))
	{
		VT_fd = 0;
	}
	else 
	{

#if !defined(sun)
		if ((fd = open("/dev/console", O_RDWR, 0)) < 0)
		{
			fprintf(stderr, "%s: Cannot open /dev/console\n", 
				MyName);
			return(-1);
		}
		if ((ioctl(fd, VT_OPENQRY, &VT_num) < 0) || (VT_num == -1))
		{
			fprintf(stderr, "%s: No free VTs\n", MyName);
			return(-1);
		}
		close(fd);
		sprintf(fn, "/dev/vt%02d", VT_num);
#else
		sprintf(fn, "/dev/fb");
#endif

		if ((VT_fd = open(fn, O_RDWR|O_NDELAY, 0)) < 0)
		{
			fprintf(stderr, "%s: Could not open VT %s\n", 
				MyName, fn);
			return(-1);
		}
#if !defined(SCO) && !defined(sun)
		if (ioctl(VT_fd, VT_WAITACTIVE, VT_num) != 0)
		{
			fprintf(stderr, "%s: VT_WAITACTIVE failed!\n", MyName);
			return(-1);
		}
#endif /* SCO */
		/*
		 * Give the other VT time to release.
		 */
		sleep(1);
	}
	(void)SET_IOPL();
	return(VT_fd);
}

/*
 * CloseVideo --
 *
 * Disable access to the video hardware.  For SYSV, close the VT if it's open.
 * Also, re-enable IO protection.
 */
void CloseVideo()
{
	if (VT_fd != -1)
	{
		(void)RESET_IOPL();
	}
	if (VT_fd > 0)
	{
		close(VT_fd);
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
#if defined(SVR4)
	int fd;
	Byte *base;

	if ((fd = open(DEV_MEM, O_RDWR)) < 0)
	{
		fprintf(stderr, "%s: Failed to open %s\n", MyName, DEV_MEM);
		return((Byte *)0);
	}
	base = (Byte *)mmap((caddr_t)0, 0x10000, PROT_READ|PROT_WRITE,
			    MAP_SHARED, fd, (off_t)0xA0000);
	close(fd);
	if ((long)base == -1)
	{
		fprintf(stderr, "%s: Failed to mmap framebuffer\n", MyName);
		return((Byte *)0);
	}
#elif defined(OLDSCO)
	static Byte *base = (Byte *)0;
	ioctl(VT_fd, KDSETMODE, KD_GRAPHICS);
	if (base == (Byte *) 0)
		base = (Byte *) ioctl(VT_fd, MAPCONS, 0L);
#else /* SVR4 */
	static Byte *base = (Byte *)0;
	static struct kd_memloc vgaDSC;

	ioctl(VT_fd, KDSETMODE, KD_GRAPHICS);
	if (base == (Byte *)0)
	{
		base = (Byte *)(((unsigned int)malloc(0x11000) & ~0xFFF) 
			+ 0x1000);
		vgaDSC.vaddr = (char *)base;
		vgaDSC.physaddr = (char *)0xA0000;
		vgaDSC.length = 0x10000;
		vgaDSC.ioflg = 1;
	}
	if (ioctl(VT_fd, KDMAPDISP, &vgaDSC) < 0)
	{
		fprintf(stderr, "%s: Failed to mmap framebuffer\n", MyName);
		return((Byte *)0);
	}
#endif /* SVR4 */
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
#ifdef SVR4
	munmap((caddr_t)base, 0x10000);
#else /* SVR4 */
# ifndef SCO
	ioctl(VT_fd, KDUNMAPDISP, 0);
# endif
	ioctl(VT_fd, KDSETMODE, KD_TEXT);
#endif
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
#ifndef SVR4
	Word tmp;
	Byte *Base = Bios_Base + Offset;

	if (BIOS_fd == -1)
	{
		if ((BIOS_fd = open(DEV_MEM, O_RDONLY, 0)) < 0)
		{
			fprintf(stderr, "%s: cannot open %s\n", 
				MyName, DEV_MEM);
			return(-1);
		}
	}
	if ((off_t)((off_t)Base & 0x7FFF) != (off_t)0)
	{
		/*
	 	 * Sanity check...
	 	 */
		(void)lseek(BIOS_fd, (off_t)((off_t)Base & 0xF8000), SEEK_SET);
		(void)read(BIOS_fd, &tmp, 2);
		if (tmp != (Word)0xAA55)
		{
			fprintf(stderr, 
				"%s: BIOS sanity check failed, addr=%x\n",
				MyName, (int)Base);
			return(-1);
		}
	}
	if (lseek(BIOS_fd, (off_t)Base, SEEK_SET) < 0)
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
#else /* SVR4 */
	Byte *Base = Bios_Base + Offset;
	Word *tmp;
	Byte *ptr;
	unsigned long page_offset = (unsigned long)Base & 0xFFF;
	unsigned long mmap_base = (unsigned long)Base & ~0xFFF;
	unsigned long mmap_len = Len + page_offset;

	if (BIOS_fd == -1)
	{
		if ((BIOS_fd = open(DEV_MEM, O_RDONLY, 0)) < 0)
		{
			fprintf(stderr, "%s: cannot open %s\n", 
				MyName, DEV_MEM);
			return(-1);
		}
	}
	if ((off_t)((off_t)Base & 0x7FFF) != (off_t)0)
	{
		tmp = (Word *)mmap((caddr_t)0, 0x1000, PROT_READ, MAP_SHARED, 
			           BIOS_fd, (off_t)((off_t)Base & 0xF8000));
		if ((int)tmp == -1)
		{
			fprintf(stderr, "%s: Failed to mmap() %s, addr=%x\n",
				MyName, DEV_MEM, (int)Base);
			return(-1);
		}
		if (*tmp != (Word)0xAA55)
		{
			fprintf(stderr, 
				"%s: BIOS sanity check failed, addr=%x\n",
				MyName, (int)Base);
			return(-1);
		}
		(void)munmap((caddr_t)tmp, 0x1000);
	}
	ptr = (Byte *)mmap((caddr_t)0, mmap_len, PROT_READ, MAP_SHARED, 
		   	   BIOS_fd, (off_t)mmap_base);
	if ((int)ptr == -1)
	{
		fprintf(stderr, "%s: Failed to mmap() %s, addr=%x\n",
			MyName, DEV_MEM, (int)Base);
		return(-1);
	}
	(void)memcpy((void *)Buffer, (void *)(ptr + page_offset), Len);
	(void)munmap((caddr_t)ptr, mmap_len);
	return(Len);
#endif /* SVR4 */
}

/*
 * EnableIOPort --
 *
 * Enable access to 'NumPorts' IO ports listed in array 'Ports'.  For SYSV, 
 * we've disabled IO protections so this is a no-op.
 */
/*ARGSUSED*/
int EnableIOPorts(NumPorts, Ports)
CONST int NumPorts;
CONST Word *Ports;
{
	return(0);
}

/*
 * DisableIOPort --
 *
 * Disable access to 'NumPorts' IO ports listed in array  'Ports'.  For SYSV, 
 * we've disabled IO protections so this is a no-op.
 */
/*ARGSUSED*/
int DisableIOPorts(NumPorts, Port)
CONST int NumPorts;
CONST Word *Port;
{
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
	syscall(3112, Delay);
}
