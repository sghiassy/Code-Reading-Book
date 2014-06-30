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

/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/chips/ct_accel.c,v 3.5.2.1 1997/05/03 09:47:59 dawes Exp $ */


#include "vga256.h"
#include "compiler.h"
#include "xf86.h"
#include "vga.h"
#include "xf86xaa.h"
#include "ct_driver.h"

#ifdef	__arm32__
#ifdef	CHIPS_HIQV
#include "xf86local.h"
#endif
#endif

#if (defined(__STDC__) && !defined(UNIXCPP)) || defined(ANSICPP)
#define CATNAME(prefix,subname) prefix##subname
#else
#define CATNAME(prefix,subname) prefix/**/subname
#endif

#ifdef CHIPS_MMIO
#ifdef CHIPS_HIQV
#include "ct_BltHiQV.h"
#define CTNAME(subname) CATNAME(ctHiQV,subname)
#else
#include "ct_BlitMM.h"
#define CTNAME(subname) CATNAME(ctMMIO,subname)
#endif
#else
#include "ct_Blitter.h"
#define CTNAME(subname) CATNAME(ct,subname)
#endif

void CTNAME(Sync)();
void CTNAME(8SetupForFillRectSolid)();
void CTNAME(16SetupForFillRectSolid)();
void CTNAME(24SetupForFillRectSolid)();
void CTNAME(SubsequentFillRectSolid)();
#ifndef CHIPS_HIQV
void CTNAME(24SubsequentFillRectSolid)();
#endif
void CTNAME(SetupForScreenToScreenCopy)();
void CTNAME(SubsequentScreenToScreenCopy)();
void CTNAME(SetupForScanlineScreenToScreenColorExpand)();
void CTNAME(SubsequentScanlineScreenToScreenColorExpand)();
void CTNAME(SetupForScanlineCPUToScreenColorExpand)();
void CTNAME(SubsequentScanlineCPUToScreenColorExpand)();
void CTNAME(SetupForScreenToScreenColorExpand)();
void CTNAME(SubsequentScreenToScreenColorExpand)();
void CTNAME(SetupForCPUToScreenColorExpand)();
void CTNAME(SubsequentCPUToScreenColorExpand)();
void CTNAME(SetupForFill8x8Pattern)();
void CTNAME(SubsequentFill8x8Pattern)();
void CTNAME(SetupFor8x8PatternColorExpand)();
void CTNAME(Subsequent8x8PatternColorExpand)();

#ifdef	__arm32__
#ifdef	CHIPS_HIQV
RegionPtr CTNAME(CopyArea)();
void CTNAME(DoPixWinBitBltCopy)();
#endif
#endif

static unsigned int CommandFlags;


