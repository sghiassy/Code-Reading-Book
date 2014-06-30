/* $XFree86: xc/programs/Xserver/hw/xfree86/os-support/bsd/bsd_video.c,v 3.14.2.1 1997/05/03 09:47:11 dawes Exp $ */
/*
 * Copyright 1992 by Rich Murphey <Rich@Rice.edu>
 * Copyright 1993 by David Wexelblat <dwex@goblin.org>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Rich Murphey and David Wexelblat 
 * not be used in advertising or publicity pertaining to distribution of 
 * the software without specific, written prior permission.  Rich Murphey and
 * David Wexelblat make no representations about the suitability of this 
 * software for any purpose.  It is provided "as is" without express or 
 * implied warranty.
 *
 * RICH MURPHEY AND DAVID WEXELBLAT DISCLAIM ALL WARRANTIES WITH REGARD TO 
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS, IN NO EVENT SHALL RICH MURPHEY OR DAVID WEXELBLAT BE LIABLE FOR 
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER 
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF 
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
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

/* $XConsortium: bsd_video.c /main/10 1996/10/25 11:37:57 kaleb $ */

#include "X.h"
#include "input.h"
#include "scrnintstr.h"

#include "xf86.h"
#include "xf86Priv.h"
#include "xf86_OSlib.h"
#include "xf86_Config.h"

#ifdef __arm32__
#include "machine/devmap.h"
struct memAccess
{
    int ioctl;
    struct map_info memInfo;
    pointer regionVirtBase;
    Bool Checked;
    Bool OK;
};

static pointer xf86MapInfoMap();
static void xf86MapInfoUnmap();
static struct memAccess *checkMapInfo();
extern int vgaPhysLinearBase;

/* A memAccess structure is needed for each possible region */ 
struct memAccess vgaMemInfo = { CONSOLE_GET_MEM_INFO, NULL, NULL, 
				    FALSE, FALSE };
struct memAccess linearMemInfo = { CONSOLE_GET_LINEAR_INFO, NULL, NULL, 
				       FALSE, FALSE };
struct memAccess ioMemInfo = { CONSOLE_GET_IO_INFO, NULL, NULL,
				   FALSE, FALSE };
#endif __arm32__

#if defined(__NetBSD__) && !defined(MAP_FILE)
#define MAP_FILE 0
#endif

/***************************************************************************/
/* Video Memory Mapping section                                            */
/***************************************************************************/

#if !defined(__mips__) && !defined(__arm32__)
#define _386BSD_MMAP_BUG
#endif

#ifdef _386BSD_MMAP_BUG
/*
 * Bug prevents multiple mappings, so just map a fixed region between 0xA0000
 * and 0xBFFFF, and return a pointer to the requested Base.
 */
static int MemMapped = FALSE;
static pointer MappedPointer = NULL;
static int MapCount = 0;
#define MAP_BASE 0xA0000
#define MAP_SIZE 0x20000
#endif

static Bool devMemChecked = FALSE;
static Bool useDevMem = FALSE;
static int  devMemFd = -1;

#if 0
static struct xf86memMap {
  int offset;
  int memSize;
} xf86memMaps[MAXSCREENS];
#endif

#ifdef HAS_APERTURE_DRV
#define DEV_APERTURE "/dev/xf86"
#endif
#define DEV_MEM "/dev/mem"

/*
 * Check if /dev/mem can be mmap'd.  If it can't print a warning when
 * "warn" is TRUE.
 */
