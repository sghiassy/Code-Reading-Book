/* $XFree86: xc/programs/Xserver/hw/xfree86/os-support/os2/os2_bios.c,v 3.6 1997/01/05 11:59:17 dawes Exp $ */
/*
 * (c) Copyright 1994 by Holger Veit
 *			<Holger.Veit@gmd.de>
 * Modified 1996 by Sebastien Marineau <marineau@genie.uottawa.ca>
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
 * HOLGER VEIT  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF 
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
 * SOFTWARE.
 * 
 * Except as contained in this notice, the name of Holger Veit shall not be
 * used in advertising or otherwise to promote the sale, use or other dealings
 * in this Software without prior written authorization from Holger Veit.
 *
 */
/* $XConsortium: os2_bios.c /main/5 1996/10/27 11:48:45 kaleb $ */

#include "X.h"
#include "input.h"
#include "scrnintstr.h"

#define I_NEED_OS2_H
#define INCL_32
#define INCL_DOS
#define INCL_DOSFILEMGR
#include "xf86.h"
#include "xf86Priv.h"
#include "xf86_OSlib.h"

/*
 * Read BIOS via xf86sup.SYS device driver
 */

#define Bios_Base 0

int xf86ReadBIOS(Base, Offset, Buf, Len)
unsigned long Base;
unsigned long Offset;
unsigned char *Buf;
int Len;
{
	HFILE fd;
	struct {
		ULONG command;
		ULONG physaddr;
		USHORT numbytes;
	} par;
	UCHAR	*dta;
	ULONG	plen,dlen;
	int 	i;
	ULONG	action;
	APIRET rc;
	ULONG Phys_address;


	Phys_address=Base+Offset;

	/* open the special device pmap$ (default with OS/2) */
	if (DosOpen((PSZ)"PMAP$", (PHFILE)&fd, (PULONG)&action,
	   (ULONG)0, FILE_SYSTEM, FILE_OPEN,
	   OPEN_SHARE_DENYNONE|OPEN_FLAGS_NOINHERIT|OPEN_ACCESS_READONLY,
	   (ULONG)0) != 0) {
		FatalError("xf86ReadBIOS: install DEVICE=path\\xf86sup.SYS!");
		return -1;
	}

	/* prepare parameter and data packets for ioctl */
	par.command 	= 0;
	par.physaddr 	= (ULONG)Bios_Base+(Phys_address & 0xffff8000);
	par.numbytes 	= (Phys_address & 0x7fff) + Len;
	plen 		= sizeof(par);

	dta		= (UCHAR*)xalloc(par.numbytes);
	dlen 		= par.numbytes;

	/* issue call to get a readonly copy of BIOS ROM */
	if (rc=DosDevIOCtl(fd, (ULONG)0x76, (ULONG)0x64,
	   (PVOID)&par, (ULONG)plen, (PULONG)&plen,
	   (PVOID)dta, (ULONG)dlen, (PULONG)&dlen)) {
		ErrorF("xf86ReadBIOS: BIOS map failed, addr=%lx, rc=%d\n", 
			Bios_Base+Phys_address,rc);
		free(dta);
		DosClose(fd);
		return -1;
	}

	/*
	 * Sanity check... No longer fatal, as some PS/1 and PS/2 fail here but still work.
	 * S. Marineau, 10/10/96
         */
	if ((Phys_address & 0x7fff) != 0 && 
		(dta[0] != 0x55 || dta[1] != 0xaa)) {
		ErrorF("BIOS sanity check failed, addr=%x\nPlease report if you encounter problems\n",
			Bios_Base+Phys_address);
	}

	/* copy data to buffer */
	memcpy(Buf,dta + (Phys_address & 0x7fff), Len);
	xfree(dta);

	/* close device */
	DosClose(fd);

	return(Len);
}