#ifdef CHIPS_MMIO
#ifdef CHIPS_HIQV
#define _ctAccelInit ctHiQVAccelInit
#else
#define _ctAccelInit ctMMIOAccelInit
#endif
#else
#define _ctAccelInit ctAccelInit
#endif
void _ctAccelInit() {
    /*
     * Set up the main acceleration flags.
     */
    xf86AccelInfoRec.Flags = BACKGROUND_OPERATIONS | PIXMAP_CACHE |
	HARDWARE_PATTERN_MONO_TRANSPARENCY | HARDWARE_PATTERN_MOD_64_OFFSET |
	HARDWARE_PATTERN_SCREEN_ORIGIN | HARDWARE_PATTERN_BIT_ORDER_MSBFIRST;
#ifdef CHIPS_HIQV
#if 0
    /* I believe this is possible for the HiQV architecture. Anyone want
     * to test it? If your machine locks up it didn't work */
    xf86AccelInfoRec.Flags |= COP_FRAMEBUFFER_CONCURRENCY;
#endif
    if (ctColorTransparency)
	xf86AccelInfoRec.Flags |= HARDWARE_PATTERN_TRANSPARENCY;
#endif

    /*
     * The following line installs a "Sync" function, that waits for
     * all coprocessor operations to complete.
     */
    xf86AccelInfoRec.Sync = CTNAME(Sync);

    /* 
     * Setup a Screen to Screen copy (BitBLT) primitive
     */  
#ifndef CHIPS_HIQV
    xf86GCInfoRec.CopyAreaFlags = NO_PLANEMASK | NO_TRANSPARENCY;
#else
    xf86GCInfoRec.CopyAreaFlags = NO_PLANEMASK;
    /* A Chips and Technologies application notes says that some
     * 65550 have a bug that prevents 16bpp transparency. It probably
     * applies to 24 bpp as well (Someone with a 65550 care to check?).
     * Selection of this controlled in Probe.
     */
    if (!ctColorTransparency)
	xf86GCInfoRec.CopyAreaFlags |= NO_TRANSPARENCY;

#ifdef	__arm32__
    xf86GCInfoRec.CopyArea = CTNAME(CopyArea);
#endif
#endif
    xf86AccelInfoRec.SetupForScreenToScreenCopy =
	CTNAME(SetupForScreenToScreenCopy);
    xf86AccelInfoRec.SubsequentScreenToScreenCopy =
	CTNAME(SubsequentScreenToScreenCopy);

    /*
     * Install the low-level functions for drawing solid filled rectangles.
     */
    xf86GCInfoRec.PolyFillRectSolidFlags = NO_PLANEMASK;
    switch (vga256InfoRec.bitsPerPixel) {
    case 8 :
        xf86AccelInfoRec.SetupForFillRectSolid =
	    CTNAME(8SetupForFillRectSolid);
        xf86AccelInfoRec.SubsequentFillRectSolid =
	    CTNAME(SubsequentFillRectSolid);
        break;
    case 16 :
        xf86AccelInfoRec.SetupForFillRectSolid =
	    CTNAME(16SetupForFillRectSolid);
        xf86AccelInfoRec.SubsequentFillRectSolid = 
	    CTNAME(SubsequentFillRectSolid);
        break;
    case 24 :
        xf86AccelInfoRec.SetupForFillRectSolid = 
	    CTNAME(24SetupForFillRectSolid);
#ifdef CHIPS_HIQV
        xf86AccelInfoRec.SubsequentFillRectSolid =
	    CTNAME(SubsequentFillRectSolid);
#else
	/*
	 * It is possible to use an RGB_EQUAL or a GXCOPY_ONLY
	 * version of this routine for 24bpp fills using the 8bpp
	 * engine. However how to set it up so that the non GXCopy
	 * operations without RGB being equal go to the right place?
	 * For this reason only the GXCopy version is here, as I felt
	 * this was probably the more useful.
	 */
        xf86AccelInfoRec.SubsequentFillRectSolid =
	    CTNAME(24SubsequentFillRectSolid);
	xf86GCInfoRec.PolyFillRectSolidFlags |= GXCOPY_ONLY;
#endif
        break;
    }

    /*
     * Setup the functions that perform monochrome colour expansion
     */

#ifdef CHIPS_HIQV
    xf86AccelInfoRec.ColorExpandFlags = NO_PLANEMASK |
	VIDEO_SOURCE_GRANULARITY_DWORD | BIT_ORDER_IN_BYTE_MSBFIRST |
	SCANLINE_PAD_DWORD | CPU_TRANSFER_PAD_QWORD | LEFT_EDGE_CLIPPING 
#if 0
        | LEFT_EDGE_CLIPPING_NEGATIVE_X;
#else
        ;
#endif
#else
    xf86AccelInfoRec.ColorExpandFlags = NO_PLANEMASK |
	VIDEO_SOURCE_GRANULARITY_DWORD | BIT_ORDER_IN_BYTE_MSBFIRST |
	SCANLINE_PAD_DWORD | CPU_TRANSFER_PAD_DWORD;
    if (vga256InfoRec.bitsPerPixel == 24)
	xf86AccelInfoRec.ColorExpandFlags |= TRIPLE_BITS_24BPP |
	    RGB_EQUAL;
#endif

#if 0 /* I have trouble with these, on both a 65545 and 65550. So Disable
       * for now. Fixup scratch buffer in FbInit if re-enabled
       */
    xf86AccelInfoRec.SetupForScanlineScreenToScreenColorExpand =
	CTNAME(SetupForScanlineScreenToScreenColorExpand);
    xf86AccelInfoRec.SubsequentScanlineScreenToScreenColorExpand =
	CTNAME(SubsequentScanlineScreenToScreenColorExpand);
    xf86AccelInfoRec.SetupForScreenToScreenColorExpand =
	CTNAME(SetupForScreenToScreenColorExpand);
    xf86AccelInfoRec.SubsequentScreenToScreenColorExpand =
	CTNAME(SubsequentScreenToScreenColorExpand);
    xf86AccelInfoRec.ScratchBufferAddr = ctColorExpandScratchAddr;
    xf86AccelInfoRec.ScratchBufferSize = ctColorExpandScratchSize;
#endif
    xf86AccelInfoRec.SetupForScanlineCPUToScreenColorExpand =
	CTNAME(SetupForScanlineCPUToScreenColorExpand);
    xf86AccelInfoRec.SubsequentScanlineCPUToScreenColorExpand =
	CTNAME(SubsequentScanlineCPUToScreenColorExpand);
    xf86AccelInfoRec.SetupForCPUToScreenColorExpand =
	CTNAME(SetupForCPUToScreenColorExpand);
    xf86AccelInfoRec.SubsequentCPUToScreenColorExpand =
	CTNAME(SubsequentCPUToScreenColorExpand);
    xf86AccelInfoRec.CPUToScreenColorExpandBase =
	(unsigned int *)ctBltDataWindow;
    xf86AccelInfoRec.CPUToScreenColorExpandRange = 64 * 1024;

    /* HiQV chips support 24bpp pattern tile. But XAA has problems with it */
    if (vga256InfoRec.bitsPerPixel != 24) {
        xf86AccelInfoRec.SetupForFill8x8Pattern =
	    CTNAME(SetupForFill8x8Pattern);
        xf86AccelInfoRec.SubsequentFill8x8Pattern =
            CTNAME(SubsequentFill8x8Pattern);
        xf86AccelInfoRec.SetupFor8x8PatternColorExpand =
	    CTNAME(SetupFor8x8PatternColorExpand);
        xf86AccelInfoRec.Subsequent8x8PatternColorExpand =
            CTNAME(Subsequent8x8PatternColorExpand);
    }

    xf86InitPixmapCache(&vga256InfoRec, vga256InfoRec.virtualY *
        vga256InfoRec.displayWidth * vga256InfoRec.bitsPerPixel / 8,
        ctCacheEnd);

}