static void checkDevMem(warn)
Bool warn;
{
	int fd;
	pointer base;

	devMemChecked = TRUE;
	if ((fd = open(DEV_MEM, O_RDWR)) >= 0)
	{
	    /* Try to map a page at the VGA address */
	    base = (pointer)mmap((caddr_t)0, 4096, PROT_READ|PROT_WRITE,
				 MAP_FILE, fd, (off_t)0xA0000);
	
	    if (base != (pointer)-1)
	    {
		munmap((caddr_t)base, 4096);
		devMemFd = fd;
		useDevMem = TRUE;
		return;
	    } else {
		/* This should not happen */
		if (warn)
		{
		    ErrorF("%s checkDevMem: warning: failed to mmap %s (%s)\n",
			   XCONFIG_PROBED, DEV_MEM, strerror(errno));
		}
		useDevMem = FALSE;
		return;
	    }
	}
#ifndef HAS_APERTURE_DRV
	if (warn)
	{ 
	    ErrorF("%s checkDevMem: warning: failed to open %s (%s)\n",
		   XCONFIG_PROBED, DEV_MEM, strerror(errno));
	    ErrorF("\tlinear framebuffer access unavailable\n");
	} 
	useDevMem = FALSE;
	return;
#else
	/* Failed to open /dev/mem, try the aperture driver */
	if ((fd = open(DEV_APERTURE, O_RDWR)) >= 0)
	{
	    /* Try to map a page at the VGA address */
	    base = (pointer)mmap((caddr_t)0, 4096, PROT_READ|PROT_WRITE,
			     MAP_FILE, fd, (off_t)0xA0000);
	
	    if (base != (pointer)-1)
	    {
		munmap((caddr_t)base, 4096);
		devMemFd = fd;
		useDevMem = TRUE;
		ErrorF("%s checkDevMem: using aperture driver %s\n",
		       XCONFIG_PROBED, DEV_APERTURE);
		return;
	    } else {

		if (warn)
		{
		    ErrorF("%s checkDevMem: warning: failed to mmap %s (%s)\n",
			   XCONFIG_PROBED, DEV_APERTURE, strerror(errno));
		}
	    }
	} else {
	    if (warn)
	    {
		ErrorF("%s checkDevMem: warning: failed to open %s and %s\n\t(%s)\n",
		   XCONFIG_PROBED, DEV_MEM, DEV_APERTURE, strerror(errno));
	    }
	}
	
	if (warn)
	{
	    ErrorF("\tlinear framebuffer access unavailable\n");
	}
	useDevMem = FALSE;
	return;

#endif
}


pointer xf86MapVidMem(ScreenNum, Region, Base, Size)
int ScreenNum;
int Region;
pointer Base;
unsigned long Size;
{
	pointer base;

#ifdef __arm32__
	struct memAccess *memInfoP;
	
	if((memInfoP = checkMapInfo(FALSE, Region)) != NULL)
	{
	    /*
	     ** xf86 passes in a physical address offset from the start
	     ** of physical memory, but xf86MapInfoMap expects an 
	     ** offset from the start of the specified region - it gets 
	     ** the physical address of the region from the display driver.
	     */
	    switch(Region)
	    {
	        case LINEAR_REGION:
		    if (vgaPhysLinearBase)
		    {
			Base -= vgaPhysLinearBase;
		    }
		    break;
		case VGA_REGION:
		    Base -= 0xA0000;
		    break;
	    }
	    
	    base = xf86MapInfoMap(memInfoP, Base, Size);
	    return (base);
	}
#endif __arm32__

	if (!devMemChecked)
		checkDevMem(FALSE);

	if (useDevMem)
	{
	    if (devMemFd < 0) 
	    {
		FatalError("xf86MapVidMem: failed to open %s (%s)\n",
			   DEV_MEM, strerror(errno));
	    }
	    base = (pointer)mmap((caddr_t)0, Size, PROT_READ|PROT_WRITE,
				 MAP_FILE, devMemFd,
				 (off_t)(unsigned long) Base);
	    if (base == (pointer)-1)
	    {
		FatalError("%s: could not mmap %s [s=%x,a=%x] (%s)\n",
			   "xf86MapVidMem", DEV_MEM, Size, Base, 
			   strerror(errno));
	    }
#if 0
	    xf86memMaps[ScreenNum].offset = (int) Base;
	    xf86memMaps[ScreenNum].memSize = Size;
#endif
	    return(base);
	}
		
	/* else, mmap /dev/vga */
#ifdef _386BSD_MMAP_BUG
	if ((unsigned long)Base < MAP_BASE ||
	    (unsigned long)Base >= MAP_BASE + MAP_SIZE)
	{
		FatalError("%s: Address 0x%x outside allowable range\n",
			   "xf86MapVidMem", Base);
	}
	if ((unsigned long)Base + Size > MAP_BASE + MAP_SIZE)
	{
		FatalError("%s: Size 0x%x too large (Base = 0x%x)\n",
			   "xf86MapVidMem", Size, Base);
	}
	if (!MemMapped)
	{
		base = (pointer)mmap(0, MAP_SIZE, PROT_READ|PROT_WRITE,
				     MAP_FILE, xf86Info.screenFd, 0);
		if (base == (pointer)-1)
		{
		    FatalError("xf86MapVidMem: Could not mmap /dev/vga (%s)\n",
			       strerror(errno));
		}
		MappedPointer = base;
		MemMapped = TRUE;
	}
	MapCount++;
	return((pointer)((unsigned long)MappedPointer +
			 ((unsigned long)Base - MAP_BASE)));

#else
#ifndef PC98
	if ((unsigned long)Base < 0xA0000 || (unsigned long)Base >= 0xC0000)
#else
	if ((unsigned long)Base < 0xA0000 || (unsigned long)Base >= 0xE8000)
#endif
	{
		FatalError("%s: Address 0x%x outside allowable range\n",
			   "xf86MapVidMem", Base);
	}
	base = (pointer)mmap(0, Size, PROT_READ|PROT_WRITE, MAP_FILE,
			     xf86Info.screenFd,
#ifdef __mips__
			     (unsigned long)Base);
#else
			     (unsigned long)Base - 0xA0000);
