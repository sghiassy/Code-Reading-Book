/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/tvga8900/tgui_oldacl.c,v 3.2.2.2 1997/05/12 11:06:14 hohndel Exp $ */
/*
 * Copyright 1997 by Alan Hourihane, Wigan, England.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Alan Hourihane not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Alan Hourihane makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * ALAN HOURIHANE DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL ALAN HOURIHANE BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Alan Hourihane, alanh@fairlite.demon.co.uk
 * 
 * Trident TGUI 9420, 9430 accelerated options.
 */

#include "vga256.h"
#include "xf86.h"
#include "vga.h"
#include "miline.h"
#include "compiler.h"

#include "xf86xaa.h"

extern unsigned char *tguiMMIOBase;
extern int TGUIRops_alu[16];
extern int TGUIRops_Pixalu[16];
extern int GE_OP;
#include "tgui_ger.h"
#include "tgui_mmio.h"

#ifdef TRIDENT_MMIO

#define MMIONAME(x)			x##MMIO

#define OLDTGUISync			MMIONAME(OLDTGUISync)
#define OLDTGUIAccelInit		MMIONAME(OLDTGUIAccelInit)
#define OLDTGUISetupForFillRectSolid	MMIONAME(OLDTGUISetupForFillRectSolid)
#define OLDTGUISubsequentFillRectSolid	MMIONAME(OLDTGUISubsequentFillRectSolid)
#define OLDTGUISetupForScreenToScreenCopy	MMIONAME(OLDTGUISetupForScreenToScreenCopy)
#define OLDTGUISubsequentScreenToScreenCopy	MMIONAME(OLDTGUISubsequentScreenToScreenCopy)
#define OLDTGUISubsequentBresenhamLine		MMIONAME(OLDTGUISubsequentBresenhamLine)
#define OLDTGUISetupForCPUToScreenColorExpand	MMIONAME(OLDTGUISetupForCPUToScreenColorExpand)
#define OLDTGUISubsequentCPUToScreenColorExpand	MMIONAME(OLDTGUISubsequentCPUToScreenColorExpand)
#define OLDTGUISetupForScreenToScreenColorExpand	MMIONAME(OLDTGUISetupForScreenToScreenColorExpand)
#define OLDTGUISubsequentScreenToScreenColorExpand	MMIONAME(OLDTGUISubsequentScreenToScreenColorExpand)
#define OLDTGUISetupForFill8x8Pattern		MMIONAME(OLDTGUISetupForFill8x8Pattern)
#define OLDTGUISubsequentFill8x8Pattern		MMIONAME(OLDTGUISubsequentFill8x8Pattern)
#define OLDTGUISetupFor8x8PatternColorExpand	MMIONAME(OLDTGUISetupFor8x8PatternColorExpand)
#define OLDTGUISubsequent8x8PatternColorExpand	MMIONAME(OLDTGUISubsequent8x8PatternColorExpand)

#endif

void OLDTGUISync();
void OLDTGUISetupForFillRectSolid();
void OLDTGUISubsequentFillRectSolid();
void OLDTGUISetupForScreenToScreenCopy();
void OLDTGUISubsequentScreenToScreenCopy();
void OLDTGUISubsequentBresenhamLine();
void OLDTGUISetupForCPUToScreenColorExpand();
void OLDTGUISubsequentCPUToScreenColorExpand();
void OLDTGUISetupForScreenToScreenColorExpand();
void OLDTGUISubsequentScreenToScreenColorExpand();
void OLDTGUISetupForFill8x8Pattern();
void OLDTGUISubsequentFill8x8Pattern();
void OLDTGUISetupFor8x8PatternColorExpand();
void OLDTGUISubsequent8x8PatternColorExpand();

/*
 * The following function sets up the supported acceleration. Call it
 * from the FbInit() function in the SVGA driver.
 */