void CTNAME(Sync)() {
    ctBLTWAIT;
}

/*
 * Solid rectangle fill.
 */

void CTNAME(8SetupForFillRectSolid)(color, rop, planemask)
    int color, rop;
    unsigned planemask;
{
    CommandFlags = ctAluConv2[rop & 0xF] | ctTOP2BOTTOM | ctLEFT2RIGHT 
	     | ctPATSOLID | ctPATMONO;
    ctBLTWAIT;
    ctSETBGCOLOR8(color);
    ctSETFGCOLOR8(color);
    ctSETPITCH(0, vga256InfoRec.displayWidth * vgaBytesPerPixel);
}

void CTNAME(16SetupForFillRectSolid)(color, rop, planemask)
    int color, rop;
    unsigned planemask;
{
    CommandFlags = ctAluConv2[rop & 0xF] | ctTOP2BOTTOM | ctLEFT2RIGHT 
	     | ctPATSOLID | ctPATMONO;
    ctBLTWAIT;
    ctSETFGCOLOR16(color);
    ctSETBGCOLOR16(color);
    ctSETPITCH(0, vga256InfoRec.displayWidth * vgaBytesPerPixel);
}

#ifdef CHIPS_HIQV
void CTNAME(24SetupForFillRectSolid)(color, rop, planemask)
    int color, rop;
    unsigned planemask;
{
    CommandFlags = ctAluConv2[rop & 0xF] | ctTOP2BOTTOM | ctLEFT2RIGHT 
	     | ctPATSOLID | ctPATMONO;
    ctBLTWAIT;
    ctSETFGCOLOR24(color);
    ctSETBGCOLOR24(color);
    ctSETPITCH(0, vga256InfoRec.displayWidth * vgaBytesPerPixel);
}
#else

static unsigned char fgpixel, bgpixel, xorpixel;
static int fillindex;
static Bool fastfill;

void CTNAME(24SetupForFillRectSolid)(color, rop, planemask)
    int color, rop;
    unsigned planemask;
{
    unsigned char pixel1, pixel2, pixel3;

    pixel3 = color & 0xFF;
    pixel2 = (color >> 8) & 0xFF;
    pixel1 = (color >> 16) & 0xFF;
    fgpixel = pixel1;
    bgpixel = pixel2;
    fillindex = 0;
    fastfill = FALSE;

    /* Test for the special case where two of the byte of the 
     * 24bpp colour are the same. This can double the speed
     */
    if (pixel1 == pixel2) {
	fgpixel = pixel3;
	bgpixel = pixel1;
	fastfill = TRUE;
	fillindex = 1;
    } else if (pixel1 == pixel3) { 
	fgpixel = pixel2;
	bgpixel = pixel1;
	fastfill = TRUE;
	fillindex = 2;
    } else if (pixel2 == pixel3) { 
	fastfill = TRUE;
    } else {
	xorpixel = pixel2 ^ pixel3;
    }

    CommandFlags = ctSRCMONO | ctSRCSYSTEM | ctTOP2BOTTOM | ctLEFT2RIGHT;
    ctBLTWAIT;
    if (fastfill) { 
	ctSETFGCOLOR8(fgpixel);
    }
    ctSETBGCOLOR8(bgpixel);
    ctSETSRCADDR(0);
    ctSETPITCH(0, vga256InfoRec.displayWidth * 3);
}

void CTNAME(24SubsequentFillRectSolid)(x, y, w, h)
    int x, y, w, h;
{
    static unsigned int dwords[3] = { 0x24499224, 0x92244992, 0x49922449};
    int srcaddr, destaddr, line, i, width, dispw;

    if(w == 0 || h == 0)
      return;

    destaddr = (y * vga256InfoRec.displayWidth + x) * 3;
    dispw = vga256InfoRec.displayWidth  * 3;
    destaddr = y * dispw + x * 3;
    w *= 3;
    width = ((w  + 31) & ~31) >> 5;

    ctBLTWAIT;
    ctSETDSTADDR(destaddr);

    if (!fastfill) ctSETFGCOLOR8(fgpixel);
    ctSETROP(CommandFlags | ctAluConv[GXcopy & 0xF]);
    ctSETDSTADDR(destaddr);
    if (fastfill) {
	ctSETHEIGHTWIDTHGO(h, w);
	line = 0;
	while (line < h) {
	    for (i = 0; i < width; i++) {
		*(unsigned int *)ctBltDataWindow = dwords[((fillindex + i) % 3)];
	    }
	    line++;
	}
    }
    else {
	ctSETHEIGHTWIDTHGO(1, w);
	i = 0;
	while(i < width){
	    *(unsigned int *)ctBltDataWindow = dwords[(i++ % 3)];
	}

	for(line = 0; (h >> line ) > 1; line++){;}

	i = 0;

	ctBLTWAIT;
	ctSETFGCOLOR8(xorpixel);
	ctSETROP(CommandFlags | ctAluConv[GXxor & 0xF] | ctBGTRANSPARENT);
	ctSETDSTADDR(destaddr);
	ctSETHEIGHTWIDTHGO(1, w);

	while(i < width) {
	    *(unsigned int *)ctBltDataWindow = dwords[((++i) % 3)];
	}

	srcaddr = destaddr;

	ctBLTWAIT;

	if(line){
	    i = 0;
	    ctBLTWAIT;
	    ctSETROP(ctTOP2BOTTOM | ctLEFT2RIGHT | ctAluConv[GXcopy & 0xF]);
	    ctSETPITCH(dispw, dispw);
	    ctSETSRCADDR(srcaddr);
	    
	    while(i < line){
	      destaddr = srcaddr + (dispw << i);
	      ctBLTWAIT;
	      ctSETDSTADDR(destaddr);
	      ctSETHEIGHTWIDTHGO((1 << i), w);
	      i++;
	    }

	    if((1 <<  line)  < h){
	      destaddr = srcaddr + (dispw << line);
	      ctBLTWAIT;
	      ctSETDSTADDR(destaddr);
	      ctSETHEIGHTWIDTHGO(h-(1 << line), w);
	    }

	    ctBLTWAIT;
	    ctSETROP(ctSRCMONO | ctSRCSYSTEM | ctTOP2BOTTOM | ctLEFT2RIGHT |
		     ctAluConv[GXcopy & 0xF]);
	    ctSETSRCADDR(0);
	    ctSETPITCH(0, dispw);
	  }
      }
}
#endif