#endif
	if (base == (pointer)-1)
	{
	    FatalError("xf86MapVidMem: Could not mmap /dev/vga (%s)\n",
		       strerror(errno));
	}
#if 0
	xf86memMaps[ScreenNum].offset = (int) Base;
	xf86memMaps[ScreenNum].memSize = Size;
	return(base);
#endif
#endif
}

#if 0
void xf86GetVidMemData(ScreenNum, Base, Size)
int ScreenNum;
int *Base;
int *Size;      
{              
   *Base = xf86memMaps[ScreenNum].offset;
   *Size = xf86memMaps[ScreenNum].memSize;
}
#endif
                             
void xf86UnMapVidMem(ScreenNum, Region, Base, Size)
int ScreenNum;
int Region;
pointer Base;
unsigned long Size;
{
#ifdef __arm32__
        struct memAccess *memInfoP;
	
	if((memInfoP = checkMapInfo(FALSE, Region)) != NULL)
	{
	    xf86MapInfoUnmap(memInfoP, Base, Size);
	}
#endif
	if (useDevMem)
	{
		munmap((caddr_t)Base, Size);
		return;
	}

#ifdef _386BSD_MMAP_BUG
	if (MapCount == 0 || MappedPointer == NULL)
		return;

	if (--MapCount == 0)
	{
		munmap((caddr_t)MappedPointer, MAP_SIZE);
		MemMapped = FALSE;
	}
#else
	munmap((caddr_t)Base, Size);
#endif
}

Bool xf86LinearVidMem()
{
	/*
	 * Call checkDevMem even if already called by xf86MapVidMem() so that
	 * a warning about no linear fb is printed.
	 */
        if (!useDevMem)
		checkDevMem(TRUE);

	return(useDevMem);
}

#ifdef __arm32__

