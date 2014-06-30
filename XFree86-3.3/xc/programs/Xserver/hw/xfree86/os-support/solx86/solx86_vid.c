/* $XFree86: xc/programs/Xserver/hw/xfree86/os-support/solx86/solx86_vid.c,v 3.4.2.1 1997/05/21 15:02:42 dawes Exp $ */
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
/* $XConsortium: solx86_vid.c /main/4 1996/02/21 17:54:20 kaleb $ */

#include "X.h"
#include "input.h"
#include "scrnintstr.h"

#define _NEED_SYSI86
#include "xf86.h"
#include "xf86Priv.h"
#include "xf86_OSlib.h"

/***************************************************************************/
/* Video Memory Mapping section                                            */
/***************************************************************************/

struct kd_memloc MapDSC[MAXSCREENS][NUM_REGIONS];
pointer AllocAddress[MAXSCREENS][NUM_REGIONS];
static char *apertureDevName;

Bool xf86LinearVidMem()
{

#ifdef HAS_APERTURE_DRV
	int	mmapFd;

	apertureDevName = "/dev/xsvc";
	if ((mmapFd = open(apertureDevName, O_RDWR)) < 0) 
	{
	    apertureDevName = "/dev/fbs/aperture";
	    if((mmapFd = open(apertureDevName, O_RDWR)) < 0)
	    {
		ErrorF("xf86LinearVidMem: failed to open "
		       "%s (%s)\n", apertureDevName, strerror(errno));
		ErrorF("xf86LinearVidMem: 'aperture'"
		       " device driver required\n");
		ErrorF("xf86LinearVidMem: linear memory access disabled\n");
		return FALSE;
	    } 
	}
	close(mmapFd);
	return TRUE;
#else
	return FALSE;
#endif

}

pointer xf86MapVidMem(ScreenNum, Region, Base, Size)
int ScreenNum;
int Region;
pointer Base;
unsigned long Size;
{
	pointer base;
	int fd;
	char solx86_vtname[20];

	/* 
	 * Solaris 2.1 x86 SVR4 (10/27/93)
	 *		The server must treat the virtual terminal device file 
	 *		as the standard SVR4 /dev/pmem. 
	 * 
	 * Uning the /dev/vtXX device as /dev/pmem only works for the
	 * A0000-FFFFF region - If we wish you mmap the linear aperture
 	 * it requires a device driver.
 	 * 
	 * So what we'll do is use /dev/vtXX for the A0000-FFFFF stuff, and
	 * try to use the /dev/fbs/aperture driver if the server
	 * tries to mmap anything > FFFFF, and HAS_APERTURE_DRV is
	 * defined. (its very very unlikely that the server will try to mmap
	 * anything below FFFFF that can't be handled by /dev/vtXX.
	 * If the server tries to mmap anything above FFFFF, and
	 * HAS_APERTURE_DRV the server will die.
	 * 
	 * DWH - 2/23/94
	 */

	if(Base < (pointer)0xFFFFF)
		sprintf(solx86_vtname,"/dev/vt%02d",xf86Info.vtno);
	else

#ifdef HAS_APERTURE_DRV
		sprintf(solx86_vtname, apertureDevName);
#else
		FatalError("%s: Could not mmap framebuffer [s=%x,a=%x] (%s)\n",
			   "xf86MapVidMem", Size, Base,
			   "aperture driver unavailable");
#endif

	if ((fd = open(solx86_vtname, O_RDWR,0)) < 0)
	{
		FatalError("xf86MapVidMem: failed to open %s (%s)\n",
			   solx86_vtname, strerror(errno));
	}
	base = (pointer)mmap((caddr_t)0, Size, PROT_READ|PROT_WRITE,
			     MAP_SHARED, fd, (off_t)Base);
	close(fd);
	if ((long)base == -1)
	{
		FatalError("%s: Could not mmap framebuffer [s=%x,a=%x] (%s)\n",
			   "xf86MapVidMem", Size, Base, strerror(errno));
	}
	return((pointer)base);
}

/* ARGSUSED */
void xf86UnMapVidMem(ScreenNum, Region, Base, Size)
int ScreenNum;
int Region;
pointer Base;
unsigned long Size;
{
	munmap(Base, Size);
}

