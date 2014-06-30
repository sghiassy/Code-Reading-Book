/* $XFree86: xc/programs/Xserver/hw/xfree86/SuperProbe/OS_LynxOS.c,v 3.4.2.1 1997/05/06 13:24:17 dawes Exp $ */
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
/* $XConsortium: OS_LynxOS.c /main/5 1996/02/21 17:11:07 kaleb $ */

#include "Probe.h"

#include <fcntl.h>
#include <conf.h>
#include <kd.h>
#include <vt.h>
#include <smem.h>

#include <kernel.h>		/* for PHYSBASE */

static int BIOS_fd = -1;

/*
 * OpenVideo --
 *
 * Enable access to the installed video hardware. 
 * For LynxOS just a check for super-user permissions.
 */
#ifdef __STDC__
int OpenVideo(void)
#else
int OpenVideo()
#endif
{
	if (geteuid() != 0)
	{
		fprintf(stderr,
			"%s: Must be run as root\n",
			MyName);
		return(-1);
	}

	return(1);
}

/*
 * CloseVideo --
 *
 * Disable access to the video hardware.
 * For Lynxos a no-op.
 */
#ifdef __STDC__
void CloseVideo(void)
#else
void CloseVideo()
#endif
{
}

/*
 * MapVGA --
 *
 * Map the VGA memory window (0xA0000-0xAFFFF) as read/write memory for
 * the process for use in probing memory.
 */
Byte *MapVGA()
{
	return(MapMem(0xa0000,0x10000));
}

Byte *MapMem(address,size)
	unsigned long address;
	unsigned long size;
{
#define SMEM_NAME	"SuperProbe-VGA"
	Byte *base;

	base = (Byte *) smem_create(SMEM_NAME, (char *)address,
		 size, SM_READ|SM_WRITE);
	if ((long)base == -1)
	{
                fprintf(stderr, "%s: Failed to mmap framebuffer\n", MyName);
		return((Byte *)0);
	}
	return(base);
}

/*
 * UnMapVGA --
 *
 * Unmap the VGA memory window.
 */
void UnMapVGA(base)
Byte *base;
{
	UnMapMem(base,0x10000);
	return;
}

void UnMapMem(base,size)
	Byte *base;
	unsigned long size;
{
	smem_create(NULL, (char *)base, 0, SM_DETACH);
	smem_remove(SMEM_NAME);
	return;
}

/*
 * ReadBIOS --
 *
 * Read 'Len' bytes from the video BIOS at address 'Bios_Base'+'Offset' into 
 * buffer 'Buffer'.
 */
#ifdef __STDC__
int ReadBIOS(const unsigned Offset, Byte *Buffer, const int Len)
#else
int ReadBIOS(Offset, Buffer, Len)
unsigned Offset;
Byte *Buffer;
int Len;
#endif
{
	Word tmp;
	Byte *Base = Bios_Base + Offset;

	if (BIOS_fd == -1)
	{
		if ((BIOS_fd = open("/dev/mem", O_RDONLY, 0)) < 0)
		{
			fprintf(stderr, "%s: cannot open /dev/mem\n", MyName);
			return(-1);
		}
	}
	if ((off_t)((off_t)Base & 0x7FFF) != (off_t)0)
	{
		/*
	 	 * Sanity check...
	 	 */
		(void)lseek(BIOS_fd, (off_t)PHYSBASE + ((off_t)Base & 0xF8000), SEEK_SET);
		(void)read(BIOS_fd, &tmp, 2);
		if (tmp != (Word)0xAA55)
		{
			fprintf(stderr, 
				"%s: BIOS sanity check failed, addr=%x\n",
				MyName, (int)Base);
			fprintf(stderr, 
				"%s: BIOS sanity check failed, Seek to =%x, Read Word %04x\n",
				MyName, (int)Base & 0xF8000, tmp);
			return(-1);
		}
	}
	/* check carefully against -1 because of PHYSBASE offset..	*/
	if (lseek(BIOS_fd, (off_t)(PHYSBASE + Base), SEEK_SET) == -1)
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
 * Enable access to 'NumPorts' IO ports listed in array 'Ports'.
 * For LynxOS this is a no-op.
 */
#ifdef __STDC__
/*ARGSUSED*/
int EnableIOPorts(const int NumPorts, const Word *Ports)
#else
/*ARGSUSED*/
int EnableIOPorts(NumPorts, Ports)
int NumPorts;
Word *Ports;
#endif
{
	return(0);
}

/*
 * DisableIOPort --
 *
 * Disable access to 'NumPorts' IO ports listed in array  'Ports'.
 * For LynxOS this is a no-op.
 */
#ifdef __STDC__
/*ARGSUSED*/
int DisableIOPorts(const int NumPorts, const Word *Port)
#else
/*ARGSUSED*/
int DisableIOPorts(NumPorts, Port)
int NumPorts;
Word *Port;
#endif
{
	return(0);
}

/*
 * ShortSleep --
 *
 * Sleep for the number of milliseconds specified in 'Delay'.
 */
#ifdef __STDC__
void ShortSleep(const int Delay)
#else
void ShortSleep(Delay)
int Delay;
#endif
{
	usleep(Delay * 1000);
}
