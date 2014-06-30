/* $XConsortium: OS_Os2.c /main/7 1996/10/27 11:46:26 kaleb $ */
/*
 * (c) Copyright 1994 by Holger Veit
 *			<Holger.Veit@gmd.de>
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

/* $XFree86: xc/programs/Xserver/hw/xfree86/SuperProbe/OS_Os2.c,v 3.6.2.1 1997/05/06 13:24:24 dawes Exp $ */


#include "Probe.h"

#define INCL_32
#define INCL_DOS
#include <os2.h>

#define UNUSED (Byte*)0xffffffffl

HFILE consFd = -1;

ULONG action;
char *videoDrvPath = "/dev/pmap$";
char *ioDrvPath = "/dev/fastio$";

Byte* videoAddr = UNUSED;
ULONG videoSize = 0x10000;
int ioenabled = FALSE;

static void sorry()
{
	fprintf(stderr,
		"Didn't find a properly installed XF86SUP.SYS at your system\n");
	fprintf(stderr,"Please install it before retrying\n");
}

/*
 * OpenVideo --
 *
 * Enable access to the installed video hardware.  For OS/2, we take
 * advantage of the driver xf86sup.sys which was specifically written
 * for XFree86/OS2.
 * 
 * At this point we also try to weed out systems with outdated EMX code.
 */
int OpenVideo()
{
	ULONG rc;
	HMODULE hmod;
	char name[CCHMAXPATH];
	char fail[9];

	if (DosOpen((PSZ)videoDrvPath, (PHFILE)&consFd, (PULONG)&action,
	   (ULONG)0, FILE_SYSTEM, FILE_OPEN,
	   OPEN_SHARE_DENYNONE|OPEN_FLAGS_NOINHERIT|OPEN_ACCESS_READONLY,
	   (ULONG)0) != 0) {
		sorry();
		exit(2);
	}

	/* check for correct EMX */
	if (_emx_rev < 50) {
		fputs ("This program requires emx.dll revision 50 (0.9c) "
			"or later.\n", stderr);
		rc = DosLoadModule (fail, sizeof (fail), "emx", &hmod);
		if (rc == 0) {
			rc = DosQueryModuleName (hmod, sizeof (name), name);
			if (rc == 0)
				fprintf (stderr, "Please delete or update `%s'.\n", name);
			DosFreeModule (hmod);
		}
		exit (2);
	}
	return 1;
}

/* this structure is used as a parameter packet for the direct access
 * to physical memory
 */
typedef struct {
	ULONG	addr;	/* PHYSADDR for map, VIRTADDR for unmap */
	ULONG	size;
} DIOParPkt;

/* this structure is used as a return value for the direct access ioctl
 * to physical memory
 */
typedef struct {
	ULONG	addr;
	USHORT	sel;
} DIODtaPkt;

static Byte* UnmapPhys(Byte* addr,ULONG size)
{
	DIOParPkt	par;
	ULONG		plen;

	par.addr	= (ULONG)addr;
	par.size	= size;
	plen 		= sizeof(par);

	if (consFd != -1)
		DosDevIOCtl(consFd, (ULONG)0x76, (ULONG)0x45,
		   (PVOID)&par, (ULONG)plen, (PULONG)&plen,
		   NULL, 0, NULL);
	return UNUSED; /* no error */
}

/*
 * CloseVideo --
 *
 * Disable access to the video hardware.
 */
void CloseVideo()
{
	if (videoAddr != UNUSED)
		videoAddr = UnmapPhys(videoAddr,videoSize);

	DosClose(consFd);
}

/*
 * MapVGA --
 *
 * Map the VGA memory window (0xA0000-0xAFFFF) as read/write memory for
 * the process for use in probing memory.
 */