/* ARGSUSED */
void xf86MapDisplay(ScreenNum, Region)
int ScreenNum;
int Region;
{
	return;
}

/* ARGSUSED */
void xf86UnMapDisplay(ScreenNum, Region)
int ScreenNum;
int Region;
{
	return;
}

/***************************************************************************/
/* I/O Permissions section                                                 */
/***************************************************************************/

static unsigned *EnabledPorts[MAXSCREENS];
static int NumEnabledPorts[MAXSCREENS];
static Bool ScreenEnabled[MAXSCREENS];
static Bool ExtendedPorts[MAXSCREENS]; /* Not used, but leave it for now to
					  to keep xf86InitPortLists() happy */
static Bool ExtendedEnabled = FALSE;
static Bool InitDone = FALSE;

void xf86ClearIOPortList(ScreenNum)
int ScreenNum;
{
	int i;

	if (!InitDone)
	{
		xf86InitPortLists(EnabledPorts, NumEnabledPorts, ScreenEnabled,
				  ExtendedPorts, MAXSCREENS);
		InitDone = TRUE;
		return;
	}
	if (EnabledPorts[ScreenNum] != (unsigned *)NULL)
		xfree(EnabledPorts[ScreenNum]);
	EnabledPorts[ScreenNum] = (unsigned *)NULL;
	NumEnabledPorts[ScreenNum] = 0;
}

void xf86AddIOPorts(ScreenNum, NumPorts, Ports)
int ScreenNum;
int NumPorts;
unsigned *Ports;
{
	int i;

	if (!InitDone)
	{
	    FatalError("xf86AddIOPorts: I/O control lists not initialised\n");
	}
	EnabledPorts[ScreenNum] = (unsigned *)xrealloc(EnabledPorts[ScreenNum], 
			(NumEnabledPorts[ScreenNum]+NumPorts)*sizeof(unsigned));
	for (i = 0; i < NumPorts; i++)
	{
		EnabledPorts[ScreenNum][NumEnabledPorts[ScreenNum] + i] =
								Ports[i];
	}
	NumEnabledPorts[ScreenNum] += NumPorts;
}

void xf86EnableIOPorts(ScreenNum)
int ScreenNum;
{

	if (ScreenEnabled[ScreenNum])
		return;

	ScreenEnabled[ScreenNum] = TRUE;

	if (sysi86(SI86V86, V86SC_IOPL, PS_IOPL) < 0)
	{
		FatalError("%s: Failed to set IOPL for I/O\n",
			   "xf86EnableIOPorts");
	}

	ExtendedEnabled = TRUE;
	return;
}

void xf86DisableIOPorts(ScreenNum)
int ScreenNum;
{
	int i;

	if (!ScreenEnabled[ScreenNum])
		return;

	ScreenEnabled[ScreenNum] = FALSE;

	if(!ExtendedEnabled)
		return;

	for (i = 0; i < MAXSCREENS; i++)
	{
		if (ScreenEnabled[i])
			return;
	}

	sysi86(SI86V86, V86SC_IOPL, 0);

	ExtendedEnabled = FALSE;

	return;
}

void xf86DisableIOPrivs()
{
	if (ExtendedEnabled)
		sysi86(SI86V86, V86SC_IOPL, 0);
	return;
}

/***************************************************************************/
/* Interrupt Handling section                                              */
/***************************************************************************/

Bool xf86DisableInterrupts()
{
	if (!ExtendedEnabled)
	{
		if (sysi86(SI86V86, V86SC_IOPL, PS_IOPL) < 0)
		{
			return(FALSE);
		}
	}

#ifdef __GNUC__
	__asm__ __volatile__("cli");
#else 
	asm("cli");
#endif /* __GNUC__ */

	if (!ExtendedEnabled)
	{
		sysi86(SI86V86, V86SC_IOPL, 0);
	}
	return(TRUE);
}

void xf86EnableInterrupts()
{
	if (!ExtendedEnabled)
	{
		if (sysi86(SI86V86, V86SC_IOPL, PS_IOPL) < 0)
		{
			return;
		}
	}

#ifdef __GNUC__
	__asm__ __volatile__("sti");
#else 
	asm("sti");
#endif /* __GNUC__ */

	if (!ExtendedEnabled)
	{
		sysi86(SI86V86, V86SC_IOPL, 0);
	}
	return;
}