/*
** Find out whether the console driver provides memory mapping information 
** for the specified region and return the map_info pointer. Print a warning if required.
*/
static struct memAccess *checkMapInfo(warn, Region)
Bool warn;
int Region;
{
    struct memAccess *memAccP;
        
    switch (Region)
    {
	case VGA_REGION:
	    memAccP = &vgaMemInfo;
	    break;
	    	    
	case LINEAR_REGION:
	    memAccP = &linearMemInfo;
	    break;
	    
	case MMIO_REGION:
	    memAccP = &ioMemInfo;
	    break;
	
	default:
	    return NULL;
	    break;
    }
    
    if(!memAccP->Checked)
    {	
	if(ioctl(xf86Info.screenFd, memAccP->ioctl, &(memAccP->memInfo)) == -1)
	{
	    if(warn)
	    {
		ErrorF("checkMapInfo: warning: failed to get map info for region %d\n\t(%s)\n",
		       Region, strerror(errno));
	    }
	}
	else
	{
	    if(memAccP->memInfo.u.map_info_mmap.map_offset 
	       != MAP_INFO_UNKNOWN)
		memAccP->OK = TRUE;
	}
	memAccP->Checked = TRUE;
    }
    if (memAccP->OK)
    {
	return memAccP;
    }
    else
    {
	return NULL;
    }
}

static pointer xf86MapInfoMap(memInfoP, Base, Size)
    struct memAccess *memInfoP;
    pointer Base;
    unsigned long Size;
{
    struct map_info *mapInfoP = &(memInfoP->memInfo);

    if (mapInfoP->u.map_info_mmap.map_size == MAP_INFO_UNKNOWN)
    {	
	Size = (unsigned long)Base + Size;
    }
    else
    {
	Size = mapInfoP->u.map_info_mmap.map_size;
    }
    
    switch(mapInfoP->method)
    {
	case MAP_MMAP:
	    /* Need to remap if size is unknown because we may not have
	       mapped the whole region initially */
	    if(memInfoP->regionVirtBase == NULL ||
	       mapInfoP->u.map_info_mmap.map_size == MAP_INFO_UNKNOWN)
	    {
		if((memInfoP->regionVirtBase = 
		    mmap((caddr_t)0,
			 Size,
			 PROT_READ|PROT_WRITE,
			 MAP_SHARED,
			 xf86Info.screenFd,
			 (unsigned long)mapInfoP->u.map_info_mmap.map_offset))
		   == (pointer)-1)
		{
		    FatalError("xf86MapInfoMap: Failed to map memory at 0x%x\n\t%s\n", 
			       mapInfoP->u.map_info_mmap.map_offset, strerror(errno));
		}
		if(mapInfoP->u.map_info_mmap.internal_offset > 0)
		    memInfoP->regionVirtBase += 
			mapInfoP->u.map_info_mmap.internal_offset;
	    }
	    break;
	    
	default:
	    FatalError("xf86MapInfoMap: Unsuported mapping method\n");
	    break;
    }
	    
    return (pointer)((int)memInfoP->regionVirtBase + (int)Base);
}

static void xf86MapInfoUnmap(memInfoP, Size)
    struct memAccess *memInfoP;
    unsigned long Size;
{
    struct map_info *mapInfoP = &(memInfoP->memInfo);
    
    switch(mapInfoP->method)
    {
	case MAP_MMAP:
	    if(memInfoP->regionVirtBase != NULL)
	    {
		if(mapInfoP->u.map_info_mmap.map_size != MAP_INFO_UNKNOWN)
		    Size = mapInfoP->u.map_info_mmap.map_size;
		munmap((caddr_t)memInfoP->regionVirtBase, Size);
		memInfoP->regionVirtBase = NULL;
	    }
	    break;
	 default:
	    FatalError("xf86MapInfoMap: Unsuported mapping method\n");
	    break;
    }
}
#endif __arm32__

#ifdef USE_I386_IOPL
/***************************************************************************/
/* I/O Permissions section                                                 */
/***************************************************************************/

static Bool ScreenEnabled[MAXSCREENS];
static Bool ExtendedEnabled = FALSE;
static Bool InitDone = FALSE;

void
xf86ClearIOPortList(ScreenNum)
int ScreenNum;
{
	if (!InitDone)
	{
		int i;
		for (i = 0; i < MAXSCREENS; i++)
			ScreenEnabled[i] = FALSE;
		InitDone = TRUE;
	}
	return;
}