void CTNAME(SubsequentFillRectSolid)(x, y, w, h)
    int x, y, w, h;
{
    int destaddr;

    destaddr = (y * vga256InfoRec.displayWidth + x) * vgaBytesPerPixel;
    ctBLTWAIT;
    ctSETROP(CommandFlags);
    ctSETDSTADDR(destaddr);
    ctSETHEIGHTWIDTHGO(h, w * vgaBytesPerPixel);
}

/*
 * Screen-to-screen BitBLT.
 *
 */
 
void CTNAME(SetupForScreenToScreenCopy)(xdir, ydir, rop, planemask,
transparency_color)
    int xdir, ydir;
    int rop;
    unsigned planemask;
    int transparency_color;
{
    CommandFlags = 0;
    /* Set up the blit direction. */
    if (ydir < 0)
	CommandFlags |= ctBOTTOM2TOP;
    else
	CommandFlags |= ctTOP2BOTTOM;
    if (xdir < 0)
	CommandFlags |= ctRIGHT2LEFT;
    else
	CommandFlags |= ctLEFT2RIGHT;
#ifdef CHIPS_HIQV
    if (transparency_color != -1) {
	CommandFlags |= ctCOLORTRANSENABLE | ctCOLORTRANSROP |
	    ctCOLORTRANSNEQUAL;
	ctBLTWAIT;
        switch (vga256InfoRec.bitsPerPixel) {
        case 8:
	    ctSETBGCOLOR8(transparency_color);
	    break;
        case 16:
	    ctSETBGCOLOR16(transparency_color);
	    break;
        case 24:
	    ctSETBGCOLOR24(transparency_color);
	    break;
        }
    } else
#endif
        ctBLTWAIT;
    ctSETROP(CommandFlags | ctAluConv[rop & 0xF]);
    ctSETPITCH(vga256InfoRec.displayWidth * vgaBytesPerPixel,
	       vga256InfoRec.displayWidth * vgaBytesPerPixel);
}

void CTNAME(SubsequentScreenToScreenCopy)(x1, y1, x2, y2, w, h)
    int x1, y1, x2, y2, w, h;
{
    unsigned int srcaddr, destaddr;
#ifdef CHIPS_HIQV
    if (CommandFlags & ctBOTTOM2TOP) {
        srcaddr = (y1 + h - 1) * vga256InfoRec.displayWidth;
	destaddr = (y2 + h - 1) * vga256InfoRec.displayWidth;
    } else {
        srcaddr = y1 * vga256InfoRec.displayWidth;
        destaddr = y2 * vga256InfoRec.displayWidth;
    }
    if (CommandFlags & ctRIGHT2LEFT) {
	srcaddr = ( srcaddr + x1 + w ) * vgaBytesPerPixel - 1 ;
	destaddr = ( destaddr + x2 + w ) * vgaBytesPerPixel - 1;
    } else {
	srcaddr = (srcaddr + x1) * vgaBytesPerPixel;
	destaddr = (destaddr + x2) * vgaBytesPerPixel;
    }
#else
    if (CommandFlags & ctTOP2BOTTOM) {
        srcaddr = y1 * vga256InfoRec.displayWidth;
        destaddr = y2 * vga256InfoRec.displayWidth;
    } else {
        srcaddr = (y1 + h - 1) * vga256InfoRec.displayWidth;
	destaddr = (y2 + h - 1) * vga256InfoRec.displayWidth;
    }
    if (CommandFlags & ctLEFT2RIGHT) {
	srcaddr = (srcaddr + x1) * vgaBytesPerPixel;
	destaddr = (destaddr + x2) * vgaBytesPerPixel;
    } else {
	srcaddr = ( srcaddr + x1 + w ) * vgaBytesPerPixel - 1 ;
	destaddr = ( destaddr + x2 + w ) * vgaBytesPerPixel - 1;
    }
#endif
    ctBLTWAIT;
    ctSETSRCADDR(srcaddr);
    ctSETDSTADDR(destaddr);
    ctSETHEIGHTWIDTHGO(h, w * vgaBytesPerPixel );
}

