/* $XFree86: xc/programs/Xserver/hw/xfree86/os-support/solx86/solx86_mmap.c,v 3.1 1996/12/23 06:51:19 dawes Exp $ */
/*
 * Copyright 1993 by David Wexelblat <dwex@XFree86.org>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of David Wexelblat not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  David Wexelblat makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * DAVID WEXELBLAT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL DAVID WEXELBLAT BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */
/* $XConsortium: solx86_mmap.c /main/3 1996/02/21 17:54:17 kaleb $ */

#include "X.h"
#include "input.h"
#include "scrnintstr.h"

#include "xf86.h"
#include "xf86Priv.h"
#include "xf86_OSlib.h"

/*
 * Read BIOS via mmap()ing /dev/pmem.
 */
int xf86ReadBIOS(Base, Offset, Buf, Len)
unsigned long Base;
unsigned long Offset;
unsigned char *Buf;
int Len;
{
	int fd;
	unsigned char *ptr;
	char	solx86_vtname[10];

	/*
     	 * Solaris 2.1 x86 SVR4 (10/27/93)
     	 *      The server must treat the virtual terminal device file
     	 *      as the standard SVR4 /dev/pmem. By default, then used VT
     	 *      is considered the "default" file to open.
     	 */
        sprintf(solx86_vtname,"/dev/vt%02d",xf86Info.vtno);
	if ((fd = open(solx86_vtname, O_RDONLY)) < 0)
    	{
        	ErrorF("xf86ReadBios: Failed to open %s (%s)\n", solx86_vtname,
               		strerror(errno));
        	return(-1);
	}	
	ptr = mmap((caddr_t)0, 0x8000, PROT_READ, MAP_SHARED, fd, (off_t)Base);
	if ((int)ptr == -1)
	{
		ErrorF("xf86ReadBios: %s mmap failed\n", solx86_vtname);
		close(fd);
		return(-1);
	}
	(void)memcpy(Buf, (void *)(ptr + Offset), Len);
	(void)munmap((caddr_t)ptr, 0x8000);
	(void)close(fd);
	return(Len);
}