Byte *MapVGA()
{
	DIOParPkt	par;
	ULONG		plen;
	DIODtaPkt	dta;
	ULONG		dlen;

	par.addr	= 0xa0000l;
	par.size	= videoSize;
	plen 		= sizeof(par);

	if (consFd != -1 &&
	    DosDevIOCtl(consFd, (ULONG)0x76, (ULONG)0x44,
	       (PVOID)&par, (ULONG)plen, (PULONG)&plen,
	       (PVOID)&dta, (ULONG)6, (PULONG)&dlen) == 0) {
		if (dlen==6) {
			videoAddr = (Byte*)dta.addr;
			return videoAddr;
		}
	} else {
		fprintf(stderr, "%s: Failed to map framebuffer\n", MyName);
		return (Byte*)0;
	}
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
	UnmapPhys(videoAddr,videoSize);
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
	struct {
		ULONG cmd;
		ULONG phys;
		USHORT len;
	} par;
	UCHAR	*dta;
	ULONG	plen,dlen;
	int 	i;

	par.cmd 	= 0;
	par.phys 	= (ULONG)Bios_Base+(Offset & 0xffff8000);
	par.len 	= (Offset & 0x7fff) + Len;
	plen 		= sizeof(par);

	dta		= (UCHAR*)malloc(par.len);
	dlen 		= Len;

	if (DosDevIOCtl(consFd, (ULONG)0x76, (ULONG)0x64,
	   (PVOID)&par, (ULONG)plen, (PULONG)&plen,
	   (PVOID)dta, (ULONG)dlen, (PULONG)&dlen)) {
		fprintf(stderr, "%s: BIOS map failed, addr=%lx\n",
			MyName, Bios_Base+Offset);
		return -1;
	}

	/*
 	 * Sanity check...
 	 */
	if ((Offset & 0x7fff) != 0 && 
		(dta[0] != 0x55 || dta[1] != 0xaa)) {
		fprintf(stderr, 
			"%s: BIOS sanity check failed, addr=%x\n",
			MyName, Bios_Base+Offset);
		return -1;
	}

	memcpy(Buffer,dta + (Offset & 0x7fff), Len);
	free(dta);
	return(Len);
}

/*
 * EnableIOPort --
 *
 * Enable access to 'NumPorts' IO ports listed in array 'Ports'.
 */
USHORT callgate[3] = {0,0,0};

/*ARGSUSED*/
int EnableIOPorts(NumPorts, Ports)
CONST int NumPorts;
CONST Word *Ports;
{
	HFILE hfd;
	ULONG dlen;

	/* no need to call multiple times */
	if (ioenabled) return 0;
	
	if (DosOpen((PSZ)ioDrvPath, (PHFILE)&hfd, (PULONG)&action,
	   (ULONG)0, FILE_SYSTEM, FILE_OPEN,
	   OPEN_SHARE_DENYNONE|OPEN_FLAGS_NOINHERIT|OPEN_ACCESS_READONLY,
	   (ULONG)0) != 0) {
		sorry();
		return -1;
	}
	callgate[0] = callgate[1] = 0;

	if (DosDevIOCtl(hfd, (ULONG)0x76, (ULONG)0x64,
		NULL, 0, NULL,
		(ULONG*)&callgate[2], sizeof(USHORT), &dlen) != 0) {
		sorry();
		DosClose(hfd);
		return -1;
	}

	asm volatile ("movl $13,%%ebx;.byte 0xff,0x1d;.long _callgate"
			: /*no outputs */ 
			: /*no inputs */
			: "eax","ebx","ecx","edx","cc");

	ioenabled = TRUE;
	return 0;
}

/*
 * DisableIOPort --
 *
 * Disable access to 'NumPorts' IO ports listed in array  'Ports'.
 * This is a no op here: I/O access is implicitly enabled for this 
 * process through a special library.
 */
/*ARGSUSED*/
int DisableIOPorts(NumPorts, Ports)
CONST int NumPorts;
CONST Word *Ports;
{
	return(0);
}

/*
 * ShortSleep --
 *
 * Sleep for the number of milliseconds specified in 'Delay'.
 * 
 */
void ShortSleep(Delay)
int Delay;
{
	DosSleep(Delay ? Delay : 1);
}