static unsigned int scanlinewidth;
static unsigned int scanlinedestaddr;

void CTNAME(SetupForScanlineScreenToScreenColorExpand)(x, y, w, h, bg, fg, rop,
planemask)
    int x, y, w, h, bg, fg, rop;
    unsigned int planemask;
{
    scanlinedestaddr = (y * vga256InfoRec.displayWidth + x) * vgaBytesPerPixel;
    scanlinewidth = w * vgaBytesPerPixel;

    ctBLTWAIT;
    CommandFlags = 0;
    if (bg == -1) {
	CommandFlags |= ctBGTRANSPARENT;	/* Background = Destination */
        switch (vga256InfoRec.bitsPerPixel) {
        case 8:
	    ctSETFGCOLOR8(fg);
	    break;
        case 16:
	    ctSETFGCOLOR16(fg);
	    break;
        case 24:
#ifdef CHIPS_HIQV
	    ctSETFGCOLOR24(fg);
#else
	    ctSETFGCOLOR8(fg);
#endif
	    break;
        }
    }
    else {
        switch (vga256InfoRec.bitsPerPixel) {
        case 8:
	    ctSETBGCOLOR8(bg);
	    ctSETFGCOLOR8(fg);
	    break;
        case 16:
	    ctSETBGCOLOR16(bg);
	    ctSETFGCOLOR16(fg);
	    break;
        case 24:
#ifdef CHIPS_HIQV
	    ctSETBGCOLOR24(bg);
	    ctSETFGCOLOR24(fg);
#else
	    ctSETBGCOLOR8(bg);
	    ctSETFGCOLOR8(fg);
#endif
	    break;
        }
    }
#ifdef CHIPS_HIQV
    ctSETMONOCTL(ctDWORDALIGN);
#endif
    ctSETROP(ctSRCMONO | ctTOP2BOTTOM | ctLEFT2RIGHT | 
	ctAluConv[rop & 0xF] | CommandFlags);
    ctSETPITCH(vga256InfoRec.displayWidth * vgaBytesPerPixel,
	       vga256InfoRec.displayWidth * vgaBytesPerPixel);
}

void CTNAME(SubsequentScanlineScreenToScreenColorExpand)(srcaddr)
    int srcaddr;
{
    ctBLTWAIT;
    ctSETSRCADDR(srcaddr / 8);
    ctSETDSTADDR(scanlinedestaddr);
    ctSETHEIGHTWIDTHGO(1, scanlinewidth);
    scanlinedestaddr += vga256InfoRec.displayWidth * vgaBytesPerPixel;
}

void CTNAME(SetupForScanlineCPUToScreenColorExpand)(x, y, w, h, bg, fg, rop,
planemask)
    int x, y, w, h, bg, fg, rop;
    unsigned int planemask;
{
    unsigned int mask;
    scanlinedestaddr = (y * vga256InfoRec.displayWidth + x) * vgaBytesPerPixel;
    scanlinewidth = w * vgaBytesPerPixel;
    CommandFlags = 0;
    ctBLTWAIT;
    if (bg == -1) {
	CommandFlags |= ctBGTRANSPARENT;	/* Background = Destination */
        switch (vga256InfoRec.bitsPerPixel) {
        case 8:
	    ctSETFGCOLOR8(fg);
	    break;
        case 16:
	    ctSETFGCOLOR16(fg);
	    break;
        case 24:
#ifdef CHIPS_HIQV
	    ctSETFGCOLOR24(fg);
#else
	    ctSETFGCOLOR8(fg);
#endif
	    break;
        }
    }
    else {
        switch (vga256InfoRec.bitsPerPixel) {
        case 8:
	    ctSETBGCOLOR8(bg);
	    ctSETFGCOLOR8(fg);
	    break;
        case 16:
	    ctSETBGCOLOR16(bg);
	    ctSETFGCOLOR16(fg);
	    break;
        case 24:
#ifdef CHIPS_HIQV
	    ctSETBGCOLOR24(bg);
	    ctSETFGCOLOR24(fg);
#else
	    ctSETBGCOLOR8(bg);
	    ctSETFGCOLOR8(fg);
#endif
	    break;
        }
    }
#ifdef CHIPS_HIQV
    ctSETMONOCTL(ctDWORDALIGN);
#endif
    ctSETSRCADDR(0);
    ctSETROP(ctSRCSYSTEM | ctSRCMONO | ctTOP2BOTTOM | ctLEFT2RIGHT | 
	ctAluConv[rop & 0xF] | CommandFlags);
    ctSETPITCH(0, vga256InfoRec.displayWidth * vgaBytesPerPixel);
}

void CTNAME(SubsequentScanlineCPUToScreenColorExpand)()
{
    ctBLTWAIT;
    ctSETDSTADDR(scanlinedestaddr);
    ctSETHEIGHTWIDTHGO(1, scanlinewidth);
    scanlinedestaddr += vga256InfoRec.displayWidth;
}

