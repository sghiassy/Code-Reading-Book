/* $XFree86: xc/programs/Xserver/hw/xfree86/os-support/mach/mach_video.c,v 3.1 1996/12/23 06:50:12 dawes Exp $ */
/*
 * Copyright 1992 by Robert Baron <Robert.Baron@ernst.mach.cs.cmu.edu>
 * Copyright 1993 by David Wexelblat <dwex@XFree86.org>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Robert Baron and David Wexelblat not 
 * be used in advertising or publicity pertaining to distribution of the 
 * software without specific, written prior permission.  Robert Baron and 
 * David Wexelblat make no representations about the suitability of this 
 * software for any purpose.  It is provided "as is" without express or 
 * implied warranty.
 *
 * ROBERT BARON AND DAVID WEXELBLAT DISCLAIMS ALL WARRANTIES WITH REGARD TO 
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS, IN NO EVENT SHALL ROBERT BARON OR DAVID WEXELBLAT BE LIABLE FOR 
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER 
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF 
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */
/* $XConsortium: mach_video.c /main/3 1996/02/21 17:51:57 kaleb $ */

#include "X.h"
#include "input.h"
#include "scrnintstr.h"

#include "xf86.h"
#include "xf86Priv.h"
#include "xf86_OSlib.h"

/***************************************************************************/
/* Video Memory Mapping section                                            */
/***************************************************************************/

#define KERN_SUCCESS 0

pointer xf86MapVidMem(ScreenNum, Region, Base, Size)
int ScreenNum;
int Region;
pointer Base;
unsigned long Size;
{
	pointer base;
	int fd;

	if ((fd = open("/dev/iopl", O_RDWR, 0)) < 0)
	{
		FatalError("xf86MapVidMem: Failed to get IOPL\n");
	}
	if (KERN_SUCCESS != vm_allocate(task_self(), &base, Size, TRUE))
	{
		FatalError("xf86MapVidMem: can't alloc framebuffer space\n");
	}

	if (mmap(base, Size, 3, 1, fd, Base) < 0)
	{
		vm_deallocate(task_self(), base, Size);
		close(fd);
		FatalError("xf86MapVidMem: Could not mmap frambuffer\n");
	}
	close(fd);
	return(base);
}

void xf86UnMapVidMem(ScreenNum, Region, Base, Size)
int ScreenNum;
int Region;
pointer Base;
unsigned long Size;
{
	if (KERN_SUCCESS != vm_deallocate(task_self(), Base, Size))
	{
		ErrorF("xf86UnMapVidMem: can't dealloc framebuffer space\n");
	}
}

Bool xf86LinearVidMem()
{
	return(TRUE);
}

/***************************************************************************/
/* I/O Permissions section                                                 */
/***************************************************************************/

/*
 * Mach disables I/O permissions completely, and OSF/1 doesn't bother 
 * with I/O permissions right now.
 */

#ifdef MACH386
static int ioplfd = -1;
static Bool ScreenEnabled[MAXSCREENS];
static Bool IOEnabled = FALSE;
static Bool InitDone = FALSE;
#endif

void xf86ClearIOPortList(ScreenNum)
int ScreenNum;
{
#ifdef MACH386
	int i;

	if (!InitDone)
	{
		for (i = 0; i < MAXSCREENS; i++)
			ScreenEnabled[i] = FALSE;
		InitDone = TRUE;
	}
#endif
	return;
}

/* ARGSUSED */
void xf86AddIOPorts(ScreenNum, NumPorts, Ports)
int ScreenNum;
int NumPorts;
unsigned *Ports;
{
	return;
}

void xf86EnableIOPorts(ScreenNum)
int ScreenNum;
{
#ifdef MACH386
	ScreenEnabled[ScreenNum] = TRUE;

	if (IOEnabled)
		return;

	if ((ioplfd = open("/dev/iopl", 0)) < 0)
		FatalError("xf86EnablePortIO: Failed to get IOPL for I/O\n");
	IOEnabled = TRUE;
#endif /* MACH386 */
	return;
}

void xf86DisableIOPorts(ScreenNum)
int ScreenNum;
{
#ifdef MACH386
	int i;

	ScreenEnabled[ScreenNum] = FALSE;

	if (!IOEnabled)
		return;

	for (i = 0; i < MAXSCREENS; i++)
		if (ScreenEnabled[i])
			return;
	close(ioplfd);
	ioplfd = -1;
	IOEnabled = FALSE;
#endif /* MACH386 */
	return;
}

void xf86DisableIOPrivs()
{
	/* XXXX Don't know what if anything to do here */
	return;
}

/***************************************************************************/
/* Interrupt Handling section                                              */
/***************************************************************************/

static Bool Mach30()
{
#ifdef MACH386
	if (syscall(-27) != 4)
	{
		return(TRUE);
	}
	else
	{
		return(FALSE);
	}
#else /* MACH386 */
	return(FALSE);
#endif /* MACH386 */
}

Bool xf86DisableInterrupts()
{
	if (Mach30())
	{
		ErrorF("Mach 3.0 does not allow interrupts to be disabled\n");
		return(FALSE);
	}
#ifdef __GNUC__
	__asm__ __volatile__("cli");
#else 
	asm("cli");
#endif /* __GNUC__ */

	return(TRUE);
}

void xf86EnableInterrupts()
{
#ifdef __GNUC__
	__asm__ __volatile__("sti");
#else 
	asm("sti");
#endif /* __GNUC__ */

	return;
}