void OLDTGUIAccelInit() {

    xf86AccelInfoRec.Flags = BACKGROUND_OPERATIONS | PIXMAP_CACHE |
                               HARDWARE_PATTERN_TRANSPARENCY |
				HARDWARE_PATTERN_ALIGN_64 |
				HARDWARE_PATTERN_MONO_TRANSPARENCY |
				HARDWARE_PATTERN_SCREEN_ORIGIN;

    xf86AccelInfoRec.Sync = OLDTGUISync;

    xf86GCInfoRec.PolyFillRectSolidFlags = NO_PLANEMASK;

    xf86AccelInfoRec.SetupForFillRectSolid = OLDTGUISetupForFillRectSolid;
    xf86AccelInfoRec.SubsequentFillRectSolid = OLDTGUISubsequentFillRectSolid;

#if 0
    xf86AccelInfoRec.ErrorTermBits = 11;
    xf86AccelInfoRec.SubsequentBresenhamLine = OLDTGUISubsequentBresenhamLine;

    xf86GCInfoRec.CopyAreaFlags = NO_PLANEMASK;

    xf86AccelInfoRec.SetupForScreenToScreenCopy =
       		OLDTGUISetupForScreenToScreenCopy;
    xf86AccelInfoRec.SubsequentScreenToScreenCopy =
	       	OLDTGUISubsequentScreenToScreenCopy;

    /* Fill 8x8 Pattern */
    xf86AccelInfoRec.SetupForFill8x8Pattern = OLDTGUISetupForFill8x8Pattern;
    xf86AccelInfoRec.SubsequentFill8x8Pattern = OLDTGUISubsequentFill8x8Pattern;

    /* 8x8 Pattern Color Expand */
#if 0
    xf86AccelInfoRec.SetupFor8x8PatternColorExpand = 
					OLDTGUISetupFor8x8PatternColorExpand;
    xf86AccelInfoRec.Subsequent8x8PatternColorExpand = 
					OLDTGUISubsequent8x8PatternColorExpand;
#endif

    /* Color Expansion */
    xf86AccelInfoRec.ColorExpandFlags = VIDEO_SOURCE_GRANULARITY_DWORD |
					BIT_ORDER_IN_BYTE_MSBFIRST |
					SCANLINE_PAD_DWORD |
					CPU_TRANSFER_PAD_DWORD |
					LEFT_EDGE_CLIPPING |
					NO_PLANEMASK;

    xf86AccelInfoRec.SetupForCPUToScreenColorExpand = 
	OLDTGUISetupForCPUToScreenColorExpand;
    xf86AccelInfoRec.SubsequentCPUToScreenColorExpand = 
	OLDTGUISubsequentCPUToScreenColorExpand;
    xf86AccelInfoRec.SetupForScreenToScreenColorExpand = 
	OLDTGUISetupForScreenToScreenColorExpand;
    xf86AccelInfoRec.SubsequentScreenToScreenColorExpand = 
	OLDTGUISubsequentScreenToScreenColorExpand;
#endif

    xf86AccelInfoRec.ServerInfoRec = &vga256InfoRec;

    xf86AccelInfoRec.PixmapCacheMemoryStart = vga256InfoRec.virtualY *
		vga256InfoRec.displayWidth * vga256InfoRec.bitsPerPixel / 8;

    xf86AccelInfoRec.PixmapCacheMemoryEnd = vga256InfoRec.videoRam * 1024 
							- 1024;
}

/*
 * This is the implementation of the Sync() function.
 */
void OLDTGUISync() {
	int count = 0, timeout = 0;
	int busy;

	for (;;) {
		OLDBLTBUSY(busy);
		if (busy != GE_BUSY) return;
		count++;
		if (count == 10000000) {
			ErrorF("Trident: BitBLT engine time-out.\n");
			count = 9990000;
			timeout++;
			if (timeout == 8) {
#if 0
				/* Reset BitBLT Engine */
				OLDTGUI_STATUS(0x00);
#endif
				return;
			}
		}
	}
}

/*
 * This is the implementation of the SetupForFillRectSolid function
 * that sets up the coprocessor for a subsequent batch for solid
 * rectangle fills.
 */

void OLDTGUISetupForFillRectSolid(color, rop, planemask)
    int color, rop;
    unsigned planemask;
{
	OLDTGUI_FCOLOUR(color);
	OLDTGUI_BCOLOUR(color);
	OLDTGUI_FMIX(rop);
	OLDTGUI_BMIX(rop);
	OLDTGUI_STYLE(0x5555);
}
/*
 * This is the implementation of the SubsequentForFillRectSolid function
 * that sends commands to the coprocessor to fill a solid rectangle of
 * the specified location and size, with the parameters from the SetUp
 * call.
 */
void OLDTGUISubsequentFillRectSolid(x, y, w, h)
    int x, y, w, h;
{
	OLDTGUI_DIMXY(w,h);
	OLDTGUI_DESTXY(x,y);
	OLDTGUI_DESTLINEAR(y * vga256InfoRec.displayWidth + x);
	OLDTGUI_COMMAND(OLDGE_FILL);
}

#if 0

/*
 * This is the implementation of the SetupForScreenToScreenCopy function
 * that sets up the coprocessor for a subsequent batch for solid
 * screen-to-screen copies. Remember, we don't handle transparency,
 * so the transparency color is ignored.
 */
static int blitxdir, blitydir;
 
void TGUISetupForScreenToScreenCopy(xdir, ydir, rop, planemask,
transparency_color)
    int xdir, ydir;
    int rop;
    unsigned planemask;
    int transparency_color;
{
    int direction = 0;

    if (xdir < 0) direction |= XNEG;
    if (ydir < 0) direction |= YNEG;
    if (transparency_color != -1)
    {
	TGUI_FCOLOUR(transparency_color);
	TGUI_BCOLOUR(transparency_color);
	direction |= TRANS_ENABLE;
    }
    TGUI_DRAWFLAG(direction | SCR2SCR);
    TGUI_FMIX(TGUIRops_alu[rop]);
    blitxdir = xdir;
    blitydir = ydir;
}
/*
 * This is the implementation of the SubsequentForScreenToScreenCopy
 * that sends commands to the coprocessor to perform a screen-to-screen
 * copy of the specified areas, with the parameters from the SetUp call.
 * In this sample implementation, the direction must be taken into
 * account when calculating the addresses (with coordinates, it might be
 * a little easier).
 */