void CTNAME(SetupForScreenToScreenColorExpand)(bg, fg, rop, planemask)
    int bg, fg, rop;
    unsigned int planemask;
{
    CommandFlags = 0;
    ctBLTWAIT;
    if (bg == -1) {
	CommandFlags |= ctBGTRANSPARENT;	/* Background = Destination */
        switch (vga256InfoRec.bitsPerPixel) {
        case 8:
	    ctSETFGCOLOR8(fg);
	    break;
        case 16:
	    ctSETFGCOLOR16(fg);
	    break;
        case 24:
#ifdef CHIPS_HIQV
	    ctSETFGCOLOR24(fg);
#else
	    ctSETFGCOLOR8(fg);
#endif
	    break;
        }
    }
    else {
        switch (vga256InfoRec.bitsPerPixel) {
        case 8:
	    ctSETBGCOLOR8(bg);
	    ctSETFGCOLOR8(fg);
	    break;
        case 16:
	    ctSETBGCOLOR16(bg);
	    ctSETFGCOLOR16(fg);
	    break;
        case 24:
#ifdef CHIPS_HIQV
	    ctSETBGCOLOR24(bg);
	    ctSETFGCOLOR24(fg);
#else
	    ctSETBGCOLOR8(bg);
	    ctSETFGCOLOR8(fg);
#endif
	    break;
        }
    }
#ifdef CHIPS_HIQV
    ctSETMONOCTL(ctDWORDALIGN);
#endif
    ctSETROP(ctSRCMONO | ctTOP2BOTTOM | ctLEFT2RIGHT | 
	ctAluConv[rop & 0xF] | CommandFlags);
    ctSETPITCH(vga256InfoRec.displayWidth * vgaBytesPerPixel,
	       vga256InfoRec.displayWidth * vgaBytesPerPixel);
}

void CTNAME(SubsequentScreenToScreenColorExpand)(srcx, srcy, x, y, w, h)
    int srcx, srcy, x, y, w, h;
{
    int srcaddr, destaddr;
    srcaddr = (srcy * vga256InfoRec.displayWidth + srcx / 8) * 
	vgaBytesPerPixel;
    destaddr = (y * vga256InfoRec.displayWidth + x) * vgaBytesPerPixel;
    ctBLTWAIT;
    ctSETSRCADDR(srcaddr);
    ctSETDSTADDR(destaddr);
    ctSETHEIGHTWIDTHGO(h, w * vgaBytesPerPixel);
}

void CTNAME(SetupForCPUToScreenColorExpand)(bg, fg, rop, planemask)
    int bg, fg, rop;
    unsigned int planemask;
{
    ctBLTWAIT;
    CommandFlags = 0;
    if (bg == -1) {
	CommandFlags |= ctBGTRANSPARENT;	/* Background = Destination */
        switch (vga256InfoRec.bitsPerPixel) {
        case 8:
	    ctSETFGCOLOR8(fg);
	    break;
        case 16:
	    ctSETFGCOLOR16(fg);
	    break;
        case 24:
#ifdef CHIPS_HIQV
	    ctSETFGCOLOR24(fg);
#else
	    ctSETFGCOLOR8(fg);
#endif
	    break;
        }
    }
    else {
        switch (vga256InfoRec.bitsPerPixel) {
        case 8:
	    ctSETBGCOLOR8(bg);
	    ctSETFGCOLOR8(fg);
	    break;
        case 16:
	    ctSETBGCOLOR16(bg);
	    ctSETFGCOLOR16(fg);
	    break;
        case 24:
#ifdef CHIPS_HIQV
	    ctSETBGCOLOR24(bg);
	    ctSETFGCOLOR24(fg);
#else
	    ctSETBGCOLOR8(bg);
	    ctSETFGCOLOR8(fg);
#endif
	    break;
        }
    }
#ifdef CHIPS_HIQV
    ctSETMONOCTL(ctDWORDALIGN);
#endif
    ctSETSRCADDR(0);
    ctSETROP(ctSRCMONO | ctSRCSYSTEM | ctTOP2BOTTOM | ctLEFT2RIGHT | 
	ctAluConv[rop & 0xF] | CommandFlags);
    ctSETPITCH(0, vga256InfoRec.displayWidth * vgaBytesPerPixel);
}

void CTNAME(SubsequentCPUToScreenColorExpand)(x, y, w, h, skipleft)
    int x, y, w, h, skipleft;
{
    int destaddr;
    destaddr = (y * vga256InfoRec.displayWidth + x + skipleft) * 
               vgaBytesPerPixel;
    ctBLTWAIT;
    ctSETDSTADDR(destaddr);
#ifdef CHIPS_HIQV
    ctSETMONOCTL(ctDWORDALIGN | ctCLIPLEFT(skipleft));
#endif
    ctSETHEIGHTWIDTHGO(h, (w - skipleft) * vgaBytesPerPixel);
}

static int patternyrot;

