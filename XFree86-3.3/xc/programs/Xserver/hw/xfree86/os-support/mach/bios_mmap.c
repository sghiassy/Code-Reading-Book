/* $XFree86: xc/programs/Xserver/hw/xfree86/os-support/mach/bios_mmap.c,v 3.4 1996/12/23 06:50:10 dawes Exp $ */
/*
 * Copyright 1992 by Robert Baron <Robert.Baron@ernst.mach.cs.cmu.edu>
 * Copyright 1993 by David Wexelblat <dwex@goblin.org>
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
/* $XConsortium: bios_mmap.c /main/4 1996/02/21 17:51:47 kaleb $ */

#include "X.h"
#include "misc.h"

#define DevicePtr int
#include "xf86_OSlib.h"

#define BIOS_SIZE 0x20000
#define KERN_SUCCESS 0

int xf86ReadBIOS(Base, Offset, Buf, Len)
unsigned long Base;
unsigned long Offset;
unsigned char *Buf;
int Len;
{
	int fd;
	int screen_addr;
	int ret;

	if ((fd = open("/dev/iopl", O_RDWR, 0)) < 0)
	{
		ErrorF("xf86ReadBIOS: Failed to open /dev/iopl\n");
		return(-1);
	}
	if (KERN_SUCCESS != vm_allocate(task_self(), &screen_addr, 
					BIOS_SIZE, TRUE))
	{
		ErrorF("xf86ReadBIOS: Failed vmallocate %x\n", BIOS_SIZE);
		close(fd);
		return(-1);
	}
	if (mmap(screen_addr, BIOS_SIZE, 3, 1, fd, Base) < 0) 
	{
		ErrorF("xf86ReadBIOS: Failed to mmap %x at %x\n", 
		       BIOS_SIZE, Base);
		vm_deallocate(task_self(), screen_addr, BIOS_SIZE);
		close(fd);
		return(-1);
	}
	memcpy(Buf, (unsigned char *)(screen_addr + Offset), Len);
	if (KERN_SUCCESS != vm_deallocate(task_self(), screen_addr, BIOS_SIZE))
	{
		ErrorF("xf86ReadBIOS: Failed vmdeallocate %x\n", BIOS_SIZE);
		close(fd);
		return(-1);
	}
	close(fd);
	return(Len);
}