void TGUISubsequentScreenToScreenCopy(x1, y1, x2, y2, w, h)
    int x1, y1, x2, y2, w, h;
{
    if (blitydir < 0) {
        y1 = y1 + h - 1;
	y2 = y2 + h - 1;
    }
    if (blitxdir < 0) {
	x1 = x1 + w - 1;
	x2 = x2 + w - 1;
    }
    TGUI_SRC_XY(x1,y1);
    TGUI_DEST_XY(x2,y2);
    TGUI_DIM_XY(w,h);
    TGUI_COMMAND(GE_BLT);
}

void TGUISubsequentBresenhamLine(x1, y1, octant, err, e1, e2, length)
    int x1, y1, octant, err, e1, e2, length;
{
	int direction = 0;

	if (octant & YMAJOR)      direction |= YMAJ;
	if (octant & XDECREASING) direction |= XNEG;
	if (octant & YDECREASING) direction |= YNEG;
	TGUI_SRC_XY(e2,e1);
	TGUI_DEST_XY(x1,y1);
	TGUI_DIM_XY(err,length);
	TGUI_DRAWFLAG(SOLIDFILL | STENCIL | direction);
	TGUI_COMMAND(GE_BRESLINE);
}

void TGUISetupForCPUToScreenColorExpand(bg, fg, rop, planemask)
    int bg, fg, rop;
    unsigned planemask;
{
	int drawflag = 0;

	TGUI_FCOLOUR(fg);
        if (bg == -1)
		drawflag |= TRANS_ENABLE;
	else
		TGUI_BCOLOUR(bg);
	TGUI_DRAWFLAG(SRCMONO | drawflag);
	TGUI_FMIX(TGUIRops_alu[rop]);
}

void TGUISubsequentCPUToScreenColorExpand(x, y, w, h, skipleft)
    int x, y, w, h, skipleft;
{
	TGUI_OUTB(0x2B, skipleft);
	TGUI_DEST_XY(x,y);
	TGUI_DIM_XY(w,h);
	TGUI_COMMAND(GE_BLT);
}

void TGUISetupForFill8x8Pattern(patternx, patterny, rop, planemask,
transparency_color)
    int patternx, patterny, rop;
    unsigned planemask;
    int transparency_color;
{
	int drawflag = 0;

	if (transparency_color != -1)
	{
		TGUI_FCOLOUR(transparency_color);
		drawflag |= TRANS_ENABLE;
	}
	TGUI_FMIX(TGUIRops_Pixalu[rop]); /* ROP */
	TGUI_DRAWFLAG(drawflag | PAT2SCR);
	TGUI_PATLOC(patterny * vga256InfoRec.displayWidth + patternx);
}

void TGUISubsequentFill8x8Pattern(patternx, patterny, x, y, w, h)
    int patternx, patterny;
    int x, y, w, h;
{
	TGUI_DEST_XY(x,y);
	TGUI_DIM_XY(w,h);
	TGUI_COMMAND(GE_BLT);
}

void TGUISetupForScreenToScreenColorExpand(bg, fg, rop, planemask)
    int bg, fg, rop;
    unsigned planemask;
{
	int drawflag = 0;

	TGUI_FCOLOUR(fg);
        if (bg == -1)
		drawflag |= TRANS_ENABLE;
	else
		TGUI_BCOLOUR(bg);
	TGUI_DRAWFLAG(SCR2SCR | SRCMONO | drawflag);
	TGUI_FMIX(TGUIRops_alu[rop]);
}

void TGUISubsequentScreenToScreenColorExpand(srcx, srcy, x, y, w, h)
    int srcx, srcy, x, y, w, h;
{
	TGUI_SRC_XY(srcx,srcy);
	TGUI_DEST_XY(x,y);
	TGUI_DIM_XY(w,h);
	TGUI_COMMAND(GE_BLT);
}

void TGUISetupFor8x8PatternColorExpand(patternx, patterny, bg, fg, rop,
planemask)
	int patternx, patterny, bg, fg, rop, planemask;
{
	int drawflag = 0;

	TGUI_FCOLOUR(fg);
	if (bg == -1)
		drawflag |= TRANS_ENABLE;
	else
		TGUI_BCOLOUR(bg);
	TGUI_FMIX(TGUIRops_Pixalu[rop]); /* ROP */
	TGUI_DRAWFLAG(drawflag | PATMONO |PAT2SCR);
	TGUI_PATLOC(patterny * vga256InfoRec.displayWidth + patternx);
}

void TGUISubsequent8x8PatternColorExpand(patternx, patterny, x, y, w, h)
	int patternx, patterny, x, y, w, h;
{
	TGUI_DEST_XY(x,y);
	TGUI_DIM_XY(w,h);
	TGUI_COMMAND(GE_BLT);
}
#endif