void CTNAME(SetupForFill8x8Pattern)(patternx, patterny, rop, planemask,
transparency_color)
    int patternx, patterny, rop;
    unsigned planemask;
    int transparency_color;
{
    unsigned int patternaddr;
    
    CommandFlags = ctAluConv2[rop & 0xF] | ctTOP2BOTTOM | ctLEFT2RIGHT;

    /* We seem to have an atypical pattern fill. The X pattern address
     * is with respect to the screen origin while the Y pattern address
     * is with respect to the pattern origin. Use screen origin but keep
     * track to the rotation in the y direction
     */
    patternaddr = (patterny * vga256InfoRec.displayWidth + 
		   (patternx & ~0x3F)) * vgaBytesPerPixel;
    patternyrot = (patternx & 0x3F) >> 3;
#ifdef CHIPS_HIQV
    if (transparency_color != -1) {
	CommandFlags |= ctCOLORTRANSENABLE | ctCOLORTRANSROP |
	    ctCOLORTRANSNEQUAL;
	ctBLTWAIT;
        switch (vga256InfoRec.bitsPerPixel) {
        case 8:
	    ctSETBGCOLOR8(transparency_color);
	    break;
        case 16:
	    ctSETBGCOLOR16(transparency_color);
	    break;
        case 24:
	    ctSETBGCOLOR24(transparency_color);
	    break;
        }
    } else
#endif
        ctBLTWAIT;
    ctSETPATSRCADDR(patternaddr);
    ctSETPITCH(8 * vgaBytesPerPixel,
	       vga256InfoRec.displayWidth * vgaBytesPerPixel);
}

void CTNAME(SubsequentFill8x8Pattern)(patternx, patterny, x, y, w, h)
    int patternx, patterny;
    int x, y, w, h;
{
    unsigned int destaddr;
    destaddr = (y * vga256InfoRec.displayWidth + x) * vgaBytesPerPixel;
    ctBLTWAIT;
    ctSETDSTADDR(destaddr);
#ifdef CHIPS_HIQV
    ctSETROP(CommandFlags | (((y + patternyrot) & 0x7) << 20));
#else
    ctSETROP(CommandFlags | (((y + patternyrot) & 0x7) << 16));
#endif
    ctSETHEIGHTWIDTHGO(h, w * vgaBytesPerPixel);
}

void CTNAME(SetupFor8x8PatternColorExpand)(patternx, patterny, bg, fg, rop,
planemask)
    int patternx, patterny, bg, fg, rop;
    unsigned int planemask;
{
    unsigned int patternaddr;

    CommandFlags = ctPATMONO | ctTOP2BOTTOM | ctLEFT2RIGHT | 
	ctAluConv2[rop & 0xF];

    patternaddr = patterny * vga256InfoRec.displayWidth * vgaBytesPerPixel +
		   (patternx >> 3);
    ctBLTWAIT;
    ctSETPATSRCADDR(patternaddr);
    if (bg == -1) {
	CommandFlags |= ctBGTRANSPARENT;	/* Background = Destination */
        switch (vga256InfoRec.bitsPerPixel) {
        case 8:
	    ctSETFGCOLOR8(fg);
	    break;
        case 16:
	    ctSETFGCOLOR16(fg);
	    break;
        case 24:
	    ctSETFGCOLOR24(fg);
	    break;
        }
    }
    else {
        switch (vga256InfoRec.bitsPerPixel) {
        case 8:
	    ctSETBGCOLOR8(bg);
	    ctSETFGCOLOR8(fg);
	    break;
        case 16:
	    ctSETBGCOLOR16(bg);
	    ctSETFGCOLOR16(fg);
	    break;
        case 24:
	    ctSETBGCOLOR24(bg);
	    ctSETFGCOLOR24(fg);
	    break;
        }
    }
#ifdef CHIPS_HIQV
    ctSETMONOCTL(ctDWORDALIGN);
#endif
    ctSETPITCH(1,vga256InfoRec.displayWidth * vgaBytesPerPixel);
}

void CTNAME(Subsequent8x8PatternColorExpand)(patternx, patterny, x, y, w, h)
    int patternx, patterny;
    int x, y, w, h;
{
    int destaddr;
    destaddr = (y * vga256InfoRec.displayWidth + x) * vgaBytesPerPixel;
    ctBLTWAIT;
    ctSETDSTADDR(destaddr);
#ifdef CHIPS_HIQV
    ctSETROP(CommandFlags | ((y & 0x7) << 20));
#else
    ctSETROP(CommandFlags | ((y & 0x7) << 16));
#endif
    ctSETHEIGHTWIDTHGO(h, w * vgaBytesPerPixel);
}

#ifdef	__arm32__
#ifdef	CHIPS_HIQV
RegionPtr CTNAME(CopyArea)(pSrcDrawable, pDstDrawable,
			    pGC, srcx, srcy, width, height, dstx, dsty)
    register DrawablePtr pSrcDrawable;
    register DrawablePtr pDstDrawable;
    GC *pGC;
    int srcx, srcy;
    int width, height;
    int dstx, dsty;
{
	/* Usual XAA handling of screen-to-screen blts. */
	if (pSrcDrawable->type == DRAWABLE_WINDOW
	    && pDstDrawable->type == DRAWABLE_WINDOW) {
	    return (*xf86GCInfoRec.cfbBitBltDispatch)(pSrcDrawable, pDstDrawable,
						      pGC, srcx, srcy, width,
						      height, dstx, dsty,
						      xf86DoBitBlt, 0L);
	}

	/* ctHiQV-specific handling of pixmap-to-screen blts. */
	if (pSrcDrawable->type == DRAWABLE_PIXMAP
	    && pDstDrawable->type == DRAWABLE_WINDOW) {
	    return (*xf86GCInfoRec.cfbBitBltDispatch)(pSrcDrawable, pDstDrawable,
						      pGC, srcx, srcy, width,
						      height, dstx, dsty,
						      CTNAME(DoPixWinBitBltCopy), 0L);
	}

	/* Fallback to cfb/vga256. */
	return (*xf86GCInfoRec.CopyAreaFallBack)(pSrcDrawable, pDstDrawable, pGC,
						 srcx, srcy, width, height, dstx, dsty);
    }