void
xf86AddIOPorts(ScreenNum, NumPorts, Ports)
int ScreenNum;
int NumPorts;
unsigned *Ports;
{
	return;
}

void
xf86EnableIOPorts(ScreenNum)
int ScreenNum;
{
	int i;

	ScreenEnabled[ScreenNum] = TRUE;

	if (ExtendedEnabled)
		return;

	if (i386_iopl(TRUE) < 0)
	{
		FatalError("%s: Failed to set IOPL for extended I/O\n",
			   "xf86EnableIOPorts");
	}
	ExtendedEnabled = TRUE;

	return;
}
	
void
xf86DisableIOPorts(ScreenNum)
int ScreenNum;
{
	int i;

	ScreenEnabled[ScreenNum] = FALSE;

	if (!ExtendedEnabled)
		return;

	for (i = 0; i < MAXSCREENS; i++)
		if (ScreenEnabled[i])
			return;

	i386_iopl(TRUE);
	ExtendedEnabled = FALSE;

	return;
}


void xf86DisableIOPrivs()
{
	if (ExtendedEnabled)
		i386_iopl(FALSE);
	return;
}

#endif /* USE_I386_IOPL */

#if defined(USE_ARC_MMAP) || defined(__arm32__)

#ifdef USE_ARM32_MMAP
#define	DEV_MEM_IOBASE	0x43000000
#endif

static Bool ScreenEnabled[MAXSCREENS];
static Bool ExtendedEnabled = FALSE;
static Bool InitDone = FALSE;

void
xf86ClearIOPortList(ScreenNum)
int ScreenNum;
{
	if (!InitDone)
	{
		int i;
		for (i = 0; i < MAXSCREENS; i++)
			ScreenEnabled[i] = FALSE;
		InitDone = TRUE;
	}
	return;
}

void
xf86AddIOPorts(ScreenNum, NumPorts, Ports)
int ScreenNum;
int NumPorts;
unsigned *Ports;
{
	return;
}

void
xf86EnableIOPorts(ScreenNum)
int ScreenNum;
{
	int i;
	int fd;
	pointer base;
#ifdef __arm32__
	struct memAccess *memInfoP;
	int *Size;
#endif

	ScreenEnabled[ScreenNum] = TRUE;

	if (ExtendedEnabled)
		return;

#ifdef USE_ARC_MMAP
	if ((fd = open("/dev/ttyC0", O_RDWR)) >= 0) {
		/* Try to map a page at the pccons I/O space */
		base = (pointer)mmap((caddr_t)0, 65536, PROT_READ|PROT_WRITE,
				MAP_FILE, fd, (off_t)0x0000);

		if (base != (pointer)-1) {
			IOPortBase = base;
		}
		else {
			ErrorF("EnableIOPorts: failed to mmap %s (%s)\n",
				"/dev/ttyC0", strerror(errno));
		}
	}
	else {
		ErrorF("EnableIOPorts: failed to open %s (%s)\n",
			"/dev/ttyC0", strerror(errno));
	}
#endif
#ifdef __arm32__
	IOPortBase = (unsigned int)-1;

	if((memInfoP = checkMapInfo(TRUE, MMIO_REGION)) != NULL)
	{
	    /* 
	     * XXX
	     * xf86MapInfoMap maps an offset from the start of video IO
	     * space (e.g. 0x3B0), but IOPortBase is expected to map to
	     * physical address 0x000, so subtract the start of video I/O
	     * space from the result.  This is safe for now becase we
	     * actually mmap the start of the page, then the start of video
	     * I/O space is added as an internal offset.
	     */
	    IOPortBase = (unsigned int)xf86MapInfoMap(memInfoP,
						      (caddr_t)0x0, 0L) 
		- memInfoP->memInfo.u.map_info_mmap.internal_offset;
	    ExtendedEnabled = TRUE;
	    return;
	}
#ifdef USE_ARM32_MMAP
	if (!devMemChecked)
	    checkDevMem(TRUE);

	if (devMemFd >= 0 && useDevMem)
	{
	    base = (pointer)mmap((caddr_t)0, 0x400, PROT_READ|PROT_WRITE,
				 MAP_FILE, devMemFd, (off_t)DEV_MEM_IOBASE);

	    if (base != (pointer)-1)
		IOPortBase = (unsigned int)base;
	}
	
        if (IOPortBase == (unsigned int)-1)
	{	
	    FatalError("xf86EnableIOPorts: failed to open mem device or map IO base. \n\
Make sure you have the Aperture Driver installed, or a kernel built with the INSECURE option\n");
	}
#else
	/* We don't have the IOBASE, so we can't map the address */
	FatalError("xf86EnableIOPorts: failed to open mem device or map IO base. \n\
Try building the server with USE_ARM32_MMAP defined\n");
#endif
#endif
	
	ExtendedEnabled = TRUE;

	return;
}

