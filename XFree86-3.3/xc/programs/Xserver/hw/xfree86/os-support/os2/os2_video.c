/* $XFree86: xc/programs/Xserver/hw/xfree86/os-support/os2/os2_video.c,v 3.10 1997/01/27 06:58:08 dawes Exp $ */
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
/* $XConsortium: os2_video.c /main/8 1996/10/27 11:49:02 kaleb $ */

#include "X.h"
#include "input.h"
#include "scrnintstr.h"

#define I_NEED_OS2_H
#define INCL_DOSFILEMGR
#include "xf86.h"
#include "xf86Priv.h"
#include "xf86_OSlib.h"

/***************************************************************************/
/* Video Memory Mapping helper functions                                   */
/***************************************************************************/

/* This section uses the xf86sup.sys driver developed for xfree86.
 * The driver allows mapping of physical memory
 * You must install it with a line DEVICE=path\xf86sup.sys in config.sys.
 */

static HFILE mapdev = -1;
static ULONG stored_virt_addr;
static char* mappath = "\\DEV\\PMAP$";
static HFILE open_mmap() 
{
	APIRET rc;
	ULONG action;

	if (mapdev != -1)
		return mapdev;

	rc = DosOpen((PSZ)mappath, (PHFILE)&mapdev, (PULONG)&action,
	   (ULONG)0, FILE_SYSTEM, FILE_OPEN,
	   OPEN_SHARE_DENYNONE|OPEN_FLAGS_NOINHERIT|OPEN_ACCESS_READONLY,
	   (ULONG)0);
	if (rc!=0)
		mapdev = -1;
	return mapdev;
}

static void close_mmap()
{
	if (mapdev != -1)
		DosClose(mapdev);
	mapdev = -1;
}

/* this structure is used as a parameter packet for the direct access
 * ioctl of pmap$
 */

/* Changed here for structure of driver PMAP$ */

typedef struct{
	ULONG addr;
	ULONG size;
	} DIOParPkt;

/* This is the data packet for the mapping function */

typedef struct {
	ULONG addr;
	USHORT sel;
	} DIODtaPkt;

/***************************************************************************/
/* Video Memory Mapping section                                            */
/***************************************************************************/

/* ARGSUSED */
pointer xf86MapVidMem(ScreenNum, Region, Base, Size)
int ScreenNum;
int Region;
pointer Base;
unsigned long Size;
{
	DIOParPkt	par;
	ULONG		plen;
	DIODtaPkt	dta;
	ULONG		dlen;
	static BOOL	ErrRedir = FALSE;
	APIRET		rc;

	par.addr	= (ULONG)Base;
	par.size	= (ULONG)Size;
	plen 		= sizeof(par);
	dlen		= sizeof(dta);

	/* First, redirect stderr to file so that video calls do not block */
	if (!ErrRedir) {
		/* hv300996 create redirect file on boot drive, instead 
		 * anywhere you are just standing
		 */
		char buf[20],dr[3];
		ULONG drive;
		APIRET rc = DosQuerySysInfo(5,5,&drive,sizeof(drive));
		if (rc) dr[0] = 0;
		else {	dr[0] = drive+96;
			dr[1] = ':';
			dr[2] = 0;
		}
		sprintf(buf,"%s\\xf86log.os2",dr);
		freopen(buf,"w",stderr); 
		ErrRedir=TRUE; 
	}

	open_mmap();
	if (mapdev == -1)
		FatalError("xf86MapVidMem: install DEVICE=path\\XF86SUP.SYS!");

	if ((rc=DosDevIOCtl(mapdev, (ULONG)0x76, (ULONG)0x44,
	      (PVOID)&par, (ULONG)plen, (PULONG)&plen,
	      (PVOID)&dta, (ULONG)dlen, (PULONG)&dlen)) == 0) {
		ErrorF("xf86-OS/2: xf86MapVidMem succeeded: (ScreenNum= %d, Base= %p, Size= 0x%x\n",
		ScreenNum, Base, Size);
		if (dlen==sizeof(dta)) {
			return (pointer)dta.addr;
		}
		/*else fail*/
	}

	/* fail */
	FatalError("xf86-OS/2: xf86MapVidMem FAILED!!: rc = %d (ScreenNum= %d, Base= %p, Size= 0x%x return len %d\n",
		rc, ScreenNum, Base, Size,dlen);
	return (pointer)0;
}

/* ARGSUSED */
void xf86UnMapVidMem(ScreenNum, Region, Base, Size)
int ScreenNum;
int Region;
pointer Base;
unsigned long Size;
{
	DIOParPkt	par;
	ULONG		plen,vmaddr;

/* We need here the VIRTADDR for unmapping, not the physical address      */
/* This should be taken care of either here by keeping track of allocated */
/* pointers, but this is also already done in the driver... Thus it would */
/* be a waste to do this tracking twice. Can this be changed when the fn. */
/* is called? This would require tracking this function in all servers,   */
/* and changing it appropriately to call this with the virtual adress	  */
/* If the above mapping function is only called once, then we can store   */
/* the virtual adress and use it here.... 				  */
	
	par.addr	= (ULONG)Base;
	par.size	= 0xffffffff; /* This is the virtual addres parameter. Set this to ignore */
	plen 		= sizeof(par);

	if (mapdev != -1)
	    DosDevIOCtl(mapdev, (ULONG)0x76, (ULONG)0x46,
	      (PVOID)&par, (ULONG)plen, (PULONG)&plen,
	      &vmaddr, sizeof(ULONG), &plen);
        ErrorF("xf86-OS/2: Unmapping physical memory at base %x, virtual address %x\n",Base,vmaddr);

/* Now if more than one region has been allocated and we close the driver, *
 * the other pointers will immediately become invalid. We avoid closing    *
 * driver for now, but this should be fixed for server exit                               */
 
	/* close_mmap(); */
}

Bool xf86LinearVidMem()
{
	/* setting it to true needs further testing */
	/* But what the heck, that's what we are here for! */
	return(TRUE);
}

/***************************************************************************/
/* Interrupt Handling section                                              */
/***************************************************************************/

Bool xf86DisableInterrupts()
{
	/* allow interrupt disabling but check for side-effects. Not a good policy on OS/2...*/
        asm ("cli");
	return(TRUE);
}

void xf86EnableInterrupts()
{
	/*Reenable*/
        asm ("sti");
	return;
}