void CTNAME(DoPixWinBitBltCopy)(pSrc, pDst, alu, prgnDst,
			     pptSrc, planemask, bitPlane)
    DrawablePtr	    pSrc, pDst;
    int		    alu;
    RegionPtr	    prgnDst;
    DDXPointPtr	    pptSrc;
    unsigned int    planemask;
    int		    bitPlane;
{
    unsigned long *psrcBase;
    int widthSrc, widthDst;
    int byteWidthSrc, byteWidthDst;
    BoxPtr pbox;
    int nbox;

    cfbGetLongWidthAndPointer(pSrc, widthSrc, psrcBase);
    widthSrc = cfbGetByteWidth(pSrc)/vgaBytesPerPixel;
    widthDst = vga256InfoRec.displayWidth;
    byteWidthSrc = widthSrc * vgaBytesPerPixel;
    byteWidthDst = widthDst * vgaBytesPerPixel;

    pbox = REGION_RECTS(prgnDst);
    nbox = REGION_NUM_RECTS(prgnDst);

    for (; nbox; pbox++, pptSrc++, nbox--) {
	unsigned int psrc, pdst;
	int w, h;

	CommandFlags = ctSRCSYSTEM | ctLEFT2RIGHT | ctTOP2BOTTOM;

	ctBLTWAIT;
	ctSETROP(CommandFlags | ctAluConv[alu & 0xF]);
	ctSETPITCH(byteWidthSrc, byteWidthDst);

	w = pbox->x2 - pbox->x1;
	h = pbox->y2 - pbox->y1;
	psrc = (pptSrc->x + pptSrc->y * widthSrc) * vgaBytesPerPixel;
	pdst = (pbox->x1 + pbox->y1 * widthDst) * vgaBytesPerPixel;

	/*
	 *  There is an annoying bug in the CT65550, or at least when used
	 *  on SHARK.  The bug manifests itself as scarring or, more
	 *  commonly, system freeze (because the CT65550 has hung the bus).
	 *
	 *  This bad behavior occurs when the number of bytes per scanline
	 *  to be sent to the CT65550 is not constant.  This can happen
	 *  because the chip insists that each line begin on a quad-word
	 *  (i.e., 8-byte) boundary and that it be padded out to an integral
	 *  number of quad-words.  The chip masks the beginning and ending
	 *  quad-words appropriately so that only the right bits are painted.
	 *
	 *  Given X's padding of pixmaps to 4-byte boundaries, it almost
	 *  always turns out that each scanline sent to the CT65550 consists
	 *  of the same number of quad-words.  But not always.  You sometimes
	 *  get a pattern where n and n+1 quad-words need to be sent in
	 *  alternating fashion.
	 *
	 *  Whether a constant or non-constant number of quad-words needs to
	 *  be sent is a function of the alignment of the source pixmap,
	 *  the starting offset into that pixmap, the width of the pixmap,
	 *  and the number of bytes to be painted on each line.  Computing
	 *  the predicate is fairly involved, but fortunately the failure
	 *  case has an easily-determined necessary condition:  the width
	 *  of the source pixmap is not a quad-word multiple.  This is not
	 *  a sufficient condition, but it's good enough that I'm using it
	 *  to switch out to a "safe" alternative.
	 *
	 *  The safe alternative is to do the blt as n 1-line blts, rather
	 *  than 1 big blt.  This is obviously going to be slower, but it's
	 *  not so much worse that I feel a need to spend time optimizing
	 *  further.
	 */
	{
	    Bool buggyCase = (byteWidthSrc & 0x7) != 0;
	    unsigned char *src = (unsigned char *)psrcBase + psrc;
	    unsigned char *dst = (unsigned char *)pdst;
	    int bytesperline = w * vgaBytesPerPixel;

	    if (!buggyCase) {
		/* One-time set-up. */
		ctSETSRCADDR((unsigned int)src);
		ctSETDSTADDR((unsigned int)dst);
		ctSETHEIGHTWIDTHGO(h, bytesperline);
	    }

	    while (h--) {
		unsigned int *qwa_src = (unsigned int *)((unsigned int)src & (~0x7));
		int qwords = (((unsigned int)src & 0x7) + bytesperline + 0x7) >> 3;

		if (buggyCase) {
		    /* N-time set-up. */
		    ctSETSRCADDR((unsigned int)src);
		    ctSETDSTADDR((unsigned int)dst);
		    ctSETHEIGHTWIDTHGO(1, bytesperline);
		}

		while (qwords--) {
		    *(unsigned int *)ctBltDataWindow = *qwa_src++;
		    *(unsigned int *)ctBltDataWindow = *qwa_src++;
		}

		src += byteWidthSrc;
		if (buggyCase) {
		    dst += byteWidthDst;
		    ctBLTWAIT;
		}
	    }

	    if (!buggyCase)
		ctBLTWAIT;
	}
    }
}
#endif
#endif