void
xf86DisableIOPorts(ScreenNum)
int ScreenNum;
{
	int i;
#ifdef __arm32__
        struct memAccess *memInfoP;
#endif

	ScreenEnabled[ScreenNum] = FALSE;

#ifdef __arm32__
	if((memInfoP = checkMapInfo(FALSE, MMIO_REGION)) != NULL)
	{
	    xf86MapInfoUnmap(memInfoP, 0);
	}
#endif
#ifdef USE_ARM32_MMAP
	if (!ExtendedEnabled)
		return;

	for (i = 0; i < MAXSCREENS; i++)
		if (ScreenEnabled[i])
			return;

	munmap((caddr_t)IOPortBase, 0x400);
	IOPortBase = (unsigned int)-1;
	ExtendedEnabled = FALSE;
#endif

	return;
}

void xf86DisableIOPrivs()
{
}

#endif /* USE_ARC_MMAP || USE_ARM32_MMAP */

/***************************************************************************/
/* Interrupt Handling section                                              */
/***************************************************************************/

Bool xf86DisableInterrupts()
{

#if !defined(__mips__) && !defined(__arm32__)
#ifdef __GNUC__
	__asm__ __volatile__("cli");
#else 
	asm("cli");
#endif /* __GNUC__ */
#endif /* !__mips__ && !__arm32__ */

	return(TRUE);
}

void xf86EnableInterrupts()
{

#if !defined(__mips__) && !defined(__arm32__)
#ifdef __GNUC__
	__asm__ __volatile__("sti");
#else 
	asm("sti");
#endif /* __GNUC__ */
#endif /* !__mips__ && !__arm32__ */

	return;
}

/***************************************************************************/
/* Set TV output mode                                                      */
/***************************************************************************/
void xf86SetTVOut(int mode)
{    
    ErrorF("GJS: xf86SetTVOut Entered!\n");
    
    switch (xf86Info.consType)
    {
#ifdef PCCONS_SUPPORT
	case PCCONS:{

	    if (ioctl (xf86Info.consoleFd, CONSOLE_X_TV_ON, &mode) < 0)
	    {
		ErrorF("xf86SetTVOut: Could not set console to TV output, %s\n", strerror(errno));
	    }
	}
	break;
#endif /* PCCONS_SUPPORT */

	default:
	    FatalError("Xf86SetTVOut: Unsupported console\n");
	    break; 
    }
    return;
}

void xf86SetRGBOut()
{    
    switch (xf86Info.consType)
    {
#ifdef PCCONS_SUPPORT
	case PCCONS:{
	    
	    if (ioctl (xf86Info.consoleFd, CONSOLE_X_TV_OFF, 0) < 0)
	    {
		ErrorF("xf86SetTVOut: Could not set console to RGB output, %s\n", strerror(errno));
	    }
	}
	break;
#endif /* PCCONS_SUPPORT */

	default:
	    FatalError("Xf86SetTVOut: Unsupported console\n");
	    break; 
    }
	return;
}
